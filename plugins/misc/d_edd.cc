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

// non-legacy EDD. does not work yet

#include "qucs_globals.h"
#include "factory.h"
#include "component.h"
#include "model.h"
#include "parameter.h"
#include "paramlist.h"
/*--------------------------------------------------------------------------*/
const std::string defsym(":EDD_"); // use a parameter?
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
using qucs::Element;
using qucs::ElementList;
using qucs::Component;
using qucs::Symbol;
using qucs::Model;
using qucs::FactorySymbol;
using qucs::symbol_dispatcher;
using qucs::CommonParamlist;
using qucs::ViewPainter;
static int _modelcounter;// factory?
/*--------------------------------------------------------------------------*/
class EDD : public FactorySymbol {
public:
	explicit EDD() : FactorySymbol(){
		set_label("equation defined device");
//		Component::set_dev_type("EDD"); //BUG
		attach_common(new qucs::COMMON_PARAMLIST());
	}
	EDD(EDD const&);
	~EDD() {};
	Element* clone() const{ return new EDD(*this); }

private: // Symbol
	std::string dev_type() const override{ return "EDD";}
	void paint(ViewPainter* p) const override{
		if(auto s=dynamic_cast<Symbol const*>(_painting)){
			return s->paint(p);
		}else{ untested();
			incomplete();
		}
	}
	rect_t bounding_rect() const override{
		if(auto s=dynamic_cast<Symbol const*>(_painting)){
			return s->bounding_rect();
		}else{ untested();
			incomplete();
			return rect_t(0,0,0,0);
		}
	}
	pos_t portPosition(index_t i) const override{
		if(auto s=dynamic_cast<Symbol const*>(_painting)){
			// BUG. ask CommonSubckt?
			trace3("EDD:portPosition from painting", i, s->numPorts(), numPorts());
			assert(numPorts() == s->numPorts());
			assert(i < s->numPorts());
			auto p = s->portPosition(i);
			return p;
		}else{ untested();
			incomplete();
			return pos_t(0,0);
		}
	}
	void prepare() override{
		incomplete();
		Component const* m = new_proto();
		auto pl = dynamic_cast<CommonParamlist const*>(m->common());
		assert(pl);
		trace2("edd prep", short_label(), pl->_params.size());
		int n=pl->_params.size();
		assert(n==pl->_params.size()); // yuck

		if(m){
			Element* elt = m->clone_instance();
			auto cl = dynamic_cast<Component*>(elt);
			assert(m->common()==cl->common());

			auto pl = dynamic_cast<CommonParamlist const*>(cl->common());
			assert(pl);
			assert(n==pl->_params.size()); // yuck

			attach_common(cl->mutable_common());

			delete cl;

			pl = dynamic_cast<CommonParamlist const*>(common());
			assert(n==pl->_params.size()); // yuck

			init(m);
		}else{ untested();
			incomplete();
		}
	}
protected:
	void refreshSymbol();

private: // Symbol
	void set_param_by_name(std::string const& name, std::string const& value) override{
		trace2("edd spbn", name, value);
		bool ok = false;
		bool changed = false;
		if(name == "$tx"){
			ok = true;
			_tx = atoi(value.c_str());
		}else if(name == "$ty"){
			ok = true;
			_ty = atoi(value.c_str());
		}else if(name[0] == '$'){
			FactorySymbol::set_param_by_name(name, value);
		}else{ untested();
			trace2("spbn fwd", name, value);

			assert(has_common());
			qucs::CommonComponent const* old = common();
			Component::set_param_by_name(name, value);
			assert(common()->param_count() <= 4);

			if(common()==old){ untested();
			}else{ untested();
				changed = true;
			}

			if(!ok){ untested();
				incomplete();
			}else{ untested();
			}
		}
		if(!ok){
			trace2("edd spbn fwd", name, value);
			Symbol::set_param_by_name(name, value);
			assert(common()->param_count() <= 4);
		}else{
		}
		if(changed){ untested();
			refreshSymbol();
		}else{
		}
	}

	index_t param_count() const override{ itested();
	  return 2 + FactorySymbol::param_count();
	}
	void set_param_by_index(index_t i, std::string const& v) override{
		bool changed = false;
		auto s = FactorySymbol::param_count() - common()->param_count();
		auto e = EDD::param_count() - common()->param_count();
		trace3("edd spbi", i, v, s);
		if(i==s){ untested();
			_tx = atoi(v.c_str());
		}else if(i==s+1){ untested();
			_ty = atoi(v.c_str());
		}else if(i<s){ untested();
			FactorySymbol::set_param_by_index(i, v);
		}else{

			if(i==s+3){
				index_t n = atoi(v.c_str());
				assert(n);
				_ports.resize(2*n);
				changed = true;
			}else{
			}

			std::string Name = param_name(i);
			trace3("spbi fwd", i, Name, v);

			assert(has_common());
			Component::set_param_by_name(Name, v);
		}

		if(changed){
			refreshSymbol();
		}else{
		}
	}
	std::string param_name_(index_t i) const{
		trace1("param_name_", i);
		if(i==0){
			return "Type";
		}else if(i==1){
			return "Branches";
		}else{
			std::string p;
			if(i%2){ itested();
				p = "Q";
			}else{ itested();
				p = "I";
			}
			return p + std::to_string(i/2);
		}
	}
	std::string param_name(index_t i) const override{
		assert(common());
		auto s = FactorySymbol::param_count() - common()->param_count();
		auto e = EDD::param_count() - common()->param_count();
		if(i==s){
			return "$ty";
		}else if(i==s+1){
			return "$tx";
		}else if(i<e){
			auto f = FactorySymbol::param_name(i);
			trace3("paramname", i, s, f);
			return f;
		}else{ itested();
			return param_name_(i-e);
		}
	}
	bool param_is_printable(index_t i) const {
		trace2("param_is_printable", i, _ports.size());
		assert(common());
		auto e = EDD::param_count() - common()->param_count();
		if(i<e){ untested();
			return true;
		}else if(i < e + _ports.size() + 2){
			return true;
		}else{
			return false;
		}
	}
	std::string param_value(index_t i) const override{
		assert(common());
		auto s = FactorySymbol::param_count() - common()->param_count();
		auto e = EDD::param_count() - common()->param_count();
		if(i==s){ untested();
			return std::to_string(_ty);
		}else if(i==s+1){ untested();
			return std::to_string(_tx);
		}else if(i<e){ untested();
			return FactorySymbol::param_value(i);
		}else{
			trace6("EDD::param_value, component", short_label(), i, s, common()->param_count(), i-s, param_name(i));
			assert(has_common());
			assert(i-e<common()->param_count());
			return Component::param_value_by_name(param_name(i));
		}
	}
	std::string param_value_by_name(std::string const& name) const override{
		trace1("EDD::param_value_by_name", name);
		if(name=="$tx"){
			return std::to_string(_tx);
		}else if(name=="$ty"){
			return std::to_string(_ty);
		}else{
			incomplete();
			return FactorySymbol::param_value_by_name(name);
		}
	}

private: // internal
	void init(Element const* proto);
	Component const* new_proto();

private: // TODO: use Component?
	index_t numPorts() const override{ itested();
		trace1("EDD::numPorts", _ports.size()); // max_nodes?
		return _ports.size();
	}
	Port& port(index_t i) override{itested();
		assert(i < _ports.size());
		return _ports[i];
	}
	void set_port_by_index(index_t num, std::string const& name) override{
		trace3("EDD::sportbi", num, _ports.size(), name);
		// if (num < max_nodes())
		if (num < numPorts()) {
			port(num).new_node(name, this);
		}else{ untested();
			incomplete();
		}
	}

private:
	int _tx{0};
	int _ty{0};
	std::vector<Port> _ports;
	Painting const* _painting{nullptr};

	int _num{0};
}d;
Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "EDD_", &d);
/*--------------------------------------------------------------------------*/
EDD::EDD(EDD const& p)
    : FactorySymbol(p),
      // _branches(p._branches),
	//	_params(p._params.size()),
      _num(p._num)
{
#if 0
	for( size_t i=0; i < p._params.size(); ++i){ untested();
		auto np = new PARAMETER<double>;
		_params[i] = np;
		if(auto p=dynamic_cast<PARAMETER<double> const*>(_params[i])){ untested();
			*np = *p;
		}else{ untested();
		}
		++i;
	}
#endif
	_ports.resize(p._ports.size());
}
/*--------------------------------------------------------------------------*/
void EDD::refreshSymbol()
{
	if(!_painting){
		int branches = numPorts()/2;
		std::string defpaint = defsym + std::to_string(branches);
//		auto e = find_looking_out(defpaint);
		auto e = qucs::symbol_dispatcher[defpaint];
		if(!e){ untested();
			message(qucs::MsgWarning, "no symbol for EDD: " + defpaint);
		}else if(auto p=dynamic_cast<Painting const*>(e)){
			_painting = p;
			trace2("EDD default painting", branches, defpaint);
		}else{ untested();
		}

	}else{ untested();
	}


}
/*--------------------------------------------------------------------------*/
void EDD::init(Element const* pp)
{
	assert(pp);
	auto proto = dynamic_cast<Component const*>(pp);
	assert(proto);
	ElementList const* ps = proto->scope();
	assert(ps); // won't work for Components.

//	_ports.resize(proto->numPorts());

//	for(auto i : _params){ untested();
//		delete i;
//	}
	trace3("EDD::init", proto->label(), _ports.size(), proto->numPorts());

	incomplete();
#if 0 // BUG BUG copy from proto. better: use common...
	for(index_t i=0; i<proto->param_count(); ++i)
	{ untested();
		auto n = proto->param_name(i);
		auto v = proto->param_value(i);
		trace3("EDD::param_import", i, n ,v );
//		auto p = new PARAMETER<double>();
//		*p = v; // BUG //
	}
#else

#endif
}
/*--------------------------------------------------------------------------*/
class EDDModel : public Model{
public:
	explicit EDDModel(Component* c) : Model(c){
	}

private:
	Element* clone()const override {return new EDDModel(*this);}
	bool is_valid(const Component* c) const override{
		assert(_component_proto);
		auto cp = dynamic_cast<Component const*>(_component_proto);
		assert(cp);
		assert(c);

		auto pl = dynamic_cast<CommonParamlist const*>(c->common());
		auto ppl = dynamic_cast<CommonParamlist const*>(cp->common());

		if(!pl){ untested();
			return false;
		}else if(!ppl){ untested();
			return false;
		}else if(pl == ppl){
			return true;
		}else{
			bool r = pl->_params == ppl->_params;
			trace3("same params?", r, pl->_params.size(), ppl->_params.size());
			return r;
		}
	}

	std::string param_value(index_t i) const override{ untested();
		assert(i<_params.size());
		return _params[i];
	}
#if 0
	void set_param_by_index(index_t i, std::string const&v){ untested();
		trace2("eddm", i, v);
		if(i==1){ untested();
			index_t n = atoi(v.c_str());
			assert(n);
			_params.resize(2+2*n);
		}else{ untested();
		}

		assert(i<_params.size());

		_params[i] = v;
	}
#endif

	std::vector<std::string> _params{1};

};
EDDModel m0(&d);
Dispatcher<Model>::INSTALL d1(&qucs::model_dispatcher, "EDDModel", &m0);
/*--------------------------------------------------------------------------*/
// set_modelname?
Component const* EDD::new_proto()
{
	std::string type_name;

	// add hash(params) to the name to speed up things.
	auto pl = dynamic_cast<CommonParamlist const*>(common());
	assert(pl->_params.size()); // yuck

	auto cached = find_proto("EDD_*");

	if(auto sym = dynamic_cast<Component const*>(cached)){
		trace2("found a proto?", short_label(), cached->short_label());
		return sym; // ->common();
	}else{
		incomplete();
		assert(owner());
		auto os = prechecked_cast<Element const*>(owner());
		assert(os);
		assert(os->scope());

		// Model* m = new EDDModel(this); // qucs::model_dispatcher.clone("EDD");
		Component* s = qucs::device_dispatcher.clone("subckt_proto");
		assert(s);

		std::string new_type = "EDD_" + std::to_string(++_modelcounter);
		s->set_label(new_type);
		trace2("edd::new_proto", common()->modelname(), new_type);
		Component::set_dev_type(new_type);

		// fresh instance, use mutable.
		auto mc = s->mutable_common();

#if 1
		// BUG: just reuse/copy common?
		auto e = EDD::param_count() - common()->param_count();
		for(index_t i=0; i< param_count(); ++i) {
			auto n = param_name(i);
			std::string v;
			try{
				v = param_value_by_name(n);
			}catch(qucs::ExceptionCantFind const&){
				trace3("BUG cant find", n, v, short_label());
				unreachable();
				continue;
			}
					
			assert(n != "");
			if(n[0] != '$'){
				trace3("EDD param export ", i, n ,v );
				mc->set_param_by_name(n, v);
			}else{
			}
	//		auto p = new PARAMETER<double>();
	//		*p = v; // BUG //
	   }
#endif
		Model* m = new EDDModel(s); // qucs::model_dispatcher.clone("EDD");
		assert(m->is_valid(this));
		mc->set_modelname(new_type);
		m->set_label(new_type);
		stash_proto(m);
		auto cp = prechecked_cast<Component const*>(m->component_proto());
		assert(cp);
		assert(m->is_valid(cp));

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
		return dynamic_cast<Component const*>(m->component_proto());
	}
}
} // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
