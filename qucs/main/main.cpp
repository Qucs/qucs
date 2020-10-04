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
#include "node.h"
#include "printerwriter.h"
#include "imagewriter.h"
#include "schematic_lang.h"
#include "sim/sim.h"

//#include "schematic.h"
#include "module.h"
#include "misc.h"
#include "exception.h"

// Wtf?
#include "components/components.h"
#include "globals.h"
// #include "components/components.h"

// for now. move to lib later.
#include "io_findf.cc"

#ifdef _WIN32
#include <Windows.h>  //for OutputDebugString
#endif

static const std::string default_simulator="qucsator"; // FIXME: get from rc? maybe from environment?

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

/*!
 * \brief attaches shared object code
 */
void attach(const char* what);

static std::string plugpath()
{
  const char* ppenv=getenv("QUCS_PLUGPATH");
  if(!ppenv){ untested();
    return QUCS_PLUGPATH;
  }else{
    return ppenv;
  }
}

// BUG: not here.
// possibly use SchematicModel(QString Filename, Lang...)
#if 0
Schematic *openSchematic(QString schematic)
{ untested();
  qDebug() << "*** try to load schematic :" << schematic;

  // QString to *char
  QByteArray ba = schematic.toLatin1();
  const char *c_sch = ba.data();

  QFile file(schematic);  // save simulator messages
  if(file.open(QIODevice::ReadOnly)) { untested();
    file.close();
  }else{ untested();
    fprintf(stderr, "Error: No such file %s\n", c_sch);
    return NULL;
  }

  // new schematic from file
  SchematicModel *sch = new SchematicModel(schematic);

  // load schematic file if possible
  if(!sch->loadDocument()) { untested();
    fprintf(stderr, "Error: Could not load schematic %s\n", c_sch);
    delete sch;
    return NULL;
  }else{ untested();
  }
  return sch;
}
#endif

// BUG: move to SchematicModel
namespace{
class sda : public SchematicSymbol{
public:
  explicit sda() : SchematicSymbol() {
    new_subckt();
  }
  ~sda(){
  }
private: // SchematicSymbol
  SchematicModel* schematicModel(){ untested();
    // obsolete.
    unreachable();
    return nullptr;
  }
  // bug, forward to schematic object (it doesn't exist yet).
  std::string getParameter(std::string const&x) const{
    if(x=="DocName"){
      return DocName;
    }else if(x=="ViewX1"){
      incomplete(); // there is no view.
      return "0";
    }else if(x=="ViewX2"){
      incomplete(); // there is no view.
      return "0";
    }else if(x=="ViewY1"){
      incomplete(); // there is no view.
      return "0";
    }else if(x=="ViewY2"){
      incomplete(); // there is no view.
      return "0";
    }else if(x=="Scale"){
      incomplete(); // there is no view.
      return "0";
    }else if(x=="GridOn"){
      incomplete(); // there is no view.
      return "0";
    }else if(x=="GridX"){
      incomplete(); // there is no view.
      return "0";
    }else if(x=="GridY"){
      incomplete(); // there is no view.
      return "0";
    }else if(x=="tmpViewX1"){
      incomplete(); // there is no view.
      return "0";
    }else{
      qDebug() << "unknown parameter" << QString::fromStdString(x);
      incomplete();
      return "unknown";
      throw ExceptionCantFind(x, "main");
    }
  }

private:
  unsigned numPorts() const{ incomplete(); return 0; }
  Net const* portValue(unsigned)const {unreachable(); return nullptr;}
  void setPort(unsigned, Node*){incomplete();}

public:
  SchematicModel* subckt(){return _subckt;}
  // Commanlist* commands(){ ... }

public: // tmo hack
  std::string DocName;
};
}

// moved to legacy/qucsator, QucsatorNetlister::save
void doNetlist(QString schematic_fn, std::string netlist, DocumentFormat const& NLN)
{
  sda xs;
  xs.setLabel(schematic_fn);
  assert(xs.subckt());
  SchematicModel& sch = *xs.subckt();
  trace1("main schematic model", &sch);
  sch.setFileInfo(schematic_fn);
  QFile file(schematic_fn);  // save simulator messages
  file.open(QIODevice::ReadOnly);
  DocumentStream stream (&file);
  SchematicLanguage const* L=nullptr;

  if(!L){
    auto D=doclang_dispatcher["leg_sch"];
    L = dynamic_cast<SchematicLanguage const*>(D);
  }else{ untested();
  }
  assert(L);

  while(!stream.atEnd()){
    L->parse(stream, xs);
  }

  QFile NetlistFile(QString::fromStdString(netlist));
  if(!NetlistFile.open(QIODevice::WriteOnly | QFile::Truncate)) { untested();
    fprintf(stderr, "Error: Could write to %s\n", netlist.c_str());
    exit(1);
  }else{
  }
  DocumentStream os(&NetlistFile);

  // sch.setOwner(&xs);

  xs.DocName = schematic_fn.toStdString(); // tmp
  NLN.save(os, xs);
}

int doPrint(QString schematic, std::string printFile,
    QString page, int dpi, QString color, QString orientation)
{ untested();

  // there are two ways:
  // - just write a pdf, svg, (e)ps or whatever from schematicModel
  // - start the application and take a screenshot.
  //   (for taking screenshots, this function is not needed)

  return 0; // why?!
#if 0
  Schematic sch(nullptr, "..."); // need X?
  // sch.setFileInfo(schematic);
  QFile file(schematic);  // save simulator messages
  file.open(QIODevice::ReadOnly);
  DocumentStream stream (&file);
  try{ untested();
    incomplete();
    // sch.parse(stream);
  }catch(...){ untested();
    return 1;
  }

//  sch->Nodes = &(sch->DocNodes);
//  sch->Wires = &(sch->DocWires);
//  sch->Diagrams = &(sch->DocDiags);
//  sch->Paintings = &(sch->DocPaints);
//  sch->Components = &(sch->DocComps);
//  sch->reloadGraphs();
  incomplete();

  qDebug() << "*** try to print file  :" << printFile;

  // determine filetype
  if (printFile.endsWith(".pdf")) { untested();
    //initial printer
    PrinterWriter *Printer = new PrinterWriter();
    Printer->setFitToPage(true);
    Printer->noGuiPrint(&sch, printFile, page, dpi, color, orientation);
  } else { untested();
    ImageWriter *Printer = new ImageWriter("");
    Printer->noGuiPrint(&sch, printFile, color);
  }
  return 0;
#endif
}

/*!
 * \brief createIcons Create component icons (png) from command line.
 */
void createIcons() { untested();
  incomplete();
#if 0

  int nCats = 0, nComps = 0;

  if(!QDir("./bitmaps_generated").exists()){ untested();
    QDir().mkdir("bitmaps_generated");
  }
  Module::registerModules ();
  QStringList cats = Category::getCategories ();

  foreach(QString category, cats) { untested();

    QList<Module *> Comps;
    Comps = Category::getModules(category);

    // crash with diagrams, skip
    if(category == "diagrams") break;

    char * File;
    QString Name;

    foreach (Module *Mod, Comps) { untested();
      if (Mod->has_element()) { untested();

        Element *e = (Mod->info) (Name, File, true);

        Component *c = (Component* ) e;

        QList<Line *> Lines      = c->Lines;
        QList<struct Arc *> Arcs = c-> Arcs;
        QList<Area *> Rects      = c-> Rects;
        QList<Area *> Ellips     = c-> Ellips;
        QList<Port *> Ports      = c->Ports;
        QList<Text*> Texts       = c->Texts;

        QGraphicsScene *scene = new QGraphicsScene();

        foreach (Line *l, Lines) { untested();
          scene->addLine(l->x1, l->y1, l->x2, l->y2, l->style);
        }

        foreach(struct Arc *a, Arcs) {
          // we need an open item here; QGraphisEllipseItem draws a filled ellipse and doesn't do the job here...
          QPainterPath *path = new QPainterPath();
          // the components do not contain the angles in degrees but in 1/16th degrees -> conversion needed
          path->arcMoveTo(a->x,a->y,a->w,a->h,a->angle/16);
          path->arcTo(a->x,a->y,a->w,a->h,a->angle/16,a->arclen/16);
          scene->addPath(*path);
        }

        foreach(Area *a, Rects) { untested();
          scene->addRect(a->x, a->y, a->w, a->h, a->Pen, a->Brush);
        }

        foreach(Area *a, Ellips) { untested();
          scene->addEllipse(a->x, a->y, a->w, a->h, a->Pen, a->Brush);
        }

        foreach(Port *p, Ports) { untested();
          scene->addEllipse(p->x-4, p->y-4, 8, 8, QPen(Qt::red));
        }

        foreach(Text *t, Texts) { untested();
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
        QPainter::RenderHints hints = 0;
        // Ask to antialias drawings if requested
        if (QucsSettings.GraphAntiAliasing) hints |= QPainter::Antialiasing;
        // Ask to antialias text if requested
        if (QucsSettings.TextAntiAliasing) hints |= QPainter::TextAntialiasing;
        painter.setRenderHints(hints);

        // pass target and source size eplicitly, otherwise sceneRect() is used
        //   for the source size, which is often wrong (see comment above)
        scene->render(&painter, image.rect(), rScene);

        image.save("./bitmaps_generated/" + QString(File) + ".png");

        fprintf(stdout, "[%s] %s\n", category.toAscii().data(), File);
      }
      nComps++;
    } // module
    nCats++;
  } // category
  fprintf(stdout, "Created %i component icons from %i categories\n", nComps, nCats);
#endif
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
void createDocData()
{ untested();
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
  foreach(QString category, cats) { untested();

    QList<Module *> Comps;
    Comps = Category::getModules(category);

    // \fixme, crash with diagrams, skip
    if(category == "diagrams") break;

    // one dir per category
    QString curDir = "./"+category+"/";
    qDebug() << "Creating dir:" << curDir;
    if(!QDir(curDir).exists()){ untested();
        QDir().mkdir(curDir);
    }

    QString File; // what is this, really?
    QString Name;

    int num = 0; // compoment id inside category

    foreach (Module *Mod, Comps) { untested();
        num += 1;

        nComps += 1;

	assert(Mod->has_element());
        Element *e = prechecked_cast<Element*>(Mod->element()->clone()); // memory leak?!
	assert(e);
      // 	(Name, File, true);
        Name = e->label();
        File = e->iconBasename();
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
        compData << "Properties; "        + QString::number(c->Props.count());
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
	fprintf(stdout, "[%s] %s %s \n", category.toAscii().data(),
	    c->obsolete_model_hack().toAscii().data(),
	    file.fileName().toAscii().data());

        QStringList compProps;
        compProps << "# Note: auto-generated file (changes will be lost on update)";
        compProps << QString("# %1; %2; %3; %4").arg(  "Name", "Value", "Display", "Description");
        foreach(Property *prop, c->Props) { untested();
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
	fprintf(stdout, "[%s] %s %s \n", category.toAscii().data(),
	    c->obsolete_model_hack().toAscii().data(),
	    fileProps.fileName().toAscii().data());
    } // module
  } // category
  fprintf(stdout, "Created data for %i components from %i categories\n", nComps, nCats);
}

// createListNetEntry prints to stdout the available netlist formats
//
//  Prints the default component entries format for
//   - Qucs schematic
//   - Qucsator netlist
//   - Verilog netlist
//   - Verilog schematic
//
// table for quick reference and testing
void createListComponentEntry()
{ untested();
  QStringList cats = Category::getCategories ();
  QFile data("/dev/stdout");
  data.open (QFile::WriteOnly | QFile::Truncate);
  QTextStream s(&data);

  auto lang = doclang_dispatcher["leg_sch"];
  assert(lang);
  auto qucsatorlang = doclang_dispatcher["qucsator"];
  assert(qucsatorlang);
  auto verilog = doclang_dispatcher["verilog"];

  foreach(QString category, cats) { untested();

    QList<Module *> Comps;
    Comps = Category::getModules(category);

    // \fixme, crash with diagrams, skip
    if(category == "diagrams") continue;

    char * File;
    QString Name;

    foreach (Module *Mod, Comps) { untested();
      qDebug() << "some module";
      Element const *e = Mod->element();
      Component const *cc = prechecked_cast<Component const*>(e);
      Element *ce = e->clone();
      assert(ce);

      s << "=====" << e->label() << "=========\n";

      if(auto c = dynamic_cast<Symbol*>(ce)){ untested();
	ce->setLabel("my_" + c->typeName());
      }else if(auto c = dynamic_cast<CmdElement*>(ce)){ untested();
	ce->setLabel("my_" + c->typeName());
      }else{ untested();
	// not sure.
      }

      lang->printItem(ce, s);
      s << "\n";

      if(verilog){ untested();
	verilog->printItem(ce, s);
      }else{ untested();
      }

      qucsatorlang->printItem(ce, s);

    } // module
  } // category
}

// TODO: turn into command.
void attach_single(std::string const&path, std::string const& what)
{
  std::string full_file_name;
  if(what.size()==0){ untested();
  }else if(what[0]=='.'){
    full_file_name=what;
  }else{
    full_file_name = findfile(what, path, R_OK);
  }

  if (full_file_name != "") {
    // found it, with search
  }else{untested();
    std::cerr << "something seriously wrong with installation\n";
    std::cerr << "cannot find plugin " + what + " in " +path + "\n";
    exit(1);
  }
  attach(full_file_name.c_str());
}

void attach_default_plugins()
{
  std::string pp = plugpath();
  attach_single(pp, "legacy" SOEXT);
  attach_single(pp, "qucsator" SOEXT);

  // TODO: remove "lib" prefix
  attach_single(pp, "qucs-default-components" SOEXT);
  attach_single(pp, "libpaintings" SOEXT);
 // attach_single(pp, "libdialogs" SOEXT);
 //
 //
 // not yet. legacy diagrams are now part of legacy (above)
 // new diagrams: load manually.
  // attach_single(pp, "libdiagrams" SOEXT);
}

void qucsMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & str)
{
  auto msg=str.toStdString();
  qucsMessageOutput(type, msg.c_str());
}

// #########################################################################
// ##########                                                     ##########
// ##########                  Program Start                      ##########
// ##########                                                     ##########
// #########################################################################
int main(int argc, char *argv[])
{
  qInstallMsgHandler(qucsMessageHandler);
  // set the Qucs version string
  QucsVersion = VersionTriplet(PACKAGE_VERSION);

  // apply default settings
  QucsSettings.font = QFont("Helvetica", 12);
  QucsSettings.largeFontSize = 16.0;
  QucsSettings.maxUndo = 20;
  QucsSettings.NodeWiring = 0;
  QucsSettings.Editor = "qucs";

  // initially center the application

  QFile qf(":/bitmaps/doesnotexist.png");
  assert(!qf.exists());
  QFile qfl(":/bitmaps/line.png");
  assert(qfl.exists());


  // default
  QucsSettings.QucsHomeDir.setPath(QDir::homePath()+QDir::toNativeSeparators("/.qucs"));
  QucsSettings.QucsWorkDir.setPath(QucsSettings.QucsHomeDir.canonicalPath());

  // load existing settings (if any)
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
  var = getenv("QUCS_LIBDIR");
  if(var != NULL) { untested();
	  QucsSettings.LibDir = QString(var);
  }else{
	  QucsSettings.LibDir =      QucsDir.canonicalPath() + "/share/qucs/library/";
  }
  QucsSettings.OctaveDir =   QucsDir.canonicalPath() + "/share/qucs/octave/";
  QucsSettings.ExamplesDir = QucsDir.canonicalPath() + "/share/qucs/examples/";
  QucsSettings.DocDir =      QucsDir.canonicalPath() + "/share/qucs/docs/";

  attach_default_plugins();

  /// \todo Make the setting up of all executables below more consistent
  var = getenv("QUCSATOR");
  if(var != NULL) {
      QucsSettings.Qucsator = QString(var);
  }
  else { untested();
      QucsSettings.Qucsator = QucsSettings.BinDir + "qucsator" + executableSuffix;
  }

  var = getenv("QUCSCONV");
  if(var != NULL) {
      QucsSettings.Qucsconv = QString(var);
  }
  else { untested();
      QucsSettings.Qucsconv = QucsSettings.BinDir + "qucsconv" + executableSuffix;
  }
  QFile file(QucsSettings.Qucsconv);
  if(!file.exists())
      qWarning() << "QucsConv not found: " << QucsSettings.Qucsconv;


  var = getenv("ADMSXMLBINDIR");
  if(var != NULL) { untested();
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
  if(var != NULL)  { untested();
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

  // simple command line parser
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
  "Options:\n"
  "  -i FILENAME    use file as input schematic\n"
  "  -o FILENAME    use file as output netlist\n"
  "  -l DOCLANG     language to be used by dump, can be a simulator name\n"
  "    --page [A4|A3|B4|B5]         set print page size (default A4)\n"
  "    --dpi NUMBER                 set dpi value (default 96)\n"
  "    --color [RGB|RGB]            set color mode (default RGB)\n"
  "    --orin [portrait|landscape]  set orientation (default portraid)\n"
  ,argv[0]);
      return 0;
    }else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")) { untested();
#ifdef GIT
      fprintf(stdout, "Qucs " PACKAGE_VERSION " (" GIT ")" "\n");
#else
      fprintf(stdout, "Qucs " PACKAGE_VERSION "\n");
#endif
      return 0;
    } else if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--dump")) { untested();
      dump_flag = true;
    } else if (!strcmp(argv[i], "-n") || !strcmp(argv[i], "--netlist")) {
      dump_flag = true;
    } else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--print")) { untested();
      print_flag = true;
    }
    else if (!strcmp(argv[i], "--page")) { untested();
      page = argv[++i];
    }
    else if (!strcmp(argv[i], "--dpi")) { untested();
      dpi = QString(argv[++i]).toInt();
    }
    else if (!strcmp(argv[i], "--color")) { untested();
      color = argv[++i];
    }
    else if (!strcmp(argv[i], "--orin")) { untested();
      orientation = argv[++i];
    }
    else if (!strcmp(argv[i], "-a")) {
      ++i;
      qDebug() << "attaching" << argv[i];
      std::string pp = plugpath();
      std::string what = argv[i];
      if(what.size()<4){ untested();
      }else if(what[what.size()-3]!='.'){
	what += SOEXT;
      }else{
      }
      attach_single(pp, what);
    }
    else if(!strcmp(argv[i], "-q") || !strcmp(argv[i], "--quit")) {
	exit(0);
    }
    else if (!strcmp(argv[i], "-i")) {
      inputfile = argv[++i];
    }
    else if (!strcmp(argv[i], "-o")) {
      outputfile = argv[++i];
    }
    else if (!strcmp(argv[i], "-l")) {
      netlang_name = argv[++i];
    }
    else if(!strcmp(argv[i], "-icons")) { untested();
      createIcons();
      return 0;
    }
    else if(!strcmp(argv[i], "-doc")) { untested();
      createDocData();
      return 0;
    } else if(!strcmp(argv[i], "-list-entries")) { untested();
      incomplete(); // don't use.
      createListComponentEntry();
      return 0;
    } else if(!strcmp(argv[i], "--list-entries")) { untested();
      createListComponentEntry();
      return 0;
    }
    else { untested();
      fprintf(stderr, "Error: Unknown option: %s\n", argv[i]);
      return -1;
    }
  }

  DocumentFormat const* fmt = docfmt_dispatcher[netlang_name];

  if(fmt){
    // just use it.
  }else if(auto sd = simulator_dispatcher[netlang_name]){ untested();
    incomplete();
    // ask a simulator.
//    fmt = sd->netLang();
  }
  if(!fmt){ untested();
    // TODO: io_error...
    std::cerr << "Error: Cannot find language for "
              << netlang_name << "\n";
    return -1;
  }

  // check operation and its required arguments
  if (dump_flag and print_flag) { untested();
    fprintf(stderr, "Error: --print and --netlist cannot be used together\n");
    return -1;
  } else if (dump_flag or print_flag) {
    if (inputfile.isEmpty()) { untested();
      fprintf(stderr, "Error: Expected input file.\n");
      return -1;
    }
    if (outputfile == "") { untested();
      fprintf(stderr, "Error: Expected output file.\n");
      return -1;
    }
    // create netlist from schematic
    if (dump_flag) {
      auto NLN=docfmt_dispatcher[netlang_name];
      if(!NLN){ untested();
	qDebug() << "no lang" << QString::fromStdString(netlang_name);
	incomplete();
	exit(1);
      }
      doNetlist(inputfile, outputfile, *NLN);
      return 0; // BUG
    } else if (print_flag) { untested();
      return doPrint(inputfile, outputfile,
          page, dpi, color, orientation);
    }
  }
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
  int result = a.exec();
  //saveApplSettings(QucsMain);
  return result;
}
// vim:ts=8:sw=2:noet
