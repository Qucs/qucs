/***************************************************************************
    copyright            : (C) 2021 Felix Salfelder
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
#include "model.h"
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
const index_t np = 3;
/*--------------------------------------------------------------------------*/
using qucs::CommonComponent;
using qucs::CommonSubckt;
//using qucs::CommonSubckt;
using qucs::CommonParamlist;
using qucs::Element;
using qucs::Component;
using qucs::Language;
using qucs::Module;
using qucs::Model;
using qucs::SubcktBase;
using qucs::Symbol;
using qucs::FactorySymbol;
using qucs::ViewPainter;
using qucs::symbol_dispatcher;
/*--------------------------------------------------------------------------*/
class Verilog : public FactorySymbol {
public:
	explicit Verilog(CommonComponent* cc);
	explicit Verilog() {}
	~Verilog() {}

private:
	Verilog(Verilog const&x);
	std::string dev_type()const override {
		return "Verilog";
	}

public:
	Element* clone() const override{
		return new Verilog(*this);
	}

	virtual rect_t bounding_rect() const{itested();
		// if(auto p=dynamic_cast<CommonSymbol const*>(common())){ untested();
		//   return p->bouning_rect();
		// }else
		assert(_painting);
		if(_painting){untested();
			return _painting->bounding_rect();
		}else{ untested();
			// assert...
			incomplete();
			return rect_t();
		}
	}
	virtual void paint(ViewPainter* v) const{itested();
		// if(auto p=dynamic_cast<CommonSymbol const*>(common())){ untested();
		//   return p->paint(v);
		// }else
		assert(_painting);
		if(_painting){untested();
			_painting->paint(v);
		}else{ untested();
			// assert...
			incomplete();
		}
		Symbol::paint(v);
	}

public:
	pos_t portPosition(index_t i) const override{
		trace2("Verilog::portPosition", i, common());
		if(auto s=dynamic_cast<Symbol const*>(_painting)){
			// BUG. ask CommonSubckt?
			assert(i < s->numPorts());
			auto p = s->portPosition(i);
			trace3("Verilog::portPosition from painting", i, s->numPorts(), p);
			return p;
		}else{ untested();
			assert(false);
		}
		auto cc = dynamic_cast<CommonSubckt const*>(common());
		trace3("portPosition", i, common(), cc);
		incomplete();
		return pos_t(0,0);
	}

private:
	void refreshSymbol();
	Element* new_model() override;
	void build_sckt(istream_t& cs, SubcktBase* proto) const;

private: // Symbol
	bool port_exists(index_t) const override;
	std::string const portName(index_t) const override;

	void set_param_by_name(std::string const& name, std::string const& value) override;
	void set_param_by_index(index_t i, std::string const& v) override;
	index_t param_count() const override{
//		trace3("Verilog", short_label(), FactorySymbol::param_count(), _param_names.size());
		return np + FactorySymbol::param_count();
	}
	bool param_is_printable(index_t i) const override{
		index_t s = Verilog::param_count() - 1 - i;
		trace3("param_is_printable", i, s, param_name(i));
		switch (s) { // BUG Symbol.
		case 0: // tx
		case 1: // ty
			return true;
		case 2: // filename
			return true;
//		case 3: // angle?
			break;
		default:
			break;
		}
		assert(common());
		if(s-np<_param_names.size()){
			return false;
//		}else if(i<common()->param_count()){ untested();
//			return true ;
		}else{
			return FactorySymbol::param_is_printable(i + _param_names.size());
		}
	}
	std::string param_name(index_t i) const override {
		index_t s = Verilog::param_count() - 1 - i;
		switch (s){
		case 0:
			return "$tx";
		case 1:
			return "$ty";
		case 2:
			return "File";
			// return "porttype"; // or so
		default:
			break;
		}
		if(s-np<_param_names.size()){
			return _param_names[s-np];
		}else{
			return FactorySymbol::param_name(i + _param_names.size());
		}
	}
/*--------------------------------------------------------------------------*/
	std::string param_value(index_t i) const override{
		trace4("Verilog::param_value", i, short_label(), param_name(i), param_is_printable(i));
		index_t s = Verilog::param_count() - 1 - i;
		switch (s) {
		case 0:
			return std::to_string(_tx);
		case 1:
			return std::to_string(_ty);
		case 2:
			return _filename;
		default:
			break;
		}
		if(0 || s-np<_param_names.size()){
			std::string n = _param_names[s-np];
			trace3("map param", i, n, short_label());
			return FactorySymbol::param_value_by_name(n);
		}else{
			return FactorySymbol::param_value(i+_param_names.size());
		}
	}
	std::string param_value_by_name(std::string const& name) const override{
		trace1("Verilog::param_value_by_name", name);
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
	Port& port(index_t i) override{
		trace1("Verilog::port", i);
		assert(i < _ports.size());
		return _ports[i];
	}

private:
	int _tx{0};
	int _ty{0};
	std::string _filename; // "File" parameter.
	std::vector<Port> _ports;
	std::vector<std::string> _param_names;
	Painting const* _painting{nullptr};
}p1; // Verilog
static Dispatcher<Element>::INSTALL d1(&symbol_dispatcher, "Verilog", &p1);
/*--------------------------------------------------------------------------*/
class ref : public Model {
public:
	explicit ref() : Model(nullptr){}
private:
	ref(ref const& p) : Model(p), _filename(p._filename), _dev_type(p._dev_type){}
private:
	Model* clone() const{return new ref(*this);}
	Model* clone_instance() const{unreachable(); return nullptr; }

	std::string dev_type() const override{return "VerilogRef";}

public:
	index_t param_count() const override{ return Model::param_count() + 2; }
	void set_param_by_index(index_t i, std::string const& v) override{
		switch (ref::param_count() - 1 - i){
		case 0:
			_filename = v;
			return;
		case 1:
			_dev_type = v;
			return;
		default:
			break;
		}
	}
	std::string param_name(index_t i) const override{
		index_t s = ref::param_count() - 1 - i;
		switch (s) {
		case 0: // fn
			return "filename";
		case 1: // dt
			return "dev_type";
		default:
			return Model::param_name(i);
		}
	}
	std::string param_value(index_t i) const override{
		switch (ref::param_count() - 1 - i){
		case 0: // fn
			return _filename;
		case 1: // dt
			return _dev_type;
		default:
			return Model::param_value(i);
		}
	}
	bool param_is_printable(index_t i) const override{
		switch (ref::param_count() - 1 - i){
		case 0: // fn
		case 1: // dt
			return true;
		default:
			return Model::param_is_printable(i);
		}
	}
	void set_param_by_name(std::string const& name, std::string const& v) {
		if(name=="filename"){
			_filename = v;
		}else if(name=="dev_type"){
			_dev_type = v;
		}else if(name=="File"){ untested();
			_filename = v;
		}else{
			Model::set_param_by_name(name, v);
		}
	}

private:
	std::string _filename;
	std::string _dev_type;
};
/*--------------------------------------------------------------------------*/
class PROTO : public SubcktBase{
public:
	PROTO() : SubcktBase(){
		new_subckt();
		attach_common(new CommonParamlist());
	}
	PROTO(PROTO const& x) : SubcktBase(x){
		new_subckt(); // copy??
	}
	PROTO* clone() const override{ return new PROTO(*this);}
	bool is_device() const override{
		return false;
	}
	bool makes_own_scope() const override{
		return true;
	}
	ElementList* scope() override{return subckt();}

	index_t param_count() const override{ return _param_names.size(); }
	void set_param_by_index(index_t i, std::string const& v){
		trace2("PROTO::set_param_by_index", i, v);
		if(i<_param_names.size()){ untested();
		}else{
			_param_names.resize(i+1);
		}

		_param_names[i] = v;
	}
	std::string param_name(index_t i) const override{ untested();
		assert(i<_param_names.size());
		return _param_names[i];
	}
	void set_param_by_name(std::string const& n, std::string const& v){
		trace2("spbn", n, v);
		SubcktBase::set_param_by_name(n, v);
	}

private: // legacy schematic needs ordered params...
	std::vector<std::string> _param_names;
}proto_;
/*--------------------------------------------------------------------------*/
// create a subdevice from a file.
Element* Verilog::new_model()
{
//	QString FileName(Props.getFirst()->Value);
	auto dotplace = _filename.find_last_of(".");
	std::string type_name;

	if (dotplace == std::string::npos) { untested();
		incomplete();
		// throw?? or try and recover??
		type_name = "Verilog" + typesep + "invalid_filename";
	}else{
		type_name = "Verilog" + typesep + _filename.substr(0, dotplace);
	}

	auto subPath = factory_param("$SUB_PATH");
	trace2("VerilogFactory::new_model", subPath, _filename);
	std::string file_found = findFile(_filename, subPath, R_OK);
	trace4("VerilogFactory::newCommon", label(), _filename, subPath, file_found);

	if(file_found != "" ){
		assert(owner());
		auto os = prechecked_cast<Element const*>(owner());
		assert(os);
		assert(os->scope());

		Component* ss = proto_.clone();
		auto s = prechecked_cast<SubcktBase*>(ss);
		assert(s);
		assert(s->subckt());

		assert(owner());
		// s->set_owner(owner());

		istream_t pstream(istream_t::_WHOLE_FILE, file_found);
		build_sckt(pstream, s);

		s->set_dev_type(type_name);
		s->set_label("Verilog");

		trace4("Verilog::made proto", s->label(), type_name, s->dev_type(), s->numPorts());
		Component* last = nullptr;
		for(auto i: *s->scope()){
			trace1("Verilog found", i->short_label()); // , i->num_ports());
			if(auto c = dynamic_cast<Component*>(i)){
				last = c;
			}else{ untested();
			}
		}
		// auto a = qucs::device_dispatcher.clone("subckt_proto");
		// auto a = new verilogProto;
		if(last){
			Model* r = new ref;
			// r->set_label("VerilogRef");
			r->set_label(type_name);
			r->set_param_by_name("filename", _filename);
			r->set_param_by_name("dev_type", last->short_label());

			s->subckt()->push_back(r);

			{ // really?
				s->set_dev_type(type_name);
				s->set_label(type_name);
			}
			trace4("Verilog stash", dev_type(), type_name, s->common()->modelname(), last->numPorts());
			// trace2("Verilog stash", type_name, common()->modelname());

			// parse params here?
			for(index_t i=0; i<last->numPorts(); ++i){
				s->set_port_by_index(i, last->port_value(i));
			}


			// stash_proto(s);
		}else{ untested();
			assert(false);
		}

		// assert(s->label()==type_name);

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
		return ss;
	}else{ untested();
		incomplete();
		return nullptr;
	}
} // new_model
/*--------------------------------------------------------------------------*/
static void parse_portcmd(istream_t& cs, SubcktBase* s)
{ untested();
	trace1("port", cs.fullstring());
	cs.reset();
	if(cs.umatch(".port_")){ untested();
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
void Verilog::build_sckt(istream_t& cmd, SubcktBase* proto) const
{
	trace1("Verilog::build_sckt", cmd.fullstring());
	assert(owner());

	Language const* L = qucs::language_dispatcher["verilog"];
	assert(L);
	try{
		while(true){
			cmd.get_line("verilog>");
			cmd.skipbl();
			trace1("Verilog::build_sckt", cmd.fullstring());
			assert(proto->scope());
			L->new__instance(cmd, proto, proto->scope());
		}
	}catch (qucs::Exception_End_Of_Input& e) {
	}

	assert(!proto->numPorts());
	assert(proto->common());

	bool gotit = false;
	auto ps = proto->scope(); // really?
	assert(ps);
	auto p_ = ps->find_("Symbol");
	Component const* last;
	if(p_==ps->end()){
		for(auto i: *proto->scope()){
			trace1("Verilog found", i->short_label()); // , i->num_ports());
			if(auto c = dynamic_cast<Component*>(i)){
				last = c;
			}else{ untested();
			}
		}
	}else{ untested();
		last = dynamic_cast<Component const*>(*p_);
	}

	if(auto p = dynamic_cast<SubcktBase const*>(last)){
		assert(p->scope());
	
		index_t ii = 0;
		for(auto i : *p->scope()){
			gotit = true;
			if(auto a=dynamic_cast<DEV_DOT*>(i)){
				istream_t cs(istream_t::_STRING, a->s());
				if(cs >> "parameter"){ // portparameter?
					if(cs >> "real"){
					}else{
					}
					std::string name;
					std::string defv;
					cs >> name;
					cs >> defv;
					trace3("symbol parameter", a->s(), name, defv);

					try{
						proto->set_param_by_index(ii, name);
						++ii;
						if (defv==""){
							incomplete();
//							defv="123";
						}else{ untested();
							}
						proto->set_param_by_name(name, defv);
					}catch(qucs::ExceptionCantFind const&){ untested();
						incomplete();
					}
				}else if(cs.umatch("portparameter")){ untested();
				}else if(cs.umatch(".port_")){ untested();
					trace1("symbol port", a->s());
					parse_portcmd(cs, proto);
				}else{ untested();
					trace1("symbol other", a->s());
				}
			}else{ untested();
			}
		}

	}else{ untested();
		unreachable(); // wrong type
	}

	if(!gotit){ untested();
		incomplete();
		trace2("build_sckt no Symbol", proto->label(), proto->numPorts());
	}else{
		trace2("build_sckt got Symbol", proto->label(), proto->numPorts());
	}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
Verilog::Verilog(Verilog const&x)
  : FactorySymbol(x),
    _tx(x._tx),
    _ty(x._ty),
    _filename(x._filename),
    _painting(x._painting)
{
	_ports.resize(x._ports.size());
}
/*--------------------------------------------------------------------------*/
void Verilog::refreshSymbol()
{
	auto fn = _filename;
	trace1("refreshSymbol", fn);
	Element const* e = nullptr;

	std::string type_name;

	auto dotplace = _filename.find_last_of(".");
	if (dotplace == std::string::npos) { untested();
		incomplete();
		// throw?? or try and recover??
		type_name = "Verilog" + typesep + "invalid_filename";
	}else{
		type_name = "Verilog" + typesep + _filename.substr(0, dotplace);
	}

	if(!common()){
		auto c = new CommonParamlist();
		c->set_modelname(type_name);
		attach_common(c);
	}else{
		incomplete();
	}

	if(fn!=""){
		assert(common());
		e = find_proto();
	}else{ untested();
		incomplete();
	}

	trace1("refreshSymbol", fn);
	if(auto new_parent=dynamic_cast<Component const*>(e)){
		// _proto == new_parent; // ?
		assert(new_parent);
		assert(new_parent->common());
		auto cc = new_parent->clone_instance();
		auto s = prechecked_cast<Component*>(cc);
		assert(s);
		assert(s->common() == new_parent->common());
		attach_common(s->mutable_common());
		init(new_parent);
		delete cc;
	}else{ untested();
		incomplete();
	}
}
/*--------------------------------------------------------------------------*/
void Verilog::init(Component const* proto)
{
	auto ps = proto->scope();
	assert(ps); // won't work for Components.

	auto p_ = ps->find_("main");
	if(p_==ps->end()){
		_ports.resize(proto->numPorts());
	}else if(auto mm=dynamic_cast<Component const*>(*p_)){ untested();
		_ports.resize(mm->numPorts());
	}else{ untested();
	}

	_param_names.clear();

#if 0 // BUG BUG copy from proto. better: use common...
	for(index_t i=0; i<proto->param_count(); ++i)
	{ untested();
		[..]
		_params.push_back(p);
		_param_names.push_back(name);
	}
#else

	SubcktBase const* symsect = nullptr;
	p_ = ps->find_("Symbol");
	Component* last = nullptr;
	if(p_==ps->end()){
		trace1("Verilog::init no symbol", short_label());
		for(auto i: *proto->scope()){
			trace1("Verilog found", i->short_label()); // , i->num_ports());
			if(auto c = dynamic_cast<Component*>(i)){
				last = c;
			}else{
			}
		}
	}else{ untested();
		for(auto i: *ps){ untested();
			trace1("Verilog::init found", i->short_label()); // , i->num_ports());
			if(auto c = dynamic_cast<Component*>(i)){ untested();
				last = c;
			}else{ untested();
			}
		}
	}

	if(auto p = dynamic_cast<SubcktBase const*>(last)){
		trace1("Verilog::init symbol", p->short_label());
		symsect = p;
		assert(p->scope());
		for(auto i : *p->scope()){
			if(auto a=dynamic_cast<DEV_DOT*>(i)){
				istream_t cs(istream_t::_STRING, a->s());
				if(cs.umatch("parameter")){ // portparameter?
					if(cs >> "real"){
					}else{
					}
					trace1("LibComp DOT", a->s());
					std::string name;
					std::string defv;
					cs >> name;
					cs >> defv;
					_param_names.push_back(name);
				}else{ untested();
					// port HERE?
				}
			}else{ untested();
			}
		}
	}else{ untested();
		incomplete(); // or no painting??
	}

	// find painting...
	if(!symsect){itested();
	}else if(!symsect->scope()->size()){ untested();
	}else if(auto p=dynamic_cast<Painting const*>(symsect)){ untested();
		trace2("got painting from symbol section", symsect->scope()->size(), symsect->numPorts());
		_painting = p;
		// check nuber f ports??
	}else{
	}

	if(!_painting){
		int np = numPorts();
		std::string defpaint = defsym + std::to_string(np);
//		auto e = find_looking_out(defpaint);
		trace2("default painting", np, defpaint);
		auto e = qucs::symbol_dispatcher[defpaint];
		if(!e){ untested();
			message(qucs::MsgFatal, "no symbol for " + label() + " " + defpaint);
		}else if(auto p=dynamic_cast<Painting const*>(e)){
			_painting = p;
		}else{ untested();
		}

	}else{ untested();
	}
#endif

	trace5("Verilog::init", dev_type(), label(), numPorts(), param_count(), _param_names.size());
}
/*--------------------------------------------------------------------------*/
bool Verilog::port_exists(index_t i) const
{
	return i<numPorts();
}
/*--------------------------------------------------------------------------*/
static std::string invalid_ = "sckt_proto_port_invalid";
std::string const Verilog::portName(index_t) const
{ untested();
	incomplete();
	// throw?
	return invalid_;
}
/*--------------------------------------------------------------------------*/
void Verilog::set_param_by_index(index_t i, std::string const& v)
{
	index_t s = Verilog::param_count() - 1 - i;
	trace7("Verilog::spbi", i, s, v, short_label(), Verilog::param_count(), _param_names.size(), param_name(i));
	switch (s){
	case 0:
		_tx = atoi(v.c_str());
		return;
	case 1:
		_ty = atoi(v.c_str());
		return;
	case 2:
		_filename = v;
		refreshSymbol();
		return;
	default:
		break;
	}

	if(s-np<_param_names.size()){
		auto n = param_name(i);
		trace4("set param fwd", i, n, v, short_label());
		return FactorySymbol::set_param_by_name(n, v);
	}else{ untested();
		trace3("set param no fwd", i, short_label(), _param_names.size());
	}
}
/*--------------------------------------------------------------------------*/
void Verilog::set_param_by_name(std::string const& name, std::string const& v)
{
	trace2("Verilog::set_param_by_name", name, v);
	if(name=="$tx"){
		_tx = atoi(v.c_str());
	}else if(name=="$ty"){
		_ty = atoi(v.c_str());
	}else if(name=="File"){ untested();
		_filename = v;
		refreshSymbol();
	}else{
		FactorySymbol::set_param_by_name(name, v);
	}
}
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
