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
#include "symbol.h"
#include "qt_compat.h"
#include "qucs_app.h"
#include "sckt_base.h"
#include "property.h"
#include "parameter.h"
#include "dot.h"
#include "viewpainter.h"

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
using qucs::SubcktBase;
/*--------------------------------------------------------------------------*/
pos_t portPosition(SubcktBase const* sckt, index_t i)
{
	std::string n = sckt->portName(i);
	ElementList const* s = sckt->scope();

	if(s){
		auto ii = s->find_(n);
		if(ii!=s->end()){
			trace1("SubcktProto::portPosition hit", i);
			return (*ii)->position();
		}else{
			trace2("SubcktProto::portPosition miss", i, n);
		}

	}else{ untested();
	}
	unreachable();
	return pos_t(0,0);
}
/*--------------------------------------------------------------------------*/
using namespace qucs;
/*--------------------------------------------------------------------------*/
class CommonLib : public CommonComponent {
public:
	explicit CommonLib(int i=0) : CommonComponent(i) {}
private:
	CommonComponent* clone() const override{
		return new CommonLib(*this);
	}

	index_t param_count() const override{ return 0; }
};
CommonLib cl(CC_STATIC_);
/*--------------------------------------------------------------------------*/
ElementList empty;
// this is a prototype
class LibComp : public SubcktBase, public Painting {
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
	Component* clone() const override{return new LibComp(*this);}
	Element* clone_instance() const override;

private:
	bool is_device() const override {return false;}
	ElementList* scope() override {return subckt();}
	std::string param_value(index_t n) const override{ untested();
		return SubcktBase::param_value(n);
	}
	std::string param_value_by_name(std::string const& n) const override{
		if(n=="$tx"){
			return "0";
		}else if(n=="$ty"){
			return "0";
		}else{
			return SubcktBase::param_value_by_name(n);
		}
	}
	index_t numPorts() const override{
		if(painting()){
			return painting()->numPorts();
////		}else if(subckt()){
////			return subckt()->numPorts();
		}else{ untested();
			unreachable();
			return 0;
		}
	}
//	virtual Port& port(unsigned) {unreachable(); return *new Port();}
//	pos_t portPosition(unsigned i) const override{
//	//	assert(subckt());
//	//	assert(subckt()->portValue(i));
//		// auto pos = subckt()->portValue(i)->position();
//		Symbol const* s = painting();
//		auto pos = s->portPosition(i);
//		return pos;
//	}

	rect_t bounding_rect() const override{itested();
		// BUG. cache.
		rect_t br;
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

	// same in nonlegacy Sub...?
	void paint(ViewPainter* v) const override{itested();
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

public: // eek
	SubcktBase const* painting() const{
		// cache?
			assert(subckt());
			auto p_ = subckt()->find_("Symbol");
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
	ElementList const* paintings() const{
		if(!_paint){
			assert(subckt());
			auto p_ = subckt()->find_("Symbol");
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
			message(MsgCritical, "no paint");
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
	mutable ElementList const* _paint; // just caching
}d0; // LibComp
static Dispatcher<Component>::INSTALL p2(&device_dispatcher, "LegacyLibProto", &d0);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// Lib instance. TODO: use paramset + common
class Lib : public Symbol{
public:
	explicit Lib(Component const* p);
	explicit Lib(CommonComponent* c=nullptr)
		: Symbol(),
		 _tx(0), _ty(0), _parent(nullptr) {
		set_dev_type("Lib");
		attach_common(c);
		new_subckt(); // hmm.
	}
	Lib(Lib const& l);

private: // Element
	Symbol* clone()const override{
		return new Lib(*this);
	}
	void paint(ViewPainter* v) const override{itested();
		// if(has_common()){untested();
		//   common()->paint(p);
		// }else
		if(auto p = dynamic_cast<Painting const*>(_parent)){
			// no-op? is the painting a sub-object?
			p->paint(v);
		}else{itested();
		}
		Symbol::paint(v);
	}
	rect_t bounding_rect() const override{itested();
		// if(has_common()){untested();
		//   return common()->bounding_rect(p);
		// }else
		if(auto p=dynamic_cast<Painting const*>(_parent)){itested();
			return p->bounding_rect();
		}else{itested();
			unreachable();
			return rect_t();
		}
	}
	std::string dev_type() const override {return "Lib";}

private: // Symbol
	index_t numPorts() const override{
		if(_parent){itested();
//			trace2("Lib::numPorts", label(), _parent->numPorts());
			return _parent->numPorts();
		}else{itested();
			return 0;
		}
	}
	// Lib::
	pos_t portPosition(index_t i) const override{
		assert(_parent);
		if(auto p = dynamic_cast<Symbol const*>(_parent)){ untested();
			return p->portPosition(i);
		}else if(auto p = dynamic_cast<LibComp const*>(_parent)){ untested();
			if(p->painting()){
				return ::portPosition(p->painting(), i);
			}else{
				assert(false);
			}
		}else if(auto p = dynamic_cast<SubcktBase const*>(_parent)){ untested();
			incomplete();

			return ::portPosition(p, i);

		}else{
			incomplete();
			assert(false);
			return pos_t(0, 0);
		}
	}
	Port& port(index_t i) override{itested();
		assert(i < _ports.size());
		return _ports[i];
	}
	void set_param_by_name(std::string const& n, std::string const& v) override{
		bool redo = false;
		if(n == "Lib"){ itested();
			_section.Value = QString::fromStdString(v);
			redo = true;
		}else if(n=="Component"){ itested();
			_component.Value = QString::fromStdString(v);
			redo = true;
		}else if(n=="$tx"){
			_tx = atoi(v.c_str());
		}else if(n=="$ty"){
			_ty = atoi(v.c_str());
		}else{itested();
			Symbol::set_param_by_name(n, v);
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

	index_t param_count() const override {
		return Symbol::param_count() + 4 + _params.size();
	}
	void set_param_by_index(index_t n, std::string const& v) override;
	std::string param_value_by_name(std::string const& n) const override{
		if (n=="$tx"){
			return std::to_string(_tx);
		}else if (n=="$ty"){
			return std::to_string(_ty);
		}else{itested();
			return Symbol::param_value_by_name(n);
		}
	}
	std::string param_value(index_t i) const override{
		int m = i - Symbol::param_count();
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
				return Symbol::param_value(i);
			}else if(m - 4 >= int(_param_names.size())){ untested();
				return Symbol::param_value(i);
			}else if(auto p = dynamic_cast<PARAMETER<double>* >(_params[m-4])){
			  	return p->string();
			}else{
				unreachable();
				return "NA";
			}
		}
	}
	std::string param_name(index_t i) const override{
		int m = i - Symbol::param_count();
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
				return Symbol::param_name(i);
			}else if(m - 4 < int(_param_names.size())){
			  	return _param_names[m-4];
			}else{
				return Symbol::param_name(i);
			}
		}
	}

private:
	void attachProto() {
		std::string t = "Lib:" + _section.Value.toStdString() + ":" + _component.Value.toStdString();
		if(_component.Value == "") { untested();
		}else if(_section.Value == "") { untested();
		}else{
			Element const* s = nullptr;
			try{
				s = find_looking_out(t);
			}catch(qucs::ExceptionCantFind const&){
			}

			if((_parent = dynamic_cast<Component const*>(s))){ untested();
				trace2("Lib::attachProto local", t, _parent);
			}else if((_parent = device_dispatcher[t])){
				trace2("Lib::attachProto device", t, _parent->short_label());
			}else if((_parent = symbol_dispatcher[t])){ untested();
				trace2("Lib::attachProto symbol", t, _parent);
			}else{
			}
		}

		if(!_parent){
			message(MsgCritical, ("cannot find " + t).c_str());
			_parent = &d0;
		}else{
		}

		_ports.resize(numPorts());
		set_dev_type(t);

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
	ElementList const* paintings() const{
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
	Component const* _parent; // TODO. common.
	std::vector<Port> _ports;
	std::vector<std::string> _param_names; // could be common?
	std::vector<PARA_BASE*> _params; // could be common
}; // Lib
Lib D(&cl);
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "Lib", &D);
/*--------------------------------------------------------------------------*/
void Lib::set_param_by_index(index_t n, std::string const& v)
{
	bool redo = false;
	int m = int(n) - int(Symbol::param_count());
	trace4("Lib:SP", n, v, m, _params.size());
//		assert(n<Lib::param_count()); not necessarily.
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
	default:itested();
		trace3("fwd", n, m, v);
		if(m - 4 < 0){
			Symbol::set_param_by_index(n, v);
		}else if(m - 4 >= int(_params.size())){itested();
			incomplete();
		// 	Symbol::setParameter(n, v);
		}else if(auto p = dynamic_cast<PARAMETER<double>* >(_params[m-4])){ untested();
			*p = v;
		}else{
			unreachable();
		}
		break;
	}

	if(_section.Value == ""){
	}else if(_component.Value == ""){
	}else if(redo){
		// BUG: not here.
		attachProto();	
		assert(_parent); // for now.
	}else{itested();
	}
}
/*--------------------------------------------------------------------------*/
Lib::Lib(Component const* p)
	  : Symbol(), _tx(0), _ty(0), _parent(p)
{
	set_dev_type("Lib"); // really?
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
Element* LibComp::clone_instance() const
{
	assert(common());
	return new Lib(this);
}
/*--------------------------------------------------------------------------*/
} //namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
