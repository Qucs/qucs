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
#include "common_sckt.h"
/*--------------------------------------------------------------------------*/
const std::string defsym(":SYMBOL_"); // use a parameter?
/*--------------------------------------------------------------------------*/
namespace qucs {
	class ViewPainter;
	class Symbol;
}
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
const std::string typesep(":");
/*--------------------------------------------------------------------------*/
using qucs::CommonComponent;
using qucs::CommonSubckt;
using qucs::Element;
using qucs::Component;
using qucs::Language;
using qucs::Module;
using qucs::SubcktBase;
using qucs::Symbol;
using qucs::SymbolFactory;
using qucs::FactorySymbol;
using qucs::ViewPainter;
using qucs::symbol_dispatcher;
/*--------------------------------------------------------------------------*/
class Sub : public FactorySymbol {
public:
	explicit Sub(CommonComponent* cc);
	explicit Sub() : FactorySymbol() {}
	~Sub();

private:
	Sub(Sub const&x);
	std::string dev_type()const override { untested();
		return "Sub";
	}

	Component const* new_model(std::string const& fn);
	void build_sckt(istream_t&, SubcktBase* proto) const;
public:
	Element* clone() const override{
		trace0("Sub::clone");
		return new Sub(*this);
	}

	virtual rect_t bounding_rect() const{ untested();
		// if(auto p=dynamic_cast<CommonSymbol const*>(common())){ untested();
		//   return p->bouning_rect();
		// }else
		if(_painting){ untested();
			return _painting->bounding_rect();
		}else{ untested();
			// assert...
			incomplete();
			return rect_t();
		}
	}
	virtual void paint(ViewPainter* v) const{ untested();
		// if(auto p=dynamic_cast<CommonSymbol const*>(common())){ untested();
		//   return p->paint(v);
		// }else
		if(_painting){ untested();
			_painting->paint(v);
		}else{ untested();
			// assert...
			incomplete();
		}

		Symbol::paint(v);
	}

public:
	// cleanup & move to baseclass ?
	pos_t portPosition(index_t i) const override{
		trace2("Sub::portPosition", i, common());
		if(auto s=dynamic_cast<Symbol const*>(_painting)){ untested();
			// BUG. ask CommonSubckt?
			assert(i < s->numPorts());
			auto p = s->portPosition(i);
			trace3("Sub::portPosition from painting", i, s->numPorts(), p);
			return p;
		}else if(auto sckt=dynamic_cast<SubcktBase const*>(_painting)){ untested();
			ElementList const* s = sckt->subckt();
			if(!s){
				s = sckt->scope();
			}else{
			}
			assert(s);

			std::string n = sckt->portName(i);
			auto ii = s->find_(n);
			if(ii!=s->end()){ untested();
				trace1("SubcktProto::portPosition hit", i);
				return (*ii)->position();
			}else{ untested();
				trace2("SubcktProto::portPosition miss", i, n);
			}
		}else if(auto s=dynamic_cast<Component const*>(_painting)){ untested();
			trace2("Sub::portPosition from component", i, s->numPorts());
			assert(false);
		}else{ untested();
			assert(false);
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
	void set_param_by_index(index_t i, std::string const& value) override;
	index_t param_count() const override{
	  return 3 + Symbol::param_count();
	}

	std::string param_name(index_t i) const override;
	std::string param_value(index_t i) const override{
		switch (int(Sub::param_count()) - (1 + i)) {
		case 0:
			return _filename;
		case 1:
			return std::to_string(_ty);
		case 2:
			return std::to_string(_tx);
		default:
			return Symbol::param_value(i);
		}
	}
	std::string param_value_by_name(std::string const& name) const override{
		trace1("Sub::param_value_by_name", name);
		if(name=="$tx"){
			return std::to_string(_tx);
		}else if(name=="$ty"){
			return std::to_string(_ty);
		}else{
			incomplete();
			return Symbol::param_value_by_name(name);
		}
	}
	void init(Component const* owner);

private: // overrides
	index_t numPorts() const override{
		return _ports.size();
	}
	Port& port(index_t i) override{itested();
		assert(i < _ports.size());
		return _ports[i];
	}

private:
	int _tx{0};
	int _ty{0};
	std::string _filename; // "File" parameter.
	std::vector<Port> _ports;
	std::vector<std::string> _param_names; // could be common?
	std::vector<PARA_BASE*> _params; // could be common
	Painting const* _painting{nullptr};
}p1; // Sub
static Dispatcher<Element>::INSTALL d1(&symbol_dispatcher, "Sub", &p1);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// create a subdevice from a file.
// if its already there, use it.
// TODO: factory needs a refresh hook.
Component const* Sub::new_model(std::string const& fn) // const
{
	auto subPath = factory_param("$SUB_PATH");
	trace2("SubFactory::newSymbol", fn, subPath);
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

	auto cached_ = find_proto(type_name);
	Element const* cached = nullptr;
	if(cached_){
		// TODO: find_again.
		cached = cached_;
	}else{
	}

	std::string file_found = findFile(fn, subPath, R_OK);
	trace4("SubFactory::newCommon", label(), fn, subPath, file_found);

	if(auto sym = dynamic_cast<Component const*>(cached)){
		return sym; // ->common();
	}else if(file_found != "" ){
		incomplete(); // rework with parser.
		assert(owner());
		auto os = prechecked_cast<Element const*>(owner());
		assert(os);
		assert(os->scope());

		Component* ss = qucs::device_dispatcher.clone("subckt_proto"); // Sub?
		auto s = prechecked_cast<SubcktBase*>(ss);
		assert(s);

		assert(owner());
		s->set_owner(owner());

		{ // move to subckt_proto variant?
			auto a = qucs::device_dispatcher.clone("subckt_proto");
			assert(a);
			a->set_label("main");
			a->set_owner(ss);
			assert(s->scope());
			s->scope()->push_back(a);
		}

		istream_t pstream(istream_t::_WHOLE_FILE, file_found);
		build_sckt(pstream, s);

		s->set_dev_type(type_name);
		s->set_label(type_name);

		// submit.
		assert(s->label()==type_name);
		stash_proto(s);

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
		message(qucs::MsgFatal,"cannot find " + fn + " in " + subPath);
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
	}else{ untested();
	}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void Sub::build_sckt(istream_t& cs, SubcktBase* proto) const
{
	trace1("SubFactory::build_sckt", cs.fullstring());
	assert(owner());

	auto LL = qucs::language_dispatcher["leg_sch"]; // use command instead?
	auto L = dynamic_cast<Language const*>(LL);
	assert(L);
	try{
		while(true){itested();
			cs.read_line();
			trace1("SubFactory::build_sckt", cs.fullstring());
			Element* e = proto;
			assert(e->scope());
			L->new__instance(cs, proto, e->scope());
		}
	}catch (qucs::Exception_End_Of_Input& e) {
	}


	assert(!proto->numPorts());
	assert(proto->common());

	bool gotit = false;
	auto ps = proto->scope(); // really?
	assert(ps);
	auto p_ = ps->find_(":SymbolSection:");
	if(p_==ps->end()){ untested();
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
				}else if(cs.umatch("portparameter")){ untested();
				}else if(cs.umatch(".port_")){
					trace1("symbol port", a->s());
					parse_portcmd(cs, proto);
				}else{ untested();
					trace1("symbol other", a->s());
				}
			}else{
			}
		}

	}else{ untested();
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
/*--------------------------------------------------------------------------*/
void Sub::set_param_by_index(index_t i, std::string const& v)
{
	switch (int(Sub::param_count()) - (1 + i)) {
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

	if(i==4){ untested();
		// needed by legacy schematic parser...?
		incomplete();
		_filename = v;
		trace1("Sub::setParameter", v);
		refreshSymbol(v);
	}else{
	}
}
/*--------------------------------------------------------------------------*/
// use common params eventually.
std::string Sub::param_name(index_t i) const
{
	switch (int(Sub::param_count()) - (1 + i)) {
	case 0:
		return "File";
		// return "porttype"; // or so
	case 1:
		return "$tx";
	case 2:
		return "$ty";
	default:
		return Symbol::param_name(i);
	}
}
/*--------------------------------------------------------------------------*/
Sub::Sub(Sub const&x)
  : FactorySymbol(x),
	 _tx(x._tx),
	 _ty(x._ty),
    _painting(x._painting)
{
	_ports.resize(x._ports.size());
}
/*--------------------------------------------------------------------------*/
Sub::~Sub()
{
	for(auto i : _params){ untested();
		delete i;
	}
	_params.clear();
	_param_names.clear();
}
/*--------------------------------------------------------------------------*/
void Sub::refreshSymbol(std::string const& fn)
{
	Component const* new_parent = nullptr;
	new_parent = new_model(fn);

	if(new_parent){
		assert(new_parent);
		assert(new_parent->common());
#if 0 // wrong
		auto cc = new_parent->common()->clone(); //really? set modelname in factory.
		attach_common(cc); // not actually shared yet. but will.
#else
		auto cc = new_parent->clone_instance();
		auto s = prechecked_cast<Component*>(cc);
		assert(s);
		attach_common(s->mutable_common());
		delete cc;
#endif

		init(new_parent);
	}else{ untested();
		incomplete();
	}
}
/*--------------------------------------------------------------------------*/
void Sub::init(Component const* proto)
{
	auto ps = proto->scope();
	assert(ps); // won't work for Components.

	auto p_ = ps->find_("main");
	if(p_==ps->end()){ untested();
		_ports.resize(proto->numPorts());
	}else if(auto mm=dynamic_cast<Component const*>(*p_)){
		_ports.resize(mm->numPorts());
	}else{ untested();
	}

	for(auto i : _params){ untested();
		delete i;
	}
	_params.clear();
	_param_names.clear();

	trace3("Sub::init", proto->label(), _ports.size(), proto->numPorts());

	incomplete();
#if 0 // BUG BUG copy from proto. better: use common...
	for(index_t i=0; i<proto->param_count(); ++i)
	{ untested();
		[..]
		_params.push_back(p);
		_param_names.push_back(name);
	}
#else

	SubcktBase const* symsect=nullptr;
	p_ = ps->find_(":SymbolSection:");
	if(p_==ps->end()){ untested();
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
	}else{ untested();
		incomplete(); // or no painting??
	}

	// find painting...
	if(!symsect){ untested();
	}else if(!symsect->scope()->size()){
	}else if(auto p=dynamic_cast<Painting const*>(symsect)){
		trace2("got painting from symbol section", symsect->scope()->size(), symsect->numPorts());
		_painting = p;
		// check nuber f ports??
	}else{ untested();
	}

	if(!_painting){
		int np = numPorts();
		std::string defpaint = defsym + std::to_string(np);
//		auto e = find_looking_out(defpaint);
		trace2("default painting", np, defpaint);
		auto e = qucs::symbol_dispatcher[defpaint];
		assert(e);
		if(auto p=dynamic_cast<Painting const*>(e)){
			_painting = p;
		}else{ untested();
		}

	}else{
	}
#endif
}
/*--------------------------------------------------------------------------*/
bool Sub::portExists(index_t i) const
{ untested();
	return i<numPorts();
}
/*--------------------------------------------------------------------------*/
static std::string invalid_ = "sckt_proto_port_invalid";
std::string const Sub::portName(index_t) const
{ untested();
	incomplete();
	// throw?
	return invalid_;
}
/*--------------------------------------------------------------------------*/
void Sub::set_param_by_name(std::string const& name, std::string const& v)
{ itested();
	trace2("Sub::setParameter", name, v);
//	if(name=="$SUB_PATH"){ untested();
//		_subPath = v;
//	}else
	if(name=="$tx"){
		_tx = atoi(v.c_str());
	}else if(name=="$ty"){
		_ty = atoi(v.c_str());
	}else if(name=="File"){
		_filename = v;
		refreshSymbol(v);
	}else{ itested();
		Symbol::set_param_by_name(name, v);
	}
}
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
