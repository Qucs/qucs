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

#include "qucs_globals.h"
#include "misc.h"
#include "painting.h"
#include "qt_compat.h"
#include "qucs_app.h"
#include "schematic_doc.h"
#include "sckt_base.h"
#include "property.h"
#include "parameter.h"
#include "dot.h"

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
class CommonLib : public CommonComponent{
public:
	explicit CommonLib(int i=0) : CommonComponent(i) {}
private:
	CommonComponent* clone() const override{
		return new CommonLib(*this);
	}
};
CommonLib cl(CC_STATIC_);
/*--------------------------------------------------------------------------*/
SchematicModel empty;
// this is a prototype
class LibComp : public SubcktBase {
private:
	LibComp(LibComp const&p)
	  : SubcktBase(p),
	    _paint(nullptr) {
		new_subckt();
		// _paint = new PaintingList(); // BUG
	}
public:
	LibComp()
	  : SubcktBase(),
	    _paint(nullptr) {
		new_subckt();
		// _paint = new PaintingList(); // BUG
		attach_common(&cl);
	}
	~LibComp() { incomplete(); };
	Symbol* clone() const override{return new LibComp(*this);}
	Symbol* clone_instance() const override;

private:
	bool is_device() const override {return false;}
	SchematicModel* scope() override {return subckt();}
	std::string paramValue(unsigned n) const override{ untested();
		return SubcktBase::paramValue(n);
	}
	std::string paramValue(std::string const& n) const override{
		if(n=="$tx"){
			return "0";
		}else if(n=="$ty"){
			return "0";
		}else{
			return SubcktBase::paramValue(n);
		}
	}
	unsigned numPorts() const override{
		if(painting()){
			return painting()->numPorts();
		}else if(subckt()){
			return subckt()->numPorts();
		}else{ untested();
			unreachable();
			return 0;
		}
	}
	virtual Port& port(unsigned) {unreachable(); return *new Port();}
	pos_t portPosition(unsigned i) const override{
	//	assert(subckt());
	//	assert(subckt()->portValue(i));
		// auto pos = subckt()->portValue(i)->position();
		Symbol const* s = painting();
		auto pos = s->portPosition(i);
		return pos;
	}
	//ElementList const* symbolPaintings() const override{
	//	incomplete(); // need painting stash in sckt();
	//	return nullptr; // &paintings();
	//}

	rect_t bounding_rect() const override{ untested();
		// BUG. cache.
		rect_t br;
//		assert(symbolPaintings());
		assert(paintings());
		for(auto p : *paintings()){ untested();
			assert(p);
			Element const* e = p;
//			trace2("br", e->boundingRect().topLeft(), e->boundingRect().bottomRight());
			if(auto p = dynamic_cast<Painting const*>(e)){
				auto c = e->center(); // p!
				incomplete(); // BUG. honour p->legacyTransformhack
				br |= ( p->bounding_rect() + c );
			}
		}
		trace4("LibComp::br", label(), paintings()->size(), br.tl(), br.br());
		return br;
	}
	void paint(ViewPainter* v) const override{ untested();
		assert(paintings());
		for(auto e : *paintings()){ untested();
			incomplete(); // BUG. honour p->legacyTransformhack
			if(auto p = dynamic_cast<Painting const*>(e)){
				v->save();
				v->translate(e->position());
				p->paint(v);
				v->restore();
			}else{
			}
		}
	}

	SubcktBase const* painting() const{
		// cache?
			assert(subckt());
			auto p_ = subckt()->find_(":SymbolSection:");
			if(p_==subckt()->end()){
				return nullptr;
			}else if(auto p = dynamic_cast<SubcktBase const*>(*p_)){
				return p;
			}else{
				return nullptr;
			}
	}

public: // HACK
	// TODO: move to painting.
	SchematicModel const* paintings() const{
		if(!_paint){
			assert(subckt());
			auto p_ = subckt()->find_(":SymbolSection:");
			if(p_==subckt()->end()){
			}else if(auto p = dynamic_cast<SubcktBase const*>(*p_)){
				assert(p->subckt());
				auto const* q = p->subckt();
				_paint = q;
			}else{
			}
		}else{
		}
		if(!_paint){
			message(QucsMsgCritical, "no paint");
			_paint = &empty;
		}else{
		}
		return _paint;
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
	mutable SchematicModel const* _paint; // just caching
}d0; // LibComp
static Dispatcher<Symbol>::INSTALL p2(&symbol_dispatcher, "LegacyLibProto", &d0);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// Lib instance. TODO: use paramset + common
class Lib : public Symbol{
public:
	explicit Lib(Symbol const* p);
	explicit Lib(CommonComponent* c=nullptr)
		: Symbol(),
		 _tx(0), _ty(0), _parent(nullptr) {
		setTypeName("Lib");
		attach_common(c);
		new_subckt(); // hmm.
	}
	Lib(Lib const& l);

private: // Element
	Symbol* clone()const override{
		return new Lib(*this);
	}
	void paint(ViewPainter* v) const override{ untested();
		// if(has_common()){untested();
		//   common()->paint(p);
		// }else
		if(auto p = dynamic_cast<Painting const*>(_parent)){
			// no-op? is the painting a sub-object?
			p->paint(v);
		}else{ untested();
		}
		Symbol::paint(v);
	}
	rect_t bounding_rect() const override{ untested();
		// if(has_common()){untested();
		//   return common()->bounding_rect(p);
		// }else
		if(_parent){itested();
			return _parent->bounding_rect();
		}else{ untested();
			unreachable();
			return rect_t();
		}
	}

private: // Symbol
	ElementList const* symbolPaintings() const override{ untested();
		if(_parent){itested();
			assert( _parent->subckt());
			incomplete();
			return nullptr; // &_parent->subckt()->paintings(); // bug or feature?
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

		/// not here. move to some elaborate/expand hook.
		if(_section.Value == ""){
		}else if(_component.Value == ""){ untested();
		}else if(redo){ untested();
			attachProto();	
			assert(_parent); // for now.
		}else{ untested();
		}
	}
	unsigned paramCount() const{ return Symbol::paramCount() + 4 + _params.size(); }
	void setParameter(unsigned n, std::string const& v) override{
		assert(n<Lib::paramCount());
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
			trace3("fwd", n, m, v);
			if(m - 4 < 0){
				Symbol::setParameter(n, v);
			}else if(m - 4 >= int(_params.size())){ untested();
				Symbol::setParameter(n, v);
			}else if(auto p = dynamic_cast<PARAMETER<double>* >(_params[m-4])){ untested();
				*p = v;
			}else{
				unreachable();
			}
			break;
		}

		if(_section.Value == ""){ untested();
		}else if(_component.Value == ""){
		}else if(redo){
			// BUG: not here.
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
		int m = i - Symbol::paramCount();
		switch(m){
		case 0:
			return std::to_string(_tx);
		case 1:
			return std::to_string(_ty);
		case 2:
			return _section.Value.toStdString();
		case 3:
			return _component.Value.toStdString();
		default:
			if(m - 4 < 0){
				return Symbol::paramValue(i);
			}else if(m - 4 >= int(_param_names.size())){ untested();
				return Symbol::paramValue(i);
			}else if(auto p = dynamic_cast<PARAMETER<double>* >(_params[m-4])){
			  	return p->string();
			}else{
				unreachable();
				return "NA";
			}
		}
	}
	std::string paramName(unsigned i) const override{
		int m = i - Symbol::paramCount();
		switch(m){
		case 0:
			return "$tx";
		case 1:
			return "$ty";
		case 2:
			return "Lib";
		case 3:
			return "Component";
		default:itested();
			if(m - 4 < 0){
				return Symbol::paramName(i);
			}else if(m - 4 < int(_param_names.size())){
			  	return _param_names[m-4];
			}else{
				return Symbol::paramName(i);
			}
		}
	}

private:
	void attachProto() {
		std::string t = "Lib:" + _section.Value.toStdString() + ":" + _component.Value.toStdString();
		if(_component.Value == "") { untested();
		}else if(_section.Value == "") { untested();
		}else{
			Symbol* s = symbol_dispatcher[t]; // BUG: userlib. need find_looking_out
			trace2("Lib::attachProto", t, s);
//			attach_common(s->mutable_common());
			_parent = s;
			if(!_parent){
				message(QucsMsgCritical, ("cannot find " + t).c_str());
			}else{
			}
		}

		if(!_parent){
			_parent = &d0;
		}else{
		}

		_ports.resize(numPorts());
		trace2("Lib::attachProto", numPorts(), _ports.size());
		// also prepare parameters here.
		set_dev_type(t); // use common maybe?
		setTypeName("Lib"); // assert?

		for(auto i : _params){
			delete i;
		}
		_params.clear();
		_param_names.clear();
		for(auto i : *paintings()){
			if(auto a=dynamic_cast<DEV_DOT*>(i)){
				istream_t cs(istream_t::_STRING, a->s());
				if(cs.umatch("parameter")){ // portparameter?
					trace1("LibComp DOT", a->s());
					std::string name;
					std::string defv;
					cs >> name;
					cs >> defv;
					auto p = new PARAMETER<double>;
					*p = defv;
					_params.push_back(p);
					_param_names.push_back(name);
				}else{
				}
			}else{
			}
		}
	}

private:
	SchematicModel const* paintings() const{
		assert(_parent);
		if(auto p = dynamic_cast<LibComp const*>(_parent)){
			return p->paintings();
		}else{
			return &empty;
		}
	}

private:
	Symbol const* proto() const;
private:
	int _tx;
	int _ty;
	Property _section;
	Property _component;
	Symbol const* _parent; // TODO. common.
	std::vector<Port> _ports;
	std::vector<std::string> _param_names; // could be common?
	std::vector<PARA_BASE*> _params; // could be common
}; // Lib
Lib D(&cl);
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "Lib", &D);
/*--------------------------------------------------------------------------*/
Lib::Lib(Symbol const* p)
	  : Symbol(), _tx(0), _ty(0), _parent(p)
{
	setTypeName("Lib"); // really?
	// new_subckt(); // hmm, maybe not.

	if(p){
		_ports.resize(p->numPorts());
		assert(p->common());
		attach_common(p->common()->clone());
	}else{
		unreachable();
	}
}
/*--------------------------------------------------------------------------*/
Lib::Lib(Lib const& l)
	: Symbol(l), _tx(l._tx), _ty(l._ty),
	_section(l._section),
	_component(l._component),
	_parent(l._parent),
	_ports(l._ports.size())
{
	new_subckt(); // hmm. copy?
	// todo: move all the model stuff into common (more work)
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// partially tAC. build a new sckt proto.
// does not fullymake sense.
#if 0
Symbol const* Lib::proto() const
{
   auto t = typeName();
	auto p = find_looking_out(t);

	if(auto sp = dynamic_cast<Symbol const*>(p)){ untested();
		trace1("cached", typeName());
		return sp;
	}else{
		trace1("not cached", typeName());

		assert(false);
#if 0
		
		// prototypes must be stashed in the project, as they are not global.
		QucsDoc* project = getProject(this);

		assert(_parent);
		//if(auto o=dynamic_cast<SchematicSymbol*>(owner()))
		project->cacheProto(_parent);
#endif
	}

	assert(_parent);
	return _parent;
}
#endif
/*--------------------------------------------------------------------------*/
// Makes the schematic symbol subcircuit with the correct number
// of ports.
#if 0
void LibComp::createSymbol()
{ untested();
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
}
#endif
/*--------------------------------------------------------------------------*/
#if 0
QString LibComp::netlist() const
{ untested();
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
  return "";
}
#endif
/*--------------------------------------------------------------------------*/
#if 0
QString LibComp::verilogCode(int)
{ untested();
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
  return "";
}
#endif
/*--------------------------------------------------------------------------*/
#if 0
QString LibComp::vhdlCode(int)
{ untested();
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
}
#endif
/*--------------------------------------------------------------------------*/
Symbol* LibComp::clone_instance() const
{
	assert(common());
	return new Lib(this);
}
/*--------------------------------------------------------------------------*/
} //namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
