/***************************************************************************
                              schematic_file.cpp
                             --------------------
    begin                : Sat Mar 27 2004
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <deque>
#include <QtCore>
#include <QMessageBox>
#include <QDir>
#include <QDebug>
#include <QStringList>
#include <QPlainTextEdit>
#include "qt_compat.h"
#include <QTextStream>
#include <QList>
#include <QProcess>
#include <QDebug>

#include "qucs.h"
#include "node.h"
#include "schematic_doc.h"
#include "diagrams/diagrams.h"
#include "paintings/paintings.h"
#include "components/spicefile.h"
#include "components/vhdlfile.h"
#include "components/verilogfile.h"
#include "components/libcomp.h"
#include "module.h"
#include "misc.h"
#include "sim/sim.h"
#include "io_trace.h"
#include "command.h"
#include "globals.h"
#include "docfmt.h"
#include "schematic_lang.h"


// Here the subcircuits, SPICE components etc are collected. It must be
// global to also work within the subcircuits.
SubMap FileList;


// -------------------------------------------------------------
// Creates a Qucs file format (without document properties) in the returning
// string. This is used to copy the selected elements into the clipboard.
QString SchematicModel::createClipboardFile()
{
  //something like
  // stringstream s
  // for (i : selectedItems){
  //   lang.print(s, i)
  // }
  // return s.str();

  return "";

#if 0
  int z=0;  // counts selected elements

  QString s("<Qucs Schematic " PACKAGE_VERSION ">\n");

  // Build element document.
  s += "<Components>\n";
  for(auto pc : components()){
    if(pc->isSelected()) {
      QTextStream str(&s);
      incomplete();
      // saveComponent(str, pc);
      s += "\n";
      ++z;
    }
  }
  s += "</Components>\n";

  s += "<Wires>\n";
  for(auto pw : wires()){
    if(pw->isSelected()) {
      z++;
      if(pw->Label) if(!pw->Label->isSelected()) {
	s += pw->save().section('"', 0, 0)+"\"\" 0 0 0>\n";
	continue;
      }
      s += pw->save()+"\n";
    }
  }
  for(auto pn : nodes()) {
    if(pn->Label) if(pn->Label->isSelected()) {
      s += pn->Label->save()+"\n";  z++; 
    }
  }
  s += "</Wires>\n";

  s += "<Diagrams>\n";
  for(auto pd : diagrams()) {
    if(pd->isSelected()) {
      s += pd->save()+"\n";  z++;
    }
  }
  s += "</Diagrams>\n";

  s += "<Paintings>\n";
  for(auto pp : paintings()){
    if(pp->isSelected()){
      if(pp->name().at(0) != '.') {  // subcircuit specific -> do not copy
        s += "<"+pp->save()+">\n";  z++;
      }
    }else{
    }
  }
  s += "</Paintings>\n";

  if(z == 0) return "";   // return empty if no selection

  return s;
#endif
}

// -------------------------------------------------------------
// Only read fields without loading them.
bool SchematicDoc::loadIntoNothing(DocumentStream *stream)
{
  QString Line, cstr;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;
  }

  QMessageBox::critical(0, QObject::tr("Error"),
	QObject::tr("Format Error:\n'Painting' field is not closed!"));
  return false;
}

// -------------------------------------------------------------
// Paste from clipboard. into pe. wtf is pe?
bool SchematicDoc::pasteFromClipboard(DocumentStream *stream, EGPList* pe)
{ untested();
#if 1
  QString Line;

  Line = stream->readLine();
  if(Line.left(16) != "<Qucs Schematic ")   // wrong file type ?
    return false;

  Line = Line.mid(16, Line.length()-17);
  VersionTriplet DocVersion = VersionTriplet(Line);
  if (DocVersion > QucsVersion) { // wrong version number ?
    if (!QucsSettings.IgnoreFutureVersion) {
      QMessageBox::critical(0, QObject::tr("Error"),
                            QObject::tr("Wrong document version: %1").arg(DocVersion.toString()));
      return false;
    }
  }

  // read content in symbol edit mode *************************
  if(isSymbolMode()) {
    incomplete(); // yikes.
    while(!stream->atEnd()) {
      Line = stream->readLine();
      if(Line == "<Components>") {
        if(!loadIntoNothing(stream)) return false; }
      else
      if(Line == "<Wires>") {
        if(!loadIntoNothing(stream)) return false; }
      else
      if(Line == "<Diagrams>") {
        if(!loadIntoNothing(stream)) return false; }
      else
      if(Line == "<Paintings>") {
	PaintingList pl;
	incomplete(); // ignore paintings.
       // if(!DocModel.loadPaintings(stream, &pl)) return false;
      } else {
        QMessageBox::critical(0, QObject::tr("Error"),
		   QObject::tr("Clipboard Format Error:\nUnknown field!"));
        return false;
      }
    }

    return true;
  }

  // read content in schematic edit mode *************************
  SchematicModel x(NULL);
  try{
    incomplete();
//    x.parse(*stream);
  }catch( std::string ){
    incomplete();
//    messagebox?
  }

  // better interface for this kind of stuff?
  assert(!pe->count());
#ifndef USE_SCROLLVIEW
  x.toScene(*scene(), pe);
#endif
  assert(_model);
  _model->merge(x);

#endif
  return true;
}

// -------------------------------------------------------------
int SchematicDoc::saveSymbolCpp (void)
{
  QFileInfo info (docName());
  QString cppfile = info.path () + QDir::separator() + DataSet;
  QFile file (cppfile);

  if (!file.open (QIODevice::WriteOnly)) {
    QMessageBox::critical (0, QObject::tr("Error"),
		   QObject::tr("Cannot save C++ file \"%1\"!").arg(cppfile));
    return -1;
  }

  DocumentStream stream (&file);

  // automatically compute boundings of drawing
  int xmin = INT_MAX;
  int ymin = INT_MAX;
  int xmax = INT_MIN;
  int ymax = INT_MIN;
  int x1, y1, x2, y2;
  int maxNum = 0;
  Painting * pp;

  stream << "  // symbol drawing code\n";
  for (pp = SymbolPaints.first (); pp != 0; pp = SymbolPaints.next ()) {
    if (pp->name() == ".ID ") continue;
    if (pp->name() == ".PortSym ") {
      if (((PortSymbol*)pp)->numberStr.toInt() > maxNum)
	maxNum = ((PortSymbol*)pp)->numberStr.toInt();
      x1 = ((PortSymbol*)pp)->cx_();
      y1 = ((PortSymbol*)pp)->cy_();
      if (x1 < xmin) xmin = x1;
      if (x1 > xmax) xmax = x1;
      if (y1 < ymin) ymin = y1;
      if (y1 > ymax) ymax = y1;
      continue;
    }
    pp->Bounding (x1, y1, x2, y2);
    if (x1 < xmin) xmin = x1;
    if (x2 > xmax) xmax = x2;
    if (y1 < ymin) ymin = y1;
    if (y2 > ymax) ymax = y2;
    stream << "  " << pp->saveCpp () << "\n";
  }

  stream << "\n  // terminal definitions\n";
  for (int i = 1; i <= maxNum; i++) {
    for (pp = SymbolPaints.first (); pp != 0; pp = SymbolPaints.next ()) {
      if (pp->name() == ".PortSym ")
	if (((PortSymbol*)pp)->numberStr.toInt() == i)
	  stream << "  " << pp->saveCpp () << "\n";
    }
  }

  stream << "\n  // symbol boundings\n"
	 << "  x1 = " << xmin << "; " << "  y1 = " << ymin << ";\n"
	 << "  x2 = " << xmax << "; " << "  y2 = " << ymax << ";\n";

  stream << "\n  // property text position\n";
  for (pp = SymbolPaints.first (); pp != 0; pp = SymbolPaints.next ())
    if (pp->name() == ".ID ")
      stream << "  " << pp->saveCpp () << "\n";

  file.close ();
  return 0;
}

// save symbol paintings in JSON format
int SchematicDoc::saveSymbolJSON()
{
  QFileInfo info (docName());
  QString jsonfile = info.path () + QDir::separator()
                   + info.completeBaseName() + "_sym.json";

  qDebug() << "saveSymbolJson for " << jsonfile;

  QFile file (jsonfile);

  if (!file.open (QIODevice::WriteOnly)) {
    QMessageBox::critical (0, QObject::tr("Error"),
		   QObject::tr("Cannot save JSON symbol file \"%1\"!").arg(jsonfile));
    return -1;
  }

  QTextStream stream (&file);

  // automatically compute boundings of drawing
  int xmin = INT_MAX;
  int ymin = INT_MAX;
  int xmax = INT_MIN;
  int ymax = INT_MIN;
  int x1, y1, x2, y2;
  int maxNum = 0;
  Painting * pp;

  stream << "{\n";

  stream << "\"paintings\" : [\n";

  // symbol drawing code"
  for (pp = SymbolPaints.first (); pp != 0; pp = SymbolPaints.next ()) {
    if (pp->name() == ".ID ") continue;
    if (pp->name() == ".PortSym ") {
      if (((PortSymbol*)pp)->numberStr.toInt() > maxNum)
	maxNum = ((PortSymbol*)pp)->numberStr.toInt();
      x1 = ((PortSymbol*)pp)->cx_();
      y1 = ((PortSymbol*)pp)->cy_();
      if (x1 < xmin) xmin = x1;
      if (x1 > xmax) xmax = x1;
      if (y1 < ymin) ymin = y1;
      if (y1 > ymax) ymax = y1;
      continue;
    }
    pp->Bounding (x1, y1, x2, y2);
    if (x1 < xmin) xmin = x1;
    if (x2 > xmax) xmax = x2;
    if (y1 < ymin) ymin = y1;
    if (y2 > ymax) ymax = y2;
    stream << "  " << pp->saveJSON() << "\n";
  }

  // terminal definitions
  //stream << "terminal \n";
  for (int i = 1; i <= maxNum; i++) {
    for (pp = SymbolPaints.first (); pp != 0; pp = SymbolPaints.next ()) {
      if (pp->name() == ".PortSym ")
	if (((PortSymbol*)pp)->numberStr.toInt() == i)
	  stream << "  " << pp->saveJSON () << "\n";
    }
  }

  stream << "],\n"; //end of paintings JSON array

  // symbol boundings
  stream
    << "  \"x1\" : " << xmin << ",\n" << "  \"y1\" : " << ymin << ",\n"
    << "  \"x2\" : " << xmax << ",\n" << "  \"y2\" : " << ymax << ",\n";

  // property text position
  for (pp = SymbolPaints.first (); pp != 0; pp = SymbolPaints.next ())
    if (pp->name() == ".ID ")
      stream << "  " << pp->saveJSON () << "\n";

  stream << "}\n";

  file.close ();
  return 0;


}

// BUG: move to SchematicModel

// -------------------------------------------------------------
void SchematicDoc::saveDocument() const
{
  // QFile file(DocName);
  // if(!file.open(QIODevice::ReadOnly)) {
  //   return -1;
  // }
  QFile file(docName());
  file.open(QIODevice::WriteOnly | QIODevice::Truncate);
  DocumentStream stream(&file);

//  sda a(DocModel, *this);

  // TODO: provide selection GUI
  auto D=docfmt_dispatcher["leg_sch"];
  assert(_root);
  D->save(stream, *_root);
}

// -------------------------------------------------------------
#if 0// moved
bool SchematicDoc::loadProperties(QTextStream *stream)
#endif
// // TODO: move to frame::setParameters
void SchematicDoc::setFrameText(int idx, QString s)
{
  incomplete();
}
#if 0
{
  if(s != FrameText[idx]){
    setChanged(true);
    FrameText[idx] = s;
    misc::convert2Unicode(FrameText[idx]);
  }else{
  }
}
#endif


#if 0
// -------------------------------------------------------------
// // TODO: move to frame::setParameters
bool SchematicModel::loadProperties(QTextStream *stream)
{
  SchematicDoc* d = _doc;
  bool ok = true;
  QString Line, cstr, nstr;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;  // field end ?
    Line = Line.trimmed();
    if(Line.isEmpty()) continue;

    if(Line.at(0) != '<') {
      QMessageBox::critical(0, QObject::tr("Error"),
		QObject::tr("Format Error:\nWrong property field limiter!"));
      return false;
    }
    if(Line.at(Line.length()-1) != '>') {
      QMessageBox::critical(0, QObject::tr("Error"),
		QObject::tr("Format Error:\nWrong property field limiter!"));
      return false;
    }
    Line = Line.mid(1, Line.length()-2);   // cut off start and end character

    cstr = Line.section('=',0,0);    // property type
    nstr = Line.section('=',1,1);    // property value
         if(cstr == "View") {
		d->ViewX1 = nstr.section(',',0,0).toInt(&ok); if(ok) {
		d->ViewY1 = nstr.section(',',1,1).toInt(&ok); if(ok) {
		d->ViewX2 = nstr.section(',',2,2).toInt(&ok); if(ok) {
		d->ViewY2 = nstr.section(',',3,3).toInt(&ok); if(ok) {
		d->Scale  = nstr.section(',',4,4).toDouble(&ok); if(ok) {
		d->tmpViewX1 = nstr.section(',',5,5).toInt(&ok); if(ok)
		d->tmpViewY1 = nstr.section(',',6,6).toInt(&ok); }}}}} }
    else if(cstr == "Grid") {
		d->GridX = nstr.section(',',0,0).toInt(&ok); if(ok) {
		d->GridY = nstr.section(',',1,1).toInt(&ok); if(ok) {
		if(nstr.section(',',2,2).toInt(&ok) == 0) d->GridOn = false;
		else d->GridOn = true; }} }
    else if(cstr == "DataSet") d->DataSet = nstr;
    else if(cstr == "DataDisplay") d->DataDisplay = nstr;
    else if(cstr == "OpenDisplay")
		if(nstr.toInt(&ok) == 0) d->SimOpenDpl = false;
		else d->SimOpenDpl = true;
    else if(cstr == "Script") d->Script = nstr;
    else if(cstr == "RunScript")
		if(nstr.toInt(&ok) == 0) d->SimRunScript = false;
		else d->SimRunScript = true;
    else if(cstr == "showFrame")
		d->setFrameType( nstr.at(0).toLatin1() - '0');
    else if(cstr == "FrameText0") d->setFrameText(0, nstr);
    else if(cstr == "FrameText1") d->setFrameText(1, nstr);
    else if(cstr == "FrameText2") d->setFrameText(2, nstr);
    else if(cstr == "FrameText3") d->setFrameText(3, nstr);
    else {
      QMessageBox::critical(0, QObject::tr("Error"),
	   QObject::tr("Format Error:\nUnknown property: ")+cstr);
      return false;
    }
    if(!ok) {
      QMessageBox::critical(0, QObject::tr("Error"),
	   QObject::tr("Format Error:\nNumber expected in property field!"));
      return false;
    }
  }

  QMessageBox::critical(0, QObject::tr("Error"),
               QObject::tr("Format Error:\n'Property' field is not closed!"));
  return false;
}
#endif

// ---------------------------------------------------
// Inserts a component without performing logic for wire optimization.
#if 0
void SchematicDoc::simpleInsertComponent(Component *c)
{
#if 0
  // assert(&_doc->components() == &components());
  // assert(&_doc->nodes() == &nodes());
  int x, y;
  // connect every node of component
  for(auto pp : c->Ports){
    int x=pp->x+c->cx_();
    int y=pp->y+c->cy_();
    qDebug() << c->label() << "port" << x << y;

    // check if new node lies upon existing node
    for(auto pn_ : nodes()){
      pn = pn_;
      if(pn->cx_() == x) if(pn->cy_() == y) {
// 	if (!pn->DType.isEmpty()) {
// 	  pp->Type = pn->DType;
// 	}
// 	if (!pp->Type.isEmpty()) {
// 	  pn->DType = pp->Type;
// 	}
	break;
      }
    }

    if(!pn) {
      // create new node, if no existing one lies at this position
      pn = new Node(x, y);
      nodes().append(pn);
    }else{
    }

    pn->Connections.append(c);  // connect schematic node to component node
    if (!pp->Type.isEmpty()) {
//      pn->DType = pp->Type;
    }

    pp->Connection = pn;  // connect component node to schematic node
  }

  components().append(c);
#endif
}
#endif

// todo: proper string processing
static std::string find_type_in_string(QString& Line)
{
  Line = Line.trimmed();
  if(Line.at(0) != '<') {
    QMessageBox::critical(0, QObject::tr("Error"),
			QObject::tr("Format Error:\nWrong line start!"));
    incomplete();
    // throw exception_CS?
  //  return 0;
  }

  QString cstr = Line.section (' ',0,0); // component type
  cstr.remove (0,1);    // remove leading "<"

  return cstr.toLatin1().data();
}

// -------------------------------------------------------------

/*!
 * \brief SchematicDoc::loadDocument tries to load a schematic document.
 * \return true/false in case of success/failure
 */
bool SchematicDoc::loadDocument()
{itested();
  QFile file(docName());
  qDebug() << "opening" << docName();
  if(!file.open(QIODevice::ReadOnly)) { untested();
    /// \todo implement unified error/warning handling GUI and CLI
    if (QucsMain)
      QMessageBox::critical(0, QObject::tr("Error"),
                 QObject::tr("Cannot load document: ")+docName());
    else
      qCritical() << "SchematicDoc::loadDocument:"
                  << QObject::tr("Cannot load document: ")+docName();
    return false;
  }else{
    assert(_model);
    _model->setFileInfo(docName());
    loadDocument(file);

#ifndef USE_SCROLLVIEW
    QGraphicsScene& s=*scene();
//    DocModel.toScene(s);
#endif
    return true;
  }
}

// -------------------------------------------------------------
// Creates a Qucs file format (without document properties) in the returning
// string. This is used to save state for undo operation.
QString SchematicDoc::createUndoString(char Op)
{
  incomplete(); // use undostack.
  return QString("obsolete");
#if 0
  Painting *pp;

  // Build element document.
  QString s = "  \n";
  s.replace(0,1,Op);
  for(auto pc : components()){
    QTextStream str(&s);
    incomplete();
    // SchematicModel::saveComponent(str, pc);
    s += "\n";
  }
  s += "</>\n";  // short end flag

  for(auto pw : wires()){
    s += pw->save()+"\n";
  }
  // save all labeled nodes as wires
#if 0
  for(auto pn : nodes()) {
    if(pn->Label) s += pn->Label->save()+"\n";
  }
#endif
  s += "</>\n";

  for(auto pd : diagrams()){
    s += pd->save()+"\n";
  }
  s += "</>\n";

  for(pp = paintings().first(); pp != 0; pp = paintings().next())
    s += "<"+pp->save()+">\n";
  s += "</>\n";

  return s;
#endif
}

// -------------------------------------------------------------
// Same as "createUndoString(char Op)" but for symbol edit mode.
QString SchematicDoc::createSymbolUndoString(char Op)
{
  incomplete(); // use undostack.
  return QString("obsolete");
#if 0
  Painting *pp;

  // Build element document.
  QString s = "  \n";
  s.replace(0,1,Op);
  s += "</>\n";  // short end flag for components
  s += "</>\n";  // short end flag for wires
  s += "</>\n";  // short end flag for diagrams

  for(pp = symbolPaintings().first(); pp != 0; pp = symbolPaintings().next())
    s += "<"+pp->save()+">\n";
  s += "</>\n";

  return s;
#endif
}

class ModelStream : public QTextStream {
public:
  explicit ModelStream(QString /* BUG const */ * filename, QIODevice::OpenModeFlag flag) :
    QTextStream(filename, flag){}

};

// exception.h? ErrorCantParse??
class ParseError : public std::exception{
};

// -------------------------------------------------------------
// Is quite similiar to "loadDocument()" but with less error checking.
// Abused for "undo" function.
bool SchematicDoc::rebuild(QString *s)
{
  incomplete(); // obsolete

#if 0
  DocModel.clear();

  QString Line;
  qDebug() << "rebuild. opening" << *s;
  DocumentStream stream(s, QIODevice::ReadOnly);
  Line = stream.readLine();  // skip identity byte

  // read content *************************
  DocModel.parse(stream);;
#endif

  return true;
}

// -------------------------------------------------------------
// Same as "rebuild(QString *s)" but for symbol edit mode.
bool SchematicDoc::rebuildSymbol(QString *s)
{
  incomplete();
#if 0
  symbolPaintings().clear();	// delete whole document

  QString Line;
  qDebug() << "rebuild. opening" << *s;
  QTextStream stream(s, QIODevice::ReadOnly);
  Line = stream.readLine();  // skip identity byte

  // read content *************************
  Line = stream.readLine();  // skip components
  Line = stream.readLine();  // skip wires
  Line = stream.readLine();  // skip diagrams
  if(!symbolPaintings().load(&stream)){
    return false;
  }

#endif
  return true;
}


// ***************************************************************
// *****                                                     *****
// *****             Functions to create netlist             *****
// *****                                                     *****
// ***************************************************************

void createNodeSet(QStringList& Collect, int& countInit,
			      Conductor *pw, Node *p1)
{
  incomplete(); // obsolete.
  if(pw->Label)
    if(!pw->Label->initValue.isEmpty())
      Collect.append("NodeSet:NS" + QString::number(countInit++) + " " +
                     p1->name() + " U=\"" + pw->Label->initValue + "\"");
}

// ----------------------------------------------------------
// Check whether this file is a qucs file and whether it is an subcircuit.
// It returns the number of subcircuit ports.
int SchematicDoc::testFile(const QString& DocName)
{
  incomplete(); // not SchematicDoc.
  QFile file(DocName);
  if(!file.open(QIODevice::ReadOnly)) {
    return -1;
  }else{
  }

  QString Line;
  // .........................................
  // To strongly speed up the file read operation the whole file is
  // read into the memory in one piece.
  QTextStream ReadWhole(&file);
  QString FileString = ReadWhole.readAll();
  file.close();
  qDebug() << "ReadWhole opening" << FileString;
  QTextStream stream(&FileString, QIODevice::ReadOnly);


  // read header ........................
  do {
    if(stream.atEnd()) {
      file.close();
      return -2;
    }
    Line = stream.readLine();
    Line = Line.trimmed();
  } while(Line.isEmpty());

  if(Line.left(16) != "<Qucs Schematic ") {  // wrong file type ?
    file.close();
    return -3;
  }

  Line = Line.mid(16, Line.length()-17);
  VersionTriplet DocVersion = VersionTriplet(Line);
  if (DocVersion > QucsVersion) { // wrong version number ?
      if (!QucsSettings.IgnoreFutureVersion) {
          file.close();
          return -4;
      }
    //file.close();
    //return -4;
  }

  // read content ....................
  while(!stream.atEnd()) {
    Line = stream.readLine();
    if(Line == "<Components>") break;
  }

  int z=0;
  while(!stream.atEnd()) {
    Line = stream.readLine();
    if(Line == "</Components>") {
      file.close();
      return z;       // return number of ports
    }

    Line = Line.trimmed();
    QString s = Line.section(' ',0,0);    // component type
    if(s == "<Port") z++;
  }
  return -5;  // component field not closed
}

// ---------------------------------------------------
// Collects the signal names for digital simulations.
void SchematicModel::collectDigitalSignals(void)
{
  incomplete();
// Node *pn=nullptr;
//
//  for(pn = nodes().first(); pn != 0; pn = nodes().next()) {
//    DigMap::Iterator it = Signals.find(pn->name());
//    if(it == Signals.end()) { // avoid redeclaration of signal
//      Signals.insert(pn->name(), DigSignal(pn->name(), pn->DType));
//    } else if (!pn->DType.isEmpty()) {
//      it.value().Type = pn->DType;
//    }
//  }
}

// ---------------------------------------------------
// Propagates the given node to connected component ports.
// obsolete.
void SchematicModel::propagateNode(Node *pn) const
{
}

#include <iostream>

/*!
 * \brief SchematicDoc::throughAllComps
 * Goes through all schematic components and allows special component
 * handling, e.g. like subcircuit netlisting.
 * \param stream is a pointer to the text stream used to collect the netlist
 * \param countInit is the reference to a counter for nodesets (initial conditions)
 * \param Collect is the reference to a list of collected nodesets
 * \param ErrText is pointer to the QPlainTextEdit used for error messages
 * \param NumPorts counter for the number of ports
 * \return true in case of success (false otherwise)
 */

// ---------------------------------------------------
bool SchematicModel::createLibNetlist(DocumentStream& stream, QPlainTextEdit
    *ErrText, int NumPorts, NetLang const& nl)
{ untested();
  incomplete(); // wrong place.
  qDebug() << "createLibNetlist";
  bool isAnalog=true;
  bool isVerilog=false;
  DigMap Signals; //?!
  int countInit = 0;
  QStringList Collect;
  Collect.clear();
  FileList.clear();
  Signals.clear();
  // Apply node names and collect subcircuits and file include
  bool creatingLib = true;
  if(!giveNodeNames(stream, countInit, Collect, ErrText, NumPorts, creatingLib, nl)) {
    creatingLib = false;
    return false;
  }else{
  }
  creatingLib = false;

  // Marking start of actual top-level subcircuit
  QString c;
  if(!isAnalog) {
    if (isVerilog)
      c = "///";
    else
      c = "---";
  }
  else c = "###";
  stream << "\n" << c << " TOP LEVEL MARK " << c << "\n";

  // Emit subcircuit components
  incomplete();
//  createSubNetlistPlain(stream, ErrText, NumPorts, creatingLib);

  Signals.clear();  // was filled in "giveNodeNames()"
  return true;
} // createLibNetlist

//#define VHDL_SIGNAL_TYPE "bit"
//#define VHDL_LIBRARIES   ""
static const std::string VHDL_SIGNAL_TYPE("std_logic");
static const std::string VHDL_LIBRARIES("\nlibrary ieee;\nuse ieee.std_logic_1164.all;\n");

// ---------------------------------------------------
void SchematicModel::createSubNetlistPlain(stream_t& str, QPlainTextEdit *ErrText,
int NumPorts, bool creatingLib, NetLang const& )
{
  incomplete(); // obsolete.
}
// ---------------------------------------------------
// Write the netlist as subcircuit to the text stream 'stream'.
// BUG: not here.
bool SchematicModel::createSubNetlist(stream_t& stream, int& countInit,
                     QStringList& Collect, QPlainTextEdit *ErrText, int NumPorts,
		  bool creatingLib, NetLang const& nl)
{ untested();
  (void) countInit;
  (void) Collect;
  (void) creatingLib;
  DigMap Signals; //??
//  int Collect_count = Collect.count();   // position for this subcircuit

  // TODO: NodeSets have to be put into the subcircuit block.
  qDebug() << "giveNodeNames" << NumPorts << "\n";
//  if(!giveNodeNames(stream, countInit, Collect, ErrText, NumPorts, false, nl)){
//    fprintf(stderr, "Error giving NodeNames in createSubNetlist\n");
//    return false;
//  }

/*  Example for TODO
      for(it = Collect.at(Collect_count); it != Collect.end(); )
      if((*it).left(4) == "use ") {  // output all subcircuit uses
        (*stream) << (*it);
        it = Collect.remove(it);
      }
      else it++;*/

  // Emit subcircuit components
  incomplete();
  // BUG legacy
  createSubNetlistPlain(stream, ErrText, NumPorts, false, nl);

  // Signals.clear();  // was filled in "giveNodeNames()"
  return true;
}

// vim:ts=8:sw=2:noet
