/***************************************************************************
                          qucsfile.cpp  -  description
                             -------------------
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

#include "diagrams/diagrams.h"
#include "paintings/paintings.h"
#include "qucsdoc.h"

#include <qmessagebox.h>


QucsFile::QucsFile(QucsDoc *p)
{
  Doc = p;

  Wires  = &(p->Wires);
  Nodes  = &(p->Nodes);
  Comps  = &(p->Comps);
  Diags  = &(p->Diags);
  Paints = &(p->Paints);

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
  for(pc = Comps->first(); pc != 0; pc = Comps->next())
    if(pc->isSelected) {
      s += pc->save()+"\n";  z++; }
  s += "</Components>\n";

  s += "<Wires>\n";
  for(pw = Wires->first(); pw != 0; pw = Wires->next())
    if(pw->isSelected) {
      s += pw->save()+"\n";  z++; }
  s += "</Wires>\n";

  s += "<Diagrams>\n";
  for(pd = Diags->first(); pd != 0; pd = Diags->next())
    if(pd->isSelected) {
      s += pd->save()+"\n";  z++; }
  s += "</Diagrams>\n";

  s += "<Paintings>\n";
  for(pp = Paints->first(); pp != 0; pp = Paints->next())
    if(pp->isSelected) {
      s += pp->save()+"\n";  z++; }
  s += "</Paintings>\n";

  if(z == 0) return "";   // return empty if no selection

  return s;
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

  // read content *************************
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line == "<Components>") {
      if(!loadComponents(stream, (QPtrList<Component>*)pe)) return false; }
    else
    if(Line == "<Wires>") {
      if(!loadWires(stream, (QPtrList<Wire>*)pe)) return false; }
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
  stream << "   <View=" << Doc->ViewX1<<","<<Doc->ViewY1<<","
			<< Doc->ViewX2<<","<<Doc->ViewY2;
  stream << ","<<Doc->Scale<<","<<Doc->PosX<<","<<Doc->PosY << ">\n";
  stream << "   <Grid=" << Doc->GridX<<","<<Doc->GridY<<","
			<< Doc->GridOn << ">\n";
  stream << "   <DataSet=" << Doc->DataSet << ">\n";
  stream << "   <DataDisplay=" << Doc->DataDisplay << ">\n";
  stream << "   <OpenDisplay=" << Doc->SimOpenDpl << ">\n";
  stream << "</Properties>\n";

  int z=0;   // to count number of subcircuit ports
  stream << "<Components>\n";    // save all components
  for(Component *pc = Comps->first(); pc != 0; pc = Comps->next()) {
    stream << pc->save() << "\n";
    if(pc->Model == "Port") z++;
  }
  stream << "</Components>\n";

  stream << "<Wires>\n";    // save all wires
  for(Wire *pw = Wires->first(); pw != 0; pw = Wires->next())
    stream << pw->save() << "\n";

  // save all labeled nodes as wires
  for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next())
    if(pn->Label) stream << pn->Label->save() << "\n";
  stream << "</Wires>\n";

  stream << "<Diagrams>\n";    // save all diagrams
  for(Diagram *pd = Diags->first(); pd != 0; pd = Diags->next())
    stream << pd->save() << "\n";
  stream << "</Diagrams>\n";

  stream << "<Paintings>\n";     // save all paintings
  for(Painting *pp = Paints->first(); pp != 0; pp = Paints->next())
    stream << pp->save() << "\n";
  stream << "</Paintings>\n";

  file.close();
  return z;
}

// -------------------------------------------------------------
bool QucsFile::loadProperties(QTextStream *stream)
{
  bool ok = true;
  QString Line, cstr, nstr;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line == "</Properties>") return true;
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
    Line = Line.stripWhiteSpace();
    if(Line.isEmpty()) continue;
    if(Line == "</Components>") return true;

    c = getComponentFromName(Line);
    if(!c) return false;

    if(List) {
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
    if (pn->Label) pn->Label->Type = isNodeLabel;
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
bool QucsFile::loadWires(QTextStream *stream, QPtrList<Wire> *List)
{
  Wire *w;
  QString Line;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line == "</Wires>") return true;
    Line = Line.stripWhiteSpace();
    if(Line.isEmpty()) continue;

    w = new Wire(0,0,0,0, (Node*)4,(Node*)4);  // (Node*)4 =  move all ports (later on)
    if(!w->load(Line)) {
      QMessageBox::critical(0, QObject::tr("Error"),
                   QObject::tr("Format Error:\nWrong 'wire' line format!"));
      return false;
    }
    if(List) List->append(w);
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
    if(Line == "</Diagrams>") return true;
    Line = Line.stripWhiteSpace();
    if(Line.isEmpty()) continue;

    cstr = Line.section(' ',0,0);    // diagram type
         if(cstr == "<Rect") d = new RectDiagram();
    else if(cstr == "<Polar") d = new PolarDiagram();
    else if(cstr == "<Tab") d = new TabDiagram();
    else if(cstr == "<Smith") d = new SmithDiagram();
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
  Painting *p;
  QString Line, cstr;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line == "</Paintings>") return true;
    Line = Line.stripWhiteSpace();
    if(Line.isEmpty()) continue;

    cstr = Line.section(' ',0,0);    // painting type
         if(cstr == "<Rectangle") p = new Rectangle();
    else if(cstr == "<Text") p = new GraphicText();
    else if(cstr == "<Line") p = new GraphicLine();
    else if(cstr == "<Arrow") p = new Arrow();
    else if(cstr == "<Ellipse") p = new Ellipse();
    else if(cstr == "<FilledEllipse") p = new Ellipse(true);
    else if(cstr == "<FilledRect") p = new Rectangle(true);
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
 		 QObject::tr("Wrong document type!"));
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
QString QucsFile::createUndoString()
{
  Wire *pw;
  Diagram *pd;
  Painting *pp;
  Component *pc;

  // Build element document.
  QString s = "<Components>\n";
  for(pc = Comps->first(); pc != 0; pc = Comps->next())
    s += pc->save()+"\n";
  s += "</Components>\n";

  s += "<Wires>\n";
  for(pw = Wires->first(); pw != 0; pw = Wires->next())
    s += pw->save()+"\n";
  // save all labeled nodes as wires
  for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next())
    if(pn->Label) s += pn->Label->save()+"\n";
  s += "</Wires>\n";

  s += "<Diagrams>\n";
  for(pd = Diags->first(); pd != 0; pd = Diags->next())
    s += pd->save()+"\n";
  s += "</Diagrams>\n";

  s += "<Paintings>\n";
  for(pp = Paints->first(); pp != 0; pp = Paints->next())
    s += pp->save()+"\n";
  s += "</Paintings>\n";

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

  // read content *************************
  while(!stream.atEnd()) {
    Line = stream.readLine();
    Line = Line.stripWhiteSpace();

    if(Line == "<Components>") {
      if(!loadComponents(&stream))  return false; }
    else
    if(Line == "<Wires>") {
      if(!loadWires(&stream))  return false; }
    else
    if(Line == "<Diagrams>") {
      if(!loadDiagrams(&stream, Diags))  return false; }
    else
    if(Line == "<Paintings>") {
      if(!loadPaintings(&stream, Paints)) return false; }
    else  return false;
  }

  return true;
}
