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

#include "qucs.h"
#include "main.h"
#include "node.h"
#include "printerwriter.h"
#include "imagewriter.h"

#include "schematic.h"
#include "module.h"

#include "components/components.h"

#ifdef _WIN32
#include <Windows.h>  //for OutputDebugString
#endif

#ifdef __MINGW32__
#define executableSuffix ".exe"
#else
#define executableSuffix ""
#endif

tQucsSettings QucsSettings;

QucsApp *QucsMain = 0;  // the Qucs application itself
QString lastDir;    // to remember last directory for several dialogs
QStringList qucsPathList;

// #########################################################################
// Loads the settings file and stores the settings.
bool loadSettings()
{
    QSettings settings("qucs","qucs");

    if(settings.contains("x"))QucsSettings.x=settings.value("x").toInt();
    if(settings.contains("y"))QucsSettings.y=settings.value("y").toInt();
    if(settings.contains("dx"))QucsSettings.dx=settings.value("dx").toInt();
    if(settings.contains("dy"))QucsSettings.dy=settings.value("dy").toInt();
    if(settings.contains("font"))QucsSettings.font.fromString(settings.value("font").toString());
    if(settings.contains("LargeFontSize"))QucsSettings.largeFontSize=settings.value("LargeFontSize").toDouble(); // use toDouble() as it can interpret the string according to the current locale
    if(settings.contains("maxUndo"))QucsSettings.maxUndo=settings.value("maxUndo").toInt();
    if(settings.contains("NodeWiring"))QucsSettings.NodeWiring=settings.value("NodeWiring").toInt();
    if(settings.contains("BGColor"))QucsSettings.BGColor.setNamedColor(settings.value("BGColor").toString());
    if(settings.contains("Editor"))QucsSettings.Editor=settings.value("Editor").toString();
    if(settings.contains("FileTypes"))QucsSettings.FileTypes=settings.value("FileTypes").toStringList();
    if(settings.contains("Language"))QucsSettings.Language=settings.value("Language").toString();
    if(settings.contains("Comment"))QucsSettings.Comment.setNamedColor(settings.value("Comment").toString());
    if(settings.contains("String"))QucsSettings.String.setNamedColor(settings.value("String").toString());
    if(settings.contains("Integer"))QucsSettings.Integer.setNamedColor(settings.value("Integer").toString());
    if(settings.contains("Real"))QucsSettings.Real.setNamedColor(settings.value("Real").toString());
    if(settings.contains("Character"))QucsSettings.Character.setNamedColor(settings.value("Character").toString());
    if(settings.contains("Type"))QucsSettings.Type.setNamedColor(settings.value("Type").toString());
    if(settings.contains("Attribute"))QucsSettings.Attribute.setNamedColor(settings.value("Attribute").toString());
    if(settings.contains("Directive"))QucsSettings.Directive.setNamedColor(settings.value("Directive").toString());
    if(settings.contains("Task"))QucsSettings.Task.setNamedColor(settings.value("Task").toString());

    if(settings.contains("Qucsator"))QucsSettings.Qucsator = settings.value("Qucsator").toString();
    //if(settings.contains("BinDir"))QucsSettings.BinDir = settings.value("BinDir").toString();
    //if(settings.contains("LangDir"))QucsSettings.LangDir = settings.value("LangDir").toString();
    //if(settings.contains("LibDir"))QucsSettings.LibDir = settings.value("LibDir").toString();
    if(settings.contains("AdmsXmlBinDir"))QucsSettings.AdmsXmlBinDir = settings.value("AdmsXmlBinDir").toString();
    if(settings.contains("AscoBinDir"))QucsSettings.AscoBinDir = settings.value("AscoBinDir").toString();
    //if(settings.contains("OctaveDir"))QucsSettings.OctaveDir = settings.value("OctaveDir").toString();
    //if(settings.contains("ExamplesDir"))QucsSettings.ExamplesDir = settings.value("ExamplesDir").toString();
    //if(settings.contains("DocDir"))QucsSettings.DocDir = settings.value("DocDir").toString();
    if(settings.contains("OctaveBinDir"))QucsSettings.OctaveBinDir.setPath(settings.value("OctaveBinDir").toString());
    if(settings.contains("QucsHomeDir"))
      if(settings.value("QucsHomeDir").toString() != "")
         QucsSettings.QucsHomeDir.setPath(settings.value("QucsHomeDir").toString());
    QucsSettings.QucsWorkDir = QucsSettings.QucsHomeDir;

    if (settings.contains("IgnoreVersion")) QucsSettings.IgnoreFutureVersion = settings.value("IgnoreVersion").toBool();
    // check also for old setting name with typo...
    else if (settings.contains("IngnoreVersion")) QucsSettings.IgnoreFutureVersion = settings.value("IngnoreVersion").toBool();
    else QucsSettings.IgnoreFutureVersion = false;

    if (settings.contains("GraphAntiAliasing")) QucsSettings.GraphAntiAliasing = settings.value("GraphAntiAliasing").toBool();
    else QucsSettings.GraphAntiAliasing = false;

    if (settings.contains("TextAntiAliasing")) QucsSettings.TextAntiAliasing = settings.value("TextAntiAliasing").toBool();
    else QucsSettings.TextAntiAliasing = false;

    QucsSettings.RecentDocs = settings.value("RecentDocs").toString().split("*",QString::SkipEmptyParts);
    QucsSettings.numRecentDocs = QucsSettings.RecentDocs.count();


    QucsSettings.spiceExtensions << "*.sp" << "*.cir" << "*.spc" << "*.spi";

    // If present read in the list of directory paths in which Qucs should
    // search for subcircuit schematics
    int npaths = settings.beginReadArray("Paths");
    for (int i = 0; i < npaths; ++i)
    {
        settings.setArrayIndex(i);
        QString apath = settings.value("path").toString();
        qucsPathList.append(apath);
    }
    settings.endArray();

    QucsSettings.numRecentDocs = 0;

    return true;
}

// #########################################################################
// Saves the settings in the settings file.
bool saveApplSettings()
{
    QSettings settings ("qucs","qucs");

    settings.setValue("x", QucsSettings.x);
    settings.setValue("y", QucsSettings.y);
    settings.setValue("dx", QucsSettings.dx);
    settings.setValue("dy", QucsSettings.dy);
    settings.setValue("font", QucsSettings.font.toString());
    // store LargeFontSize as a string, so it will be also human-readable in the settings file (will be a @Variant() otherwise)
    settings.setValue("LargeFontSize", QString::number(QucsSettings.largeFontSize));
    settings.setValue("maxUndo", QucsSettings.maxUndo);
    settings.setValue("NodeWiring", QucsSettings.NodeWiring);
    settings.setValue("BGColor", QucsSettings.BGColor.name());
    settings.setValue("Editor", QucsSettings.Editor);
    settings.setValue("FileTypes", QucsSettings.FileTypes);
    settings.setValue("Language", QucsSettings.Language);
    settings.setValue("Comment", QucsSettings.Comment.name());
    settings.setValue("String", QucsSettings.String.name());
    settings.setValue("Integer", QucsSettings.Integer.name());
    settings.setValue("Real", QucsSettings.Real.name());
    settings.setValue("Character", QucsSettings.Character.name());
    settings.setValue("Type", QucsSettings.Type.name());
    settings.setValue("Attribute", QucsSettings.Attribute.name());
    settings.setValue("Directive", QucsSettings.Directive.name());
    settings.setValue("Task", QucsSettings.Task.name());
    //settings.setValue("Qucsator", QucsSettings.Qucsator);
    //settings.setValue("BinDir", QucsSettings.BinDir);
    //settings.setValue("LangDir", QucsSettings.LangDir);
    //settings.setValue("LibDir", QucsSettings.LibDir);
    settings.setValue("AdmsXmlBinDir", QucsSettings.AdmsXmlBinDir.canonicalPath());
    settings.setValue("AscoBinDir", QucsSettings.AscoBinDir.canonicalPath());
    //settings.setValue("OctaveDir", QucsSettings.OctaveDir);
    //settings.setValue("ExamplesDir", QucsSettings.ExamplesDir);
    //settings.setValue("DocDir", QucsSettings.DocDir);
    settings.setValue("OctaveBinDir", QucsSettings.OctaveBinDir.canonicalPath());
    settings.setValue("QucsHomeDir", QucsSettings.QucsHomeDir.canonicalPath());
    settings.setValue("IgnoreVersion", QucsSettings.IgnoreFutureVersion);
    settings.setValue("GraphAntiAliasing", QucsSettings.GraphAntiAliasing);
    settings.setValue("TextAntiAliasing", QucsSettings.TextAntiAliasing);

    // Copy the list of directory paths in which Qucs should
    // search for subcircuit schematics from qucsPathList
    settings.remove("Paths");
    settings.beginWriteArray("Paths");
    int i = 0;
    foreach(QString path, qucsPathList) {
         settings.setArrayIndex(i);
         settings.setValue("path", path);
         i++;
     }
     settings.endArray();

  return true;

}

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
  }

#ifdef _WIN32
  OutputDebugStringA(msg);
#endif
}

Schematic *openSchematic(QString schematic)
{
  qDebug() << "*** try to load schematic :" << schematic;

  QFile file(schematic);  // save simulator messages
  if(file.open(QIODevice::ReadOnly)) {
    file.close();
  }
  else {
    fprintf(stderr, "Error: Could not load schematic %s\n", schematic.ascii());
    return NULL;
  }

  // populate Modules list
  Module::registerModules ();

  // new schematic from file
  Schematic *sch = new Schematic(0, schematic);

  // load schematic file if possible
  if(!sch->loadDocument()) {
    fprintf(stderr, "Error: Could not load schematic %s\n", schematic.ascii());
    delete sch;
    return NULL;
  }
  return sch;
}

int doNetlist(QString schematic, QString netlist)
{
  Schematic *sch = openSchematic(schematic);
  if (sch == NULL) {
    return 1;
  }

  qDebug() << "*** try to write netlist  :" << netlist;

  QStringList Collect;

  QPlainTextEdit *ErrText = new QPlainTextEdit();  //dummy
  QFile NetlistFile;
  QTextStream   Stream;

  Collect.clear();  // clear list for NodeSets, SPICE components etc.

  NetlistFile.setFileName(netlist);
  if(!NetlistFile.open(QIODevice::WriteOnly)) {
    fprintf(stderr, "Error: Could not load netlist %s\n", netlist.ascii());
    return -1;
  }

  Stream.setDevice(&NetlistFile);
  int SimPorts = sch->prepareNetlist(Stream, Collect, ErrText);

  if(SimPorts < -5) {
    NetlistFile.close();
    QByteArray ba = netlist.toLatin1();
    fprintf(stderr, "Error: Could not prepare netlist %s\n", ba.data());
    /// \todo better handling for error/warnings
    qCritical() << ErrText->toPlainText();
    return 1;
  }

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

  QString SimTime = sch->createNetlist(Stream, SimPorts);
  delete(sch);

  NetlistFile.close();

  return 0;
}

int doPrint(QString schematic, QString printFile,
    QString page, int dpi, QString color, QString orientation)
{
  Schematic *sch = openSchematic(schematic);
  if (sch == NULL) {
    return 1;
  }

  sch->Nodes = &(sch->DocNodes);
  sch->Wires = &(sch->DocWires);
  sch->Diagrams = &(sch->DocDiags);
  sch->Paintings = &(sch->DocPaints);
  sch->Components = &(sch->DocComps);
  sch->reloadGraphs();

  qDebug() << "*** try to print file  :" << printFile;

  // determine filetype
  if (printFile.endsWith(".pdf")) {
    //initial printer
    PrinterWriter *Printer = new PrinterWriter();
    Printer->setFitToPage(true);
    Printer->noGuiPrint(sch, printFile, page, dpi, color, orientation);
  } else {
    ImageWriter *Printer = new ImageWriter("");
    Printer->noGuiPrint(sch, printFile, color);
  }
  return 0;
}

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

        Component *c = (Component* ) e;

        QList<Line *> Lines      = c->Lines;
        QList<struct Arc *> Arcs = c-> Arcs;
        QList<Area *> Rects      = c-> Rects;
        QList<Area *> Ellips     = c-> Ellips;
        QList<Port *> Ports      = c->Ports;
        QList<Text*> Texts       = c->Texts;

        QGraphicsScene *scene = new QGraphicsScene();

        foreach (Line *l, Lines) {
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

        foreach(Area *a, Rects) {
          scene->addRect(a->x, a->y, a->w, a->h, a->Pen, a->Brush);
        }

        foreach(Area *a, Ellips) {
          scene->addEllipse(a->x, a->y, a->w, a->h, a->Pen, a->Brush);
        }

        foreach(Port *p, Ports) {
          scene->addEllipse(p->x-4, p->y-4, 8, 8, QPen(Qt::red));
        }

        foreach(Text *t, Texts) {
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
        compData << "Description; "       + c->Description;
        compData << "Identifier; ``"      + c->Model + "``"; // backticks for reST verbatim
        compData << "Default name; ``"    + c->Name  + "``";
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
        fprintf(stdout, "[%s] %s %s \n", category.toAscii().data(), c->Model.toAscii().data(), file.name().toAscii().data());

        QStringList compProps;
        compProps << "# Note: auto-generated file (changes will be lost on update)";
        compProps << QString("# %1; %2; %3; %4").arg(  "Name", "Value", "Display", "Description");
        foreach(Property *prop, c->Props) {
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
        fprintf(stdout, "[%s] %s %s \n", category.toAscii().data(), c->Model.toAscii().data(), fileProps.name().toAscii().data());
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
      Component *c = (Component* ) e;

      QString qucsEntry = c->save();
      fprintf(stdout, "%s; qucs    ; %s\n", c->Model.toAscii().data(), qucsEntry.toAscii().data());

      // add dummy ports/wires, avoid segfault
      int port = 0;
      foreach (Port *p, c->Ports) {
        Node *n = new Node(0,0);
        n->Name="_net"+QString::number(port);
        p->Connection = n;
        port +=1;
      }

      // skip Subcircuit, segfault, there is nothing to netlist
      if (c->Model == "Sub" or c->Model == ".Opt") {
        fprintf(stdout, "WARNING, qucsator netlist not generated for %s\n\n", c->Model.toAscii().data());
        continue;
      }

      QString qucsatorEntry = c->getNetlist();
      fprintf(stdout, "%s; qucsator; %s\n", c->Model.toAscii().data(), qucsatorEntry.toAscii().data());
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
  qInstallMsgHandler(qucsMessageOutput);
  // apply default settings
  QucsSettings.font = QFont("Helvetica", 12);
  QucsSettings.largeFontSize = 16.0;
  QucsSettings.maxUndo = 20;
  QucsSettings.NodeWiring = 0;

  // initially center the application
  QApplication a(argc, argv);
  QDesktopWidget *d = a.desktop();
  int w = d->width();
  int h = d->height();
  QucsSettings.x = w/8;
  QucsSettings.y = h/8;
  QucsSettings.dx = w*3/4;
  QucsSettings.dy = h*3/4;

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
  var = getenv("QUCS_LIBDIR");
  if(var != NULL) {
	  QucsSettings.LibDir = QString(var);
  }else{
	  QucsSettings.LibDir =      QucsDir.canonicalPath() + "/share/qucs/library/";
  }
  QucsSettings.OctaveDir =   QucsDir.canonicalPath() + "/share/qucs/octave/";
  QucsSettings.ExamplesDir = QucsDir.canonicalPath() + "/share/qucs/docs/examples/";
  QucsSettings.DocDir =      QucsDir.canonicalPath() + "/share/qucs/docs/";

  QucsSettings.Editor = "qucs";
  QucsSettings.QucsHomeDir.setPath(QDir::homeDirPath()+QDir::convertSeparators ("/.qucs"));
  QucsSettings.QucsWorkDir.setPath(QucsSettings.QucsHomeDir.canonicalPath());

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

  var = getenv("OCTAVEBINDIR");
  if(var != NULL)  {
      QucsSettings.OctaveBinDir.setPath(QString(var));
  }
  else  {
#ifdef __MINGW32__
      QucsSettings.OctaveBinDir.setPath(QString("C:/Software/Octave-3.6.4/bin/"));
#else
      QFile octaveExec("/usr/bin/octave");
      if(octaveExec.exists())QucsSettings.OctaveBinDir.setPath(QString("/usr/bin/"));
      QFile octaveExec1("/usr/local/bin/octave");
      if(octaveExec1.exists()) QucsSettings.OctaveBinDir.setPath(QString("/usr/local/bin/"));
#endif
  }
  loadSettings();

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
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

  QTranslator tor( 0 );
  QString lang = QucsSettings.Language;
  if(lang.isEmpty())
    lang = QTextCodec::locale();
  tor.load( QString("qucs_") + lang, QucsSettings.LangDir);
  a.installTranslator( &tor );

  // This seems to be neccessary on a few system to make strtod()
  // work properly !???!
  setlocale (LC_NUMERIC, "C");

  QString inputfile;
  QString outputfile;

  bool netlist_flag = false;
  bool print_flag = false;
  QString page = "A4";
  int dpi = 96;
  QString color = "RGB";
  QString orientation = "portraid";

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
    }
    else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")) {
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
    else if (!strcmp(argv[i], "-i")) {
      inputfile = argv[++i];
    }
    else if (!strcmp(argv[i], "-o")) {
      outputfile = argv[++i];
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
      fprintf(stderr, "Error: Unknown option: %s\n", argv[i]);
      return -1;
    }
  }

  // check operation and its required arguments
  if (netlist_flag and print_flag) {
    fprintf(stderr, "Error: --print and --netlist cannot be used together\n");
    return -1;
  } else if (netlist_flag or print_flag) {
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
    }
  }

  QucsMain = new QucsApp();
  a.setMainWidget(QucsMain);
  
  QucsMain->show();
  int result = a.exec();
  //saveApplSettings(QucsMain);
  return result;
}
