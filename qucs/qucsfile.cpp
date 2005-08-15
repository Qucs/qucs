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


// ---------------------------------------------------
// Follows the wire lines in order to determine the node names for
// each component. Output into "stream", NodeSets are collected in "NS".
bool QucsFile::giveNodeNames(QTextStream *stream, int& countInit,
                             QStringList& Collect, QTextEdit *ErrText)
{
  Node *p1, *p2;
  Wire *pw;
  Element *pe;

  // delete the node names
  for(p1 = Nodes->first(); p1 != 0; p1 = Nodes->next())
    if(p1->Label)  p1->Name = p1->Label->Name;
    else p1->Name = "";

  // set the wire names to the connected node
  for(pw = Wires->first(); pw != 0; pw = Wires->next())
    if(pw->Label != 0) pw->Port1->Name = pw->Label->Name;

  bool r;
  QString s;
  // give the ground nodes the name "gnd", and insert subcircuits
  for(Component *pc = Comps->first(); pc != 0; pc = Comps->next())
    if(pc->isActive)
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
				stream, countInit, Collect, ErrText);
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


  bool setName=false;
  QPtrList<Node> Cons;
  // work on named nodes first in order to preserve the user given names
  for(p1 = Nodes->first(); p1 != 0; p1 = Nodes->next()) {
    if(p1->Name.isEmpty()) continue;
    if(p1->Label)
      if(!p1->Label->initValue.isEmpty())
	Collect.append("NodeSet:NS" + QString::number(countInit++) + " " + p1->Name +
	      " U=\"" + p1->Label->initValue + "\"");

    Cons.append(p1);
    for(p2 = Cons.first(); p2 != 0; p2 = Cons.next())
      for(pe = p2->Connections.first(); pe != 0; pe = p2->Connections.next())
        if(pe->Type == isWire) {
          pw = (Wire*)pe;
          if(p2 != pw->Port1) {
            if(pw->Port1->Name.isEmpty()) {
              pw->Port1->Name = p1->Name;
              Cons.append(pw->Port1);
              setName = true;
            }
          }
          else {
            if(pw->Port2->Name.isEmpty()) {
              pw->Port2->Name = p1->Name;
              Cons.append(pw->Port2);
              setName = true;
            }
          }
	  if(setName) {
	    Cons.findRef(p2);   // back to current Connection
	    if(pw->Label)
	      if(!pw->Label->initValue.isEmpty())
		Collect.append("NodeSet:NS" + QString::number(countInit++) + " " +
		      p1->Name + " U=\"" + pw->Label->initValue + "\"");
	    setName = false;
	  }
        }
    Cons.clear();
  }


  int z=0;
  // give names to the remaining (unnamed) nodes
  for(p1 = Nodes->first(); p1!=0; p1 = Nodes->next()) { // work on all nodes
    if(!p1->Name.isEmpty()) continue;    // already named ?
    p1->Name = "_net" + QString::number(z++);   // create node name
    if(p1->Label)
      if(!p1->Label->initValue.isEmpty())
	Collect.append("NodeSet:NS" + QString::number(countInit++) + " " + p1->Name +
			" U=\"" + p1->Label->initValue + "\"");

    Cons.append(p1);
    // create list with connections to the node
    for(p2 = Cons.first(); p2 != 0; p2 = Cons.next())
      for(pe = p2->Connections.first(); pe != 0; pe = p2->Connections.next())
        if(pe->Type == isWire) {
          pw = (Wire*)pe;
          if(p2 != pw->Port1) {
            if(pw->Port1->Name.isEmpty()) {
              pw->Port1->Name = p1->Name;
              Cons.append(pw->Port1);
              setName = true;
            }
          }
          else {
            if(pw->Port2->Name.isEmpty()) {
              pw->Port2->Name = p1->Name;
              Cons.append(pw->Port2);
              setName = true;
            }
          }
	  if(setName) {
	    Cons.findRef(p2);   // back to current Connection
	    if(pw->Label)
	      if(!pw->Label->initValue.isEmpty())
		Collect.append("NodeSet:Nb" + QString::number(countInit++) + " " +
		      p1->Name + " U=\"" + pw->Label->initValue + "\"");
	    setName = false;
	  }
        }
    Cons.clear();
  }

  return true;
}

// ---------------------------------------------------
// Write the netlist as subcircuit to the text stream 'NetlistFile'.
bool QucsFile::createSubNetlist(QTextStream *stream, int& countInit,
                                QStringList& Collect, QTextEdit *Error)
{
  int i, z;
  QString s;
  if(!giveNodeNames(stream, countInit, Collect, Error)) return false;

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
    }

  QString  Type = Doc->DocName;
  QFileInfo Info(Type);
  if(Info.extension() == "sch")  Type = Type.left(Type.length()-4);
  if(Type.at(0) <= '9') if(Type.at(0) >= '0')  Type = '_' + Type;
  Type.replace(QRegExp("\\W"), "_"); // replace all none [a-zA-Z0-9] with _
  (*stream) << "\n.Def:" << Type << " " << sl.join(" ") << "\n";
  (*stream) << s;  // outputs the NodeSets (if any)


  // write all components with node names into the netlist file
  for(pc = Comps->first(); pc != 0; pc = Comps->next()) {
    if(pc->Model.at(0) == '.') continue;  // no simulations in subcircuits
    if(pc->Model == "Eqn") continue;  // no equations in subcircuits
    s = pc->NetList();
    if(!s.isEmpty())  // not inserted: subcircuit ports, disabled components
      (*stream) << "   " << s << "\n";
  }

  (*stream) << ".Def:End\n\n";
  return true;
}

// ---------------------------------------------------
// Determines the node names and writes subcircuits into netlist file.
bool QucsFile::prepareNetlist(QTextStream& stream, QStringList& Collect,
                              QTextEdit *Error)
{
  if(Doc->showBias > 0)  Doc->showBias = -1;  // do not show DC bias anymore

  // first line is documentation
  stream << "# Qucs " << PACKAGE_VERSION << "  " << Doc->DocName << "\n";

  int countInit = 0;
  StringList.clear();
  if(!giveNodeNames(&stream, countInit, Collect, Error))
    return false;
  
  return true;
}

// .................................................
// write all components with node names into the netlist file
void QucsFile::createNetlist(QTextStream& stream)
{
  QString s;
  for(Component *pc = Comps->first(); pc != 0; pc = Comps->next()) {
    s = pc->NetList();
    if(!s.isEmpty())  // not inserted: subcircuit ports, disabled components
      stream << s << "\n";
  }
}
