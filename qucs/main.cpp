/***************************************************************************
                                 main.cpp
                                ----------
    begin                : Thu Aug 28 2003
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
 * \file main.cpp
 * \brief Implementation of the main application.
 */

#include "qucs_functions.h"

// void attach(const char*); not yet.

/*!
 * \brief qucsMessageOutput handles qDebug, qWarning, qCritical, qFatal.
 * \param type Message type (Qt enum)
 * \param msg Message
 *
 * The message handler is used to get control of the messages.
 * Particulary on Windows, as the messages are sent to the debugger and do not
 * show on the terminal. The handler could aslo be extended to create a log
 * mechanism.
 * <http://qt-project.org/doc/qt-4.8/debug.html#warning-and-debugging-messages>
 * <http://qt-project.org/doc/qt-4.8/qtglobal.html#qInstallMsgHandler>
 */
void qucsMessageOutput(QtMsgType type, const QMessageLogContext &, const QString &str)
{
  const char *msg = str.toUtf8().data();
  switch (type) {
  case QtInfoMsg:
    fprintf(stderr,"Info %s\n", msg);
    break;
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
  }

#ifdef _WIN32
  OutputDebugStringA(msg);
#endif
}

/*!
 * \brief attaches shared object code
 */
void attach(const char* what);

/*!
 * \brief createIcons Create component icons (png) from command line.
 */
void createIcons() {

  int nCats = 0, nComps = 0;

  if(!QDir("./bitmaps_generated").exists()){
    QDir().mkdir("bitmaps_generated");
  }
  Module::registerModules ();
  QStringList cats = Category::getCategories ();

  foreach(QString category, cats) {

    QList<Module *> Comps;
    Comps = Category::getModules(category);

    // crash with diagrams, skip
    if(category == "diagrams") break;

    char * File;
    QString Name;

    foreach (Module *Mod, Comps) {
      if (Mod->info) {

        Element *e = (Mod->info) (Name, File, true);

        Component *c = (Component* ) e; // BUG
					//
        std::list<qucs::Line>const& Lines      = c->lines();
        std::list<qucs::Arc>const& Arcs        = c->arcs();
        std::list<qucs::Area>const& Rects      = c->rects();
        std::list<qucs::Area>const& Ellips     = c->ellips();
        std::list<qucs::Port>const& Ports      = c->Ports;
        std::list<qucs::Text>const& Texts      = c->texts();

        QGraphicsScene *scene = new QGraphicsScene();

        for (auto l = Lines.begin(); l != Lines.end(); ++l) {
          scene->addLine(l->x1, l->y1, l->x2, l->y2, l->style);
        }

        for (auto a = Arcs.begin(); a != Arcs.end(); ++a) {
          // we need an open item here; QGraphisEllipseItem draws a filled ellipse and doesn't do the job here...
          QPainterPath *path = new QPainterPath();
          // the components do not contain the angles in degrees but in 1/16th degrees -> conversion needed
          path->arcMoveTo(a->x,a->y,a->w,a->h,a->angle/16);
          path->arcTo(a->x,a->y,a->w,a->h,a->angle/16,a->arclen/16);
          scene->addPath(*path);
        }

        for (auto a = Rects.begin(); a != Rects.end(); ++a) {
          scene->addRect(a->x, a->y, a->w, a->h, a->Pen, a->Brush);
        }

        for (auto a = Ellips.begin(); a != Ellips.end(); ++a) {
          scene->addEllipse(a->x, a->y, a->w, a->h, a->Pen, a->Brush);
        }

        for (auto p = Ports.begin(); p != Ports.end(); ++p) {
          scene->addEllipse(p->x-4, p->y-4, 8, 8, QPen(Qt::red));
        }

        for (auto t = Texts.begin(); t != Texts.end(); ++t) {
          QFont myFont;
          myFont.setPointSize(10);
          QGraphicsTextItem* item  = new QGraphicsTextItem(t->s);
          item->setX(t->x);
          item->setY(t->y);
          item->setFont(myFont);

          scene->addItem(item);
        }

        // this uses the size of the component as icon size
        // Qt bug ? The returned sceneRect() is often 1 px short on bottom
        //   and right sides without anti-aliasing. 1 px more missing on top
        //   and left when anti-aliasing is used
        QRectF rScene = scene->sceneRect().adjusted(-1,-1,1,1);
        // image and scene need to be the same size, since render()
        //   will fill the entire image, otherwise the scaling will
        //   introduce artifacts
        QSize sImage = rScene.size().toSize(); // rounding seems not to be an issue
        // ARGB32_Premultiplied is faster (Qt docs)
        //QImage image(sImage.toSize(), QImage::Format_ARGB32);
        QImage image(sImage, QImage::Format_ARGB32_Premultiplied);
        // this uses a fixed size for the icon (32 x 32)
        //QImage image(32, 32, QImage::Format_ARGB32);
        image.fill(Qt::transparent);

        QPainter painter(&image);
        QPainter::RenderHints hints;
        // Ask to antialias drawings if requested
        if (QucsSettings.GraphAntiAliasing) hints |= QPainter::Antialiasing;
        // Ask to antialias text if requested
        if (QucsSettings.TextAntiAliasing) hints |= QPainter::TextAntialiasing;
        painter.setRenderHints(hints);

        // pass target and source size eplicitly, otherwise sceneRect() is used
        //   for the source size, which is often wrong (see comment above)
        scene->render(&painter, image.rect(), rScene);

        image.save("./bitmaps_generated/" + QString(File) + ".png");

        fprintf(stdout, "[%s] %s\n", category.toLatin1().data(), File);
      }
      nComps++;
    } // module
    nCats++;
  } // category
  fprintf(stdout, "Created %i component icons from %i categories\n", nComps, nCats);
}

/*!
 * \brief createDocData Create data used for documentation.
 *
 * It creates the following:
 *  - list of categories: categories.txt
 *  - category directory, ex.: ./lumped components/
 *    - CSV with component data fields. Ex [component#]_data.csv
 *    - CSV with component properties. Ex [component#]_props.csv
 */
void createDocData() {

  QMap<int, QString> typeMap;
  typeMap.insert(0x30000, "Component");
  typeMap.insert(0x30002, "ComponentText");
  typeMap.insert(0x10000, "AnalogComponent");
  typeMap.insert(0x20000, "DigitalComponent") ;

  Module::registerModules ();
  QStringList cats = Category::getCategories ();
  int nCats = cats.size();

  QStringList catHeader;
  catHeader << "# Note: auto-generated file (changes will be lost on update)";
  QFile file("categories.txt");
  if (!file.open(QFile::WriteOnly | QFile::Text)) return;
  QTextStream out(&file);
  out << cats.join("\n");
  file.close();

  int nComps = 0;

  // table for quick reference, schematic and netlist entry
  foreach(QString category, cats) {

    QList<Module *> Comps;
    Comps = Category::getModules(category);

    // \fixme, crash with diagrams, skip
    if(category == "diagrams") break;

    // one dir per category
    QString curDir = "./"+category+"/";
    qDebug() << "Creating dir:" << curDir;
    if(!QDir(curDir).exists()){
        QDir().mkdir(curDir);
    }

    char * File;
    QString Name;

    int num = 0; // compoment id inside category

    foreach (Module *Mod, Comps) {
        num += 1;

        nComps += 1;

        Element *e = (Mod->info) (Name, File, true);
        Component *c = (Component* ) e;

        // object info
        QStringList compData;

        compData << "# Note: auto-generated file (changes will be lost on update)";
        compData << "Caption; "           + Name;
        compData << "Description; "       + c->description();
        compData << "Identifier; ``"      + c->obsolete_model_hack() + "``"; // backticks for reST verbatim
        compData << "Default name; ``"    + c->name()  + "``";
        compData << "Type; "              + typeMap.value(c->Type);
        compData << "Bitmap file; "       + QString(File);
        compData << "Properties; "        + QString::number(c->Props.size());
        compData << "Category; "          + category;

        // 001_data.csv - CSV file with component data
        QString ID = QString("%1").arg(num,3,'d',0,'0');
        QString objDataFile;
        objDataFile = QString("%1_data.csv").arg( ID  ) ;

        QFile file(curDir + objDataFile);
        if (!file.open(QFile::WriteOnly | QFile::Text)) return;
        QTextStream out(&file);
        out << compData.join("\n");
        file.close();
        fprintf(stdout, "[%s] %s %s \n", category.toLatin1().data(), c->obsolete_model_hack().toLatin1().data(), file.fileName().toLatin1().data());

        QStringList compProps;
        compProps << "# Note: auto-generated file (changes will be lost on update)";
        compProps << QString("# %1; %2; %3; %4").arg(  "Name", "Value", "Display", "Description");
        for (auto prop = c->Props.begin(); prop != c->Props.end(); ++prop) {
          compProps << QString("%1; \"%2\"; %3; \"%4\"").arg(
                         prop->Name,
                         prop->Value,
                         prop->display?"yes":"no",
                         prop->Description.replace("\"","\"\"")); // escape quote in quote
        }

        // 001_props.csv - CSV file with component properties
        QString objPropFile = QString("%1_prop.csv").arg( ID ) ;

        QFile fileProps(curDir + objPropFile );
        if (!fileProps.open(QFile::WriteOnly | QFile::Text)) return;
        QTextStream outProps(&fileProps);
        outProps << compProps.join("\n");
        compProps.clear();
        file.close();
        fprintf(stdout, "[%s] %s %s \n", category.toLatin1().data(), c->obsolete_model_hack().toLatin1().data(), fileProps.fileName().toLatin1().data());
    } // module
  } // category
  fprintf(stdout, "Created data for %i components from %i categories\n", nComps, nCats);
}

/*!
 * \brief createListNetEntry prints to stdout the available netlist formats
 *
 * Prints the default component entries format for:
 *  - Qucs schematic
 *  - Qucsator netlist
 */
void createListComponentEntry(){

  Module::registerModules ();
  QStringList cats = Category::getCategories ();
  // table for quick reference, schematic and netlist entry
  foreach(QString category, cats) {

    QList<Module *> Comps;
    Comps = Category::getModules(category);

    // \fixme, crash with diagrams, skip
    if(category == "diagrams") break;

    char * File;
    QString Name;

    foreach (Module *Mod, Comps) {
      Element *e = (Mod->info) (Name, File, true);
		// dangerous. better precheck cast
      Component *c = (Component* ) e;

		// FIXME: cleanup
		QTextStream s;
		c->getSchematic()->saveComponent(s, c);
      QString qucsEntry = *(s.string());
      fprintf(stdout, "%s; qucs    ; %s\n", c->obsolete_model_hack().toLatin1().data(), qucsEntry.toLatin1().data());

      // add dummy ports/wires, avoid segfault
      int port = 0;
      for (auto p = c->Ports.begin(); p != c->Ports.end(); ++p) {
        static std::list<std::shared_ptr<Node> > heap;  //  FIXME: lost
        std::shared_ptr<Node> n(new Node(0,0));
        heap.push_back(n);
        n->Name="_net"+QString::number(port);
        p->Connection = n;
        port +=1;
      }

      // skip Subcircuit, segfault, there is nothing to netlist
      if (c->obsolete_model_hack() == "Sub" or c->obsolete_model_hack() == ".Opt") {
        fprintf(stdout, "WARNING, qucsator netlist not generated for %s\n\n", c->obsolete_model_hack().toLatin1().data());
        continue;
      }

      QString qucsatorEntry = c->getNetlist();
      fprintf(stdout, "%s; qucsator; %s\n", c->obsolete_model_hack().toLatin1().data(), qucsatorEntry.toLatin1().data());
      } // module
    } // category
}

// #########################################################################
// ##########                                                     ##########
// ##########                  Program Start                      ##########
// ##########                                                     ##########
// #########################################################################
int main(int argc, char *argv[])
{
  qInstallMessageHandler(qucsMessageOutput);
  // set the Qucs version string
  QucsVersion = VersionTriplet(PACKAGE_VERSION);

  // apply default settings
  QucsSettings.font = QFont("Helvetica", 12);
  QucsSettings.largeFontSize = 16.0;
  QucsSettings.maxUndo = 20;
  QucsSettings.NodeWiring = 0;
  QucsSettings.Editor = "qucs";

  // initially center the application
  QApplication a(argc, argv);
  QDesktopWidget *d = a.desktop();
  int w = d->width();
  int h = d->height();
  QucsSettings.x = w/8;
  QucsSettings.y = h/8;
  QucsSettings.dx = w*3/4;
  QucsSettings.dy = h*3/4;

  // default
  QucsSettings.QucsHomeDir.setPath(QDir::homePath()+QDir::toNativeSeparators("/.qucs"));
  QucsSettings.QucsWorkDir.setPath(QucsSettings.QucsHomeDir.canonicalPath());

  // load existing settings (if any)
  loadSettings();

  // continue to set up overrides or default settings (some are saved on exit)

  // check for relocation env variable
  char* var = getenv("QUCSDIR");
  QDir QucsDir;
  if (var!= NULL)
  {
      QucsDir = QDir(QString(var));
      qDebug() << "QUCSDIR set: " << QucsDir.absolutePath();
  }
  else
  {
     QString QucsApplicationPath = QCoreApplication::applicationDirPath();
     #ifdef __APPLE__
     QucsDir = QDir(QucsApplicationPath.section("/bin",0,0));
     #else
     QucsDir = QDir(QucsApplicationPath);
     QucsDir.cdUp();
     #endif

  }

  QucsSettings.BinDir =      QucsDir.absolutePath() + "/bin/";
  QucsSettings.LangDir =     QucsDir.canonicalPath() + "/share/qucs/lang/";
  var = getenv("QUCS_LIBRARY");
  if(var != NULL) {
	  QucsSettings.LibDir = QString(var);
  }else{
	  QucsSettings.LibDir = QucsDir.canonicalPath() + "/share/qucs/library/";
  }
  QucsSettings.OctaveDir =   QucsDir.canonicalPath() + "/share/qucs/octave/";
  QucsSettings.ExamplesDir = QucsDir.canonicalPath() + "/share/qucs/examples/";
  QucsSettings.DocDir =      QucsDir.canonicalPath() + "/share/qucs/docs/";


  /// \todo Make the setting up of all executables below more consistent
  var = getenv("QUCSATOR");
  if(var != NULL) {
      QucsSettings.Qucsator = QString(var);
  }
  else {
      QucsSettings.Qucsator = QucsSettings.BinDir + "qucsator" + executableSuffix;
  }

  var = getenv("QUCSCONV");
  if(var != NULL) {
      QucsSettings.Qucsconv = QString(var);
  }
  else {
      QucsSettings.Qucsconv = QucsSettings.BinDir + "qucsconv" + executableSuffix;
  }
  QFile file(QucsSettings.Qucsconv);
  if(!file.exists())
      qWarning() << "QucsConv not found: " << QucsSettings.Qucsconv;


  var = getenv("ADMSXMLBINDIR");
  if(var != NULL) {
      QucsSettings.AdmsXmlBinDir.setPath(QString(var));
  }
  else {
      // default admsXml bindir same as Qucs
      QString admsExec;
#ifdef __MINGW32__
      admsExec = QDir::toNativeSeparators(QucsSettings.BinDir+"/"+"admsXml.exe");
#else
      admsExec = QDir::toNativeSeparators(QucsSettings.BinDir+"/"+"admsXml");
#endif
      QFile adms(admsExec);
      if(adms.exists())
        QucsSettings.AdmsXmlBinDir.setPath(QucsSettings.BinDir);
  }

  var = getenv("ASCOBINDIR");
  if(var != NULL)  {
      QucsSettings.AscoBinDir.setPath(QString(var));
  }
  else  {
      // default ASCO bindir same as Qucs
      QString ascoExec;
#ifdef __MINGW32__
      ascoExec = QDir::toNativeSeparators(QucsSettings.BinDir+"/"+"asco.exe");
#else
      ascoExec = QDir::toNativeSeparators(QucsSettings.BinDir+"/"+"asco");
#endif
      QFile asco(ascoExec);
      if(asco.exists())
        QucsSettings.AscoBinDir.setPath(QucsSettings.BinDir);
  }


  var = getenv("QUCS_OCTAVE");
  if (var != NULL) {
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


  a.setFont(QucsSettings.font);

  // set codecs
  QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
  // QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

  QTranslator tor( 0 );
  QString lang = QucsSettings.Language;
  if(lang.isEmpty()) {
    QLocale loc;
    lang = loc.name();
  }
  tor.load( QString("qucs_") + lang, QucsSettings.LangDir);
  a.installTranslator( &tor );

  // This seems to be neccessary on a few system to make strtod()
  // work properly !???!
  setlocale (LC_NUMERIC, "C");

  QString inputfile;
  QString outputfile;
  QString outputtype;

  bool netlist_flag = false;
  bool print_flag = false;
  bool dump_flag = false;
  QString page = "A4";
  int dpi = 96;
  QString color = "RGB";
  QString orientation = "portrait";
  QStringList files;

  // simple command line parser
  for (int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
      fprintf(stdout,
  "Usage: %s [-hv] \n"
  "       qucs -n -i FILENAME -o FILENAME\n"
  "       qucs -p -i FILENAME -o FILENAME.[pdf|png|svg|eps] \n\n"
  "  -h, --help     display this help and exit\n"
  "  -v, --version  display version information and exit\n"
  "  -n, --netlist  convert Qucs schematic into netlist\n"
  "  -p, --print    print Qucs schematic to file (eps needs inkscape)\n"
  "  -platform [xcb,vnc,offscreen,...]  Allows you to select a platform plugin.\n"
  "                                     In case you ar trying to run it headless it will give you a list\n"
  "  -d, --dump     Directly dump input file format into desired output file format\n"
  "  -t, --type [sch|vs] Output format for dump operation: Verilog Schematic or Legacy Schematic \n"
  "  -q, --quit     Exit quietly\n"
  "    --page [A4|A3|B4|B5]         set print page size (default A4)\n"
  "    --dpi NUMBER                 set dpi value (default 96)\n"
  "    --color [RGB|RGB]            set color mode (default RGB)\n"
  "    --orin [portraid|landscape]  set orientation (default portraid)\n"
  "  -i FILENAME    use file as input schematic\n"
  "  -o FILENAME    use file as output netlist\n"
  "  -icons         create component icons under ./bitmaps_generated\n"
  "  -doc           dump data for documentation:\n"
  "                 * file with of categories: categories.txt\n"
  "                 * one directory per category (e.g. ./lumped components/)\n"
  "                   - CSV file with component data ([comp#]_data.csv)\n"
  "                   - CSV file with component properties. ([comp#]_props.csv)\n"
  "  -list-entries  list component entry formats for schematic and netlist\n"
  , argv[0]);
      return 0;
    }else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")) {
#ifdef GIT
      fprintf(stdout, "Qucs " PACKAGE_VERSION " (" GIT ")" "\n");
#else
      fprintf(stdout, "Qucs " PACKAGE_VERSION "\n");
#endif
      return 0;
    }
    else if (!strcmp(argv[i], "-n") || !strcmp(argv[i], "--netlist")) {
      netlist_flag = true;
    }
    else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--print")) {
      print_flag = true;
    }
    else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--dump")) {
      dump_flag = true;
    }
    else if (!strcmp(argv[i], "--page")) {
      page = argv[++i];
    }
    else if (!strcmp(argv[i], "--dpi")) {
      dpi = QString(argv[++i]).toInt();
    }
    else if (!strcmp(argv[i], "--color")) {
      color = argv[++i];
    }
    else if (!strcmp(argv[i], "--orin")) {
      orientation = argv[++i];
    }
    else if (!strcmp(argv[i], "--type") || !strcmp(argv[i], "-t")) {
      outputtype = argv[++i];
    }
    else if (!strcmp(argv[i], "-a")) {
      attach(argv[++i]);
    }
    else if(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quit")) {
	exit(0);
    }
    else if (!strcmp(argv[i], "-i")) {
      inputfile = argv[++i];
      if(!QFileInfo(inputfile).isAbsolute()) {
        inputfile = QDir(QDir::currentPath()).absoluteFilePath(inputfile);
      }
      files.append(inputfile);
    }
    else if (!strcmp(argv[i], "-o")) {
      outputfile = argv[++i];
      if(!QFileInfo(outputfile).isAbsolute()) {
        outputfile = QDir(QDir::currentPath()).absoluteFilePath(outputfile);
      }
    }
    else if(!strcmp(argv[i], "-icons")) {
      createIcons();
      return 0;
    }
    else if(!strcmp(argv[i], "-doc")) {
      createDocData();
      return 0;
    }
    else if(!strcmp(argv[i], "-list-entries")) {
      createListComponentEntry();
      return 0;
    }
    else {
      if(!QString(argv[i]).endsWith(".sch") && !QString(argv[i]).endsWith(".sch")) {
        fprintf(stderr, "Error: Unknown option: %s\n", argv[i]);
        return -1;
      }
    }
  }

  // load documents given as command line arguments
  if(!files.size()) {
    for (int i = 1; i < argc; ++i) {
      QString arg = argv[i];
      QByteArray ba = arg.toLatin1();
      const char *c_arg = ba.data();
      if(*(c_arg) != '-') {
        if(!QFileInfo(arg).isAbsolute()) {
          arg = QDir::cleanPath(QDir(QDir::currentPath()).filePath(arg));
        }
        files.append(arg);
      }
    }
  }

  // Open the files
  QucsMain = new QucsApp(files);

  // check operation and its required arguments
  if (netlist_flag and print_flag and dump_flag) {
    fprintf(stderr, "Error: --print, --netlist and --dump cannot be used together\n");
    return -1;
  } else if (netlist_flag or print_flag or dump_flag) {
    if (inputfile.isEmpty()) {
      fprintf(stderr, "Error: Expected input file.\n");
      return -1;
    }
    if (outputfile.isEmpty()) {
      fprintf(stderr, "Error: Expected output file.\n");
      return -1;
    }
    // create netlist from schematic
    if (netlist_flag) {
      return doNetlist(inputfile, outputfile);
    } else if (print_flag) {
      return doPrint(inputfile, outputfile,
          page, dpi, color, orientation);
    } else if (dump_flag) {
      return doDump(inputfile, outputfile, outputtype);
    }
  }

  QucsMain->show();
  int result = a.exec();
  //saveApplSettings(QucsMain);
  return result;
}
// vim:ts=8:sw=2:noet
