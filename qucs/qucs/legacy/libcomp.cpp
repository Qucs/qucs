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
#include "sckt_base.h"

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

private:
	std::string paramValue(unsigned n) const override{ untested();
		return SchematicSymbol::paramValue(n);
	}
	std::string paramValue(std::string const& n) const override{ untested();
		if(n=="$tx"){ untested();
			return "0";
		}else if(n=="$ty"){ untested();
			return "0";
		}else{ untested();
			return SchematicSymbol::paramValue(n);
		}
	}
	unsigned numPorts() const override{
		if(subckt()){
			return subckt()->numPorts();
		}else{ untested();
			unreachable();
			return 0;
		}
	}
	virtual Port& port(unsigned) {unreachable(); return *new Port();}
	pos_t portPosition(unsigned i) const override{
		untested();
		assert(subckt());
		assert(subckt()->portValue(i));
		auto pos = subckt()->portValue(i)->position();
		return pos;
	}

	QRectF boundingRect() const override{ untested();
		// BUG. cache.
		QRectF br;
//		assert(symbolPaintings());
		for(auto p : paintings()){ untested();
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
	int _tx;
	int _ty;
	Property _section;
	Property _component;

}d0;
static Dispatcher<Symbol>::INSTALL p2(&symbol_dispatcher, "LegacyLibProto", &d0);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/// THIS IS WRONG> must be symbol
class Lib : public Symbol{
public:
	explicit Lib():Symbol(), _tx(0), _ty(0), _parent(nullptr) {
		setTypeName("Lib"); // really?
		new_subckt(); // hmm.
	}
	Lib( Lib const& l) : Symbol(l), _parent(l._parent),
		_ports(0)
	{
		_ports.resize(l._ports.size());
		new_subckt(); // hmm. copy?
		              // todo: move all the model stuff into common (more work)
	}

private: // Element
	Symbol* clone()const override{
		return new Lib(*this);
	}
	void paint(ViewPainter* p) const override{ untested();
		if(_parent){itested();
			// no-op?
			// ((Element*)_parent)->paint(p);
		}else{ untested();
		}
		Symbol::paint(p);
	}
	QRectF boundingRect() const override{ untested();
		if(_parent){itested();
			return _parent->boundingRect();
		}else{ untested();
			unreachable();
			return QRectF();
		}
	}

private: // Symbol
	PaintingList const* symbolPaintings() const override{ untested();
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

		if(_section.Value == ""){
		}else if(_component.Value == ""){ untested();
		}else if(redo){ untested();
			attachProto();	
			assert(_parent); // for now.
		}else{ untested();
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

		if(_section.Value == ""){ untested();
		}else if(_component.Value == ""){
		}else if(redo){
			attachProto();	
			assert(_parent); // for now.
		}else{ untested();
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
		std::string t = "Lib:" + _section.Value.toStdString() + ":" + _component.Value.toStdString();
		if(_component.Value == "") { untested();
		}else if(_section.Value == "") { untested();
		}else{
			Symbol* s = symbol_dispatcher[t];
			trace2("Lib::attachProto", t, s);
			_parent = s;
			if(!_parent){
//				throw ...
				trace1("Lib::attachProto fail", t);
			}
		}

		_ports.resize(numPorts());
		trace2("Lib::attachProto", numPorts(), _ports.size());
		// also prepare parameters here.
	}
	Symbol const* proto(SchematicModel const* scope) const;

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
// partially tAC. build a new sckt proto.
// does not fullymake sense.
Symbol const* Lib::proto(SchematicModel const* scope) const
{
   auto t = QString::fromStdString(typeName());
	auto p = scope->findProto(t);
	if(p){ untested();
		trace1("cached", typeName());
		return p;
	}else{
		trace1("not cached", typeName());

		assert(_parent);
		scope->cacheProto(_parent); // this must be wrong. cache protos in list
                                  // command as needed.
	}

	assert(_parent);
	return _parent;
}
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
{ untested();
#if 0
  tx = INT_MIN;
  ty = INT_MIN;
  if(loadSymbol() > 0) { untested();
    if(tx == INT_MIN)  tx = x1+4;
    if(ty == INT_MIN)  ty = y2+4;
  }else{ untested();
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
bool LibComp::createSubNetlist(DocumentStream& stream, QStringList &FileList,
			       int type)
{ untested();
#if 0
  int r = -1;
  QString FileString;
  QStringList Includes;
  if(type&1) { untested();
    r = loadSection("Model", FileString, &Includes);
  } else if(type&2) { untested();
    r = loadSection("VHDLModel", FileString, &Includes);
  } else if(type&4) { untested();
    r = loadSection("VerilogModel", FileString, &Includes);
  }
  if(r < 0)  return false;

  // also include files
  int error = 0;
  for(QStringList::Iterator it = Includes.begin();
      it != Includes.end(); ++it ) { untested();
    QString s = getSubcircuitFile()+"/"+*it;
    if(FileList.indexOf(s) >= 0) continue;
    FileList.append(s);

    // load file and stuff into stream
    QFile file(s);
    if(!file.open(QIODevice::ReadOnly)) { untested();
      error++;
    } else { untested();
      QByteArray FileContent = file.readAll();
      file.close();
      //?stream->writeRawBytes(FileContent.value(), FileContent.size());
      stream << FileContent.data(); // BUG. lang?!
    }
  }

  stream << "\n" << FileString << "\n"; //??
  return error > 0 ? false : true;
#endif
}

// -------------------------------------------------------
QString LibComp::netlist() const
{ untested();
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
{ untested();
#if 0
  QString s = "  Sub_" + createType() + " " + Name + " (";

  // output all node names
  QListIterator<Port *> iport(Ports);
  Port *pp = iport.next();
  if(pp)  s += pp->Connection->name();
  while (iport.hasNext()) { untested();
    pp = iport.next();
    s += ", "+pp->Connection->name();   // node names
  }

  s += ");\n";
  return s;
#endif
}
/*--------------------------------------------------------------------------*/
QString LibComp::vhdlCode(int)
{ untested();
#if 0
  QString s = "  " + Name + ": entity Sub_" + createType() + " port map (";

  // output all node names
  QListIterator<Port *> iport(Ports);
  Port *pp = iport.next();
  if(pp)  s += pp->Connection->name();
  while (iport.hasNext()) { untested();
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
