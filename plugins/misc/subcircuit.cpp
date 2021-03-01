/***************************************************************************
    copyright            : (C) 2018, 2019, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <io_trace.h>
#include <limits.h>

#include "common_sckt.h"
#include "docfmt.h"
#include "symbol.h"
#include "dot.h"
#include "factory.h"
#include "language.h"
#include "misc.h"
#include "module.h"
#include "parameter.h"
#include "qio.h"
#include "qucs_globals.h"
#include "sckt_base.h"

/*--------------------------------------------------------------------------*/
namespace qucs {
	class ViewPainter;
}
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
const std::string typesep(":");
const std::string defsym(":SYMBOL_"); // use a parameter?
/*--------------------------------------------------------------------------*/
using qucs::CommonComponent;
using qucs::CommonSubckt;
using qucs::Element;
using qucs::Language;
using qucs::Module;
using qucs::ViewPainter;
/*--------------------------------------------------------------------------*/
// possibly not needed.
class CommonSub : public CommonComponent{
public:
	explicit CommonSub(int i=0) : CommonComponent(i) {}
private:
	CommonComponent* clone() const override{ untested();
		return new CommonSub(*this);
	}
};
CommonSub cs(CC_STATIC_);
/*--------------------------------------------------------------------------*/
class SubFactory;
/*--------------------------------------------------------------------------*/
// a subcircuit instance with a factory hook
// not really a paramset yet...
class Paramset : public Symbol {
public:
	explicit Paramset(CommonComponent* cc);
	explicit Paramset() {}
	~Paramset() {}

private:
	Paramset(Paramset const&x);

	std::string dev_type()const override { untested();
		return "Sub";
	}

public:
	Element* clone() const override{
		trace0("Paramset::clone");
		return new Paramset(*this);
	}

	virtual rect_t bounding_rect() const{
		if(_painting){
			return _painting->bounding_rect();
		}else{
			// assert...
			incomplete();
			return rect_t();
		}
	}
	virtual void paint(ViewPainter* v) const{
		if(_painting){
			_painting->paint(v);
		}else{
			// assert...
			incomplete();
		}
	}

public:
	pos_t portPosition(index_t i) const override{
		trace2("Paramset::portPosition", i, common());
		if(auto s=dynamic_cast<Symbol const*>(_painting)){
			assert(i < s->numPorts());
			auto p = s->portPosition(i);
			trace3("Paramset::portPosition", i, s->numPorts(), p);
			return p;
		}else{ untested();
			assert(false);
		}
		auto cs = prechecked_cast<CommonSubckt const*>(common());
		if(cs){ untested();
			return cs->portPosition(i);
		}else{
			message(qucs::MsgWarning, "bogus port\n");
		}
		auto cc = dynamic_cast<CommonSubckt const*>(common());
		trace3("portPosition", i, common(), cc);
		incomplete();
		return pos_t(0,0);
		//_proto
		//	  return Symbol::portPosition(i);
	}

private:
	void refreshSymbol(std::string const&);

private: // Symbol
	bool portExists(index_t) const override;
	std::string const portName(index_t) const override;

	void set_param_by_name(std::string const& name, std::string const& value) override;
	void setParameter(index_t i, std::string const& value) override;
	index_t paramCount() const override{
	  return 3 + Symbol::paramCount();
	}

	std::string paramName(index_t i) const override;
	std::string paramValue(index_t i) const override{
		switch (int(Paramset::paramCount()) - (1 + i)) {
		case 0:
			return _filename;
//		case 1: untested();
//			return "dunno";
		case 1:
			return std::to_string(_ty);
		case 2:
			return std::to_string(_tx);
		default:
			return Symbol::paramValue(i);
		}
	}
	std::string paramValue(std::string const& name) const override{
		trace1("Paramset::paramValue", name);
		if(name=="$tx"){
			return std::to_string(_tx);
		}else if(name=="$ty"){
			return std::to_string(_ty);
		}else{
			incomplete();
			return Symbol::paramValue(name);
		}
	}
	void init(Symbol const* proto);

private: // overrides
	index_t numPorts() const override{
		return _ports.size();
	}
	Port& port(index_t i) override{itested();
		assert(i < _ports.size());
		return _ports[i];
	}

public:
	SubFactory* _factory{nullptr};

private:
	int _tx{0};
	int _ty{0};
	std::string _subPath;
	std::string _filename; // "File" parameter.
	std::vector<Port> _ports;
	std::vector<std::string> _param_names; // could be common?
	std::vector<PARA_BASE*> _params; // could be common
	Painting const* _painting{nullptr};
}p1; // Paramset
/*--------------------------------------------------------------------------*/
class SubFactory : public Element, public SymbolFactory {
public:
	explicit SubFactory() : Element(), SymbolFactory() {}
	SubFactory(SubFactory const& f) : Element(f), SymbolFactory(f)
	{
//		_proto = symbol_dispatcher.clone("paramset"); ?
		_proto = new Paramset();
		_proto->_factory = this;
	}
	
	Element* clone_instance() const override;
	Element* clone()const{return new SubFactory(*this);}

	Symbol const* newSymbol(std::string const& fn) const;

private:
	void set_param_by_name(std::string const& name, std::string const& v) override
	{
		trace2("SF::set_param", name, v);
		if(name=="$SUB_PATH"){
			_subPath = v;
		}else{ untested();
			throw qucs::ExceptionCantFind(name, label());
		}
	}
	void build_sckt(istream_t&, SubcktBase* proto) const;

private:
	Paramset* _proto{nullptr};
	std::string _subPath;
}d0;
static Dispatcher<Element>::INSTALL p(&element_dispatcher, "LegacySub", &d0);
static Module::INSTALL pp("stuff", &d0);
/*--------------------------------------------------------------------------*/
Element* SubFactory::clone_instance() const
{
	assert(_proto);
	auto new_instance = prechecked_cast<Paramset*>(_proto->clone());
	assert(!new_instance->subckt());

	if(this == &d0){ untested();
	}else{
		assert(new_instance->_factory == this);
	}

	return new_instance;
}
/*--------------------------------------------------------------------------*/
#if 0
Paramset::Paramset(CommonComponent* cc)
	: Symbol() // sckt?
{ untested();
	attach_common(cc);
	setTypeName("Sub"); // dev_type_key.
}
#endif
/*--------------------------------------------------------------------------*/
// use common params eventually.
std::string Paramset::paramName(index_t i) const
{
	switch (int(Paramset::paramCount()) - (1 + i)) {
	case 0:
		return "File";
		// return "porttype"; // or so
	case 1:
		return "$tx";
	case 2:
		return "$ty";
	default:
		return Symbol::paramName(i);
	}
}
/*--------------------------------------------------------------------------*/
Paramset::Paramset(Paramset const&x)
  : Symbol(x),
    _factory(x._factory),
	 _tx(x._tx),
	 _ty(x._ty),
    _painting(x._painting)
{
	setTypeName("Sub"); // dev_type_key
	_ports.resize(x._ports.size());
}
/*--------------------------------------------------------------------------*/
// void Paramset::set_dev_type(const std::string& new_type){ untested();
//    hmm, type does not reflect file name, cant build it...
// }
/*--------------------------------------------------------------------------*/
void Paramset::refreshSymbol(std::string const& fn)
{
	auto dotplace = fn.find_last_of(".");

#ifndef NDEBUG
	std::string type_name;
	if (dotplace == std::string::npos) { untested();
		// throw?? or try and recover??
		type_name = "Sub" + typesep + "invalid_filename";
	}else{
		type_name = "Sub" + typesep + fn.substr(0, dotplace);
	}
#endif

	setTypeName("Sub"); // still // dev_type_key
	assert(_factory);
	Symbol const* new_parent = _factory->newSymbol(fn);
	assert(new_parent);
//	auto cc = new_common->clone(); //really? set modelname in factory.
	assert(new_parent->common());
	auto cc = new_parent->common()->clone(); //really? set modelname in factory.
//	cc->set_modelname(type_name);
//	assert(cc->modelname()==type_name);
	attach_common(cc); // not actually shared yet. but will.
	assert(common()->modelname()==type_name);

	init(new_parent);
}
/*--------------------------------------------------------------------------*/
void Paramset::init(Symbol const* proto)
{
	auto ps = proto->scope();
	assert(ps); // won't work for Components.

	auto p_ = ps->find_("main");
	if(p_==ps->end()){
		_ports.resize(proto->numPorts());
	}else if(auto mm=dynamic_cast<Symbol const*>(*p_)){
		_ports.resize(mm->numPorts());
	}

	for(auto i : _params){
		delete i;
	}
	_params.clear();
	_param_names.clear();

	trace3("Paramset::init", proto->label(), _ports.size(), proto->numPorts());

	incomplete();
#if 0 // BUG BUG copy from proto. better: use common...
	for(index_t i=0; i<proto->paramCount(); ++i)
	{
		[..]
		_params.push_back(p);
		_param_names.push_back(name);
	}
#else

	SubcktBase const* symsect=nullptr;
	p_ = ps->find_(":SymbolSection:");
	if(p_==ps->end()){
		// no symbol
	}else if(auto p = dynamic_cast<SubcktBase const*>(*p_)){
		symsect = p;
		assert(p->scope());
		for(auto i : *p->scope()){
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
					// port HERE?
				}
			}else{
			}
		}
	}else{
		incomplete(); // or no painting??
	}

	// find painting...
	if(!symsect){
	}else if(symsect->scope()->size()){
		trace2("got painting from symbol section", symsect->scope()->size(), symsect->numPorts());
		_painting = symsect;
		// check nuber f ports??
	}else{
	}

	if(!_painting){
		int np = numPorts();
		std::string defpaint = defsym + std::to_string(np);
//		auto e = find_looking_out(defpaint);
		trace2("default painting", np, defpaint);
		auto e = symbol_dispatcher[defpaint];
		assert(e);
		if(auto p=dynamic_cast<Painting const*>(e)){
			_painting = p;
		}else{
		}

	}else{
	}
#endif
}
/*--------------------------------------------------------------------------*/
// create a subdevice from a file.
// if its already there, use it.
// TODO: factory needs a refresh hook.
Symbol const* SubFactory::newSymbol(std::string const& fn) const
{
	trace1("SubFactory::newSymbol", fn);
//	QString FileName(Props.getFirst()->Value);
	auto dotplace = fn.find_last_of(".");
	std::string type_name;

	if (dotplace == std::string::npos) { untested();
		incomplete();
		// throw?? or try and recover??
		type_name = "Sub" + typesep + "invalid_filename";
	}else{
		type_name = "Sub" + typesep + fn.substr(0, dotplace);
	}

	auto cached_ = _scope->find_(type_name);
	Element* cached = nullptr;
	if(cached_ != _scope->end()){
		// TODO: find_again.
		cached = *cached_;
	}else{
	}

	std::string file_found = findFile(fn, _subPath, R_OK);
	trace4("SubFactory::newCommon", label(), fn, _subPath, file_found);

	if(auto sym = dynamic_cast<Symbol const*>(cached)){
		return sym; // ->common();
	}else if(file_found != "" ){
		incomplete(); // rework with parser.
		assert(owner());
		auto os = prechecked_cast<Symbol const*>(owner());
		assert(os);
		assert(os->scope());

		Symbol* ss = symbol_dispatcher.clone("subckt_proto"); // symbol_proto?
		auto s = prechecked_cast<SubcktBase*>(ss);
		assert(s);

		istream_t pstream(istream_t::_WHOLE_FILE, file_found);
		build_sckt(pstream, s);

		s->set_dev_type(type_name);
		s->set_label(type_name);

		assert(_scope);

		trace4("made proto", s->label(), type_name, s->typeName(), s->numPorts());
		assert(_scope->find_(type_name) == _scope->end());
		_scope->push_back(s);

		assert(s->label()==type_name);

#if 0
		if(loadSymbol(FileName) > 0) { untested();
			if(tx == INT_MIN)  tx = x1+4;
			if(ty == INT_MIN)  ty = y2+4;
			// remove unused ports
			QMutableListIterator<ComponentPort *> ip(Ports);
			ComponentPort *pp;
			while (ip.hasNext()) { untested();
				pp = ip.next();
				if(!pp->avail) { untested();
					pp = ip.peekNext();
					ip.remove();
				}
			}
		}
#endif
		return s;
	}else{ untested();
		incomplete();
		return nullptr;
	}
}
/*--------------------------------------------------------------------------*/
static void parse_portcmd(istream_t& cs, SubcktBase* s)
{
	trace1("port", cs.fullstring());
	cs.reset();
	if(cs.umatch(".port_")){
		int x, y, n;
		std::string l;
		Get(cs, "x", &x);
		Get(cs, "y", &y);
		Get(cs, "n", &n);
		Get(cs, "l{abel}", &l);

		trace3("got port", x, y, s->numPorts());
		trace3("pd", n, l, cs.fullstring());
		s->set_port_by_index(n, l);
	}else{
	}
}
/*--------------------------------------------------------------------------*/
void SubFactory::build_sckt(istream_t& cs, SubcktBase* proto) const
{
	trace1("SubFactory::build_sckt", cs.fullstring());
	assert(owner());

	auto LL = language_dispatcher["leg_sch"]; // use command instead?
	auto L = dynamic_cast<Language const*>(LL);
	assert(L);
	cs.read_line();
	while(!cs.is_end()){itested();
		trace1("SubFactory::build_sckt", cs.fullstring());
		Element* e = proto;
		assert(e->scope());
		// assert(proto->scope() == proto->subckt()); nope
		// L->parse_top_item(s, _root->subckt());
		L->new__instance(cs, proto, e->scope());
		cs.read_line();
	}

	assert(!proto->numPorts());
	assert(proto->common());

	bool gotit = false;
	auto ps = proto->scope(); // really?
	assert(ps);
	auto p_ = ps->find_(":SymbolSection:");
	if(p_==ps->end()){
		trace2("no SymbolSection", proto->label(), proto->numPorts());
	}else if(auto p = dynamic_cast<SubcktBase const*>(*p_)){
		assert(p->scope());
	
		for(auto i : *p->scope()){
			gotit = true;
			if(auto a=dynamic_cast<DEV_DOT*>(i)){
				istream_t cs(istream_t::_STRING, a->s());
				if(cs.umatch("parameter")){ // portparameter?
					trace1("symbol parameter", a->s());
					std::string name;
					std::string defv;
					cs >> name;
					cs >> defv;

					try{
						proto->set_param_by_name(name, defv);
					}catch(qucs::ExceptionCantFind const&){
						incomplete();
					}
					 // auto p = new PARAMETER<double>
					 // *p = defv;
					 // proto->_params.push_back(p);
					 // proto->_param_names.push_back(name);
				}else if(cs.umatch("portparameter")){
				}else if(cs.umatch(".port_")){
					trace1("symbol port", a->s());
					parse_portcmd(cs, proto);
				}else{
					trace1("symbol other", a->s());
				}
			}else{
			}
		}

	}else{
		unreachable(); // wrong type
	}

	if(!gotit){
		incomplete();
		trace2("build_sckt no Symbol", proto->label(), proto->numPorts());
	}else{
		trace2("build_sckt got Symbol", proto->label(), proto->numPorts());
	}
}
/*--------------------------------------------------------------------------*/
bool Paramset::portExists(index_t i) const
{
	return i<numPorts();
	assert(scope());
	trace1("Paramset::portExists", i);
	incomplete();
	return false;
}
/*--------------------------------------------------------------------------*/
static std::string invalid_ = "sckt_proto_port_invalid";

std::string const Paramset::portName(index_t) const
{ untested();
	incomplete();
	// throw?
	return invalid_;
}
/*--------------------------------------------------------------------------*/
void Paramset::set_param_by_name(std::string const& name, std::string const& v)
{
	trace2("Paramset::setParameter", name, v);
	if(name=="$SUB_PATH"){ untested();
		_subPath = v;
	}else if(name=="$tx"){
		_tx = atoi(v.c_str());
	}else if(name=="$ty"){
		_ty = atoi(v.c_str());
	}else if(name=="File"){
		_filename = v;
		trace1("Paramset::setParameter2", v);
		refreshSymbol(v);
	}else{
		Symbol::set_param_by_name(name, v);
	}
}
/*--------------------------------------------------------------------------*/
void Paramset::setParameter(index_t i, std::string const& v)
{
	switch (int(Paramset::paramCount()) - (1 + i)) {
	case 0:
		_filename = v;
		refreshSymbol(v);
	break;
	case 1:
		_tx = atoi(v.c_str());
	break;
	case 2:
		_ty = atoi(v.c_str());
	break;
	}

	if(i==4){ // needed by legacy schematic parser...?
		incomplete();
		_filename = v;
		trace1("Paramset::setParameter", v);
		refreshSymbol(v);
	};
}
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
