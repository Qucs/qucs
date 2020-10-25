/***************************************************************************
    copyright            : (C) 2005 by Michael Margraf
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

#include "globals.h"
#include "misc.h"
#include "painting.h"
#include "qt_compat.h"
#include "qucs.h"
#include "schematic_doc.h"

#include <limits.h>

#include <QTextStream>
#include <QDir>
#include <QRegExp>
#include <QDebug>

class QTextStream;
class DocumentStream;
class QString;
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
class LibComp : public SchematicSymbol  {
private:
	LibComp(LibComp const&p) : SchematicSymbol(p) {
		new_subckt();
	}
public:
	LibComp();
	~LibComp() {};
	Symbol* clone() const{return new LibComp(*this);}
	Symbol* clone_instance() const{incomplete(); return new LibComp(*this);}

	bool createSubNetlist(DocumentStream&, QStringList&, int type=1); // BUG
	QString getSubcircuitFile() const;

private:
	std::string paramValue(unsigned n) const override{ untested();
		return SchematicSymbol::paramValue(n);
	}
	std::string paramValue(std::string const& n) const override{
		if(n=="$tx"){
			return "0";
		}else if(n=="$ty"){
			return "0";
		}else{
			return SchematicSymbol::paramValue(n);
		}
	}
	unsigned numPorts() const override{
		if(subckt()){
			return subckt()->numPorts();
		}else{
			unreachable();
			return 0;
		}
	}
	virtual Port& port(unsigned i) {unreachable(); return *new Port();}
	pos_t portPosition(unsigned i) const override{
		untested();
		assert(subckt());
		assert(subckt()->portValue(i));
		auto pos = subckt()->portValue(i)->position();
		return pos;
	}

	QRectF boundingRect() const override{
		// BUG. cache.
		QRectF br;
//		assert(symbolPaintings());
		for(auto p : paintings()){
			assert(p);
			Element const* e = p;
			trace2("br", e->boundingRect().topLeft(), e->boundingRect().bottomRight());
			auto c = e->center();
			auto cc = makeQPointF(c);
			br |= e->boundingRect().translated(cc);
		}
		trace4("br", label(), symbolPaintings()->size(), br.topLeft(), br.bottomRight());
		return br;
	}

protected:
	QString netlist() const;
	QString vhdlCode(int);
	QString verilogCode(int);
	void createSymbol();

private:
	int  loadSymbol();
	int  loadSection(const QString&, QString&, QStringList* i=0);
	QString createType() const;

	int _tx;
	int _ty;
	Property _section;
	Property _component;

}d0;
static Dispatcher<Symbol>::INSTALL p2(&symbol_dispatcher, "LegacyLibProto", &d0);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class Lib : public Symbol{
public:
	explicit Lib():Symbol(), _tx(0), _ty(0), _parent(nullptr) {
		setTypeName("Lib"); // really?
	}
	Lib( Lib const& l) : Symbol(l), _parent(l._parent),
		_ports(0)
	{
		_ports.resize(l._ports.size());
	}

private: // Element
	Symbol* clone()const override{
		return new Lib(*this);
	}
	void paint(ViewPainter* p) const override{
		if(_parent){itested();
			// no-op?
			// ((Element*)_parent)->paint(p);
		}else{ untested();
		}
		Symbol::paint(p);
	}
	QRectF boundingRect() const override{
		if(_parent){itested();
			return _parent->boundingRect();
		}else{ untested();
			unreachable();
			return QRectF();
		}
	}

private: // Symbol
	PaintingList const* symbolPaintings() const override{
		if(_parent){itested();
			assert( _parent->subckt());
			return &_parent->subckt()->paintings();
		}else{ untested();
			return nullptr;
		}
	}
	unsigned numPorts() const override{
		if(_parent){itested();
			trace2("Lib::numPorts", label(), _parent->numPorts());
			return _parent->numPorts();
		}else{itested();
			return 0;
		}
	}
	// Lib::
	pos_t portPosition(unsigned i) const override{
		assert(_parent);
		return _parent->portPosition(i);
	}
	Port& port(unsigned i) override{itested();
		assert(i < _ports.size());
		return _ports[i];
	}
	void setParameter(std::string const& n, std::string const& v) override{
		bool redo = false;
		if(n == "Lib"){ itested();
			_section.Value = QString::fromStdString(v);
			redo = true;
		}else if(n=="Component"){ itested();
			_component.Value = QString::fromStdString(v);
			redo = true;
		}else{itested();
			Symbol::setParameter(n, v);
		}

		if(redo){
			attachProto();	
		}else{
		}
	}
	unsigned paramCount() const{ return Symbol::paramCount() + 4; }
	void setParameter(unsigned n, std::string const& v) override{
		bool redo = false;
		int m = int(n) - int(Symbol::paramCount());
		trace3("Lib:SP", n, v, m);
		switch(m){
		case 0:
			break;
		case 1:
			break;
		case 2:
			_section.Value = QString::fromStdString(v);
			redo = true;
			break;
		case 3:
			_component.Value = QString::fromStdString(v);
			redo = true;
			break;
		default: untested();
			trace2("fwd", n, v);
			Symbol::setParameter(n, v);
			break;
		}

		if(redo){
			attachProto();	
		}else{
		}
	}
	std::string paramValue(std::string const& n) const override{
		if (n=="$tx"){
			return std::to_string(_tx);
		}else if (n=="$ty"){
			return std::to_string(_ty);
		}else{itested();
			return Symbol::paramValue(n);
		}
	}
	std::string paramValue(unsigned i) const override{
		int j = i - Symbol::paramCount();
		switch(j){
		case 0:
			return std::to_string(_tx);
		case 1:
			return std::to_string(_ty);
		case 2:
			return _section.Value.toStdString();
		case 3:
			return _component.Value.toStdString();
		default: untested();
			return Symbol::paramName(i);
		}
	}
	std::string paramName(unsigned i) const override{
		int j = i - Symbol::paramCount();
		switch(j){
		case 0:
			return "$tx";
		case 1:
			return "$ty";
		case 2:
			return "Lib";
		case 3:
			return "Component";
		default:itested();
			return Symbol::paramName(i);
		}
	}

private:
	void attachProto() {
		if(_component.Value == "") {
		}else if(_section.Value == "") {
		}else{
			std::string t = "Lib:" + _section.Value.toStdString() + ":" + _component.Value.toStdString();
			Symbol* s = symbol_dispatcher[t];
			trace2("Lib::attachProto", t, s);
			_parent = s;
		}

		_ports.resize(numPorts());
		trace2("Lib::attachProto", numPorts(), _ports.size());
		// also prepare parameters here.
	}

private:
	int _tx;
	int _ty;
	Property _section;
	Property _component;
	Symbol const* _parent; // BUG. common.
	std::vector<Port> _ports;
}D; // Lib
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "Lib", &D);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
LibComp::LibComp()
	: SchematicSymbol()
{
  // Description = QObject::tr("Component taken from Qucs library");

  // Ports.append(new Port(0, 0)); // dummy port because of being device
}
/*--------------------------------------------------------------------------*/
// Makes the schematic symbol subcircuit with the correct number
// of ports.
void LibComp::createSymbol()
{
#if 0
  tx = INT_MIN;
  ty = INT_MIN;
  if(loadSymbol() > 0) {
    if(tx == INT_MIN)  tx = x1+4;
    if(ty == INT_MIN)  ty = y2+4;
  }else{
    // only paint a rectangle
    Lines.append(new Line(-15, -15, 15, -15, QPen(Qt::darkBlue,2)));
    Lines.append(new Line( 15, -15, 15,  15, QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-15,  15, 15,  15, QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-15, -15,-15,  15, QPen(Qt::darkBlue,2)));

    x1 = -18; y1 = -18;
    x2 =  18; y2 =  18;

    tx = x1+4;
    ty = y2+4;
  }
#endif
}
/*--------------------------------------------------------------------------*/
// Loads the section with name "Name" from library file into "Section".
//  negative return value seems to indicate an error state.
int LibComp::loadSection(const QString& Name, QString& Section,
			 QStringList *Includes)
{
#if 0
  QDir Directory(QucsSettings.libDir());
  QFile file(Directory.absoluteFilePath(Props.first()->Value + ".lib"));
  if(!file.open(QIODevice::ReadOnly)){
	  // throw?
    return -1;
  }else{
  }

  QString libDefaultSymbol;

  QTextStream ReadWhole(&file);
  Section = ReadWhole.readAll();
  file.close();


  if(Section.left(14) != "<Qucs Library "){
	  // wrong file type ?
    return -2;
  }else{
  }

  int Start, End = Section.indexOf(' ', 14);
  if(End < 15) return -3;
  QString Line = Section.mid(14, End-14); // extract version string
  VersionTriplet LibVersion = VersionTriplet(Line);
  if (LibVersion > QucsVersion) // wrong version number ?
    return -3;

  if(Name == "Symbol") {
    Start = Section.indexOf("\n<", 14); // library has default symbol
    if(Start > 0)
      if(Section.mid(Start+2, 14) == "DefaultSymbol>") {
        Start += 16;
        End = Section.indexOf("\n</DefaultSymbol>", Start);
        if(End < 0)  return -9;
        libDefaultSymbol = Section.mid(Start, End-Start);
      }else{
		}
  }else{
  }

  // search component
  Line = "\n<Component " + Props.next()->Value + ">";
  Start = Section.indexOf(Line);
  if(Start < 0)  return -4;  // component not found
  Start = Section.indexOf('\n', Start);
  if(Start < 0)  return -5;  // file corrupt
  Start++;
  End = Section.indexOf("\n</Component>", Start);
  if(End < 0)  return -6;  // file corrupt
  Section = Section.mid(Start, End-Start+1);
  
  // search model includes
  if(Includes) {
    int StartI, EndI;
    StartI = Section.indexOf("<"+Name+"Includes");
    if(StartI >= 0) {  // includes found
      StartI = Section.indexOf('"', StartI);
      if(StartI < 0)  return -10;  // file corrupt
      EndI = Section.indexOf('>', StartI);
      if(EndI < 0)  return -11;  // file corrupt
      StartI++; EndI--;
      QString inc = Section.mid(StartI, EndI-StartI);
      QStringList f = inc.split(QRegExp("\"\\s+\""));
      for(QStringList::Iterator it = f.begin(); it != f.end(); ++it ) {
	Includes->append(*it);
      }
    }
  }

  // search model
  Start = Section.indexOf("<"+Name+">");
  if(Start < 0) {
    if((Name == "Symbol") && (!libDefaultSymbol.isEmpty())) {
      // component does not define its own symbol but the library defines a default symbol
      Section = libDefaultSymbol;
      return 0;
    } else {
      return -7;  // symbol not found
    }
  }
  Start = Section.indexOf('\n', Start);
  if(Start < 0)  return -8;  // file corrupt
  while(Section.at(++Start) == ' ') ;
  End = Section.indexOf("</"+Name+">", Start);
  if(End < 0)  return -9;  // file corrupt

  // snip actual model
  Section = Section.mid(Start, End-Start);
#endif
  return 0;
}
/*--------------------------------------------------------------------------*/
// Loads the symbol for the subcircuit from the schematic file and
// returns the number of painting elements.
int LibComp::loadSymbol()
{
#if 0
  int z, Result;
  QString FileString, Line;
  z = loadSection("Symbol", FileString);
  if(z < 0) {
    if(z != -7)  return z;

    // If library component not defined as subcircuit, then load
    // new component and transfer data to this component.
    z = loadSection("Model", Line);
    if(z < 0)  return z;

    Element *e=nullptr; incomplete(); // getComponentFromName(Line);
    Component *pc = component(e);
    if(pc == 0)  return -20;

    copyComponent(pc);

    pc->Props.setAutoDelete(false);
    delete pc;

    return 1;
  }


  z  = 0;
  x1 = y1 = INT_MAX;
  x2 = y2 = INT_MIN;

  QTextStream stream(&FileString, QIODevice::ReadOnly);
  while(!stream.atEnd()) {
    Line = stream.readLine();
    Line = Line.trimmed();
    if(Line.isEmpty())  continue;
    if(Line.at(0) != '<') return -11;
    if(Line.at(Line.length()-1) != '>') return -12;
    Line = Line.mid(1, Line.length()-2); // cut off start and end character
    Result = analyseLine(Line, 2);
    if(Result < 0) return -13;   // line format error
    z += Result;
  }

  x1 -= 4;  x2 += 4;   // enlarge component boundings a little
  y1 -= 4;  y2 += 4;
  return z;      // return number of ports
#endif
  return 0;
}
/*--------------------------------------------------------------------------*/
QString LibComp::getSubcircuitFile() const
{
#if 0
  QDir Directory(QucsSettings.libDir());
  QString FileName = Directory.absoluteFilePath(Props.first()->Value);
  return misc::properAbsFileName(FileName);
#endif
  return "";
}
/*--------------------------------------------------------------------------*/
bool LibComp::createSubNetlist(DocumentStream& stream, QStringList &FileList,
			       int type)
{
  int r = -1;
  QString FileString;
  QStringList Includes;
  if(type&1) {
    r = loadSection("Model", FileString, &Includes);
  } else if(type&2) {
    r = loadSection("VHDLModel", FileString, &Includes);
  } else if(type&4) {
    r = loadSection("VerilogModel", FileString, &Includes);
  }
  if(r < 0)  return false;

  // also include files
  int error = 0;
  for(QStringList::Iterator it = Includes.begin();
      it != Includes.end(); ++it ) {
    QString s = getSubcircuitFile()+"/"+*it;
    if(FileList.indexOf(s) >= 0) continue;
    FileList.append(s);

    // load file and stuff into stream
    QFile file(s);
    if(!file.open(QIODevice::ReadOnly)) {
      error++;
    } else {
      QByteArray FileContent = file.readAll();
      file.close();
      //?stream->writeRawBytes(FileContent.value(), FileContent.size());
      stream << FileContent.data(); // BUG. lang?!
    }
  }

  stream << "\n" << FileString << "\n"; //??
  return error > 0 ? false : true;
}
/*--------------------------------------------------------------------------*/
QString LibComp::createType() const
{
#if 0
  QString Type = misc::properFileName(Props.first()->Value);
  return misc::properName(Type + "_" + Props.next()->Value);
#endif
}

// -------------------------------------------------------
QString LibComp::netlist() const
{
#if 0
  QString s = "Sub:"+Name;   // output as subcircuit

  // output all node names
  foreach(Port *p1, Ports)
    s += " "+p1->Connection->name();   // node names

  // output property
  s += " Type=\""+createType()+"\"";   // type for subcircuit

  // output user defined parameters
  for(Property *pp = Props.at(2); pp != 0; pp = Props.next())
    s += " "+pp->Name+"=\""+pp->Value+"\"";

  return s + '\n';
#endif
  return "";
}
/*--------------------------------------------------------------------------*/
QString LibComp::verilogCode(int)
{
#if 0
  QString s = "  Sub_" + createType() + " " + Name + " (";

  // output all node names
  QListIterator<Port *> iport(Ports);
  Port *pp = iport.next();
  if(pp)  s += pp->Connection->name();
  while (iport.hasNext()) {
    pp = iport.next();
    s += ", "+pp->Connection->name();   // node names
  }

  s += ");\n";
  return s;
#endif
}
/*--------------------------------------------------------------------------*/
QString LibComp::vhdlCode(int)
{
#if 0
  QString s = "  " + Name + ": entity Sub_" + createType() + " port map (";

  // output all node names
  QListIterator<Port *> iport(Ports);
  Port *pp = iport.next();
  if(pp)  s += pp->Connection->name();
  while (iport.hasNext()) {
    pp = iport.next();
    s += ", "+pp->Connection->name();   // node names
  }

  s += ");\n";
  return s;
#endif
}
/*--------------------------------------------------------------------------*/
} //namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
