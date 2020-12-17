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

// QUCS (the GUI)

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <iostream>

#include <stdlib.h>
#include <ctype.h>
#include <locale.h>

#include <QApplication>
#include <QString>
#include <QStringList>
#include <QTextCodec>
#include <QTranslator>
#include <QFile>
#include <QMessageBox>
#include <QRegExp>
#include <QtSvg>
#include <QDebug>

#include "qucs.h"
#include "docfmt.h"
#include "printerwriter.h"
#include "imagewriter.h"
#include "schematic_lang.h"
#include "simulator.h"

#include "module.h"
#include "misc.h"
#include "exception.h"
#include "io_error.h"
#include "globals.h"

#ifdef _WIN32
#include <Windows.h>  //for OutputDebugString
#endif

static const std::string default_simulator="qucsator"; // FIXME: get from rc? maybe from environment?

// temporary stuff.
namespace OS{
  inline std::string getenv(const std::string& s) {
    char* ev = ::getenv(s.c_str());
    if (ev) {
      return ev;
    }else{itested();
      return "";
    }
  }
  inline void setenv(const std::string& Name, const std::string& Value, bool Overwrite=true) {
    if (Name == "") {untested();
      throw Exception("setenv: bad name " + Name);
    }else if (Value == "") {untested();
      throw Exception("setenv: bad value " + Value);
    }else if (!Overwrite && getenv(Name) != "") {
      error(bDEBUG, "setenv: " + Name + " overwrite prohibited");
    }else{
      std::string ev = Name + "=" + Value;
      char *es = ::strdup(ev.c_str());	//BUG// memory leak
      assert(es);
      if (::putenv(es) != 0) {untested();
	throw Exception("");
      }else{
      }
    }
  }
}

void setSimulator(char const* name)
{ untested();
  Simulator const* S = simulator_dispatcher[name];
  if(S){ untested();
    QucsSettings.setSimulator(S);
  }else{ untested();
    // message(5, "cannot find simulator " + std::string(name));
    std::cerr << "cannot find simulator " << std::string(name) << "\n";
    exit(1);
  }
}

/*!
 * \brief qucsMessageOutput handles qDebug, qWarning, qCritical, qFatal.
 * \param type Message type (Qt enum)
 * \param msg Message
 *
 * The message handler is used to get control of the messages.
 * Particulary on Windows, as the messages are sent to the debugger and do not
 * show on the terminal. The handler could also be extended to create a log
 * mechanism.
 * <http://qt-project.org/doc/qt-4.8/debug.html#warning-and-debugging-messages>
 * <http://qt-project.org/doc/qt-4.8/qtglobal.html#qInstallMsgHandler>
 */
void qucsMessageOutput(QtMsgType type, const char *msg)
{
  switch (type) {
  case QtDebugMsg:
    fprintf(stderr, "Debug: %s\n", msg);
    break;
  case QtWarningMsg:
    fprintf(stderr, "Warning: %s\n", msg);
    break;
  case QtCriticalMsg:
    fprintf(stderr, "Critical: %s\n", msg);
    break;
  case QtFatalMsg:
    fprintf(stderr, "Fatal: %s\n", msg);
    abort();
  default:
    fprintf(stderr, "Default %s\n", msg);
  }

#ifdef _WIN32
  OutputDebugStringA(msg);
#endif
}


// moved to legacy/qucsator, QucsatorNetlister::save
void doNetlist(QString schematic_fn, std::string netlist, DocumentFormat& fmt)
{
  std::string sfn = schematic_fn.toStdString();
  QucsDoc d(nullptr, "", nullptr);
  d.setLabel("main");

  Symbol* root = symbol_dispatcher.clone("schematic_root");
  assert(root);
  root->setParameter("$filename", sfn);
  root->setOwner(&d);
  root->setLabel(sfn);
  assert(root->subckt()); // BUG

  Symbol& xs = *root;
  SchematicModel& sch = *root->subckt();
  trace1("main schematic model", &sch);
  sch.setFileInfo(schematic_fn);
  QFile file(schematic_fn);  // save simulator messages
  file.open(QIODevice::ReadOnly);
  istream_t stream (&file);
  DocumentFormat const* L = nullptr;

  if(!L){
    auto D = command_dispatcher["leg_sch"];
    L = dynamic_cast<DocumentFormat const*>(D);
  }else{ untested();
  }
  assert(L);

#if 0 // TODO
  CMD::command("get", xs->subckt);?
  => L->do_it(stream, document);
#else
//  L->load(stream, &xs);
  {
    auto l=doclang_dispatcher["leg_sch"];
    assert(l);
    auto L=dynamic_cast<SchematicLanguage const*>(l);
    assert(L);

    stream.read_line();
    while(!stream.atEnd()){
      L->new__instance(stream, root, &sch);
      stream.read_line();
    }
  }
  sch.setOwner(root); // needed?
#endif

//  QFile NetlistFile(QString::fromStdString(netlist));
//  if(!NetlistFile.open(QIODevice::WriteOnly | QFile::Truncate)) { untested();
//    fprintf(stderr, "Error: Could write to %s\n", netlist.c_str());
//    exit(1);
//  }else{
//  }
//  DocumentStream os(&NetlistFile);


#if 1
//  xs.DocName = schematic_fn.toStdString(); // tmp
#else
  xs.setParam("$filename", schematic_fn.toStdString());
#endif
  istream_t cs(istream_t::_STRING, netlist);
//  fmt.save(os, &xs);
  fmt.do_it(cs, xs.subckt());
}
/*--------------------------------------------------------------------------*/
void attach_single(std::string const& what)
{
  CMD::command(std::string("attach ") + what, nullptr);
}
/*--------------------------------------------------------------------------*/
void attach_default_plugins()
{
  attach_single("legacy");
//  attach_single("legacy-misc");
  attach_single("qucsator");

  attach_single("legacy-components");
  attach_single("paintings");
  attach_single("legacylib");
  attach_single("misc");
 // attach_single(pp, "libdialogs" SOEXT);
 //
 //
 // not yet. legacy diagrams are now part of legacy (above)
 // new diagrams: load manually.
  // attach_single(pp, "diagrams" SOEXT);
}
/*--------------------------------------------------------------------------*/
void qucsMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & str)
{
  auto msg=str.toStdString();
  qucsMessageOutput(type, msg.c_str());
}
/*--------------------------------------------------------------------------*/
static void prepare_env()
{
  static const char* plugpath="PLUGPATH=" QUCS_PLUGPATH
                              "\0         (reserved space)                 ";

  std::string ldlpath = OS::getenv("LD_LIBRARY_PATH");
  if (ldlpath != "") {
    ldlpath += ":";
  }else{
  }
  assert(strlen("PLUGPATH=") == 9);
  OS::setenv("QUCS_PLUGPATH", ldlpath + (plugpath+9), false);
}
/*--------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
  prepare_env();
  qInstallMsgHandler(qucsMessageHandler);
  QucsVersion = VersionTriplet(PACKAGE_VERSION);

  QFile qf(":/bitmaps/doesnotexist.png");
  assert(!qf.exists());
  QFile qfl(":/bitmaps/line.png");
  assert(qfl.exists());

  loadSettings();

  // continue to set up overrides or default settings (some are saved on exit)

  // check for relocation env variable
  char* var = getenv("QUCSDIR");
  QDir QucsDir(QUCS_PREFIX);
  if (var!= NULL) { untested();
      QucsDir = QDir(QString(var));
      qDebug() << "QUCSDIR set: " << QucsDir.absolutePath();
  }else{
  }

  QucsSettings.BinDir =      QucsDir.absolutePath() + "/bin/";
  QucsSettings.LangDir =     QucsDir.canonicalPath() + "/share/qucs/lang/";

  // BUG BUG BUG. not here
  var = getenv("QUCS_LIBRARY");
  if(var != NULL) {
    trace2("QUCS_LIBRARY", var, QucsDir.canonicalPath());
    QucsSettings.setLibDir(QString(var));
  }else{
    trace0("no QUCS_LIBRARY");
    QucsSettings.setLibDir(QucsDir.canonicalPath() + "/share/qucs/library/");
  }
  QucsSettings.OctaveDir =   QucsDir.canonicalPath() + "/share/qucs/octave/";
  QucsSettings.ExamplesDir = QucsDir.canonicalPath() + "/share/qucs/examples/";
  QucsSettings.DocDir =      QucsDir.canonicalPath() + "/share/qucs/docs/";

  attach_default_plugins();

  /// \todo Make the setting up of all executables below more consistent
  var = getenv("QUCSATOR");
  if(var != NULL) {
      QucsSettings.Qucsator = QString(var);
  } else { untested();
      QucsSettings.Qucsator = QucsSettings.BinDir + "qucsator" + executableSuffix;
  }

  var = getenv("QUCSCONV");
  if(var != NULL) {
      QucsSettings.Qucsconv = QString(var);
  }else{ untested();
      QucsSettings.Qucsconv = QucsSettings.BinDir + "qucsconv" + executableSuffix;
  }
  QFile file(QucsSettings.Qucsconv);
  if(!file.exists())
      qWarning() << "QucsConv not found: " << QucsSettings.Qucsconv;


  var = getenv("ADMSXMLBINDIR");
  if(var != NULL) { untested();
      QucsSettings.AdmsXmlBinDir.setPath(QString(var));
  }else{
      // default admsXml bindir same as Qucs
      QString admsExec;
      admsExec = QDir::toNativeSeparators(QucsSettings.BinDir+"/admsXml" executableSuffix);
      QFile adms(admsExec);
      if(adms.exists()){ untested();
        QucsSettings.AdmsXmlBinDir.setPath(QucsSettings.BinDir);
      }else{
      }
  }

  var = getenv("ASCOBINDIR");
  if(var != NULL)  { untested();
      QucsSettings.AscoBinDir.setPath(QString(var));
  }else{
      // default ASCO bindir same as Qucs
      QString ascoExec;
      ascoExec = QDir::toNativeSeparators(QucsSettings.BinDir+"/asco" executableSuffix);
      QFile asco(ascoExec);
      if(asco.exists()){ untested();
        QucsSettings.AscoBinDir.setPath(QucsSettings.BinDir);
      }else{
      }
  }


  var = getenv("QUCS_OCTAVE");
  if (var != NULL) { untested();
      QucsSettings.QucsOctave = QString(var);
  } else {
      QucsSettings.QucsOctave.clear();
  }

  if(!QucsSettings.BGColor.isValid())
    QucsSettings.BGColor.setRgb(255, 250, 225);


  // syntax highlighting
  if(!QucsSettings.Comment.isValid())
    QucsSettings.Comment = Qt::gray;
  if(!QucsSettings.String.isValid())
    QucsSettings.String = Qt::red;
  if(!QucsSettings.Integer.isValid())
    QucsSettings.Integer = Qt::blue;
  if(!QucsSettings.Real.isValid())
    QucsSettings.Real = Qt::darkMagenta;
  if(!QucsSettings.Character.isValid())
    QucsSettings.Character = Qt::magenta;
  if(!QucsSettings.Type.isValid())
    QucsSettings.Type = Qt::darkRed;
  if(!QucsSettings.Attribute.isValid())
    QucsSettings.Attribute = Qt::darkCyan;
  if(!QucsSettings.Directive.isValid())
    QucsSettings.Directive = Qt::darkCyan;
  if(!QucsSettings.Task.isValid())
    QucsSettings.Task = Qt::darkRed;



  // set codecs
  QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

  QTranslator tor( 0 );
  QString lang = QucsSettings.Language;
  if(lang.isEmpty()) {
    QLocale loc;
    lang = loc.name();
  }else{ untested();
  }
  tor.load( QString("qucs_") + lang, QucsSettings.LangDir);

  //{ untested();
  //}

  // This seems to be neccessary on a few system to make strtod()
  // work properly !???!
  setlocale (LC_NUMERIC, "C");

  QString inputfile;
  std::string outputfile;

  bool dump_flag = false;
  bool print_flag = false;
  QString page = "A4";
  int dpi = 96;
  QString color = "RGB";
  QString orientation = "portrait";
  std::string netlang_name = default_simulator;

  // parse_args(argc, argv);
  for (int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) { untested();
      fprintf(stdout,
  "Usage: %s [COMMAND] [OPTIONS]\n\n"
  "Commands:\n"
  "  -h, --help     display this help and exit\n"
  "  -v, --version  display version information and exit\n"
  "  -n, --netlist  obsolete. use --dump\n"
  "  -d, --dump     dump schematic, requires -i, -o\n"
  "  -p, --print    print Qucs schematic to file, requires -i, -o (eps needs inkscape)\n"
  "  -q, --quit     exit\n"
  "    --page [A4|A3|B4|B5]         set print page size (default A4)\n"
  "    --dpi NUMBER                 set dpi value (default 96)\n"
  "    --color [RGB|RGB]            set color mode (default RGB)\n"
  "    --orin [portrait|landscape]  set orientation (default portrait)\n"
  "  -i FILENAME    use file as input schematic\n"
  "  -o FILENAME    write output to filename\n"
  "  -icons         create component icons under ./bitmaps_generated\n"
  "  -doc           dump data for documentation:\n"
  "                 * file with of categories: categories.txt\n"
  "                 * one directory per category (e.g. ./lumped components/)\n"
  "                   - CSV file with component data ([comp#]_data.csv)\n"
  "                   - CSV file with component properties. ([comp#]_props.csv)\n"
  "  --list-entries list component entry formats for schematic and netlist\n"
  "Options:\n"
  "  -i FILENAME    use file as input schematic\n"
  "  -o FILENAME    use file as output netlist\n"
  "  -l DOCLANG     language to be used by dump, can be a simulator name\n"
  "  -s SIMULATOR   choose a simulator\n"
  "  -r FORMAT      run simulation\n"
  "    --page [A4|A3|B4|B5]         set print page size (default A4)\n"
  "    --dpi NUMBER                 set dpi value (default 96)\n"
  "    --color [RGB|RGB]            set color mode (default RGB)\n"
  "    --orin [portrait|landscape]  set orientation (default portrait)\n"
  ,argv[0]);
      return 0;
    }else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")) { untested();
#ifdef GIT
      fprintf(stdout, "Qucs " PACKAGE_VERSION " (" GIT ")" "\n");
#else
      fprintf(stdout, "Qucs " PACKAGE_VERSION "\n");
#endif
      return 0;
    }else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--dump")) { untested();
      dump_flag = true;
    }else if (!strcmp(argv[i], "-n") || !strcmp(argv[i], "--netlist")) {
      dump_flag = true;
    }else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--print")) { untested();
      print_flag = true;
    }else if (!strcmp(argv[i], "--page")) { untested();
      page = argv[++i];
    }else if (!strcmp(argv[i], "--dpi")) { untested();
      dpi = QString(argv[++i]).toInt();
    }else if (!strcmp(argv[i], "--color")) { untested();
      color = argv[++i];
    }else if (!strcmp(argv[i], "--orin")) { untested();
      orientation = argv[++i];
    }else if (!strcmp(argv[i], "-a")) {
      ++i;
      std::string what = argv[i];
      trace2("attaching", argv[i], what);
      attach_single(what);
    }else if(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quit")) {
	exit(0);
    }else if (!strcmp(argv[i], "-i")) {
      inputfile = argv[++i];
    }else if (!strcmp(argv[i], "-o")) {
      outputfile = argv[++i];
    }else if (!strcmp(argv[i], "-l")) {
      netlang_name = argv[++i];
    }else if (!strcmp(argv[i], "-s")) {
      setSimulator(argv[++i]);
    }else if(!strcmp(argv[i], "-icons")) { untested();
      CMD::command(std::string("createicons "), nullptr);
      return 0;
    }else if(!strcmp(argv[i], "-doc")) { untested();
      CMD::command(std::string("createdocdata "), nullptr);
      return 0;
    }else if(!strcmp(argv[i], "-list-entries")) {
      incomplete(); // don't use.
      CMD::command(std::string("listcompentry "), nullptr);
      return 0;
    }else if(!strcmp(argv[i], "--list-entries")) {
      CMD::command(std::string("listcompentry "), nullptr);
      return 0;
    }else{
      fprintf(stderr, "Error: Unknown option: %s\n", argv[i]);
      return -1;
    }
  }

  Command const* cmd = command_dispatcher[netlang_name];
  assert(cmd);
  auto fmt=prechecked_cast<DocumentFormat const*>(cmd);

  if(fmt){
    // just use it.
  }else if(simulator_dispatcher[netlang_name]){ untested();
    incomplete();
    // ask a simulator.
//    fmt = sd->netLang();
  }else{
  }

  int result = 0;

  if(!fmt){ untested();
    // TODO: io_error...
    std::cerr << "Error: Cannot find language for "
              << netlang_name << "\n";
    result = -1;
  }else if (dump_flag and print_flag) { untested();
    fprintf(stderr, "Error: --print and --netlist cannot be used together\n");
    result = -1;
  } else if (dump_flag or print_flag) {
    if (inputfile.isEmpty()) { untested();
      fprintf(stderr, "Error: Expected input file.\n");
      result = -1;
    }else if (outputfile == "") { untested();
      fprintf(stderr, "Error: Expected output file.\n");
      result = -1;
    }else if (dump_flag) {
      auto cmd = command_dispatcher[netlang_name];
      auto fmt = prechecked_cast<DocumentFormat*>(cmd);
      if(!fmt){ untested();
	qDebug() << "no lang" << QString::fromStdString(netlang_name);
	incomplete();
	result = 1;
      }else{
	doNetlist(inputfile, outputfile, *fmt);
	result = 0;
      }
    } else if (print_flag) { untested();
      incomplete();
      CMD::command(std::string("doprint "), nullptr);
//      return doPrint(inputfile, outputfile,
//          page, dpi, color, orientation);
    }else{
    }
  }else{
    //{ untested();
    QApplication a(argc, argv);
  //  Q_INIT_RESOURCE();
    QDesktopWidget *d = a.desktop();
    int w = d->width();
    int h = d->height();
    QucsSettings.x = w/8;
    QucsSettings.y = h/8;
    QucsSettings.dx = w*3/4;
    QucsSettings.dy = h*3/4;
    a.setFont(QucsSettings.font);
    a.installTranslator( &tor );
    //}

    Module::registerModules (); // BUG
    QucsMain = new QucsApp();

    QucsMain->show();
    result = a.exec();
    //saveApplSettings(QucsMain);
  }
  return result;
}
// vim:ts=8:sw=2:noet
