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
	std::string dev_type() const override{untested(); return "EDD";}
	pos_t portPosition(index_t i) const override{ untested();
		if(auto s=dynamic_cast<Symbol const*>(_painting)){ untested();
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
	void prepare() override{ untested();
		trace1("edd prep", short_label());
		incomplete();
		Model const* m = new_model();

		if(m){ untested();
			auto cc = m->new_common();
			// attach_common(cc);

			// init(m->component_proto());
		}else{ untested();
			incomplete();
		}
	}
protected:
	void refreshSymbol();

private: // Symbol
	void set_param_by_name(std::string const& name, std::string const& value) override{ untested();
		trace2("edd spbn", name, value);
		bool ok = false;
		bool changed = false;
		if(name == "$tx"){ untested();
			ok = true;
			_tx = atoi(value.c_str());
		}else if(name == "$ty"){ untested();
			ok = true;
			_ty = atoi(value.c_str());
		}else if(name[0] == '$'){ untested();
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
		if(!ok){ untested();
			trace2("edd spbn fwd", name, value);
			Symbol::set_param_by_name(name, value);
			assert(common()->param_count() <= 4);
		}else{ untested();
		}
		if(changed){ untested();
			refreshSymbol();
		}else{ untested();
		}
	}

	index_t param_count() const override{ untested();
	  return 2 + FactorySymbol::param_count();
	}
	void set_param_by_index(index_t i, std::string const& v) override{ untested();
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
		}else{ untested();

			if(i==s+3){ untested();
				index_t n = atoi(v.c_str());
				assert(n);
				_ports.resize(2*n);
				changed = true;
			}else{ untested();
			}

			std::string Name = param_name(i);
			trace3("spbi fwd", i, Name, v);

			assert(has_common());
			Component::set_param_by_name(Name, v);
		}

		if(changed){ untested();
			refreshSymbol();
		}else{ untested();
		}
	}
	std::string param_name_(index_t i) const{ untested();
		trace1("param_name_", i);
		if(i==0){ untested();
			return "Type";
		}else if(i==1){ untested();
			return "Branches";
		}else{ untested();
			std::string p;
			if(i%2){ itested();
				p = "Q";
			}else{ itested();
				p = "I";
			}
			return p + std::to_string(i/2);
		}
	}
	std::string param_name(index_t i) const override{ untested();
		assert(common());
		auto s = FactorySymbol::param_count() - common()->param_count();
		auto e = EDD::param_count() - common()->param_count();
		if(i==s){ untested();
			return "$ty";
		}else if(i==s+1){ untested();
			return "$tx";
		}else if(i<e){ untested();
			auto f = FactorySymbol::param_name(i);
			trace3("paramname", i, s, f);
			return f;
		}else{ itested();
			return param_name_(i-e);
		}
	}
	bool param_is_printable(index_t i) const { untested();
		trace2("param_is_printable", i, _ports.size());
		assert(common());
		auto e = EDD::param_count() - common()->param_count();
		if(i<e){ untested();
			return true;
		}else if(i < e + _ports.size() + 2){ untested();
			return true;
		}else{ untested();
			return false;
		}
	}
	std::string param_value(index_t i) const override{ untested();
		assert(common());
		auto s = FactorySymbol::param_count() - common()->param_count();
		auto e = EDD::param_count() - common()->param_count();
		if(i==s){ untested();
			return std::to_string(_ty);
		}else if(i==s+1){ untested();
			return std::to_string(_tx);
		}else if(i<e){ untested();
			return FactorySymbol::param_value(i);
		}else{ untested();
			trace6("EDD::param_value, component", short_label(), i, s, common()->param_count(), i-s, param_name(i));
			assert(has_common());
			assert(i-e<common()->param_count());
			return Component::param_value_by_name(param_name(i));
		}
	}
	std::string param_value_by_name(std::string const& name) const override{ untested();
		trace1("EDD::param_value_by_name", name);
		if(name=="$tx"){ untested();
			return std::to_string(_tx);
		}else if(name=="$ty"){ untested();
			return std::to_string(_ty);
		}else{ untested();
			incomplete();
			return FactorySymbol::param_value_by_name(name);
		}
	}

private: // internal
	void init(Element const* proto);
	Model const* new_model();

private: // TODO: use Component?
	index_t numPorts() const override{ itested();
		trace1("EDD::numPorts", _ports.size()); // max_nodes?
		return _ports.size();
	}
	Port& port(index_t i) override{itested();
		assert(i < _ports.size());
		return _ports[i];
	}
	void set_port_by_index(index_t num, std::string const& name) override{ untested();
		trace3("EDD::sportbi", num, _ports.size(), name);
		// if (num < max_nodes())
		if (num < numPorts()) { untested();
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
{ untested();
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
{ untested();
	if(!_painting){ untested();
		int branches = numPorts()/2;
		std::string defpaint = defsym + std::to_string(branches);
//		auto e = find_looking_out(defpaint);
		auto e = qucs::symbol_dispatcher[defpaint];
		if(!e){ untested();
			message(qucs::MsgWarning, "no symbol for EDD: " + defpaint);
		}else if(auto p=dynamic_cast<Painting const*>(e)){ untested();
			_painting = p;
			trace2("EDD default painting", branches, defpaint);
		}else{ untested();
		}

	}else{ untested();
	}


}
/*--------------------------------------------------------------------------*/
void EDD::init(Element const* pp)
{ untested();
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
	explicit EDDModel(Component* c) : Model(c){ untested();
	}

private:
	Element* clone()const override {return new EDDModel(*this);}
	bool is_valid(const Component* c) const override{ untested();
		trace2("is_valid", param_count(), c->param_count());

		index_t N = 5; // x, y, tx, ty, show
		if( param_count()+N != c->param_count()){ untested();
			return false;
		}else{ untested();
			for(index_t i=0; i<param_count(); ++i){ untested();
				if(param_value(i) != c->param_value(N+i)){ untested();
					return false;
				}
			}
		}
		return true;
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

}; // m0
// Dispatcher<Model>::INSTALL d1(&qucs::model_dispatcher, "EDD", &m0);
/*--------------------------------------------------------------------------*/
Model const* EDD::new_model()
{ untested();
	std::string type_name;

	// add hash(params) to the name to speed up things.
	auto cached_ = find_proto("EDD_*");

	Element const* cached = nullptr;
	if(cached_){ untested();
		// TODO: find_again.
		cached = cached_;
	}else{ untested();
	}

	if(auto sym = dynamic_cast<Model const*>(cached)){ untested();
		return sym; // ->common();
	}else{ untested();
		incomplete(); // rework with parser.
		assert(owner());
		auto os = prechecked_cast<Element const*>(owner());
		assert(os);
		assert(os->scope());

		// Model* m = new EDDModel(this); // qucs::model_dispatcher.clone("EDD");
		Component* s = qucs::device_dispatcher.clone("subckt_proto");
		Model* m = new EDDModel(s); // qucs::model_dispatcher.clone("EDD");

		std::string new_label = "EDD_" + std::to_string(++_modelcounter);
		m->set_label(new_label);

		stash_proto(m);

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
		return m;
	}
}
} // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
