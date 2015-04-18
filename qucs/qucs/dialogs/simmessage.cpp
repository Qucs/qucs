/***************************************************************************
                              simmessage.cpp
                             ----------------
    begin                : Sat Sep 6 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*!
 * \file simmessage.cpp
 * \brief Simulation dialog implementation
 *
 * \todo Use QPlainTextEdit instead of QTextEdit ?
 */

#include <stdlib.h>
#include <iostream>
using namespace std;
#include <QLabel>
#include <QGroupBox>
#include <QTimer>
#include <QPushButton>
#include <QTextEdit>
#include <QDateTime>
#include <QRegExp>
#include <QTextStream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProcess>
#include <QProgressBar>
#include <QDebug>
#include <QMessageBox>

#include "simmessage.h"
#include "main.h"
#include "module.h"
#include "qucs.h"
#include "textdoc.h"
#include "schematic.h"
#include "components/opt_sim.h"
#include "components/vhdlfile.h"
#include "misc.h"

#ifdef __MINGW32__
#define executableSuffix ".exe"
#else
#define executableSuffix ""
#endif

/*!
 * \brief Create a simulation messages dialog.
 *
 *  Create a new dialog to show the simulation steps progress and the
 *  simulator output messages
 */
SimMessage::SimMessage(QWidget *w, QWidget *parent)
		: QDialog(parent) 
{
  setWindowTitle(tr("Qucs Simulation Messages"));
  QucsDoc *Doc;
  DocWidget = w;
  if(DocWidget->inherits("QTextEdit"))
    Doc = (QucsDoc*) ((TextDoc*)DocWidget);
  else
    Doc = (QucsDoc*) ((Schematic*)DocWidget);

  DocName = Doc->DocName;
  DataDisplay = Doc->DataDisplay;
  Script = Doc->Script;
  QFileInfo Info(DocName);
  DataSet = QDir::convertSeparators(Info.path()) +
    QDir::separator() + Doc->DataSet;
  showBias = Doc->showBias;     // save some settings as the document...
  SimOpenDpl = Doc->SimOpenDpl; // ...could be closed during the simulation.
  SimRunScript = Doc->SimRunScript;

  all = new QVBoxLayout(this);
  all->setSpacing(5);
  all->setMargin(5);
  QGroupBox *Group1 = new QGroupBox(tr("Progress:"));
  all->addWidget(Group1);
  QVBoxLayout *vbox1 = new QVBoxLayout();
  Group1->setLayout(vbox1);

  ProgText = new QTextEdit();
  vbox1->addWidget(ProgText);
  ProgText->setAcceptRichText(false);
  ProgText->setReadOnly(true);
  //ProgText->setWordWrapMode(QTextOption::NoWrap);
  ProgText->setMinimumSize(400,80);
  wasLF = false;

  QGroupBox *HGroup = new QGroupBox();
  QHBoxLayout *hbox = new QHBoxLayout();
  //HGroup->setInsideMargin(5);
  //HGroup->setInsideSpacing(5);
  all->addWidget(HGroup);
  QLabel *progr = new QLabel(tr("Progress:"));
  hbox->addWidget(progr);
  SimProgress = new QProgressBar();
  hbox->addWidget(SimProgress);
//  SimProgress->setPercentageVisible(false);
  HGroup->setLayout(hbox);

  QGroupBox *Group2 = new QGroupBox(tr("Errors and Warnings:"));
  all->addWidget(Group2);
  QVBoxLayout *vbox2 = new QVBoxLayout();

  ErrText = new QTextEdit();
  vbox2->addWidget(ErrText);
  ErrText->setAcceptRichText(false);
  ErrText->setReadOnly(true);
  ErrText->setWordWrapMode(QTextOption::NoWrap);
  ErrText->setMinimumSize(400,80);
  Group2->setLayout(vbox2);

  QHBoxLayout *Butts = new QHBoxLayout();
  all->addLayout(Butts);

  Display = new QPushButton(tr("Goto display page"));
  Butts->addWidget(Display);
  Display->setDisabled(true);
  connect(Display,SIGNAL(clicked()),SLOT(slotDisplayButton()));

  Abort = new QPushButton(tr("Abort simulation"));
  Butts->addWidget(Abort);
  connect(Abort,SIGNAL(clicked()),SLOT(reject()));
  connect(Abort,SIGNAL(clicked()),SLOT(AbortSim()));
  connect(this,SIGNAL(rejected()),SLOT(AbortSim()));
}

/*!
 * \brief Delete the simulation messages dialog.
 *
 *  Delete the simulation messages dialog and kill the simulation process,
 *  if still running.
 */
SimMessage::~SimMessage()
{
  if(SimProcess.state()==QProcess::Running)  SimProcess.kill();
  delete all;
}

// ------------------------------------------------------------------------
bool SimMessage::startProcess()
{
  Abort->setText(tr("Abort simulation"));
  Display->setDisabled(true);

  QString txt = tr("Starting new simulation on %1 at %2").
    arg(QDate::currentDate().toString("ddd dd. MMM yyyy")).
    arg(QTime::currentTime().toString("hh:mm:ss:zzz"));
  ProgText->insertPlainText(txt + "\n\n");

  SimProcess.blockSignals(false);
 /* On Qt4 it shows as running even before we .start it. FIXME*/
  if(SimProcess.state()==QProcess::Running ||SimProcess.state()==QProcess::Starting) {
    qDebug() << "running!";
    ErrText->insertPlainText(tr("ERROR: Simulator is still running!\n"));
    FinishSimulation(-1);
    return false;
  }

  Collect.clear();  // clear list for NodeSets, SPICE components etc.
  ProgText->insertPlainText(tr("creating netlist... "));
  NetlistFile.setFileName(QucsSettings.QucsHomeDir.filePath("netlist.txt"));
   if(!NetlistFile.open(QIODevice::WriteOnly)) {
    ErrText->insertPlainText(tr("ERROR: Cannot write netlist file!\n"));
    FinishSimulation(-1);
    return false;
  }

  Stream.setDevice(&NetlistFile);

  if(!DocWidget->inherits("QTextEdit")) {
    SimPorts =
       ((Schematic*)DocWidget)->prepareNetlist(Stream, Collect, ErrText);
    if(SimPorts < -5) {
      NetlistFile.close();
      ErrText->insertPlainText(tr("ERROR: Cannot simulate a text file!\n"));
      FinishSimulation(-1);
      return false;
    }
  }
  Collect.append("*");   // mark the end


  disconnect(&SimProcess, 0, 0, 0);
  connect(&SimProcess, SIGNAL(readyReadStandardError()), SLOT(slotDisplayErr()));
  connect(&SimProcess, SIGNAL(readyReadStandardOutput()),
                       SLOT(slotReadSpiceNetlist()));
  connect(&SimProcess, SIGNAL(finished(int)),
                       SLOT(slotFinishSpiceNetlist(int)));

  nextSPICE();
  return true;
  // Since now, the Doc pointer may be obsolete, as the user could have
  // closed the schematic !!!
}

/*!
 * \brief Converts a spice netlist into Qucs format and outputs it.
 */
void SimMessage::nextSPICE()
{
  QString Line;
  for(;;) {  // search for next SPICE component
    Line = *(Collect.begin());
    Collect.erase(Collect.begin());
    if(Line == "*") {  // worked on all components ?
      startSimulator(); // <<<<<================== go on ===
      return;
    }
// FIXME #warning SPICE section below not being covered?
    qDebug() << "goin thru SPICE branch on simmmessage.cpp";
    if(Line.left(5) == "SPICE") {
      if(Line.at(5) != 'o') insertSim = true;
      else insertSim = false;
      break;
    }
    Collect.append(Line);
  }


  QString FileName = Line.section('"', 1,1);
  Line = Line.section('"', 2);  // port nodes
  if(Line.isEmpty())  makeSubcircuit = false;
  else  makeSubcircuit = true;

  QString prog;
  QStringList com;
  prog = QucsSettings.BinDir + "qucsconv" + executableSuffix;
  if(makeSubcircuit)
    com << "-g" << "_ref";
  com << "-if" << "spice" << "-of" << "qucs";

  QFile SpiceFile;
  if(FileName.indexOf(QDir::separator()) < 0)  // add path ?
    SpiceFile.setFileName(QucsSettings.QucsWorkDir.path() + QDir::separator() + FileName);
  else
    SpiceFile.setFileName(FileName);
  if(!SpiceFile.open(QIODevice::ReadOnly)) {
    ErrText->insertPlainText(tr("ERROR: Cannot open SPICE file \"%1\".\n").arg(FileName));
    FinishSimulation(-1);
    return;
  }

  if(makeSubcircuit) {
    Stream << "\n.Def:" << misc::properName(FileName) << " ";

    Line.replace(',', ' ');
    Stream << Line;
    if(!Line.isEmpty()) Stream << " _ref";
  }
  Stream << "\n";

  ProgressText = "";

  qDebug() << "start QucsConv" << prog << com.join(" ");
  SimProcess.start(prog, com);

  if(!SimProcess.Running) {
    ErrText->insertPlainText(tr("SIM ERROR: Cannot start QucsConv!\n"));
    FinishSimulation(-1);
    return;
  }

  QByteArray SpiceContent = SpiceFile.readAll();
  SpiceFile.close();
  QString command(SpiceContent); //to convert byte array to string
  SimProcess.setStandardInputFile(command);  //? FIXME works?
  qDebug() << command;
  connect(&SimProcess, SIGNAL(wroteToStdin()), SLOT(slotCloseStdin()));
}

// ------------------------------------------------------------------------
void SimMessage::slotCloseStdin()
{
  //SimProcess.closeStdin(); //? channel not available in Qt4?
  disconnect(&SimProcess, SIGNAL(wroteToStdin()), 0, 0);
}

// ------------------------------------------------------------------------
void SimMessage::slotReadSpiceNetlist()
{
  int i;
  QString s;
  ProgressText += QString(SimProcess.readAllStandardOutput());

  while((i = ProgressText.indexOf('\n')) >= 0) {

    s = ProgressText.left(i);
    ProgressText.remove(0, i+1);


    s = s.trimmed();
    if(s.isEmpty()) continue;
    if(s.at(0) == '#') continue;
    if(s.at(0) == '.') if(s.left(5) != ".Def:") { // insert simulations later
      if(insertSim) Collect.append(s);
      continue;
    }
    Stream << "  " << s << '\n';
  }
}

// ------------------------------------------------------------------------
void SimMessage::slotFinishSpiceNetlist(int status )
{
  Q_UNUSED(status);

  if(makeSubcircuit)
    Stream << ".Def:End\n\n";

  nextSPICE();
}

// ------------------------------------------------------------------------
#ifdef __MINGW32__
#include <windows.h>
static QString pathName(QString longpath) {
  const char * lpath = QDir::convertSeparators(longpath).ascii();
  char spath[2048];
  int len = GetShortPathNameA(lpath,spath,sizeof(spath)-1);
  spath[len] = '\0';
  return QString(spath);
}
#else
static QString pathName(QString longpath) {
  return longpath;
}
#endif


/*!
 * \brief SimMessage::startSimulator simulates the document in view.
 */
void SimMessage::startSimulator()
{
  // Using the Doc pointer here is risky as the user may have closed
  // the schematic, but converting the SPICE netlists is (hopefully)
  // faster than the user (I have no other idea).

  QString SimTime;
  QString Program;
  QStringList Arguments;
  QString SimPath = QDir::convertSeparators (QucsSettings.QucsHomeDir.absolutePath());
#ifdef __MINGW32__
  QString QucsDigiLib = "qucsdigilib.bat";
  QString QucsDigi = "qucsdigi.bat";
  QString QucsVeri = "qucsveri.bat";
#else
  QString QucsDigiLib = "qucsdigilib";
  QString QucsDigi = "qucsdigi";
  QString QucsVeri = "qucsveri";
#endif
  SimOpt = NULL;
  bool isVerilog = false;

  // Simulate text window.
  if(DocWidget->inherits("QTextEdit")) {

    TextDoc * Doc = (TextDoc*)DocWidget;

    // Take VHDL file in memory as it could contain unsaved changes.
    Stream << Doc->toPlainText();
    NetlistFile.close();
    ProgText->insertPlainText(tr("done.")+"\n");  // of "creating netlist...

    // Simulation.
    if (Doc->simulation) {
      SimTime = Doc->SimTime;
      QString libs = Doc->Libraries.toLower();
      /// \todo \bug error: unrecognized command line option '-Wl'
#ifdef __MINGW32__
      if(libs.isEmpty()) {
        libs = "";
      }
      else {
        libs.replace(" ",",-l");
        libs = "-Wl,-l" + libs;
      }
#else
      if(libs.isEmpty()) {
        libs = "-c";
      }
      else {
        libs.replace(" ",",-l");
        libs = "-c,-l" + libs;
      }
#endif
      Program = pathName(QucsSettings.BinDir + QucsDigi);
      Arguments  << QucsSettings.QucsHomeDir.filePath("netlist.txt")
                 << DataSet << SimTime << pathName(SimPath)
                 << pathName(QucsSettings.BinDir) << libs;
    }
    // Module.
    else {
      QString text = Doc->toPlainText();
      VHDL_File_Info VInfo (text);
      QString entity = VInfo.EntityName.toLower();
      QString lib = Doc->Library.toLower();
      if (lib.isEmpty()) lib = "work";
      QString dir = QDir::convertSeparators (QucsSettings.QucsHomeDir.path());
      QDir vhdlDir(dir);
      if(!vhdlDir.exists("vhdl"))
	if(!vhdlDir.mkdir("vhdl")) {
	  ErrText->insertPlainText(tr("ERROR: Cannot create VHDL directory \"%1\"!\n")
			  .arg(vhdlDir.path()+"/vhdl"));
	  return;
	}
      vhdlDir.setPath(vhdlDir.path()+"/vhdl");
      if(!vhdlDir.exists(lib))
	if(!vhdlDir.mkdir(lib)) {
	  ErrText->insertPlainText(tr("ERROR: Cannot create VHDL directory \"%1\"!\n")
			  .arg(vhdlDir.path()+"/"+lib));
	  return;
	}
      vhdlDir.setPath(vhdlDir.path()+"/"+lib);
      QFile destFile;
      destFile.setFileName(vhdlDir.filePath(entity+".vhdl"));
      if(!destFile.open(QIODevice::WriteOnly)) {
	ErrText->insertPlainText(tr("ERROR: Cannot create \"%1\"!\n")
			.arg(destFile.fileName()));
	return;
      }
      destFile.write(text.toAscii(), text.length());
      destFile.close();
      Program = pathName(QucsSettings.BinDir + QucsDigiLib);
      Arguments << QucsSettings.QucsHomeDir.filePath("netlist.txt")
                << pathName(SimPath)
                << entity
                << lib;
    }
  }
  // Simulate schematic window.
  else {
    // output NodeSets, SPICE simulations etc.
    for(QStringList::Iterator it = Collect.begin();
	it != Collect.end(); ++it) {
      // don't put library includes into netlist...
      if ((*it).right(4) != ".lst" &&
	  (*it).right(5) != ".vhdl" &&
	  (*it).right(4) != ".vhd" &&
	  (*it).right(2) != ".v") {
	Stream << *it << '\n';
      }
    }
    Stream << '\n';

    isVerilog = ((Schematic*)DocWidget)->isVerilog;
    SimTime = ((Schematic*)DocWidget)->createNetlist(Stream, SimPorts);
    if(SimTime.length()>0&&SimTime.at(0) == '\xA7') {
      NetlistFile.close();
      ErrText->insertPlainText(SimTime.mid(1));
      FinishSimulation(-1);
      return;
    }
    if (isVerilog) {
      Stream << "\n"
	     << "  initial begin\n"
	     << "    $dumpfile(\"digi.vcd\");\n"
	     << "    $dumpvars();\n"
	     << "    #" << SimTime << " $finish;\n"
	     << "  end\n\n"
	     << "endmodule // TestBench\n";
    }
    NetlistFile.close();
    ProgText->insertPlainText(tr("done.")+"\n");  // of "creating netlist...

    if(SimPorts < 0) {

      // append command arguments
      // append netlist with same arguments
      if (! Module::vaComponents.isEmpty()) {

          /*! Only pass modules to Qucsator that are indeed used on
           * the schematic,it might be the case that the user loaded the icons,
           * but did not compiled the module. Qucsator will not find the library.
           *
           * Check if used symbols have corresponing lib before running
           * Qucsator? Need to search on the netlis.txt? Is there other data
           * structure containig the netlist?
           *
          */
          QStringList usedComponents;

          if (!NetlistFile.open(QIODevice::ReadOnly))
             QMessageBox::critical(this, tr("Error"), tr("Cannot read netlist!"));
          else {
             QString net = QString(NetlistFile.readAll());

             QMapIterator<QString, QString> i(Module::vaComponents);
             while (i.hasNext()) {
                 i.next();
                 if (net.contains(i.key()))
                     usedComponents << i.key();
             }
             NetlistFile.close();
          }

          if (! usedComponents.isEmpty()) {


            /// \todo remvoe the command line arguments? use only netlist annotation?
            //Arguments << "-p" << QucsSettings.QucsWorkDir.absolutePath()
            //          << "-m" << usedComponents;
            //qDebug() << "Command :" << Program << Arguments.join(" ");

            /// Anotate netlist with Verilog-A dynamic path and module names
            ///
            if (!NetlistFile.open(QFile::Append | QFile::Text))
               QMessageBox::critical(this, tr("Error"), tr("Cannot read netlist!"));
            else {
               QTextStream out(&NetlistFile);
               out << "\n";
               out << "# --path=" << QucsSettings.QucsWorkDir.absolutePath() << "\n";
               out << "# --module=" << usedComponents.join(" ") << "\n";

               NetlistFile.close();
            }
          }
      } // vaComponents not empty

      if((SimOpt = findOptimization((Schematic*)DocWidget))) {
	    ((Optimize_Sim*)SimOpt)->createASCOnetlist();

        Program = QucsSettings.AscoBinDir.canonicalPath();
        Program = QDir::toNativeSeparators(Program+"/"+"asco"+QString(executableSuffix));
        Arguments << "-qucs" << QucsSettings.QucsHomeDir.filePath("asco_netlist.txt")
                  << "-o" << "asco_out";
      }
      else {
        Program = QucsSettings.BinDir + "qucsator" + executableSuffix;
        Arguments << "-b" << "-g" << "-i"
                  << QucsSettings.QucsHomeDir.filePath("netlist.txt")
                  << "-o" << DataSet;
      }
    }
    else {
      if (isVerilog) {
          Program = QDir::toNativeSeparators(QucsSettings.BinDir + QucsVeri);
          Arguments << QDir::toNativeSeparators(QucsSettings.QucsHomeDir.filePath("netlist.txt"))
                    << DataSet
                    << SimTime
                    << QDir::toNativeSeparators(SimPath)
                    << QDir::toNativeSeparators(QucsSettings.BinDir)
                    << "-c";
      } else {
/// \todo \bug error: unrecognized command line option '-Wl'
#ifdef __MINGW32__
    Program = QDir::toNativeSeparators(pathName(QucsSettings.BinDir + QucsDigi));
    Arguments << QDir::toNativeSeparators(QucsSettings.QucsHomeDir.filePath("netlist.txt"))
              << DataSet
              << SimTime
              << QDir::toNativeSeparators(SimPath)
              << QDir::toNativeSeparators(QucsSettings.BinDir) << "-Wall" << "-c";
#else
    Program = QDir::toNativeSeparators(pathName(QucsSettings.BinDir + QucsDigi));
    Arguments << QucsSettings.QucsHomeDir.filePath("netlist.txt")
              << DataSet << SimTime << pathName(SimPath)
		      << pathName(QucsSettings.BinDir) << "-Wall" << "-c";

#endif
      }
    }
  }

  disconnect(&SimProcess, 0, 0, 0);
  connect(&SimProcess, SIGNAL(readyReadStandardError()), SLOT(slotDisplayErr()));
  connect(&SimProcess, SIGNAL(readyReadStandardOutput()), SLOT(slotDisplayMsg()));
  connect(&SimProcess, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(slotSimEnded(int, QProcess::ExitStatus)));

#ifdef SPEEDUP_PROGRESSBAR
  waitForUpdate = false;
#endif
  wasLF = false;

  ProgressText = "";

#ifdef __MINGW32__
  QString sep(";"); // path separator
#else
  QString sep(":");
#endif

  // append process PATH
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  // insert Qucs bin dir, so ASCO can find qucsator
  env.insert("PATH", env.value("PATH") + sep + QucsSettings.BinDir );
  SimProcess.setProcessEnvironment(env);
  QFile file(Program);
  if ( !file.exists() ){
    ErrText->insertPlainText(tr("ERROR: Program not found: %1\n").arg(Program));
    FinishSimulation(-1);
    return;
  }
  else
    file.close();

  qDebug() << "Command :" << Program << Arguments.join(" ");
  SimProcess.start(Program, Arguments); // launch the program

  if(!SimProcess.Running) {
    ErrText->insertPlainText(tr("ERROR: Cannot start simulator!\n"));
    FinishSimulation(-1);
    return;
  }

}

// ------------------------------------------------------------------------
Component * SimMessage::findOptimization(Schematic *Doc) {
  Component *pc;
  for(pc=Doc->Components->first(); pc!=0; pc=Doc->Components->next())
    if(pc->isActive)
      if(pc->Model == ".Opt")
	return pc;
  return NULL;
}


/*!
 * \brief called when the process sends an output to stdout.
 */
void SimMessage::slotDisplayMsg()
{
  int i;
  ProgressText += QString(SimProcess.readAllStandardOutput());
  if(wasLF) {
    i = ProgressText.lastIndexOf('\r');
    if(i > 1) {
#ifdef SPEEDUP_PROGRESSBAR
      iProgress = 10*int(ProgressText.at(i-2).toLatin1()-'0') +
                     int(ProgressText.at(i-1).toLatin1()-'0');
      if(!waitForUpdate) {
        QTimer::singleShot(20, this, SLOT(slotUpdateProgressBar()));
        waitForUpdate = true;
      }
#else
      SimProgress->setMaximum(100);
      SimProgress->setValue(
         10*int(ProgressText.at(i-2).toLatin1()-'0') +
            int(ProgressText.at(i-1).toLatin1()-'0'));
#endif
      ProgressText.remove(0, i+1);
    }

    if(ProgressText.size()>0&&ProgressText.at(0).toLatin1() <= '\t')
      return;
  }
  else {
    i = ProgressText.indexOf('\t');
    if(i >= 0) {
      wasLF = true;
      ProgText->insertPlainText(ProgressText.left(i));
      ProgressText.remove(0, i+1);
      return;
    }
  }

  ProgText->insertPlainText(ProgressText);
  ProgressText = "";
  wasLF = false;
}

#ifdef SPEEDUP_PROGRESSBAR
// ------------------------------------------------------------------------
void SimMessage::slotUpdateProgressBar()
{
  SimProgress->setProgress(iProgress, 100);
  waitForUpdate = false;
}
#endif

/*! 
 * \brief Insert process stderr output in the Error Message output window.
 *
 *  Called when the process sends an output to stderr.
 */
void SimMessage::slotDisplayErr()
{
  QTextCursor cursor = ErrText->textCursor();
  cursor.movePosition(QTextCursor::End);
  ErrText->insertPlainText(QString(SimProcess.readAllStandardError()));
}

/*!
 * \brief Check the simulation process exit status.
 *
 *  Called when the simulation process terminates; inserts an error 
 *  message in the Error Message output window in case the simulation
 *  process does not exit normally.
 *  \param[in] exitCode exit code of the process
 *  \param[in] exitStatus exit status of the process
 *  \todo use a macro for the bugs report email (needs to define it for CMake)
 */
void SimMessage::slotSimEnded(int exitCode, QProcess::ExitStatus exitStatus )
{
  int stat;
  
  qDebug() << "SimMessage::slotSimEnded() : exitCode = " << exitCode << ", exitStatus = " << exitStatus;
  qDebug() << "SimMessage::slotSimEnded() : SimProcess.error() = " << SimProcess.error();
  
  if (exitStatus == QProcess::NormalExit) {
    stat = exitCode;
  } else {
    stat = -1;
    ErrText->insertPlainText(tr("ERROR: Simulator crashed!\n"));
    ErrText->insertPlainText(tr("Please report this error to qucs-bugs@lists.sourceforge.net\n"));
  }
  FinishSimulation(stat); // 0 = normal  | -1 = crash
}

/*!
 * \brief Add end-of-simulation messages and save the relevant data.
 *
 *  Called when the simulation ended with errors before starting the 
 *  simulator process.
 *  \param[in] Status exit status of the process (0 = normal, -1 = crash)
 */
void SimMessage::FinishSimulation(int Status)
{
  Abort->setText(tr("Close window"));
  Display->setDisabled(false);
  SimProgress->setValue(100);  // progress bar to 100%

  QDate d = QDate::currentDate();   // get date of today
  QTime t = QTime::currentTime();   // get time

  if(Status == 0) {
    QString txt = tr("Simulation ended on %1 at %2").
      arg(d.toString("ddd dd. MMM yyyy")).
      arg(t.toString("hh:mm:ss:zzz"));
    ProgText->insertPlainText("\n" + txt + "\n" + tr("Ready.") + "\n");
  }
  else {
    QString txt = tr("Errors occurred during simulation on %1 at %2").
      arg(d.toString("ddd dd. MMM yyyy")).
      arg(t.toString("hh:mm:ss:zzz"));
    ProgText->insertPlainText("\n" + txt + "\n" + tr("Aborted.") + "\n");
  }

  QFile file(QucsSettings.QucsHomeDir.filePath("log.txt"));  // save simulator messages
  if(file.open(QIODevice::WriteOnly)) {
    QTextStream stream(&file);
    stream << tr("Output:\n-------") << "\n\n";
    for(int z=0; z<ProgText->document()->blockCount(); z++)
      stream << ProgText->document()->findBlockByNumber(z).text() << "\n";
    stream << "\n\n\n" << tr("Errors:\n-------") << "\n\n";
    for(int z=0; z<ErrText->document()->blockCount(); z++)
      stream << ErrText->document()->findBlockByNumber(z).text() << "\n";
    file.close();
  }

  if(Status == 0) {
    if(SimOpt) { // save optimization data
      QFile ifile(QucsSettings.QucsHomeDir.filePath("asco_out.dat"));
      QFile ofile(DataSet);
      if(ifile.open(QIODevice::ReadOnly)) {
	if(ofile.open(QIODevice::WriteOnly)) {
	  QByteArray data = ifile.readAll();
	  ofile.write(data);
	  ofile.close();
	}
	ifile.close();
      }
      if(((Optimize_Sim*)SimOpt)->loadASCOout())
	((Schematic*)DocWidget)->setChanged(true,true);
    }
  }

  emit SimulationEnded(Status, this);
}

/*!
 * \brief Close and delete simulation dialog
 *
 * To call accept(), which is protected, from the outside.
 * Called from the main GUI.
 */
void SimMessage::slotClose()
{
  accept();
}

// ------------------------------------------------------------------------
void SimMessage::slotDisplayButton()
{
  emit displayDataPage(DocName, DataDisplay);
  accept();
}

void SimMessage::AbortSim()
{
    SimProcess.kill();
}
