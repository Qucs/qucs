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

#include <qmessagebox.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qregexp.h>
#include <qprocess.h>
#include <qtextedit.h>

#include "node.h"
#include "diagrams/diagrams.h"
#include "paintings/paintings.h"
#include "components/spicefile.h"
#include "components/libcomp.h"
#include "schematic.h"
#include "main.h"


extern QDir QucsWorkDir;

// Here the subcircuits, SPICE components etc are collected. It must be
// global to also work within the subcircuits.
QStringList StringList;


// -------------------------------------------------------------
// Creates a Qucs file format (without document properties) in the returning
// string. This is used to copy the selected elements into the clipboard.
QString Schematic::createClipboardFile()
{
  int z=0;  // counts selected elements
  Wire *pw;
  Diagram *pd;
  Painting *pp;
  Component *pc;

  QString s("<Qucs Schematic " PACKAGE_VERSION ">\n");

  // Build element document.
  s += "<Components>\n";
  for(pc = Components->first(); pc != 0; pc = Components->next())
    if(pc->isSelected) {
      s += pc->save()+"\n";  z++; }
  s += "</Components>\n";

  s += "<Wires>\n";
  for(pw = Wires->first(); pw != 0; pw = Wires->next())
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
  for(pd = Diagrams->first(); pd != 0; pd = Diagrams->next())
    if(pd->isSelected) {
      s += pd->save()+"\n";  z++; }
  s += "</Diagrams>\n";

  s += "<Paintings>\n";
  for(pp = Paintings->first(); pp != 0; pp = Paintings->next())
    if(pp->isSelected)
      if(pp->Name.at(0) != '.') {  // subcircuit specific -> do not copy
        s += "<"+pp->save()+">\n";  z++; }
  s += "</Paintings>\n";

  if(z == 0) return "";   // return empty if no selection

  return s;
}

// -------------------------------------------------------------
// Only read fields without loading them.
bool Schematic::loadIntoNothing(QTextStream *stream)
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
bool Schematic::pasteFromClipboard(QTextStream *stream, QPtrList<Element> *pe)
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
  if(symbolMode) {
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
int Schematic::saveDocument()
{
  QFile file(DocName);
  if(!file.open(IO_WriteOnly)) {
    QMessageBox::critical(0, QObject::tr("Error"),
				QObject::tr("Cannot save document!"));
    return -1;
  }

  QTextStream stream(&file);

  stream << "<Qucs Schematic " << PACKAGE_VERSION << ">\n";

  stream << "<Properties>\n";
  if(symbolMode) {
    stream << "  <View=" << tmpViewX1<<","<<tmpViewY1<<","
			 << tmpViewX2<<","<<tmpViewY2<< ",";
    stream <<tmpScale<<","<<tmpPosX<<","<<tmpPosY << ">\n";
  }
  else {
    stream << "  <View=" << ViewX1<<","<<ViewY1<<","
			 << ViewX2<<","<<ViewY2<< ",";
    stream << Scale <<","<<contentsX()<<","<<contentsY() << ">\n";
  }
  stream << "  <Grid=" << GridX<<","<<GridY<<","
			<< GridOn << ">\n";
  stream << "  <DataSet=" << DataSet << ">\n";
  stream << "  <DataDisplay=" << DataDisplay << ">\n";
  stream << "  <OpenDisplay=" << SimOpenDpl << ">\n";
  stream << "</Properties>\n";

  Painting *pp;
  stream << "<Symbol>\n";     // save all paintings for symbol
  for(pp = SymbolPaints.first(); pp != 0; pp = SymbolPaints.next())
    stream << "  <" << pp->save() << ">\n";
  stream << "</Symbol>\n";

  stream << "<Components>\n";    // save all components
  for(Component *pc = DocComps.first(); pc != 0; pc = DocComps.next())
    stream << "  " << pc->save() << "\n";
  stream << "</Components>\n";

  stream << "<Wires>\n";    // save all wires
  for(Wire *pw = DocWires.first(); pw != 0; pw = DocWires.next())
    stream << "  " << pw->save() << "\n";

  // save all labeled nodes as wires
  for(Node *pn = DocNodes.first(); pn != 0; pn = DocNodes.next())
    if(pn->Label) stream << "  " << pn->Label->save() << "\n";
  stream << "</Wires>\n";

  stream << "<Diagrams>\n";    // save all diagrams
  for(Diagram *pd = DocDiags.first(); pd != 0; pd = DocDiags.next())
    stream << "  " << pd->save() << "\n";
  stream << "</Diagrams>\n";

  stream << "<Paintings>\n";     // save all paintings
  for(pp = DocPaints.first(); pp != 0; pp = DocPaints.next())
    stream << "  <" << pp->save() << ">\n";
  stream << "</Paintings>\n";

  file.close();
  return 0;
}

// -------------------------------------------------------------
bool Schematic::loadProperties(QTextStream *stream)
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
		ViewX1 = nstr.section(',',0,0).toInt(&ok); if(ok) {
		ViewY1 = nstr.section(',',1,1).toInt(&ok); if(ok) {
		ViewX2 = nstr.section(',',2,2).toInt(&ok); if(ok) {
		ViewY2 = nstr.section(',',3,3).toInt(&ok); if(ok) {
		Scale  = nstr.section(',',4,4).toDouble(&ok); if(ok) {
		int PosX = nstr.section(',',5,5).toInt(&ok); if(ok) {
		int PosY = nstr.section(',',6,6).toInt(&ok); if(ok)
		setContentsPos(PosX, PosY); }}}}}} }
    else if(cstr == "Grid") {
		GridX = nstr.section(',',0,0).toInt(&ok); if(ok) {
		GridY = nstr.section(',',1,1).toInt(&ok); if(ok) {
		if(nstr.section(',',2,2).toInt(&ok) == 0) GridOn = false;
		else GridOn = true; }} }
    else if(cstr == "DataSet") DataSet = nstr;
    else if(cstr == "DataDisplay") DataDisplay = nstr;
    else if(cstr == "OpenDisplay")
		if(nstr.toInt(&ok) == 0) SimOpenDpl = false;
		else SimOpenDpl = true;
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
void Schematic::simpleInsertComponent(Component *c)
{
  Node *pn;
  int x, y;
  // connect every node of component
  for(Port *pp = c->Ports.first(); pp != 0; pp = c->Ports.next()) {
    x = pp->x+c->cx;
    y = pp->y+c->cy;

    // check if new node lies upon existing node
    for(pn = DocNodes.first(); pn != 0; pn = DocNodes.next())
      if(pn->cx == x) if(pn->cy == y)  break;

    if(pn == 0) { // create new node, if no existing one lies at this position
      pn = new Node(x, y);
      DocNodes.append(pn);
    }
    pn->Connections.append(c);  // connect schematic node to component node

    pp->Connection = pn;  // connect component node to schematic node
  }

  DocComps.append(c);
}

// -------------------------------------------------------------
bool Schematic::loadComponents(QTextStream *stream, QPtrList<Component> *List)
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
void Schematic::simpleInsertWire(Wire *pw)
{
  Node *pn;
  // check if first wire node lies upon existing node
  for(pn = DocNodes.first(); pn != 0; pn = DocNodes.next())
    if(pn->cx == pw->x1) if(pn->cy == pw->y1) break;

  if(!pn) {   // create new node, if no existing one lies at this position
    pn = new Node(pw->x1, pw->y1);
    DocNodes.append(pn);
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
  for(pn = DocNodes.first(); pn != 0; pn = DocNodes.next())
    if(pn->cx == pw->x2) if(pn->cy == pw->y2) break;

  if(!pn) {   // create new node, if no existing one lies at this position
    pn = new Node(pw->x2, pw->y2);
    DocNodes.append(pn);
  }
  pn->Connections.append(pw);  // connect schematic node to component node
  pw->Port2 = pn;

  DocWires.append(pw);
}

// -------------------------------------------------------------
bool Schematic::loadWires(QTextStream *stream, QPtrList<Element> *List)
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
bool Schematic::loadDiagrams(QTextStream *stream, QPtrList<Diagram> *List)
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
bool Schematic::loadPaintings(QTextStream *stream, QPtrList<Painting> *List)
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
bool Schematic::loadDocument()
{
  QFile file(DocName);
  if(!file.open(IO_ReadOnly)) {
    QMessageBox::critical(0, QObject::tr("Error"),
                 QObject::tr("Cannot load document: ")+DocName);
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
 		 QObject::tr("Wrong document type: ")+DocName);
    return false;
  }

  Line = Line.mid(16, Line.length()-17);
  if(!checkVersion(Line)) { // wrong version number ?
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
      if(!loadPaintings(&stream, &SymbolPaints)) {
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
      if(!loadDiagrams(&stream, &DocDiags)) { file.close(); return false; } }
    else
    if(Line == "<Paintings>") {
      if(!loadPaintings(&stream, &DocPaints)) { file.close(); return false; } }
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
QString Schematic::createUndoString(char Op)
{
  Wire *pw;
  Diagram *pd;
  Painting *pp;
  Component *pc;

  // Build element document.
  QString s = "  \n";
  s.at(0) = Op;
  for(pc = DocComps.first(); pc != 0; pc = DocComps.next())
    s += pc->save()+"\n";
  s += "</>\n";  // short end flag

  for(pw = DocWires.first(); pw != 0; pw = DocWires.next())
    s += pw->save()+"\n";
  // save all labeled nodes as wires
  for(Node *pn = DocNodes.first(); pn != 0; pn = DocNodes.next())
    if(pn->Label) s += pn->Label->save()+"\n";
  s += "</>\n";

  for(pd = DocDiags.first(); pd != 0; pd = DocDiags.next())
    s += pd->save()+"\n";
  s += "</>\n";

  for(pp = DocPaints.first(); pp != 0; pp = DocPaints.next())
    s += "<"+pp->save()+">\n";
  s += "</>\n";

  return s;
}

// -------------------------------------------------------------
// Same as "createUndoString(char Op)" but for symbol edit mode.
QString Schematic::createSymbolUndoString(char Op)
{
  Painting *pp;

  // Build element document.
  QString s = "  \n";
  s.at(0) = Op;
  s += "</>\n";  // short end flag for components
  s += "</>\n";  // short end flag for wires
  s += "</>\n";  // short end flag for diagrams

  for(pp = SymbolPaints.first(); pp != 0; pp = SymbolPaints.next())
    s += "<"+pp->save()+">\n";
  s += "</>\n";

  return s;
}

// -------------------------------------------------------------
// Is quite similiar to "loadDocument()" but with less error checking.
// Used for "undo" function.
bool Schematic::rebuild(QString *s)
{
  DocWires.clear();	// delete whole document
  DocNodes.clear();
  DocComps.clear();
  DocDiags.clear();
  DocPaints.clear();

  QString Line;
  QTextStream stream(s, IO_ReadOnly);
  Line = stream.readLine();  // skip identity byte

  // read content *************************
  if(!loadComponents(&stream))  return false;
  if(!loadWires(&stream))  return false;
  if(!loadDiagrams(&stream, &DocDiags))  return false;
  if(!loadPaintings(&stream, &DocPaints)) return false;

  return true;
}

// -------------------------------------------------------------
// Same as "rebuild(QString *s)" but for symbol edit mode.
bool Schematic::rebuildSymbol(QString *s)
{
  SymbolPaints.clear();	// delete whole document

  QString Line;
  QTextStream stream(s, IO_ReadOnly);
  Line = stream.readLine();  // skip identity byte

  // read content *************************
  Line = stream.readLine();  // skip components
  Line = stream.readLine();  // skip wires
  Line = stream.readLine();  // skip diagrams
  if(!loadPaintings(&stream, &SymbolPaints)) return false;

  return true;
}


// ***************************************************************
// *****                                                     *****
// *****             Functions to create netlist             *****
// *****                                                     *****
// ***************************************************************

void Schematic::createNodeSet(QStringList& Collect, int& countInit,
                             Conductor *pw, Node *p1)
{
  if(pw->Label)
    if(!pw->Label->initValue.isEmpty())
      Collect.append("NodeSet:NS" + QString::number(countInit++) + " " +
                     p1->Name + " U=\"" + pw->Label->initValue + "\"");
}

// ---------------------------------------------------
void Schematic::throughAllNodes(bool User, QStringList& Collect,
                               int& countInit, bool Analog)
{
  int z=0;
  Node *pn, *p2;
  Wire *pw;
  Element *pe;
  bool setName=false;
  QPtrList<Node> Cons;

  for(pn = DocNodes.first(); pn != 0; pn = DocNodes.next()) {
    if(pn->Name.isEmpty() == User) continue;  // already named ?
    if(!User) {
      if(Analog) pn->Name = "_net";
      else  pn->Name = "net_net";   // VHDL names must not begin with '_'
      pn->Name += QString::number(z++);  // create numbered node name
    }
    else
      if(pn->State)  continue;  // already worked on

    if(!Analog)  // collect all node names for VHDL signal declaration
      if(Signals.findIndex(pn->Name) < 0)  // avoid redeclaration of signal
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
bool Schematic::giveNodeNames(QTextStream *stream, int& countInit,
                   QStringList& Collect, QTextEdit *ErrText, int NumPorts)
{
  // delete the node names
  for(Node *pn = DocNodes.first(); pn != 0; pn = DocNodes.next()) {
    pn->State = 0;
    if(pn->Label) {
      if(NumPorts < 0)
        pn->Name = pn->Label->Name;
      else
        pn->Name = "net" + pn->Label->Name;
    }
    else pn->Name = "";
  }

  // set the wire names to the connected node
  for(Wire *pw = DocWires.first(); pw != 0; pw = DocWires.next())
    if(pw->Label != 0) {
      if(NumPorts < 0)
        pw->Port1->Name = pw->Label->Name;
      else  // avoid to use reserved VHDL words
        pw->Port1->Name = "net" + pw->Label->Name;
    }

  bool r;
  QString s;
  // give the ground nodes the name "gnd", and insert subcircuits etc.
  for(Component *pc = DocComps.first(); pc != 0; pc = DocComps.next()) {
    if(pc->isActive != COMP_IS_ACTIVE) continue;

    if(NumPorts < 0) {
      if((pc->Type & isAnalogComponent) == 0) {
        ErrText->insert(QObject::tr("ERROR: Component \"%1\" has no analog model.").arg(pc->Name));
        return false;
      }
    }
    else {
      if((pc->Type & isDigitalComponent) == 0) {
        ErrText->insert(QObject::tr("ERROR: Component \"%1\" has no digital model.").arg(pc->Name));
        return false;
      }
    }

    if(pc->Model == "GND") {
      pc->Ports.getFirst()->Connection->Name = "gnd";
      continue;
    }

    if(pc->Model == "Sub") {
      s = pc->Props.getFirst()->Value;
      if(StringList.findIndex(s) >= 0)
        continue;   // insert each subcircuit just one time

      StringList.append(s);
      Schematic *d = new Schematic(0, QucsWorkDir.filePath(s));
      if(!d->loadDocument()) {  // load document if possible
        delete d;
        ErrText->insert(QObject::tr("ERROR: Cannot load subcircuit \"%1\".").arg(s));
        return false;
      }
      d->DocName = s;
      r = d->createSubNetlist(stream, countInit, Collect, ErrText, NumPorts);
      delete d;
      if(!r) return false;
      continue;
    }

    if(pc->Model == "Lib") {
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
      continue;
    }

    if(pc->Model == "SPICE") {
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
      continue;
    }

    if(pc->Model == "VHDL") {
      s = pc->Props.getFirst()->Value;
      if(s.isEmpty()) {
        ErrText->insert(QObject::tr("ERROR: No file name in VHDL component \"%1\".").
                        arg(pc->Name));
        return false;
      }
      if(StringList.findIndex(s) >= 0)
        continue;   // insert each vhdl component just one time
      StringList.append(s);

      QFileInfo Info(s);
      if(Info.isRelative())
        s = QucsWorkDir.filePath(s);

      QFile f(s);
      if(!f.open(IO_ReadOnly)) {
        ErrText->insert(
           QObject::tr("ERROR: Cannot open VHDL file \"%1\".").arg(s));
        return false;
      }

      // Write the whole VHDL file into the netlist output.
      QTextStream streamVHDL(&f);
      s = streamVHDL.read();
      f.close();
      (*stream) << '\n' << s << '\n';
      continue;
    }
  }


  // work on named nodes first in order to preserve the user given names
  throughAllNodes(true, Collect, countInit, NumPorts<0);

  // give names to the remaining (unnamed) nodes
  throughAllNodes(false, Collect, countInit, NumPorts<0);

  return true;
}

// ---------------------------------------------------
// Write the netlist as subcircuit to the text stream 'NetlistFile'.
bool Schematic::createSubNetlist(QTextStream *stream, int& countInit,
                     QStringList& Collect, QTextEdit *ErrText, int NumPorts)
{
  int i, z;
//  int Collect_count = Collect.count();   // position for this subcircuit
  QString s;
  // TODO: NodeSets have to be put into the subcircuit block.
  if(!giveNodeNames(stream, countInit, Collect, ErrText, NumPorts))
    return false;

/*  Example for TODO
      for(it = Collect.at(Collect_count); it != Collect.end(); )
      if((*it).left(4) == "use ") {  // output all subcircuit uses
        (*stream) << (*it);
        it = Collect.remove(it);
      }
      else it++;*/

  QStringList SubcircuitPorts;
  QStringList::Iterator it;
  Component *pc;
  // collect subcircuit ports and sort their node names into "SubcircuitPorts"
  for(pc = DocComps.first(); pc != 0; pc = DocComps.next())
    if(pc->Model == "Port") {
      i  = pc->Props.first()->Value.toInt();
      for(z=SubcircuitPorts.size(); z<i; z++)
        SubcircuitPorts.append(" ");
      it = SubcircuitPorts.at(i-1);
      (*it) = pc->Ports.getFirst()->Connection->Name;
      if(NumPorts >= 0) {
        Signals.remove(Signals.find(*it));  // remove node name of output port
        switch(pc->Props.at(1)->Value.at(0).latin1()) {
          case 'a': (*it) += ": inout";  // attribut "analog" is "inout"
                    break;
          case 'o': Signals.append(*it);   // output ports need workaround
                    (*it) = "net_out" + (*it);
                    // no "break;" here !!!
          default:  (*it) += ": " + pc->Props.at(1)->Value;
        }
        (*it) += " bit";
      }
    }

  QString  Type = properName(DocName);

  if(NumPorts < 0)
    (*stream) << "\n.Def:" << Type << " " << SubcircuitPorts.join(" ") << '\n';
  else {
    (*stream) << "\nentity Sub_" << Type << " is\n"
              << "  port (" << SubcircuitPorts.join(";\n        ") << ");\n"
              << "end entity;\n"
              << "use work.all;\n"
              << "architecture Arch_Sub_" << Type << " of Sub_" << Type << " is\n";
    if(!Signals.isEmpty())
      (*stream) << "  signal " << Signals.join(",\n         ") << " : bit;\n";

    (*stream) << "begin\n";

    if(Signals.findIndex("gnd") >= 0)
      (*stream) << "  gnd <= '0';\n";  // should appear only once
  }
  Signals.clear();  // was filled in "giveNodeNames()"


  // write all components with node names into the netlist file
  for(pc = DocComps.first(); pc != 0; pc = DocComps.next()) {
    if(pc->isActive == COMP_IS_OPEN) continue;  // should it be simulated ?
  
    if(pc->Model.at(0) == '.') {  // no simulations in subcircuits
      ErrText->insert(
        QObject::tr("WARNING: Ignore simulation component in subcircuit \"%1\".").arg(DocName));
      continue;
    }
    if(pc->Model == "Eqn") {  // no equations in subcircuits
      ErrText->insert(
        QObject::tr("WARNING: Ignore equation in subcircuit \"%1\".").arg(DocName));
      continue;
    }

    if(pc->isActive == COMP_IS_ACTIVE) {
      if(NumPorts < 0)
        s = pc->NetList();
      else
        s = pc->VHDL_Code(NumPorts);
    }
    else {
      if(NumPorts < 0)
        s = pc->getShortenNetlist();
      else
        s = pc->getShortenVHDL();
    }

    if(!s.isEmpty())  // not inserted: subcircuit ports, disabled components
      (*stream) << s << "\n";
  }

  if(NumPorts < 0)
    (*stream) << ".Def:End\n\n";
  else
    (*stream) << "end architecture;\n\n";

  return true;
}

// ---------------------------------------------------
// Determines the node names and writes subcircuits into netlist file.
int Schematic::prepareNetlist(QTextStream& stream, QStringList& Collect,
                              QTextEdit *ErrText)
{
  if(showBias > 0)  showBias = -1;  // do not show DC bias anymore

  bool isTruthTable = false;
  int allTypes = 0, NumPorts = 0;
  // Detect simulation domain (analog/digital) by looking at component types.
  for(Component *pc = DocComps.first(); pc != 0; pc = DocComps.next()) {
    if(pc->isActive == COMP_IS_OPEN) continue;
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
/*      ErrText->insert(
         QObject::tr("ERROR: No simulation specified on this page."));
      return -10;*/

      // If no simulation exists, assume analog simulation. There may
      // be a simulation within a SPICE file. Otherwise Qucsator will
      // output an error.
      allTypes |= isAnalogComponent;
      NumPorts = -1;
    }
    else {
      if(NumPorts < 1) {
        ErrText->insert(
           QObject::tr("ERROR: Digital simulation needs at least one digital source."));
        return -10;
      }
      if(!isTruthTable)  NumPorts = 0;
    }
  }
  else NumPorts = -1;


  // first line is documentation
  if(allTypes & isAnalogComponent)
    stream << "#";
  else
    stream << "--";
  stream << " Qucs " << PACKAGE_VERSION << "  " << DocName << "\n";
//  if((allTypes & isAnalogComponent) == 0)
//    stream << "library ieee;\nuse ieee.std_logic_1164.all;\n\n";


  int countInit = 0;
  if(!giveNodeNames(&stream, countInit, Collect, ErrText, NumPorts))
    return -10;

  if(allTypes & isAnalogComponent)
    return NumPorts;

  stream << "entity TestBench is\n"
         << "end entity;\n"
         << "use work.all;\n";
  return NumPorts;
}

// .................................................
// write all components with node names into the netlist file
QString Schematic::createNetlist(QTextStream& stream, int NumPorts)
{
  if(NumPorts >= 0) {
    stream << "architecture Arch_TestBench of TestBench is\n"
           << "  signal " << Signals.join(",\n         ")
           << " : bit;\n"
           << "begin\n";

    if(Signals.findIndex("gnd") >= 0)
      stream << "  gnd <= '0';\n";  // should appear only once
  }
  Signals.clear();  // was filled in "giveNodeNames()"
  StringList.clear();

  QString s, Time;
  for(Component *pc = DocComps.first(); pc != 0; pc = DocComps.next()) {
    if(pc->isActive == COMP_IS_OPEN) continue;  // should it be simulated ?
  
    if(NumPorts < 0) {
      if(pc->isActive == COMP_IS_ACTIVE)
        s = pc->NetList();
      else
        s = pc->getShortenNetlist();
    }
    else {
      if(pc->Model.at(0) == '.') {  // simulation component ?
        if(NumPorts > 0)  // truth table simulation ?
          Time = QString::number((1 << NumPorts) - 1) + " ns";
        else {
          Time = pc->Props.at(1)->Value;
          if(!VHDL_Time(Time, pc->Name))  return Time;  // wrong time format
        }
      }
      if(pc->isActive == COMP_IS_ACTIVE)
        s = pc->VHDL_Code(NumPorts);
      else
        s = pc->getShortenVHDL();
      if(s.at(0) == '§')  return s;   // return error
    }

    if(!s.isEmpty())  // not inserted: subcircuit ports, disabled components
      stream << s << "\n";
  }

  if(NumPorts >= 0)
    stream << "end architecture;\n";

  return Time;
}
