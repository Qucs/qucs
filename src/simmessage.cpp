/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// Control simulator through "message" dialog

#include <stdlib.h>
#include <iostream>
#include <QLabel>
#include <QGroupBox>
#include <QTimer>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QDateTime>
#include <QRegExp>
#include <QTextStream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QDebug>
#include <QMessageBox>
#include <QTextBlock>

#include "simmessage.h"
#include "module.h"
#include "qucs_app.h"
#include "qucsdoc.h"
#include "misc.h"
#include "qucs_globals.h"
#include "docfmt.h"
/*--------------------------------------------------------------------------*/
namespace qucs {
/*--------------------------------------------------------------------------*/
SimMessage::SimMessage(Simulator *sim, Doc const* doc)
  : QDialog(), _simulator(sim), _doc(doc)
{
  setAttribute(Qt::WA_DeleteOnClose); // here?
  std::string _what = "DC"; // TODO
  setWindowTitle(tr("Qucs Simulation Messages"));
  assert(doc);

  DocName = doc->docName();
  DataDisplay = doc->DataDisplay;
  Script = doc->Script;
  QFileInfo Info(DocName);
  //DataSet = QDir::toNativeSeparators(Info.path()) +
  //  QDir::separator() + doc->DataSet;
  showBias = doc->showBias;     // save some settings as the document...
  SimOpenDpl = doc->SimOpenDpl; // ...could be closed during the simulation.
  SimRunScript = doc->SimRunScript;

  all = new QVBoxLayout(this);
  all->setSpacing(5);
  all->setMargin(5);
  QGroupBox *Group1 = new QGroupBox(tr("Progress:"));
  all->addWidget(Group1);
  QVBoxLayout *vbox1 = new QVBoxLayout();
  Group1->setLayout(vbox1);

  ProgText = new QPlainTextEdit();
  vbox1->addWidget(ProgText);
  ProgText->setReadOnly(true);
  //ProgText->setWordWrapMode(QTextOption::NoWrap);
  ProgText->setMinimumSize(400,80);
  wasLF = false;
  simKilled = false;

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

  ErrText = new QPlainTextEdit();
  vbox2->addWidget(ErrText);
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
  // Abort will close the window, which in turn will abort the simulation
  connect(Abort,SIGNAL(clicked()),SLOT(reject()));
  connect(this,SIGNAL(rejected()),SLOT(AbortSim()));
  connect(this, SIGNAL(finished(int)), SLOT(deleteLater));
}
/*--------------------------------------------------------------------------*/
SimMessage::~SimMessage()
{
  if(_simulator){
    _simulator->detachCtrl(this);
  }else{
  }
 // ~Doc?
//  if(SimProcess.state()==QProcess::Running) {
//    SimProcess.kill();
//  }else{
//  }
  delete all;
}
/*--------------------------------------------------------------------------*/
void SimMessage::startProcess(istream_t& cs)
{
  Abort->setText(tr("Abort simulation"));
  Display->setDisabled(true);

  QString txt = tr("Starting new simulation on %1 at %2").
    arg(QDate::currentDate().toString("ddd dd. MMM yyyy")).
    arg(QTime::currentTime().toString("hh:mm:ss:zzz"));
  ProgText->appendPlainText(txt + "\n");

#if 0
  SimProcess.blockSignals(false);
 /* On Qt4 it shows as running even before we .start it. FIXME*/
  if(SimProcess.state()==QProcess::Running ||SimProcess.state()==QProcess::Starting) { untested();
    qDebug() << "running";
    ErrText->appendPlainText(tr("ERROR: Simulator is still running"));
    FinishSimulation(-1);
    return false;
  }else{itested();
  }
#endif


#if 0
  auto Doc = dynamic_cast<QucsDoc const*>(DocWidget);

  if(App::isTextDocument(Doc)) { untested();
    incomplete();
    // throw(Error(" Cannot simulate a text file");
    return;
  }else{
  }
#endif


#if 0 //???
  if(SimPorts < -5) {
    _netlistFile.close();
    ErrText->appendPlainText(tr("ERROR: Cannot simulate a text file!"));
    FinishSimulation(-1);
    return false;
  }
#endif

  // Collect.append("*");   // mark the end??
  //
  // simulator->init(doc)??


//  nextSPICE(); /// ???
  assert(_simulator);
  // do_it?
  _simulator->attachCtrl(this);
  _simulator->do_it(cs, _doc->scope());

  // old comment (recheck)
  // Since now, the Doc pointer may be obsolete, as the user could have
  // closed the schematic !!!
} // startProcess

// void SimProcess::do_it(istream_t& what)
// {
//   _simulator->run(what, this);
// }

void SimMessage::do_it(istream_t cs, qucs::Doc& d)
{ untested();
//  _simMessage->start(); // really?

//  disconnect(&SimProcess, 0, 0, 0);
//  connect(&SimProcess, SIGNAL(readyReadStandardError()), SLOT(slotDisplayErr()));
//  connect(&SimProcess, SIGNAL(readyReadStandardOutput()),
//                       SLOT(slotReadSpiceNetlist()));
//  connect(&SimProcess, SIGNAL(finished(int)),
//                       SLOT(slotFinishSpiceNetlist(int)));

//  startSimulator();
  _simulator->do_it(cs, d.scope());
}

#if 0
// BUG: must create netlist in netlister.
void SimMessage::nextSPICE()
{ untested();

  incomplete();
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
  if(Line.isEmpty()){
    makeSubcircuit = false;
  } else{
    makeSubcircuit = true;
  }

  QString prog;
  QStringList com;
  prog = QucsSettings.Qucsconv;
  if(makeSubcircuit){
    com << "-g" << "_ref";
  }else{
  }
  com << "-if" << "spice" << "-of" << "qucs";

  QFile SpiceFile;
  if(FileName.indexOf(QDir::separator()) < 0){
    // add path ?
    SpiceFile.setFileName(QucsSettings.QucsWorkDir.path() + QDir::separator() + FileName);
  }else{
    SpiceFile.setFileName(FileName);
  }

  if(!SpiceFile.open(QIODevice::ReadOnly)) {
    ErrText->appendPlainText(tr("ERROR: Cannot open SPICE file \"%1\".").arg(FileName));
    FinishSimulation(-1);
    return;
  }else{
  }

  if(makeSubcircuit) {
    Stream << "\n.Def:" << misc::properName(FileName) << " ";

    Line.replace(',', ' ');
    Stream << Line;
    if(!Line.isEmpty()) Stream << " _ref";
  }else{
  }
  Stream << "\n";

  ProgressText = "";

  qDebug() << "start QucsConv" << prog << com.join(" ");
  SimProcess.start(prog, com);

  if(SimProcess.state() != QProcess::Running) {
    ErrText->appendPlainText(tr("SIM ERROR: Cannot start QucsConv!"));
    FinishSimulation(-1);
    return;
  }else{
  }

  QByteArray SpiceContent = SpiceFile.readAll();
  SpiceFile.close();
  QString command(SpiceContent); //to convert byte array to string
  SimProcess.setStandardInputFile(command);  //? FIXME works?
  qDebug() << command;
  connect(&SimProcess, SIGNAL(wroteToStdin()), SLOT(slotCloseStdin()));
}
/*--------------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------*/
void SimMessage::slotFinishSpiceNetlist(int)
{
  Q_UNUSED(status);

  if(makeSubcircuit){ untested();
    Stream << ".Def:End\n\n";
  }else{ untested();
  }

  nextSPICE();
}
#endif
/*--------------------------------------------------------------------------*/
// "run" simulator?
void SimMessage::startSimulator(std::string const&)
{
  incomplete();

    // wrong place.
//    isVerilog = ((SchematicDoc*)DocWidget)->isVerilog;

#if 0
  QString SimTime;
  QStringList Arguments;
  QString SimPath = QDir::toNativeSeparators(QucsSettings.QucsHomeDir.absolutePath());
  QString QucsDigiLib = "qucsdigilib" BATCH_FILE_SUFFIX;
  QString QucsDigi = "qucsdigi" BATCH_FILE_SUFFIX;
  QString QucsVeri = "qucsveri" BATCH_FILE_SUFFIX;
  SimOpt = NULL;
  bool isVerilog = false;
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

  if(TextDoc const* Doc=dynamic_cast<TextDoc const*>(DocWidget)){ untested();
    incomplete();

    // Take VHDL file in memory as it could contain unsaved changes.
    // Stream << Doc->toPlainText();
    _netlistFile.close();
    ProgText->insertPlainText(tr("done.\n"));  // of "creating netlist...

    if (Doc->simulation) {
      SimTime = Doc->SimTime;

      QString libs = Doc->Libraries.toLower();
#ifdef __MINGW32__ // -> platform.h
      if(libs.isEmpty()) {
        libs = "";
      } else {
        libs.replace(" ",",-l");
        libs = "-Wl,-l" + libs;
      }
#else
      if(libs.isEmpty()) {
        libs = "-c";
      } else {
        libs.replace(" ",",-l");
        libs = "-c,-l" + libs;
      }
#endif

      Program = pathName(QucsSettings.BinDir + QucsDigi);
      Arguments  << QucsSettings.QucsHomeDir.filePath("netlist.txt")
                 << DataSet << SimTime << pathName(SimPath)
                 << pathName(QucsSettings.BinDir) << libs;
    }else{ // no Doc->simulation, but textDoc
      incomplete();
#if 0
      QString text = Doc->toPlainText();
      VHDL_File_Info VInfo (text);
      QString entity = VInfo.EntityName.toLower();
      QString lib = Doc->Library.toLower();
      if (lib.isEmpty()) lib = "work";
      QString dir = QDir::toNativeSeparators(QucsSettings.QucsHomeDir.path());
      QDir vhdlDir(dir);
      if(!vhdlDir.exists("vhdl"))
	if(!vhdlDir.mkdir("vhdl")) {
	  ErrText->appendPlainText(tr("ERROR: Cannot create VHDL directory \"%1\"!")
			  .arg(vhdlDir.path()+"/vhdl"));
	  return;
	}
      vhdlDir.setPath(vhdlDir.path()+"/vhdl");
      if(!vhdlDir.exists(lib))
	if(!vhdlDir.mkdir(lib)) {
	  ErrText->appendPlainText(tr("ERROR: Cannot create VHDL directory \"%1\"!")
			  .arg(vhdlDir.path()+"/"+lib));
	  return;
	}
      vhdlDir.setPath(vhdlDir.path()+"/"+lib);
      QFile destFile;
      destFile.setFileName(vhdlDir.filePath(entity+".vhdl"));
      if(!destFile.open(QIODevice::WriteOnly)) {
	ErrText->appendPlainText(tr("ERROR: Cannot create \"%1\"!")
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
#endif
    }
  }else if(SchematicDoc const* d=dynamic_cast<SchematicDoc const*>(DocWidget)){itested();
    incomplete();

    assert(sim); //for now.
    auto dl = sim->netLister();
    assert(dl);
    DocumentFormat const* n = prechecked_cast<DocumentFormat const*>(dl);
    assert(n);

    {
      //
      //
    }

    if(SimTime.length()>0&&SimTime.at(0) == '\xA7') {
    //  _netlistFile.close();
      ErrText->insertPlainText(SimTime.mid(1));
      FinishSimulation(-1);
      return;
    }
#if 0
    if (isVerilog) {
      Stream << "\n"
	     << "  initial begin\n"
	     << "    $dumpfile(\"digi.vcd\");\n"
	     << "    $dumpvars();\n"
	     << "    #" << SimTime << " $finish;\n"
	     << "  end\n\n"
	     << "endmodule // TestBench\n";
    }
#endif
    _netlistFile.close();
    ProgText->insertPlainText(tr("done.\n"));  // of "creating netlist...

    if(1 || SimPorts < 0) {

      // append command arguments
      // append netlist with same arguments
      if (Module::vaComponents.isEmpty()) {
      }else if(0){ untested();
        // ?
        // BUG: use netlist format
        // BUG: parsing netlist.txt here??
        incomplete();

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

          if (!_netlistFile.open(QIODevice::ReadOnly)) {
             QMessageBox::critical(this, tr("Error"), tr("Cannot read netlist!"));
          }else{
             QString net = QString(_netlistFile.readAll());

             QMapIterator<QString, QString> i(Module::vaComponents);
             while (i.hasNext()) {
                 i.next();
                 if (net.contains(i.key()))
                     usedComponents << i.key();
             }
             _netlistFile.close();
          }

          if (! usedComponents.isEmpty()) {
            /// \todo remvoe the command line arguments? use only netlist annotation?
            //Arguments << "-p" << QucsSettings.QucsWorkDir.absolutePath()
            //          << "-m" << usedComponents;
            //qDebug() << "taskElement :" << Program << Arguments.join(" ");

            /// Anotate netlist with Verilog-A dynamic path and module names
            ///
            if (!_netlistFile.open(QFile::Append | QFile::Text)){
               QMessageBox::critical(this, tr("Error"), tr("Cannot read netlist!"));
            }else{
               QTextStream out(&_netlistFile);
               out << "\n";
               out << "# --path=" << QucsSettings.QucsWorkDir.absolutePath() << "\n";
               out << "# --module=" << usedComponents.join(" ") << "\n";

               _netlistFile.close();
            }
          }
      } // vaComponents not empty

      {
        Program = QucsSettings.Qucsator;
        Arguments << "-b" << "-g" << "-i"
                  << QucsSettings.QucsHomeDir.filePath("netlist.txt")
                  << "-o" << DataSet;
      }
    } else {
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
#ifdef __MINGW32__ // -> platform.h
    Program = QDir::toNativeSeparators(pathName(QucsSettings.BinDir + QucsDigi));
    Arguments << QDir::toNativeSeparators(QucsSettings.QucsHomeDir.filePath("netlist.txt"))
              << DataSet
              << SimTime
              << QDir::toNativeSeparators(SimPath)
              << QDir::toNativeSeparators(QucsSettings.BinDir) << "-Wall" << "-c";
#else
    Program = QDir::toNativeSeparators(pathName(QucsSettings.BinDir + QucsDigi));
    Arguments << QucsSettings.QucsHomeDir.filePath("netlist.txt")
              << DataSet
              << SimTime
              << pathName(SimPath)
	      << pathName(QucsSettings.BinDir) << "-Wall" << "-c";

#endif
      }
    }
  }else{
    incomplete();
//    throw ..
  }

#if 0
  disconnect(&SimProcess, 0, 0, 0);
  connect(&SimProcess, SIGNAL(readyReadStandardError()), SLOT(slotDisplayErr()));
  connect(&SimProcess, SIGNAL(readyReadStandardOutput()), SLOT(slotDisplayMsg()));
  connect(&SimProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
                       SLOT(slotSimEnded(int, QProcess::ExitStatus)));
  connect(&SimProcess, SIGNAL(stateChanged(QProcess::ProcessState)),
                       SLOT(slotStateChanged(QProcess::ProcessState)));
#endif

#ifdef SPEEDUP_PROGRESSBAR
  waitForUpdate = false;
#endif
  wasLF = false;

  ProgressText = "";

  QString sep(PATHSEP);
  SimProcess.setProcessEnvironment(env);

  qDebug() << "taskElement :" << Program << Arguments.join(" ");
  SimProcess.start(Program, Arguments); // launch the program

  // delete sim;
#endif
} // SimMessage::startSimulator

// ------------------------------------------------------------------------


/*!
 * \brief called when the process sends an output to stdout.
 */
void SimMessage::slotDisplayMsg(std::string& s)
{
  int i;
  ProgressText += QString::fromStdString(s); // (SimProcess.readAllStandardOutput());
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
  }else{
    i = ProgressText.indexOf('\t'); // marker for progress indicator
    if(i >= 0) {
      wasLF = true;
      QString tmps = ProgressText.left(i).trimmed();
      if (!tmps.isEmpty()) // avoid adding a newline if no text to show
	ProgText->appendPlainText(tmps);
      ProgressText.remove(0, i+1);
      return;
    }
  }

  QString tmps = ProgressText.trimmed();
  if (!tmps.isEmpty()) // avoid adding a newline if no text to show
    ProgText->appendPlainText(tmps);
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
void SimMessage::slotDisplayErr(std::string& e)
{
  ErrText->appendPlainText(QString::fromStdString(e));
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
void SimMessage::slotSimEnded()
{
  assert(_simulator);
  int status = _simulator->state();
  if (status < 0 && !simKilled) {
    // as when killed by user exitStatus will be QProcess::CrashExit
    ErrText->appendPlainText(tr("ERROR: Simulator crashed!"));
    // ErrText->appendPlainText(tr("Please report this error to qucs-bugs@lists.sourceforge.net"));
  }
  FinishSimulation(); // 0 = normal , !=0 = error
}

/*!
 * \brief Add end-of-simulation messages and save the relevant data.
 *
 *  Called when the simulation ended with errors before starting the 
 *  simulator process.
 *  \param[in] Status exit status of the process (0 = normal, !=0 = error)
 */
void SimMessage::FinishSimulation()
{
  assert(_simulator);
  int status = _simulator->state();
  Abort->setText(tr("Close window"));
  Display->setDisabled(false);
  SimProgress->setValue(100);  // progress bar to 100%

  QDate d = QDate::currentDate();   // get date of today
  QTime t = QTime::currentTime();   // get time

  if(status == 0) {
    QString txt = tr("Simulation ended on %1 at %2").
      arg(d.toString("ddd dd. MMM yyyy")).
      arg(t.toString("hh:mm:ss:zzz"));
    ProgText->appendPlainText("\n" + txt + "\n" + tr("Ready."));
  }else{
    QString txt = tr("Errors occurred during simulation on %1 at %2").
      arg(d.toString("ddd dd. MMM yyyy")).
      arg(t.toString("hh:mm:ss:zzz"));
    ProgText->appendPlainText("\n" + txt + "\n" + tr("Aborted."));
  }

  // save simulator messages
  QDir dir(QString::fromStdString(QucsSettings.homeDir()));
  QFile file(dir.filePath("log.txt"));
  if(file.open(QIODevice::WriteOnly)) {
    QTextStream stream(&file);
    stream << tr("Output:\n-------") << "\n\n";
    for(int z=0; z<ProgText->document()->blockCount(); z++)
      stream << ProgText->document()->findBlockByNumber(z).text() << "\n";
    stream << "\n\n\n" << 
      tr("Errors and Warnings:\n--------------------") << "\n\n";
    for(int z=0; z<ErrText->document()->blockCount(); z++)
      stream << ErrText->document()->findBlockByNumber(z).text() << "\n";
    file.close();
  }

  if(status){
  }else if(SimOpt) { // save optimization data
#if 0
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
      if(((Optimize_Sim*)SimOpt)->loadASCOout()){
        ((SchematicDoc*)DocWidget)->setChanged(true,true);
      }
#endif
  }else{
  }

  emit SimulationEnded(status, this /*???*/);
} // FinishSimulation
/*--------------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------*/
void SimMessage::slotDisplayButton()
{
  emit displayDataPage(DocName, DataDisplay);
  accept();
}
/*--------------------------------------------------------------------------*/
void SimMessage::AbortSim()
{
  ErrText->appendPlainText(tr("Simulation aborted by the user"));
  simKilled = true;
  assert(_simulator);
  _simulator->kill();
}
/*--------------------------------------------------------------------------*/
void SimMessage::stateChange()
{
  assert(_simulator);
  int state = _simulator->state();

  if(state == Simulator::sst_error){ untested();
    ErrText->appendPlainText(tr("ERROR"));
  }else if(state == Simulator::sst_idle){
    slotSimEnded();
  }else if(state == Simulator::sst_running){
    ProgText->appendPlainText(tr("running..."));
  }else{
    trace1("stateChange", state);
    incomplete();
  }
}
/*--------------------------------------------------------------------------*/
static const int loglevel = qucs::MsgLog;
void SimMessage::message(int level, std::string msg)
{
  if(level==loglevel){
    ProgText->appendPlainText(QString::fromStdString(msg));
  }else{
    ErrText->appendPlainText(QString::fromStdString(msg));
  }
}
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:et
