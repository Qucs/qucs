/***************************************************************************
                                qucsfile.cpp
                               --------------
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

#include "qucsfile.h"

#include "node.h"
#include "diagrams/diagrams.h"
#include "paintings/paintings.h"
#include "components/spicefile.h"
#include "components/libcomp.h"
#include "qucsdoc.h"
#include "main.h"

#include <qmessagebox.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qregexp.h>
#include <qprocess.h>
#include <qtextedit.h>


extern QDir QucsWorkDir;
QStringList StringList;


QucsFile::QucsFile(QucsDoc *p)
{
  Doc = p;

  Wires  = &(p->DocWires);
  Nodes  = &(p->DocNodes);
  Comps  = &(p->DocComps);
  Diags  = &(p->DocDiags);
  Paints = &(p->DocPaints);
  SymbolPaints = &(p->SymbolPaints);
}

QucsFile::~QucsFile()
{
}

// -------------------------------------------------------------
// Creates a Qucs file format (without document properties) in the returning
// string. This is used to copy the selected elements into the clipboard.
QString QucsFile::createClipboardFile()
{
  int z=0;  // counts selected elements
  Wire *pw;
  Diagram *pd;
  Painting *pp;
  Component *pc;

  QString s("<Qucs Schematic " PACKAGE_VERSION ">\n");

  // Build element document.
  s += "<Components>\n";
  for(pc = Doc->Comps->first(); pc != 0; pc = Doc->Comps->next())
    if(pc->isSelected) {
      s += pc->save()+"\n";  z++; }
  s += "</Components>\n";

  s += "<Wires>\n";
  for(pw = Doc->Wires->first(); pw != 0; pw = Doc->Wires->next())
    if(pw->isSelected) {
      z++;
      if(pw->Label) if(!pw->Label->isSelected) {
	s += pw->save().section('"', 0, 0)+"\"\" 0 0 0>\n";
	continue;
      }
      s += pw->save()+"\n";
    }
  for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next())
    if(pn->Label) if(pn->Label->isSelected) {
      s += pn->Label->save()+"\n";  z++; }
  s += "</Wires>\n";

  s += "<Diagrams>\n";
  for(pd = Doc->Diags->first(); pd != 0; pd = Doc->Diags->next())
    if(pd->isSelected) {
      s += pd->save()+"\n";  z++; }
  s += "</Diagrams>\n";

  s += "<Paintings>\n";
  for(pp = Doc->Paints->first(); pp != 0; pp = Doc->Paints->next())
    if(pp->isSelected)
      if(pp->Name.at(0) != '.') {  // subcircuit specific -> do not copy
        s += "<"+pp->save()+">\n";  z++; }
  s += "</Paintings>\n";

  if(z == 0) return "";   // return empty if no selection

  return s;
}

// -------------------------------------------------------------
// Only read fields without loading them.
bool QucsFile::loadIntoNothing(QTextStream *stream)
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
// Paste from clipboard.
bool QucsFile::pasteFromClipboard(QTextStream *stream, QPtrList<Element> *pe)
{
  QString Line;

  Line = stream->readLine();
  if(Line.left(16) != "<Qucs Schematic ")   // wrong file type ?
    return false;

  QString s = PACKAGE_VERSION;
  Line = Line.mid(16, Line.length()-17);
  if(Line != s) {  // wrong version number ?
    QMessageBox::critical(0, QObject::tr("Error"),
                 QObject::tr("Wrong document version: ")+Line);
    return false;
  }

  // read content in symbol edit mode *************************
  if(Doc->symbolMode) {
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
        if(!loadPaintings(stream, (QPtrList<Painting>*)pe)) return false; }
      else {
        QMessageBox::critical(0, QObject::tr("Error"),
		   QObject::tr("Clipboard Format Error:\nUnknown field!"));
        return false;
      }
    }

    return true;
  }

  // read content in schematic edit mode *************************
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line == "<Components>") {
      if(!loadComponents(stream, (QPtrList<Component>*)pe)) return false; }
    else
    if(Line == "<Wires>") {
      if(!loadWires(stream, pe)) return false; }
    else
    if(Line == "<Diagrams>") {
      if(!loadDiagrams(stream, (QPtrList<Diagram>*)pe)) return false; }
    else
    if(Line == "<Paintings>") {
      if(!loadPaintings(stream, (QPtrList<Painting>*)pe)) return false; }
    else {
      QMessageBox::critical(0, QObject::tr("Error"),
		   QObject::tr("Clipboard Format Error:\nUnknown field!"));
      return false;
    }
  }

  return true;
}

// -------------------------------------------------------------
// Returns the number of subcircuit ports.
int QucsFile::save()
{
  QFile file(Doc->DocName);
  if(!file.open(IO_WriteOnly)) {
    QMessageBox::critical(0, QObject::tr("Error"),
				QObject::tr("Cannot save document!"));
    return -1;
  }

  QTextStream stream(&file);

  stream << "<Qucs Schematic " << PACKAGE_VERSION << ">\n";

  stream << "<Properties>\n";
  if(Doc->symbolMode) {
    stream << "  <View=" << Doc->tmpViewX1<<","<<Doc->tmpViewY1<<","
			 << Doc->tmpViewX2<<","<<Doc->tmpViewY2<< ",";
    stream <<Doc->tmpScale<<","<<Doc->tmpPosX<<","<<Doc->tmpPosY << ">\n";
  }
  else {
    stream << "  <View=" << Doc->ViewX1<<","<<Doc->ViewY1<<","
			 << Doc->ViewX2<<","<<Doc->ViewY2<< ",";
    stream << Doc->Scale <<","<<Doc->PosX<<","<<Doc->PosY << ">\n";
  }
  stream << "  <Grid=" << Doc->GridX<<","<<Doc->GridY<<","
			<< Doc->GridOn << ">\n";
  stream << "  <DataSet=" << Doc->DataSet << ">\n";
  stream << "  <DataDisplay=" << Doc->DataDisplay << ">\n";
  stream << "  <OpenDisplay=" << Doc->SimOpenDpl << ">\n";
  stream << "</Properties>\n";

  Painting *pp;
  stream << "<Symbol>\n";     // save all paintings for symbol
  for(pp = SymbolPaints->first(); pp != 0; pp = SymbolPaints->next())
    stream << "  <" << pp->save() << ">\n";
  stream << "</Symbol>\n";

  stream << "<Components>\n";    // save all components
  for(Component *pc = Comps->first(); pc != 0; pc = Comps->next())
    stream << "  " << pc->save() << "\n";
  stream << "</Components>\n";

  stream << "<Wires>\n";    // save all wires
  for(Wire *pw = Wires->first(); pw != 0; pw = Wires->next())
    stream << "  " << pw->save() << "\n";

  // save all labeled nodes as wires
  for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next())
    if(pn->Label) stream << "  " << pn->Label->save() << "\n";
  stream << "</Wires>\n";

  stream << "<Diagrams>\n";    // save all diagrams
  for(Diagram *pd = Diags->first(); pd != 0; pd = Diags->next())
    stream << "  " << pd->save() << "\n";
  stream << "</Diagrams>\n";

  stream << "<Paintings>\n";     // save all paintings
  for(pp = Paints->first(); pp != 0; pp = Paints->next())
    stream << "  <" << pp->save() << ">\n";
  stream << "</Paintings>\n";

  file.close();
  return 0;
}

// -------------------------------------------------------------
bool QucsFile::loadProperties(QTextStream *stream)
{
  bool ok = true;
  QString Line, cstr, nstr;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;  // field end ?
    Line = Line.stripWhiteSpace();
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
		Doc->ViewX1 = nstr.section(',',0,0).toInt(&ok); if(ok) {
		Doc->ViewY1 = nstr.section(',',1,1).toInt(&ok); if(ok) {
		Doc->ViewX2 = nstr.section(',',2,2).toInt(&ok); if(ok) {
		Doc->ViewY2 = nstr.section(',',3,3).toInt(&ok); if(ok) {
		Doc->Scale  = nstr.section(',',4,4).toDouble(&ok); if(ok) {
		Doc->PosX   = nstr.section(',',5,5).toInt(&ok); if(ok) {
		Doc->PosY   = nstr.section(',',6,6).toInt(&ok); }}}}}} }
    else if(cstr == "Grid") {
		Doc->GridX = nstr.section(',',0,0).toInt(&ok); if(ok) {
		Doc->GridY = nstr.section(',',1,1).toInt(&ok); if(ok) {
		if(nstr.section(',',2,2).toInt(&ok) == 0) Doc->GridOn = false;
		else Doc->GridOn = true; }} }
    else if(cstr == "DataSet") Doc->DataSet = nstr;
    else if(cstr == "DataDisplay") Doc->DataDisplay = nstr;
    else if(cstr == "OpenDisplay")
		if(nstr.toInt(&ok) == 0) Doc->SimOpenDpl = false;
		else Doc->SimOpenDpl = true;
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

// ---------------------------------------------------
// Inserts a component without performing logic for wire optimization.
void QucsFile::simpleInsertComponent(Component *c)
{
  Node *pn;
  int x, y;
  // connect every node of component
  for(Port *pp = c->Ports.first(); pp != 0; pp = c->Ports.next()) {
    x = pp->x+c->cx;
    y = pp->y+c->cy;

    // check if new node lies upon existing node
    for(pn = Nodes->first(); pn != 0; pn = Nodes->next())  // check every node
      if(pn->cx == x) if(pn->cy == y)  break;

    if(pn == 0) { // create new node, if no existing one lies at this position
      pn = new Node(x, y);
      Nodes->append(pn);
    }
    pn->Connections.append(c);  // connect schematic node to component node

    pp->Connection = pn;  // connect component node to schematic node
  }

  Comps->append(c);
}

// -------------------------------------------------------------
bool QucsFile::loadComponents(QTextStream *stream, QPtrList<Component> *List)
{
  QString Line, cstr;
  Component *c;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;
    Line = Line.stripWhiteSpace();
    if(Line.isEmpty()) continue;

    c = getComponentFromName(Line);
    if(!c) return false;

    if(List) {  // "paste" ?
      int z;
      for(z=c->Name.length()-1; z>=0; z--) // cut off number of component name
        if(!c->Name.at(z).isDigit()) break;
      c->Name = c->Name.left(z+1);
      List->append(c);
    }
    else  simpleInsertComponent(c);
  }

  QMessageBox::critical(0, QObject::tr("Error"),
	   QObject::tr("Format Error:\n'Component' field is not closed!"));
  return false;
}

// -------------------------------------------------------------
// Inserts a wire without performing logic for optimizing.
void QucsFile::simpleInsertWire(Wire *pw)
{
  Node *pn;
  // check if first wire node lies upon existing node
  for(pn = Nodes->first(); pn != 0; pn = Nodes->next())  // check every node
    if(pn->cx == pw->x1) if(pn->cy == pw->y1) break;

  if(!pn) {   // create new node, if no existing one lies at this position
    pn = new Node(pw->x1, pw->y1);
    Nodes->append(pn);
  }

  if(pw->x1 == pw->x2) if(pw->y1 == pw->y2) {
    pn->Label = pw->Label;   // wire with length zero are just node labels
    if (pn->Label) {
      pn->Label->Type = isNodeLabel;
      pn->Label->pOwner = pn;
    }
    delete pw;           // delete wire because this is not a wire
    return;
  }
  pn->Connections.append(pw);  // connect schematic node to component node
  pw->Port1 = pn;

  // check if second wire node lies upon existing node
  for(pn = Nodes->first(); pn != 0; pn = Nodes->next())  // check every node
    if(pn->cx == pw->x2) if(pn->cy == pw->y2) break;

  if(!pn) {   // create new node, if no existing one lies at this position
    pn = new Node(pw->x2, pw->y2);
    Nodes->append(pn);
  }
  pn->Connections.append(pw);  // connect schematic node to component node
  pw->Port2 = pn;

  Wires->append(pw);
}

// -------------------------------------------------------------
bool QucsFile::loadWires(QTextStream *stream, QPtrList<Element> *List)
{
  Wire *w;
  QString Line;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;
    Line = Line.stripWhiteSpace();
    if(Line.isEmpty()) continue;

    // (Node*)4 =  move all ports (later on)
    w = new Wire(0,0,0,0, (Node*)4,(Node*)4);
    if(!w->load(Line)) {
      QMessageBox::critical(0, QObject::tr("Error"),
		QObject::tr("Format Error:\nWrong 'wire' line format!"));
      delete w;
      return false;
    }
    if(List) {
      if(w->x1 == w->x2) if(w->y1 == w->y2) if(w->Label) {
	w->Label->Type = isMovingLabel;
	List->append(w->Label);
	delete w;
	continue;
      }
      List->append(w);
      if(w->Label)  List->append(w->Label);
    }
    else simpleInsertWire(w);
  }

  QMessageBox::critical(0, QObject::tr("Error"),
		QObject::tr("Format Error:\n'Wire' field is not closed!"));
  return false;
}

// -------------------------------------------------------------
bool QucsFile::loadDiagrams(QTextStream *stream, QPtrList<Diagram> *List)
{
  Diagram *d;
  QString Line, cstr;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;
    Line = Line.stripWhiteSpace();
    if(Line.isEmpty()) continue;

    cstr = Line.section(' ',0,0);    // diagram type
         if(cstr == "<Rect") d = new RectDiagram();
    else if(cstr == "<Polar") d = new PolarDiagram();
    else if(cstr == "<Tab") d = new TabDiagram();
    else if(cstr == "<Smith") d = new SmithDiagram();
    else if(cstr == "<ySmith") d = new SmithDiagram(0,0,false);
    else if(cstr == "<PS") d = new PSDiagram();
    else if(cstr == "<SP") d = new PSDiagram(0,0,false);
    else if(cstr == "<Rect3D") d = new Rect3DDiagram();
    else if(cstr == "<Curve") d = new CurveDiagram();
    else if(cstr == "<Time") d = new TimingDiagram();
    else if(cstr == "<Truth") d = new TruthDiagram();
    else {
      QMessageBox::critical(0, QObject::tr("Error"),
		   QObject::tr("Format Error:\nUnknown diagram!"));
      return false;
    }

    if(!d->load(Line, stream)) {
      QMessageBox::critical(0, QObject::tr("Error"),
		QObject::tr("Format Error:\nWrong 'diagram' line format!"));
      delete d;
      return false;
    }
    List->append(d);
  }

  QMessageBox::critical(0, QObject::tr("Error"),
	       QObject::tr("Format Error:\n'Diagram' field is not closed!"));
  return false;
}

// -------------------------------------------------------------
bool QucsFile::loadPaintings(QTextStream *stream, QPtrList<Painting> *List)
{
  Painting *p=0;
  QString Line, cstr;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;

    Line = Line.stripWhiteSpace();
    if(Line.isEmpty()) continue;
    if( (Line.at(0) != '<') || (Line.at(Line.length()-1) != '>')) {
      QMessageBox::critical(0, QObject::tr("Error"),
	QObject::tr("Format Error:\nWrong 'painting' line delimiter!"));
      return false;
    }
    Line = Line.mid(1, Line.length()-2);  // cut off start and end character

    cstr = Line.section(' ',0,0);    // painting type
         if(cstr == "Line") p = new GraphicLine();
    else if(cstr == "EArc") p = new EllipseArc();
    else if(cstr == ".PortSym") p = new PortSymbol();
    else if(cstr == ".ID") p = new ID_Text();
    else if(cstr == "Text") p = new GraphicText();
    else if(cstr == "Rectangle") p = new Rectangle();
    else if(cstr == "Arrow") p = new Arrow();
    else if(cstr == "Ellipse") p = new Ellipse();
    else {
      QMessageBox::critical(0, QObject::tr("Error"),
		QObject::tr("Format Error:\nUnknown painting!"));
      return false;
    }

    if(!p->load(Line)) {
      QMessageBox::critical(0, QObject::tr("Error"),
	QObject::tr("Format Error:\nWrong 'painting' line format!"));
      delete p;
      return false;
    }
    List->append(p);
  }

  QMessageBox::critical(0, QObject::tr("Error"),
	QObject::tr("Format Error:\n'Painting' field is not closed!"));
  return false;
}

// -------------------------------------------------------------
bool QucsFile::load()
{
  QFile file(Doc->DocName);
  if(!file.open(IO_ReadOnly)) {
    QMessageBox::critical(0, QObject::tr("Error"),
                 QObject::tr("Cannot load document: ")+Doc->DocName);
    return false;
  }

  QString Line;
  QTextStream stream(&file);

  // read header **************************
  do {
    if(stream.atEnd()) {
      file.close();
      return true;
    }

    Line = stream.readLine();
  } while(Line.isEmpty());

  if(Line.left(16) != "<Qucs Schematic ") {  // wrong file type ?
    file.close();
    QMessageBox::critical(0, QObject::tr("Error"),
 		 QObject::tr("Wrong document type: ")+Doc->DocName);
    return false;
  }

  QString s = PACKAGE_VERSION;
  s.remove('.');
  Line = Line.mid(16, Line.length()-17);
  Line.remove('.');
  if(Line > s) {  // wrong version number ? (only backward compatible)
    file.close();
    QMessageBox::critical(0, QObject::tr("Error"),
		 QObject::tr("Wrong document version: ")+Line);
    return false;
  }

  // read content *************************
  while(!stream.atEnd()) {
    Line = stream.readLine();
    Line = Line.stripWhiteSpace();
    if(Line.isEmpty()) continue;

    if(Line == "<Symbol>") {
      if(!loadPaintings(&stream, SymbolPaints)) {
	file.close();
	return false;
      }
    }
    else
    if(Line == "<Properties>") {
      if(!loadProperties(&stream)) { file.close(); return false; } }
    else
    if(Line == "<Components>") {
      if(!loadComponents(&stream)) { file.close(); return false; } }
    else
    if(Line == "<Wires>") {
      if(!loadWires(&stream)) { file.close(); return false; } }
    else
    if(Line == "<Diagrams>") {
      if(!loadDiagrams(&stream, Diags)) { file.close(); return false; } }
    else
    if(Line == "<Paintings>") {
      if(!loadPaintings(&stream, Paints)) { file.close(); return false; } }
    else {
      QMessageBox::critical(0, QObject::tr("Error"),
		   QObject::tr("File Format Error:\nUnknown field!"));
      file.close();
      return false;
    }
  }

  file.close();
  return true;
}

// -------------------------------------------------------------
// Creates a Qucs file format (without document properties) in the returning
// string. This is used to save state for undo operation.
QString QucsFile::createUndoString(char Op)
{
  Wire *pw;
  Diagram *pd;
  Painting *pp;
  Component *pc;

  // Build element document.
  QString s = "  \n";
  s.at(0) = Op;
  for(pc = Comps->first(); pc != 0; pc = Comps->next())
    s += pc->save()+"\n";
  s += "</>\n";  // short end flag

  for(pw = Wires->first(); pw != 0; pw = Wires->next())
    s += pw->save()+"\n";
  // save all labeled nodes as wires
  for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next())
    if(pn->Label) s += pn->Label->save()+"\n";
  s += "</>\n";

  for(pd = Diags->first(); pd != 0; pd = Diags->next())
    s += pd->save()+"\n";
  s += "</>\n";

  for(pp = Paints->first(); pp != 0; pp = Paints->next())
    s += "<"+pp->save()+">\n";
  s += "</>\n";

  return s;
}

// -------------------------------------------------------------
// Same as "createUndoString(char Op)" but for symbol edit mode.
QString QucsFile::createSymbolUndoString(char Op)
{
  Painting *pp;

  // Build element document.
  QString s = "  \n";
  s.at(0) = Op;
  s += "</>\n";  // short end flag for components
  s += "</>\n";  // short end flag for wires
  s += "</>\n";  // short end flag for diagrams

  for(pp = SymbolPaints->first(); pp != 0; pp = SymbolPaints->next())
    s += "<"+pp->save()+">\n";
  s += "</>\n";

  return s;
}

// -------------------------------------------------------------
// Is quite similiar to "load()" but with less error checking.
// Used for "undo" function.
bool QucsFile::rebuild(QString *s)
{
  Wires->clear();	// delete whole document
  Nodes->clear();
  Comps->clear();
  Diags->clear();
  Paints->clear();

  QString Line;
  QTextStream stream(s, IO_ReadOnly);
  Line = stream.readLine();  // skip identity byte

  // read content *************************
  if(!loadComponents(&stream))  return false;
  if(!loadWires(&stream))  return false;
  if(!loadDiagrams(&stream, Diags))  return false;
  if(!loadPaintings(&stream, Paints)) return false;

  return true;
}

// -------------------------------------------------------------
// Same as "rebuild(QString *s)" but for symbol edit mode.
bool QucsFile::rebuildSymbol(QString *s)
{
  SymbolPaints->clear();	// delete whole document

  QString Line;
  QTextStream stream(s, IO_ReadOnly);
  Line = stream.readLine();  // skip identity byte

  // read content *************************
  Line = stream.readLine();  // skip components
  Line = stream.readLine();  // skip wires
  Line = stream.readLine();  // skip diagrams
  if(!loadPaintings(&stream, SymbolPaints)) return false;

  return true;
}


// ***************************************************************
// *****                                                     *****
// *****             Functions to create netlist             *****
// *****                                                     *****
// ***************************************************************

void QucsFile::createNodeSet(QStringList& Collect, int& countInit,
                             Conductor *pw, Node *p1)
{
  if(pw->Label)
    if(!pw->Label->initValue.isEmpty())
      Collect.append("NodeSet:NS" + QString::number(countInit++) + " " +
                     p1->Name + " U=\"" + pw->Label->initValue + "\"");
}

// ---------------------------------------------------
void QucsFile::throughAllNodes(bool User, QStringList& Collect,
                               int& countInit, bool Analog)
{
  int z=0;
  Node *pn, *p2;
  Wire *pw;
  Element *pe;
  bool setName=false;
  QPtrList<Node> Cons;

  for(pn = Nodes->first(); pn != 0; pn = Nodes->next()) {
    if(pn->Name.isEmpty() == User) continue;  // already named ?
    if(!User) {
      if(Analog)  pn->Name = "_net";
      else  pn->Name = "nnnet";   // VHDL names must not begin with '_'
      pn->Name += QString::number(z++);  // create node name
    }
    else
      if(pn->State)  continue;  // already worked on

    if(!Analog)  // collect all node names for VHDL signal declaration
      Signals.append(pn->Name);

    createNodeSet(Collect, countInit, pn, pn);

    pn->State = 1;
    Cons.append(pn);
    for(p2 = Cons.first(); p2 != 0; p2 = Cons.next())
      for(pe = p2->Connections.first(); pe != 0; pe = p2->Connections.next())
	if(pe->Type == isWire) {
	  pw = (Wire*)pe;
	  if(p2 != pw->Port1) {
	    if(pw->Port1->Name.isEmpty()) {
	      pw->Port1->Name = pn->Name;
	      pw->Port1->State = 1;
	      Cons.append(pw->Port1);
	      setName = true;
	    }
	  }
	  else {
	    if(pw->Port2->Name.isEmpty()) {
	      pw->Port2->Name = pn->Name;
	      pw->Port2->State = 1;
	      Cons.append(pw->Port2);
	      setName = true;
	    }
	  }
	  if(setName) {
	    Cons.findRef(p2);   // back to current Connection
	    createNodeSet(Collect, countInit, pw, pn);
	    setName = false;
	  }
	}
    Cons.clear();
  }
}

// ---------------------------------------------------
// Follows the wire lines in order to determine the node names for
// each component. Output into "stream", NodeSets are collected in
// "Collect" and counted with "countInit".
bool QucsFile::giveNodeNames(QTextStream *stream, int& countInit,
                   QStringList& Collect, QTextEdit *ErrText, int NumPorts)
{
  // delete the node names
  for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next()) {
    pn->State = 0;
    if(pn->Label)  pn->Name = pn->Label->Name;
    else pn->Name = "";
  }

  // set the wire names to the connected node
  for(Wire *pw = Wires->first(); pw != 0; pw = Wires->next())
    if(pw->Label != 0) pw->Port1->Name = pw->Label->Name;

  bool r;
  QString s;
  // give the ground nodes the name "gnd", and insert subcircuits etc.
  for(Component *pc = Comps->first(); pc != 0; pc = Comps->next())
    if(pc->isActive) {
      if(NumPorts < 0) {
        if((pc->Type & isAnalogComponent) == 0) {
          ErrText->insert(QObject::tr("ERROR: Component \"%1\" has no analog model.").arg(pc->Name));
          return false;
        }
      }
      else {
        if((pc->Type & isDigitalComponent) == 0) {
          if(pc->Name.isEmpty()) 
            ErrText->insert(QObject::tr("ERROR: No ground symbol allowed in digital simulation."));
          else
            ErrText->insert(QObject::tr("ERROR: Component \"%1\" has no digital model.").arg(pc->Name));
          return false;
        }
      }

      if(pc->Model == "GND") pc->Ports.getFirst()->Connection->Name = "gnd";
      else if(pc->Model == "Sub") {
	     s = pc->Props.getFirst()->Value;
	     if(StringList.findIndex(s) >= 0)
		continue;   // insert each subcircuit just one time

	     StringList.append(s);
	     QucsDoc *d = new QucsDoc(0, QucsWorkDir.filePath(s));
             if(!d->File.load()) {  // load document if possible
               delete d;
               ErrText->insert(QObject::tr("ERROR: Cannot load subcircuit \"%1\".").arg(s));
               return false;
             }
	     d->DocName = s;
	     r = d->File.createSubNetlist(
			stream, countInit, Collect, ErrText, NumPorts);
	     delete d;
	     if(!r) return false;
           }
      else if(pc->Model == "Lib") {
	     s  = pc->Props.first()->Value + "_";
	     s += pc->Props.next()->Value;
	     if(StringList.findIndex(s) >= 0)
		continue;   // insert each subcircuit just one time

	     StringList.append(s);
	     r = ((LibComp*)pc)->outputSubNetlist(stream);
	     if(!r) {
               ErrText->insert(
	         QObject::tr("ERROR: Cannot load library component \"%1\".").arg(pc->Name));
	       return false;
	     }
      }
      else if(pc->Model == "SPICE") {
	     s = pc->Props.first()->Value;
	     if(s.isEmpty()) {
               ErrText->insert(QObject::tr("ERROR: No file name in SPICE component \"%1\".").
			       arg(pc->Name));
	       return false;
	     }
	     if(StringList.findIndex(s) >= 0)
		continue;   // insert each spice component just one time

	     StringList.append(s);
	     s += '"'+pc->Props.next()->Value;
	     if(pc->Props.next()->Value == "yes")  s = "SPICE \""+s;
	     else  s = "SPICEo\""+s;
	     Collect.append(s);
	   }
    }  // of "if(active)"


  // work on named nodes first in order to preserve the user given names
  throughAllNodes(true, Collect, countInit, NumPorts<0);

  // give names to the remaining (unnamed) nodes
  throughAllNodes(false, Collect, countInit, NumPorts<0);

  return true;
}

// ---------------------------------------------------
// Write the netlist as subcircuit to the text stream 'NetlistFile'.
bool QucsFile::createSubNetlist(QTextStream *stream, int& countInit,
                     QStringList& Collect, QTextEdit *ErrText, int NumPorts)
{
  int i, z;
  QString s;
  // TODO: NodeSets have to be put into the subcircuit block.
  if(!giveNodeNames(stream, countInit, Collect, ErrText, NumPorts))
    return false;

  QStringList sl;
  QStringList::Iterator it;
  Component *pc;
  // collect all subcircuit ports and sort their node names into "sl"
  for(pc = Comps->first(); pc != 0; pc = Comps->next())
    if(pc->Model == "Port") {
      i  = pc->Props.first()->Value.toInt();
      for(z=sl.size(); z<i; z++)
        sl.append(" ");
      it = sl.at(i-1);
      (*it) = pc->Ports.getFirst()->Connection->Name;
      if(NumPorts >= 0) {
        (*it) += ": ";
        if(pc->Props.at(1)->Value.at(0) == 'o')  // output port ?
          (*it) += "out";
        else
          (*it) += "in";
        (*it) += " bit";
      }
    }

  QString  Type = properName(Doc->DocName);

  if(NumPorts < 0)
    (*stream) << "\n.Def:" << Type << " " << sl.join(" ") << '\n';
  else {

    // remove all node names connected to ports
    for(it = sl.begin(); it != sl.end(); it++)
      Signals.remove(Signals.find((*it).section(':',0,0)));

    for(it = Collect.begin(); it != Collect.end(); )
      if((*it).left(4) == "use ") {  // output all subcircuit uses
        (*stream) << (*it);
        it = Collect.remove(it);
      }
      else it++;

    (*stream) << "\nentity " << Type << " is\n"
              << "  port (" << sl.join(";\n        ") << ");\n"
              << "end entity;\n"
              << "architecture ARCH" << Type << " of " << Type << " is\n";
    if(!Signals.isEmpty())
      (*stream) << "  signal " << Signals.join(",\n         ") << " : bit;\n";

    // store own subcircuit declaration
    Collect.append("use work." + Type + ";\n");
    (*stream) << "begin\n";
  }
  Signals.clear();  // was filled in "giveNodeNames()"


  // write all components with node names into the netlist file
  for(pc = Comps->first(); pc != 0; pc = Comps->next()) {
    if(pc->Model.at(0) == '.') {  // no simulations in subcircuits
      ErrText->insert(
        QObject::tr("WARNING: Ignore simulation component in subcircuit \"%1\".").arg(Doc->DocName));
      continue;
    }
    if(pc->Model == "Eqn") {  // no equations in subcircuits
      ErrText->insert(
        QObject::tr("WARNING: Ignore equation in subcircuit \"%1\".").arg(Doc->DocName));
      continue;
    }

    if(NumPorts < 0)
      s = pc->NetList();
    else
      s = pc->VHDL_Code(NumPorts);

    if(!s.isEmpty())  // not inserted: subcircuit ports, disabled components
      (*stream) << "   " << s << "\n";
  }

  if(NumPorts < 0)
    (*stream) << ".Def:End\n\n";
  else
    (*stream) << "end architecture;\n\n";

  return true;
}

// ---------------------------------------------------
// Determines the node names and writes subcircuits into netlist file.
int QucsFile::prepareNetlist(QTextStream& stream, QStringList& Collect,
                              QTextEdit *ErrText)
{
  if(Doc->showBias > 0)  Doc->showBias = -1;  // do not show DC bias anymore

  bool isTruthTable = false;
  int allTypes = 0, NumPorts = 0;
  // Detect simulation domain (analog/digital) by looking at component types.
  for(Component *pc = Comps->first(); pc != 0; pc = Comps->next()) {
    if(!pc->isActive) continue;
    if(pc->Model.at(0) == '.') {
      if(pc->Model == ".Digi") {
        if(allTypes & isDigitalComponent) {
          ErrText->insert(
             QObject::tr("ERROR: Only one digital simulation allowed."));
          return -10;
        }
        if(pc->Props.getFirst()->Value != "TimeList")
          isTruthTable = true;
        allTypes |= isDigitalComponent;
      }
      else allTypes |= isAnalogComponent;

      if((allTypes & isComponent) == isComponent) {
        ErrText->insert(
           QObject::tr("ERROR: Analog and digital simulations cannot be mixed."));
        return -10;
      }
    }
    else if(pc->Model == "DigiSource")  NumPorts++;
  }

  if((allTypes & isAnalogComponent) == 0) {
    if(allTypes == 0) {
      ErrText->insert(
         QObject::tr("ERROR: No simulation specified on this page."));
      return -10;
    }
    else if(NumPorts < 1) {
      ErrText->insert(
         QObject::tr("ERROR: Digital simulation needs at least one digital source."));
      return -10;
    }

    if(!isTruthTable)  NumPorts = 0;
  }
  else NumPorts = -1;


  // first line is documentation
  if(allTypes & isAnalogComponent)
    stream << "#";
  else
    stream << "--";
  stream << " Qucs " << PACKAGE_VERSION << "  " << Doc->DocName << "\n";
//  if((allTypes & isAnalogComponent) == 0)
//    stream << "library ieee;\nuse ieee.std_logic_1164.all;\n\n";


  int countInit = 0;
  StringList.clear();  // no subcircuits yet
  if(!giveNodeNames(&stream, countInit, Collect, ErrText, NumPorts))
    return -10;

  if(allTypes & isAnalogComponent)
    return NumPorts;

  QString  Type = properName(Doc->DocName);
  stream << "entity " << Type << " is\n"
         << "end entity;\n\n";
  return NumPorts;
}

// .................................................
// write all components with node names into the netlist file
QString QucsFile::createNetlist(QTextStream& stream, int NumPorts)
{
  QString  Type = properName(Doc->DocName);
  if(NumPorts >= 0)
    stream << "architecture ARCH" << Type << " of " << Type << " is\n"
           << "  signal " << Signals.join(",\n         ")
           << " : bit;\n"
           << "begin\n";
  Signals.clear();  // was filled in "giveNodeNames()"

  QString s, Time;
  for(Component *pc = Comps->first(); pc != 0; pc = Comps->next()) {
    if(NumPorts < 0)
      s = pc->NetList();
    else {
      if(pc->Model.at(0) == '.')
        Time = pc->Props.at(1)->Value;
      s = pc->VHDL_Code(NumPorts);
    }

    if(!s.isEmpty())  // not inserted: subcircuit ports, disabled components
      stream << s << "\n";
  }

  if(NumPorts >= 0)
    stream << "end architecture;\n";

  if(NumPorts > 0)
    return QString::number(1 << NumPorts) + "ns";
  return Time;
}
