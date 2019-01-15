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
#include <QStringList>
#include <QPlainTextEdit>
#include "qt_compat.h"
#include <QTextStream>
#include <QList>
#include <QProcess>
#include <QDebug>

#include "qucs.h"
#include "node.h"
#include "schematic.h"
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


// Here the subcircuits, SPICE components etc are collected. It must be
// global to also work within the subcircuits.
SubMap FileList;


// -------------------------------------------------------------
// Creates a Qucs file format (without document properties) in the returning
// string. This is used to copy the selected elements into the clipboard.
QString SchematicModel::createClipboardFile()
{
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
bool Schematic::pasteFromClipboard(QTextStream *stream, Q3PtrList<Element> *pe)
{
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
        if(!loadPaintings(stream, (Q3PtrList<Painting>*)pe)) return false; }
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
      if(!loadComponents(stream, (Q3PtrList<Component>*)pe)) return false; }
    else
    if(Line == "<Wires>") {
      if(!loadWires(stream, pe)) return false; }
    else
    if(Line == "<Diagrams>") {
      if(!loadDiagrams(stream, (Q3PtrList<Diagram>*)pe)) return false; }
    else
    if(Line == "<Paintings>") {
      if(!loadPaintings(stream, (Q3PtrList<Painting>*)pe)) return false; }
    else {
      QMessageBox::critical(0, QObject::tr("Error"),
		   QObject::tr("Clipboard Format Error:\nUnknown field!"));
      return false;
    }
  }

  return true;
}

// -------------------------------------------------------------
int Schematic::saveSymbolCpp (void)
{
  QFileInfo info (DocName);
  QString cppfile = info.path () + QDir::separator() + DataSet;
  QFile file (cppfile);

  if (!file.open (QIODevice::WriteOnly)) {
    QMessageBox::critical (0, QObject::tr("Error"),
		   QObject::tr("Cannot save C++ file \"%1\"!").arg(cppfile));
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
int Schematic::saveSymbolJSON()
{
  QFileInfo info (DocName);
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
namespace{
class sda : public ModelAccess{
public:
  sda(SchematicModel const& m, Schematic const& s) : _m(m), _s(s) {
  }
private: // SchematicSymbol
  SchematicModel const& schematicModel() const{
    incomplete();
    return _m;
  }
  SchematicModel* schematicModel() {
    return nullptr;
  }
  std::string getParameter(std::string const&) const{
    incomplete();
    return "incomplete";
  }
private:
  SchematicModel const& _m;
  Schematic const& _s;
};
}

// -------------------------------------------------------------
// Returns the number of subcircuit ports.
void Schematic::saveDocument() const
{
  // QFile file(DocName);
  // if(!file.open(QIODevice::ReadOnly)) {
  //   return -1;
  // }
  QFile file(DocName);
  file.open(QIODevice::WriteOnly | QIODevice::Truncate);
  DocumentStream stream(&file);

  sda a(DocModel, *this);

  // TODO: provide selection GUI
  auto D=docfmt_dispatcher["leg_sch"];
  D->save(stream, a);
}


// -------------------------------------------------------------
#if 0// moved
bool Schematic::loadProperties(QTextStream *stream)
{
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
		ViewX1 = nstr.section(',',0,0).toInt(&ok); if(ok) {
		ViewY1 = nstr.section(',',1,1).toInt(&ok); if(ok) {
		ViewX2 = nstr.section(',',2,2).toInt(&ok); if(ok) {
		ViewY2 = nstr.section(',',3,3).toInt(&ok); if(ok) {
		Scale  = nstr.section(',',4,4).toDouble(&ok); if(ok) {
		tmpViewX1 = nstr.section(',',5,5).toInt(&ok); if(ok)
		tmpViewY1 = nstr.section(',',6,6).toInt(&ok); }}}}} }
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
    else if(cstr == "Script") Script = nstr;
    else if(cstr == "RunScript")
		if(nstr.toInt(&ok) == 0) SimRunScript = false;
		else SimRunScript = true;
    else if(cstr == "showFrame")
		setFrameType( nstr.at(0).toLatin1() - '0');
    else if(cstr == "FrameText0") misc::convert2Unicode(FrameText[0] = nstr);
    else if(cstr == "FrameText1") misc::convert2Unicode(FrameText[1] = nstr);
    else if(cstr == "FrameText2") misc::convert2Unicode(FrameText[2] = nstr);
    else if(cstr == "FrameText3") misc::convert2Unicode(FrameText[3] = nstr);
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
void Schematic::simpleInsertComponent(Component *c)
{
  incomplete();
  return DocModel.simpleInsertComponent(c);

  Node *pn;
  // connect every node of component
  for(auto pp : c->Ports){
    int x=pp->x+c->cx_();
    int y=pp->y+c->cy_();
    qDebug() << c->label() << "port" << x << y;

    // check if new node lies upon existing node
    for(pn = nodes().first(); pn != 0; pn = nodes().next()){
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

    if(pn == 0) { // create new node, if no existing one lies at this position
      pn = new Node(x, y);
      nodes().append(pn);
    }
    pn->Connections.append(c);  // connect schematic node to component node
    if (!pp->Type.isEmpty()) {
//      pn->DType = pp->Type;
    }

    pp->Connection = pn;  // connect component node to schematic node
  }

  components().append(c);
}

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
// TODO: fixed in qt5 branch
bool Schematic::loadComponents(QTextStream *stream, Q3PtrList<Component> *List)
{
  assert(false);
  (void) stream;
  (void) List;
  unreachable();
#if 0
  QString Line, cstr;
  Component *c;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<' && Line.at(1) == '/'){
      // ?!
      return true;
    }
    Line = Line.trimmed();
    if(Line.isEmpty()) continue;

    /// \todo enable user to load partial schematic, skip unknown components
    qDebug() << "loadline" << Line;
    Element* e = getComponentFromName(Line);

    if(Command* cc=command(e)){
      qDebug() << "got command";
      simpleInsertCommand(cc);
    }else if(Component* c=component(e)){
     // c->setSchematic(this);
	
      if(List) {  // "paste" ?
	int z;
	for(z=c->name().length()-1; z>=0; z--) // cut off number of component name
	  if(!c->name().at(z).isDigit()) break;
	c->obsolete_name_override_hack(c->name().left(z+1));
	List->append(c);
      }else{
	simpleInsertComponent(c);
      }
    }else{
      incomplete();
    }
  }

  QMessageBox::critical(0, QObject::tr("Error"),
	   QObject::tr("Format Error:\n'Component' field is not closed!"));
  return false;
#endif
}

// -------------------------------------------------------------
// Inserts a wire without performing logic for optimizing.
void Schematic::simpleInsertWire(Wire *pw)
{
  Node *pn;
  // check if first wire node lies upon existing node
  for(pn = nodes().first(); pn != 0; pn = nodes().next())
    if(pn->cx_() == pw->x1_()) if(pn->cy_() == pw->y1_()) break;

  if(!pn) {   // create new node, if no existing one lies at this position
    pn = new Node(pw->x1_(), pw->y1_());
    nodes().append(pn);
  }

  if(pw->x1_() == pw->x2_()) if(pw->y1_() == pw->y2_()) {
    pn->Label = pw->Label;   // wire with length zero are just node labels
    if (pn->Label) {
      pn->Label->Type = isNodeLabel;
      pn->Label->pOwner = pn;
    }
    // yikes.
    // delete pw;           // delete wire because this is not a wire
    // return;
  }
  pn->Connections.append(pw);  // connect schematic node to component node
  pw->Port1 = pn;

  // check if second wire node lies upon existing node
  for(pn = nodes().first(); pn != 0; pn = nodes().next())
    if(pn->cx_() == pw->x2_()) if(pn->cy_() == pw->y2_()) break;

  if(!pn) {   // create new node, if no existing one lies at this position
    pn = new Node(pw->x2_(), pw->y2_());
    nodes().append(pn);
  }
  pn->Connections.append(pw);  // connect schematic node to component node
  pw->Port2 = pn;

  wires().append(pw);
}

// -------------------------------------------------------------
bool Schematic::loadWires(QTextStream *stream, Q3PtrList<Element> *List)
{
  incomplete();
  unreachable();
  Wire *w;
  QString Line;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;
    Line = Line.trimmed();
    if(Line.isEmpty()) continue;

    // (Node*)4 =  move all ports (later on)
    w = new Wire(0,0,0,0, (Node*)4,(Node*)4);
    if(!w->obsolete_load(Line)) {
      QMessageBox::critical(0, QObject::tr("Error"),
		QObject::tr("Format Error:\nWrong 'wire' line format!"));
      delete w;
      return false;
    }
    if(List) {
      if(w->x1_() == w->x2_()) if(w->y1_() == w->y2_()) if(w->Label) {
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
bool Schematic::loadDiagrams(QTextStream *stream, Q3PtrList<Diagram> *List)
{
  Diagram *d;
  QString Line, cstr;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;
    Line = Line.trimmed();
    if(Line.isEmpty()) continue;

    cstr = Line.section(' ',0,0);    // diagram type
    std::string what=cstr.toStdString();

    if(auto x=diagram_dispatcher[what.c_str()+1]){
	d=prechecked_cast<Diagram*>(x->clone());
	assert(d);
      qDebug() << "gotit" << what.c_str();
    }else
#if 0 // incomplete. see qt5 rework
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
    /*else if(cstr == "<Phasor") d = new PhasorDiagram();
    else if(cstr == "<Waveac") d = new Waveac();*/
    else
#endif
    {
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
bool Schematic::loadPaintings(QTextStream *stream, Q3PtrList<Painting> *List)
{
  incomplete();
  (void) stream;
  (void) List;
  return false;
# if 0 // PaintingList::load
  Painting *p=0;
  QString Line, cstr;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line.at(0) == '<') if(Line.at(1) == '/') return true;

    Line = Line.trimmed();
    if(Line.isEmpty()) continue;
    if( (Line.at(0) != '<') || (Line.at(Line.length()-1) != '>')) {
      QMessageBox::critical(0, QObject::tr("Error"),
	QObject::tr("Format Error:\nWrong 'painting' line delimiter!"));
      return false;
    }
    Line = Line.mid(1, Line.length()-2);  // cut off start and end character

    cstr = Line.section(' ',0,0);    // painting type
    qDebug() << cstr;
    if(Painting const* pp=painting_dispatcher[cstr.toStdString()]){
      p=prechecked_cast<Painting*>(pp->clone());
      assert(p);
    }else{
      QMessageBox::critical(0, QObject::tr("Error"),
		QObject::tr("Format Error:\nUnknown painting " + cstr));
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
#endif
}

/*!
 * \brief Schematic::loadDocument tries to load a schematic document.
 * \return true/false in case of success/failure
 */
bool Schematic::loadDocument()
{
  QFile file(docName());
  qDebug() << "opening" << docName();
  if(!file.open(QIODevice::ReadOnly)) { untested();
    /// \todo implement unified error/warning handling GUI and CLI
    if (QucsMain)
      QMessageBox::critical(0, QObject::tr("Error"),
                 QObject::tr("Cannot load document: ")+docName());
    else
      qCritical() << "Schematic::loadDocument:"
                  << QObject::tr("Cannot load document: ")+docName();
    return false;
  }else{
    DocModel.setFileInfo(docName());

    // pre-qt5 kludge
    do_loadDocument(file);
    // scene()->loadModel(DocModel); // ??
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
QString Schematic::createUndoString(char Op)
{
  Wire *pw;
  Diagram *pd;
  Painting *pp;
  Component *pc;

  // Build element document.
  QString s = "  \n";
  s.replace(0,1,Op);
  for(pc = components().first(); pc != 0; pc = components().next()) {
    QTextStream str(&s);
    incomplete();
    // SchematicModel::saveComponent(str, pc);
    s += "\n";
  }
  s += "</>\n";  // short end flag

  for(pw = wires().first(); pw != 0; pw = wires().next())
    s += pw->save()+"\n";
  // save all labeled nodes as wires
  for(Node *pn = nodes().first(); pn != 0; pn = nodes().next())
    if(pn->Label) s += pn->Label->save()+"\n";
  s += "</>\n";

  for(pd = diagrams().first(); pd != 0; pd = diagrams().next())
    s += pd->save()+"\n";
  s += "</>\n";

  for(pp = paintings().first(); pp != 0; pp = paintings().next())
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
  s.replace(0,1,Op);
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
  wires().clear();	// delete whole document
  nodes().clear();
  components().clear();
  diagrams().clear();
  paintings().clear();

  QString Line;
  QTextStream stream(s, QIODevice::ReadOnly);
  Line = stream.readLine();  // skip identity byte

  // read content *************************
  if(!loadComponents(&stream))  return false;
  if(!loadWires(&stream))  return false;
  if(!loadDiagrams(&stream, &diagrams()))  return false;
  if(!loadPaintings(&stream, &paintings())) return false;

  return true;
}

// -------------------------------------------------------------
// Same as "rebuild(QString *s)" but for symbol edit mode.
bool Schematic::rebuildSymbol(QString *s)
{
  SymbolPaints.clear();	// delete whole document

  QString Line;
  QTextStream stream(s, QIODevice::ReadOnly);
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

// ?!?!@
void Schematic::createNodeSet(QStringList& Collect, int& countInit,
			      Conductor *pw, Node *p1)
{
  if(pw->Label)
    if(!pw->Label->initValue.isEmpty())
      Collect.append("NodeSet:NS" + QString::number(countInit++) + " " +
                     p1->name() + " U=\"" + pw->Label->initValue + "\"");
}

// ---------------------------------------------------
#if 0 // obsolete
void SchematicModel::throughAllNodes(unsigned& z) const
{ untested();
  z = 0; // number cc.

  for(auto pn : nodes()){
    pn->resetNumber();
  }

  for(auto pn : nodes()){
    if(pn->hasNumber()){
    }else{
      pn->setNumber(z++);
      propagateNode(pn);
    }
  }

  qDebug() << "got" << nodes().count() << "nodes and" << z << "cc";
  nc = z;
} // throughAllNodes
#endif

// ----------------------------------------------------------
// Checks whether this file is a qucs file and whether it is an subcircuit.
// It returns the number of subcircuit ports.
int Schematic::testFile(const QString& DocName)
{
  QFile file(DocName);
  if(!file.open(QIODevice::ReadOnly)) {
    return -1;
  }

  QString Line;
  // .........................................
  // To strongly speed up the file read operation the whole file is
  // read into the memory in one piece.
  QTextStream ReadWhole(&file);
  QString FileString = ReadWhole.readAll();
  file.close();
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
void Schematic::collectDigitalSignals(void)
{
  incomplete();
//  Node *pn;
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
void SchematicModel::propagateNode(Node *pn) const
{
  std::deque<Node*> q;

  assert(pn->hasNumber());
  int z=pn->number();

  q.push_back(pn);
  while(!q.empty()){
    auto cur=q.front();
    q.pop_front();
    cur->setNumber(z);

    for(auto pe : cur->Connections){
      if(Wire* pw=wire(pe)){
	if(cur == pw->Port2) {
	  if(!pw->Port1->hasNumber()){
	    q.push_back(pw->Port1);
	  }else{
	  }
	}else if(cur == pw->Port1) { untested();
	  if(!pw->Port2->hasNumber()){
	    q.push_back(pw->Port2);
	  }else{
	  }
	}else{
	  // wires have exactly 2 connections.
	  assert(false);
	}
      }
    }
  }
}

#include <iostream>

/*!
 * \brief Schematic::throughAllComps
 * Goes through all schematic components and allows special component
 * handling, e.g. like subcircuit netlisting.
 * \param stream is a pointer to the text stream used to collect the netlist
 * \param countInit is the reference to a counter for nodesets (initial conditions)
 * \param Collect is the reference to a list of collected nodesets
 * \param ErrText is pointer to the QPlainTextEdit used for error messages
 * \param NumPorts counter for the number of ports
 * \return true in case of success (false otherwise)
 */

// BUG: produces output
// to be called from qucsator language only..
#if 0 // moved to qucsator.cc
bool Schematic::throughAllComps(QTextStream *stream, int& countInit,
                   QStringList& Collect, QPlainTextEdit *ErrText, int NumPorts,
		   NetLang const& nl)
{
  incomplete();
  return true;
  bool r;
  QString s;

  // give the ground nodes the name "gnd", and insert subcircuits etc.
  for(auto it=components().begin(); it!=components().end(); ++it) {
    Component *pc=*it;

    if(pc->isActive != COMP_IS_ACTIVE) continue;

    // check analog/digital typed components
    if(isAnalog) {
      if((pc->Type & isAnalogComponent) == 0) {
        ErrText->appendPlainText(QObject::tr("ERROR: Component \"%1\" has no analog model.").arg(pc->name() ));
        return false;
      }
    } else {
      if((pc->Type & isDigitalComponent) == 0) {
        ErrText->appendPlainText(QObject::tr("ERROR: Component \"%1\" has no digital model.").arg(pc->name() ));
        return false;
      }
    }

    pc->tAC(*stream, this, Collect, countInit, NumPorts, nl); //?!!
    // handle ground symbol
    if(pc->obsolete_model_hack() == "GND") { // BUG.
      qDebug() << "GND hack" << pc->Ports.first()->Connection->name();
      pc->Ports.first()->Connection->setName("gnd");
      continue;
#if 0 // moved to Subcircuit::tAC
    }else if(dynamic_cast<Subcircuit const*>(pc)) {
      assert(pc->obsolete_model_hack()=="Sub"); // really?
      int i;
      // tell the subcircuit it belongs to this schematic
      pc->setSchematic (this);
      QString f = pc->getSubcircuitFile();
      SubMap::Iterator it = FileList.find(f);
      if(it != FileList.end())
      {
        if (!it.value().PortTypes.isEmpty())
        {
          i = 0;
          // apply in/out signal types of subcircuit
          foreach(Port *pp, pc->Ports)
          {
            pp->Type = it.value().PortTypes[i];
            pp->Connection->DType = pp->Type;
            i++;
          }
        }
        continue;   // insert each subcircuit just one time
      }

      // The subcircuit has not previously been added
      SubFile sub = SubFile("SCH", f);
      FileList.insert(f, sub);


      // load subcircuit schematic
      s = pc->Props.first()->Value;

//      qDebug() << "reading subckt schematic" << pc->getSubcircuitFile();
      Schematic *d = new Schematic(0, pc->getSubcircuitFile());
      if(!d->loadDocument()) { // BUG. "try".
          delete d;
          /// \todo implement error/warning message dispatcher for GUI and CLI modes.
          QString message = QObject::tr("ERROR: Cannot load subcircuit \"%1\".").arg(s);
          if (QucsMain) // GUI is running
            ErrText->appendPlainText(message);
          else // command line
            qCritical() << "Schematic::throughAllComps" << message;
          return false;
      }else{
      }
      d->DocName = s;
      d->isVerilog = isVerilog;
      d->isAnalog = isAnalog;
      d->creatingLib = creatingLib;
      r = d->createSubNetlist(stream, countInit, Collect, ErrText, NumPorts, nl);
      if (r)
      {
        i = 0;
        // save in/out signal types of subcircuit
        foreach(Port *pp, pc->Ports)
        {
            //if(i>=d->PortTypes.count())break;
            pp->Type = d->PortTypes[i];
            pp->Connection->DType = pp->Type;
            i++;
        }
        sub.PortTypes = d->PortTypes;
        FileList.insert(f, sub);
      }
      delete d;
      if(!r)
      {
        return false;
      }
      continue; // BUG
    }else{
#endif
    }

#if 0 // does not work
    if(LibComp* lib = dynamic_cast</*const*/LibComp*>(pc)) {
      if(creatingLib) {
	ErrText->appendPlainText(
	    QObject::tr("WARNING: Skipping library component \"%1\".").
	    arg(pc->name()));
	continue;
      }
      QString scfile = pc->getSubcircuitFile();
      s = scfile + "/" + pc->Props.at(1)->Value;
      SubMap::Iterator it = FileList.find(s);
      if(it != FileList.end())
        continue;   // insert each library subcircuit just one time
      FileList.insert(s, SubFile("LIB", s));


      //FIXME: use different netlister for different purposes
      unsigned whatisit = isAnalog?1:(isVerilog?4:2);
      r = lib->createSubNetlist(stream, Collect, whatisit);
      if(!r) {
	ErrText->appendPlainText(
	    QObject::tr("ERROR: \"%1\": Cannot load library component \"%2\" from \"%3\"").
	    arg(pc->name(), pc->Props.at(1)->Value, scfile));
	return false;
      }
      continue; // BUG
    }
#endif

    // handle SPICE subcircuit components
    if(pc->obsolete_model_hack() == "SPICE") { // BUG
      incomplete();
      s = pc->Props.first()->Value;
      // tell the spice component it belongs to this schematic
      pc->setSchematic (this);
      if(s.isEmpty()) {
        ErrText->appendPlainText(QObject::tr("ERROR: No file name in SPICE component \"%1\".").
                        arg(pc->name()));
        return false;
      }
      QString f = pc->getSubcircuitFile();
      SubMap::Iterator it = FileList.find(f);
      if(it != FileList.end())
        continue;   // insert each spice component just one time
      FileList.insert(f, SubFile("CIR", f));

      SpiceFile *sf = (SpiceFile*)pc;
      incomplete();
      // r = sf->createSubNetlist(stream);
      ErrText->appendPlainText(sf->getErrorText());
      if(!r){
        return false;
      }
      continue; // BUG
    } else if(pc->obsolete_model_hack() == "VHDL" || pc->obsolete_model_hack() == "Verilog") {
      incomplete();
      if(isVerilog && pc->obsolete_model_hack() == "VHDL")
	continue;
      if(!isVerilog && pc->obsolete_model_hack() == "Verilog")
	continue;
      s = pc->Props.getFirst()->Value;
      if(s.isEmpty()) {
        ErrText->appendPlainText(QObject::tr("ERROR: No file name in %1 component \"%2\".").
			arg(pc->obsolete_model_hack()).
                        arg(pc->name()));
        return false;
      }
      QString f = pc->getSubcircuitFile();
      SubMap::Iterator it = FileList.find(f);
      if(it != FileList.end())
        continue;   // insert each vhdl/verilog component just one time
      s = ((pc->obsolete_model_hack() == "VHDL") ? "VHD" : "VER");
      FileList.insert(f, SubFile(s, f));

      if(pc->obsolete_model_hack() == "VHDL") {
	incomplete();
//	VHDL_File *vf = (VHDL_File*)pc;
//	r = vf->createSubNetlist(stream);
//	ErrText->appendPlainText(vf->getErrorText());
//	if(!r) {
//	  return false;
//	}
      }else if(pc->obsolete_model_hack() == "Verilog") {
	incomplete();
//	Verilog_File *vf = (Verilog_File*)pc;
//	r = vf->createSubNetlist(stream);
//	ErrText->appendPlainText(vf->getErrorText());
//	if(!r) {
//	  return false;
//	}
      }else{
      }
      continue; // BUG
    }
  }
  return true;
}
#endif

// ---------------------------------------------------
// Follows the wire lines in order to determine the node names for
// each component. Output into "stream", NodeSets are collected in
// "Collect" and counted with "countInit".
bool Schematic::giveNodeNames(QTextStream *stream, int& countInit,
                   QStringList& Collect, QPlainTextEdit *ErrText, int NumPorts,
		   NetLang const& nl)
{ untested();
  incomplete();
  (void) stream;
  (void) countInit;
  (void) nl;
  (void) Collect;
  (void) NumPorts;
  (void) ErrText;
  return 0;
#if 0
  // delete the node names
  for(auto pn : nodes()) {
    pn->State = 0;
    if(pn->Label) {
      if(isAnalog){
	qDebug() << "attach label" << pn->Label->name();
        pn->setName(pn->Label->name());
        pn->setLabel(pn->Label->name());
        // pn->setNumber(pn->Label->number());
      } else{
	assert(0);
	incomplete();
        pn->setName("net" + pn->Label->name());
      }
    } else {
      pn->setName("");
    }
  }

  // set the wire names to the connected node
  for(auto pw : wires()){
    if(pw->Label != 0) {
      if(isAnalog)
        pw->Port1->setName(pw->Label->name());
      else  // avoid to use reserved VHDL words
        pw->Port1->setName("net" + pw->Label->name());
    }
  }

  // go through components
  // BUG: ejects declarations
  if(!throughAllComps(stream, countInit, Collect, ErrText, NumPorts, nl)){
    fprintf(stderr, "Error: Could not go throughAllComps\n");
    return false;
  }

  // number connected components
  unsigned z=0;
  throughAllNodes(z);
  countInit = z;

  if(!isAnalog){
    incomplete();
    // collect all node names for VHDL signal declaration
    // collectDigitalSignals();
  }

  return true;
#endif
}

// ---------------------------------------------------
bool Schematic::createLibNetlist(QTextStream *stream, QPlainTextEdit *ErrText,
				 int NumPorts, NetLang const& nl)
{
  int countInit = 0;
  QStringList Collect;
  Collect.clear();
  FileList.clear();
  Signals.clear();
  // Apply node names and collect subcircuits and file include
  creatingLib = true;
  if(!giveNodeNames(stream, countInit, Collect, ErrText, NumPorts, nl)) {
    creatingLib = false;
    return false;
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
  (*stream) << "\n" << c << " TOP LEVEL MARK " << c << "\n";

  // Emit subcircuit components
  incomplete();
  createSubNetlistPlain(stream, ErrText, NumPorts);

  Signals.clear();  // was filled in "giveNodeNames()"
  return true;
} // createLibNetlist

//#define VHDL_SIGNAL_TYPE "bit"
//#define VHDL_LIBRARIES   ""
static const std::string VHDL_SIGNAL_TYPE("std_logic");
static const std::string VHDL_LIBRARIES("\nlibrary ieee;\nuse ieee.std_logic_1164.all;\n");

void Schematic::createSubNetlistPlain(QTextStream*, QPlainTextEdit*, int){
  unreachable();
}

// ---------------------------------------------------
// what is this?
void SchematicModel::createSubNetlistPlain(stream_t& str, QPlainTextEdit *ErrText,
int NumPorts, bool creatingLib, NetLang const& )
{
  incomplete(); // obsolete.
  bool isAnalog=true;
  auto stream=&str;
  int i, z;
  QString s;
  QStringList SubcircuitPortNames;
  QStringList SubcircuitPortTypes;
  QStringList InPorts;
  QStringList OutPorts;
  QStringList InOutPorts;
  QStringList::iterator it_name;
  QStringList::iterator it_type;
  Component *pc;

  // probably creating a library currently
  QTextStream * tstream = stream;
  QFile ofile;
  if(creatingLib) {
    //QString f = misc::properAbsFileName(DocName) + ".lst";
    //ofile.setFileName(f);
    //if(!ofile.open(IO_WriteOnly)) {
    //  ErrText->appendPlainText(tr("ERROR: Cannot create library file \"%s\".").arg(f));
    //  return;
    //}
    tstream = new QTextStream(&ofile);
  }

  // collect subcircuit ports and sort their node names into
  // "SubcircuitPortNames"
  PortTypes.clear();
  for(pc = components().first(); pc != 0; pc = components().next()) {
    if(dynamic_cast<Command const*> (pc)){
      // ignore commands.
    }else if(pc->obsolete_model_hack().at(0) == '.') { // no simulations in subcircuits
      incomplete();
//      ErrText->appendPlainText(
//        QObject::tr("WARNING: Ignore simulation component in subcircuit \"%1\".").arg(docName())+"\n");
      continue;
    }else if(dynamic_cast<Port const*>(pc)
      ||pc->obsolete_model_hack() == "Port" // BUG
      ){
      if(pc->obsolete_model_hack() != "Port"){
	incomplete();
      }

      i = pc->Props.first()->Value.toInt();
      for(z=SubcircuitPortNames.size(); z<i; z++) { // add empty port names
        SubcircuitPortNames.append(" ");
        SubcircuitPortTypes.append(" ");
      }
      it_name = SubcircuitPortNames.begin();
      it_type = SubcircuitPortTypes.begin();
      for(int n=1;n<i;n++)
      {
        it_name++;
        it_type++;
      }
      (*it_name) = pc->Ports.first()->Connection->name();
      qDebug() << "found" << *it_name;
//      DigMap::Iterator it = Signals.find(*it_name);
//      if(it!=Signals.end())
//        (*it_type) = it.value().Type;
      // propagate type to port symbol
      // pc->Ports.first()->Connection->DType = *it_type;

      bool isVerilog=false;
      if(!isAnalog) {
        if (isVerilog) {
          //Signals.remove(*it_name); // remove node name
          switch(pc->Props.at(1)->Value.at(0).toLatin1()) {
            case 'a':
              InOutPorts.append(*it_name);
              break;
            case 'o':
              OutPorts.append(*it_name);
              break;
              default:
                InPorts.append(*it_name);
          }
        }
        else {
          // remove node name of output port
          //Signals.remove(*it_name);
          switch(pc->Props.at(1)->Value.at(0).toLatin1()) {
            case 'a':
              (*it_name) += " : inout"; // attribute "analog" is "inout"
              break;
            case 'o': // output ports need workaround
              //Signals.insert(*it_name, DigSignal(*it_name, *it_type));
              (*it_name) = "net_out" + (*it_name);
              // fall through
            default:
              (*it_name) += " : " + pc->Props.at(1)->Value;
          }
          (*it_name) += " " + ((*it_type).isEmpty() ?
          QString::fromStdString(VHDL_SIGNAL_TYPE) : (*it_type));
        }
      }
    }else{
      assert(pc->obsolete_model_hack() != "Port");
    }
  }

  // remove empty subcircuit ports (missing port numbers)
  for(it_name = SubcircuitPortNames.begin(),
      it_type = SubcircuitPortTypes.begin();
      it_name != SubcircuitPortNames.end(); ) {
    if(*it_name == " ") {
      it_name = SubcircuitPortNames.erase(it_name);
      it_type = SubcircuitPortTypes.erase(it_type);
    } else {
      PortTypes.append(*it_type);
      it_name++;
      it_type++;
    }
  }

//  assert(_symbol); // for now.
  QString Type("incomplete/obsolete"); // QString::fromStdString(_symbol->type());

  Painting *pi;
  if(isAnalog) {
    incomplete();
    // ..... analog subcircuit ...................................
    qDebug() << "eject Def";
    (*tstream) << "\n.Def:" << Type << " " << SubcircuitPortNames.join(" ");
    for(pi = SymbolPaints.first(); pi != 0; pi = SymbolPaints.next())
      if(pi->name() == ".ID ") {
        ID_Text *pid = (ID_Text*)pi;
        QList<SubParameter *>::const_iterator it;
        for(it = pid->Parameter.constBegin(); it != pid->Parameter.constEnd(); it++) {
          s = (*it)->Name; // keep 'Name' unchanged
          (*tstream) << " " << s.replace("=", "=\"") << '"';
        }
        break;
      }
    (*tstream) << '\n';

    auto nlp=doclang_dispatcher["qucsator"];
    NetLang const* n=prechecked_cast<NetLang const*>(nlp);
    assert(n);
//    NetLang const& nl=*n;

    // write all components with node names into netlist file
    for(auto pc : components() ){ untested();
      qDebug() << "comps" << QString::fromStdString(pc->type());
      if(dynamic_cast<Port const*>(pc)){
	  incomplete();
      }else if(pc->type() == "GND") { untested();
      }else{
	try{
	  // fixme.
	  (*tstream) << pc->getNetlist();
	}catch(std::exception const&){
	  nlp->printItem(pc, *tstream);
	}
      }
    }

    (*tstream) << ".Def:End\n";

  } else {
    bool isVerilog=false;
    if (isVerilog) {
      incomplete(); // use verilog
      // ..... digital subcircuit ...................................
      (*tstream) << "\nmodule Sub_" << Type << " ("
              << SubcircuitPortNames.join(", ") << ");\n";

      // subcircuit in/out connections
      if(!InPorts.isEmpty())
        (*tstream) << " input " << InPorts.join(", ") << ";\n";
      if(!OutPorts.isEmpty())
        (*tstream) << " output " << OutPorts.join(", ") << ";\n";
      if(!InOutPorts.isEmpty())
        (*tstream) << " inout " << InOutPorts.join(", ") << ";\n";

      // subcircuit connections ?!?!
//      if(!Signals.isEmpty()) {
//        QList<DigSignal> values = Signals.values();
//        QList<DigSignal>::const_iterator it;
//        for (it = values.constBegin(); it != values.constEnd(); ++it) {
//          (*tstream) << " wire " << (*it).Name << ";\n";
//        }
//      }
      (*tstream) << "\n";

      // subcircuit parameters
      for(pi = SymbolPaints.first(); pi != 0; pi = SymbolPaints.next())
        if(pi->name() == ".ID ") {
          QList<SubParameter *>::const_iterator it;
          ID_Text *pid = (ID_Text*)pi;
          for(it = pid->Parameter.constBegin(); it != pid->Parameter.constEnd(); it++) {
            s = (*it)->Name.section('=', 0,0);
            QString v = misc::Verilog_Param((*it)->Name.section('=', 1,1));
            (*tstream) << " parameter " << s << " = " << v << ";\n";
          }
          (*tstream) << "\n";
          break;
        }

      // write all equations into netlist file
      for(pc = components().first(); pc != 0; pc = components().next()) {
        if(pc->obsolete_model_hack() == "Eqn") {
          (*tstream) << pc->get_Verilog_Code(NumPorts);
        }
      }

      // if(Signals.find("gnd") != Signals.end())
      (*tstream) << " assign gnd = 0;\n"; // should appear only once

      // write all components into netlist file
      for(pc = components().first(); pc != 0; pc = components().next()) {
        if(pc->obsolete_model_hack() != "Eqn") {
          s = pc->get_Verilog_Code(NumPorts);
          if(s.length()>0 && s.at(0) == '\xA7') {  //section symbol
            ErrText->appendPlainText(s.mid(1));
          }
          else (*tstream) << s;
        }
      }

      (*tstream) << "endmodule\n";
    } else {
      // ..... digital subcircuit ...................................
      (*tstream) << QString::fromStdString(VHDL_LIBRARIES);
      (*tstream) << "entity Sub_" << Type << " is\n"
                << " port ("
                << SubcircuitPortNames.join(";\n ") << ");\n";

      for(pi = SymbolPaints.first(); pi != 0; pi = SymbolPaints.next())
        if(pi->name() == ".ID ") {
          ID_Text *pid = (ID_Text*)pi;
          QList<SubParameter *>::const_iterator it;

          if (pid->Parameter.size()) {
            (*tstream) << " generic (";
            for(it = pid->Parameter.constBegin(); it != pid->Parameter.constEnd(); it++) {
              s = (*it)->Name;
              QString t = (*it)->Type.isEmpty() ? "real" : (*it)->Type;
              (*tstream) << s.replace("=", " : "+t+" := ") << ";\n ";
            }
            (*tstream) << ");\n";
          }
          break;
        }

      (*tstream) << "end entity;\n"
                  << "use work.all;\n"
                  << "architecture Arch_Sub_" << Type << " of Sub_" << Type
                  << " is\n";

     // if(!Signals.isEmpty()) {
     //   QList<DigSignal> values = Signals.values();
     //   QList<DigSignal>::const_iterator it;
     //   for (it = values.constBegin(); it != values.constEnd(); ++it) {
     //     (*tstream) << " signal " << (*it).Name << " : "
     //     << ((*it).Type.isEmpty() ?
     //         QString::fromStdString(VHDL_SIGNAL_TYPE) : (*it).Type) << ";\n";
     //   }
     // }

      // write all equations into netlist file
      for(pc = components().first(); pc != 0; pc = components().next()) {
        if(pc->obsolete_model_hack() == "Eqn") {
          ErrText->appendPlainText(
                      QObject::tr("WARNING: Equations in \"%1\" are 'time' typed.").
          arg(pc->name()));
          (*tstream) << pc->get_VHDL_Code(NumPorts);
        }
      }

      (*tstream) << "begin\n";

      // if(Signals.find("gnd") != Signals.end())
      (*tstream) << " gnd <= '0';\n"; // should appear only once

      // write all components into netlist file
      for(pc = components().first(); pc != 0; pc = components().next()) {
        if(pc->obsolete_model_hack() != "Eqn") {
            s = pc->get_VHDL_Code(NumPorts);
            if(s.length()>0 && s.at(0) == '\xA7') {  //section symbol
              ErrText->appendPlainText(s.mid(1));
          }
          else (*tstream) << s;
        }
      }

      (*tstream) << "end architecture;\n";
    }
  }

  // close file
  if(creatingLib) {
    ofile.close();
    delete tstream;
  }
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

// ---------------------------------------------------
// Determines the node names and writes subcircuits into netlist file.
// moved to QucsatorNetlister::prepareSave
#if 0
int Schematic::prepareNetlist(QTextStream& stream, QStringList& Collect,
                              QPlainTextEdit *ErrText, NetLang const& nl)
{
  incomplete();
  return 0;

  // TODO:
  // SomeNetlister.generate(stream, Collect, nl);
  // return;

  if(showBias > 0) showBias = -1;  // do not show DC bias anymore

  isVerilog = false;
  isAnalog = true;
  bool isTruthTable = false;
  int allTypes = 0, NumPorts = 0;

  // Detect simulation domain (analog/digital) by looking at component types.
  for(Component *pc = components().first(); pc != 0; pc = components().next()) {
    if(pc->isActive == COMP_IS_OPEN){
      // open circuit (or so)
//    }else if(pc->obsolete_model_hack().at(0) == '.') {
    }else if(dynamic_cast<Command const*>(pc)) {

      qDebug() << pc->obsolete_model_hack();
      if(pc->obsolete_model_hack() == ".Digi") {
        if(allTypes & isDigitalComponent) {
          ErrText->appendPlainText(
             QObject::tr("ERROR: Only one digital simulation allowed."));
          return -10;
        }else if(pc->Props.getFirst()->Value != "TimeList"){
          isTruthTable = true;
	}
	if(pc->Props.getLast()->Value != "VHDL")
	        isVerilog = true;
        allTypes |= isDigitalComponent;
	      isAnalog = false;
      }else{
       	allTypes |= isAnalogComponent;
      }
      if((allTypes & isComponent) == isComponent) {
        ErrText->appendPlainText(
           QObject::tr("ERROR: Analog and digital simulations cannot be mixed."));
        return -10;
      }
    }else if(pc->obsolete_model_hack() == "DigiSource"){
      NumPorts++;
    }else{
    }
  }

  if((allTypes & isAnalogComponent) == 0) {
    if(allTypes == 0) {
      // If no simulation exists, assume analog simulation. There may
      // be a simulation within a SPICE file. Otherwise Qucsator will
      // output an error.
      isAnalog = true;
      allTypes |= isAnalogComponent;
      NumPorts = -1;
    }
    else {
      if(NumPorts < 1 && isTruthTable) {
        ErrText->appendPlainText(
           QObject::tr("ERROR: Digital simulation needs at least one digital source."));
        return -10;
      }
      if(!isTruthTable) NumPorts = 0;
    }
  }
  else {
    NumPorts = -1;
    isAnalog = true;
  }

  // first line is documentation
  if(allTypes & isAnalogComponent)
    stream << "#";
  else if (isVerilog)
    stream << "//";
  else{
    stream << "--";
  }

  stream << " Qucs " << PACKAGE_VERSION << "  " << DocName << "\n";

  // set timescale property for verilog schematics
  if (isVerilog) {
    stream << "\n`timescale 1ps/100fs\n";
  }

  int countInit = 0;  // counts the nodesets to give them unique names

  // BUG: giveNodeNames ejects subcircuit declarations (WTF?)
  if(!giveNodeNames(&stream, countInit, Collect, ErrText, NumPorts, nl)){
    fprintf(stderr, "Error giving NodeNames\n");
    return -10;
  }

  if(allTypes & isAnalogComponent){
  }else if (!isVerilog) {
    stream << QString::fromStdString(VHDL_LIBRARIES);
    stream << "entity TestBench is\n"
	   << "end entity;\n"
	   << "use work.all;\n";
  }

  return NumPorts;
}
#endif

// ---------------------------------------------------
// Write the beginning of digital netlist to the text stream 'stream'.
// FIXME: really use lang. get rid of isVerilog
void Schematic::beginNetlistDigital(QTextStream& stream, NetLang const& /*lang*/)
{
  if (isVerilog) {
    stream << "module TestBench ();\n";
    QList<DigSignal> values = Signals.values();
    QList<DigSignal>::const_iterator it;
    for (it = values.constBegin(); it != values.constEnd(); ++it) {
      stream << "  wire " << (*it).Name << ";\n";
    }
    stream << "\n";
  } else {
    stream << "architecture Arch_TestBench of TestBench is\n";
    QList<DigSignal> values = Signals.values();
    QList<DigSignal>::const_iterator it;
    for (it = values.constBegin(); it != values.constEnd(); ++it) {
      stream << "  signal " << (*it).Name << " : "
	     << ((*it).Type.isEmpty() ?
		 QString::fromStdString(VHDL_SIGNAL_TYPE) : (*it).Type) << ";\n";
    }
    stream << "begin\n";
  }

  if(Signals.find("gnd") != Signals.end()) {
    if (isVerilog) {
      stream << "  assign gnd = 0;\n";
    } else {
      stream << "  gnd <= '0';\n";  // should appear only once
    }
  }
}

// ---------------------------------------------------
// Write the end of digital netlist to the text stream 'stream'.
// FIXME: use lang, not isVerilog
void Schematic::endNetlistDigital(QTextStream& stream, NetLang const& /*lang*/)
{
  if (isVerilog) {
  } else {
    stream << "end architecture;\n";
  }
}

// ---------------------------------------------------
// write all components with node names into the netlist file
// return some Time.
QString Schematic::createNetlist(QTextStream& stream, int NumPorts, NetLang const& nl)
{ untested();
  return "incomplete";
#if 0 // qucsatorNetlister::createNetlist
  if(!isAnalog) {
    beginNetlistDigital(stream, nl);
  }

  Signals.clear();  // was filled in "giveNodeNames()"
  FileList.clear();

  QString s, Time;
  for(Component *pc = components().first(); pc != 0; pc = components().next()) {
    if(isAnalog) {
      if(pc->type()=="GND"){ // qucsator hack
      }else{
	nl.printItem(pc, stream);
      }
    } else { // FIXME: use different lang to print things differently
      if(pc->obsolete_model_hack() == ".Digi" && pc->isActive) {  // simulation component ?
        if(NumPorts > 0) { // truth table simulation ?
	  if (isVerilog)
	    Time = QString::number((1 << NumPorts));
	  else
	    Time = QString::number((1 << NumPorts) - 1) + " ns";
        } else {
          Time = pc->Props.at(1)->Value;
	  if (isVerilog) {
	    if(!misc::Verilog_Time(Time, pc->name())) return Time;
	  } else {
	    if(!misc::VHDL_Time(Time, pc->name())) return Time;  // wrong time format
	  }
        }
      }
      if (isVerilog) {
	s = pc->get_Verilog_Code(NumPorts);
      } else {
	s = pc->get_VHDL_Code(NumPorts);
      }
      if (s.length()>0 && s.at(0) == '\xA7'){
          return s; // return error
      }
      stream << s;
    }
  }

  if(!isAnalog) {
    endNetlistDigital(stream, nl);
  }

  return Time;
#endif
}

// vim:ts=8:sw=2:noet
