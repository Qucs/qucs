/***************************************************************************
    copyright            : (C) 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*--------------------------------------------------------------------------*/
#include "command.h"
#include "qio.h"
#include "element_list.h"
#include "qucs_globals.h"
#include "painting.h"
#include "language.h"
#include "symbol.h"
#include "dot.h"
#include "sckt_base.h"
#include "viewpainter.h"
#include "painting.h"
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
using qucs::Painting;
using qucs::SubcktBase;
using qucs::Component;
/*--------------------------------------------------------------------------*/
class SymbolSection : public SubcktBase, public Painting{
public:
	SymbolSection() : SubcktBase(), Painting() {
		new_subckt();
	}
	~SymbolSection(){
//		for(auto pp : _ports){
//			delete pp;
//		}
	}
private:
	SymbolSection(SymbolSection const& p) : SubcktBase(p), Painting(p) {
//		incomplete(); so what.
		new_subckt();
	}

	void setParameter(int, std::string const&){ untested();
	}

private: // BUG? a SubcktBase is a Painting...
	rect_t bounding_rect() const override {
		assert(subckt());
		rect_t br; // cache??
		for(auto p : *subckt()){ itested();
			assert(p);
			Element const* e = p;
			//			trace2("br", e->boundingRect().topLeft(), e->boundingRect().bottomRight());
			if(auto p = dynamic_cast<Painting const*>(e)){
				auto c = e->center(); // p!
				// incomplete(); // BUG. honour p->legacyTransformhack
				br |= ( p->bounding_rect() + c );
			}
		}
		trace4("LibComp::br", label(), subckt()->size(), br.tl(), br.br());
		return br;
	}
	void paint(ViewPainter* v) const override{itested();
		assert(subckt());
		for(auto e : *subckt()){ itested();
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

private: // Sckt
//	index_t numPorts() const override{ untested();
//		return _numports; in e_comp
//	}
	bool makes_own_scope()const override { return true;}
	ElementList* scope() override{
		return subckt();
	}
private: // Symbol
	bool is_device() const override { return false; }

	// sckt_base.
//	unsigned numPorts() const override{
//		if(subckt()){
//			return subckt()->numPorts();
//		}else{ untested();
//			unreachable();
//			return 0;
//		}
//	}
//	Port& port(unsigned i) override {
//		assert(scope());
//		if(_ports.size() > i){
//		}else{
//			_ports.resize(i+1);
//		}
//
//		if(_ports[i]){
//		}else{
//			_ports[i] = new Port();
//		}
//
//		return *_ports[i];
//	}
	std::string const portName(index_t i) const override{
		trace1("symbol portName", i);
		return port_value(i);
	}
public:
	Component* clone() const override{
		return new SymbolSection(*this);
	}
private: // use e_comp
// 	index_t _numports{0};
//	std::vector<Port*> _ports;
}symbolSection;
/*--------------------------------------------------------------------------*/
static void parse_dot(istream_t& cs, SubcktBase* s)
{
	trace1("symbol dot", cs.fullstring());
	if(cs.umatch(".port_")){
		int x, y, n;
		std::string l;
		Get(cs, "x", &x);
		Get(cs, "y", &y);
		Get(cs, "n", &n);
		Get(cs, "l{abel}", &l);

		trace3("got port", x, y, s->numPorts());
		trace3("symbol set port", n, l, cs.fullstring());
		s->set_port_by_index(n, l);
		trace1("port set", s->numPorts());
		assert(l==s->port_value(n));
	}else{
	}
}
/*--------------------------------------------------------------------------*/
class SymbolCommand : public Command{
	void do_it(istream_t& cs, ElementList* s) override{
	  auto fullstring = cs.fullString();
	  trace1("SymbolSection", fullstring);

	  Component* sc = symbolSection.clone(); // symbol_dispatcher.clone("symbolSection");
	  auto* sym = dynamic_cast<SubcktBase*>(sc);
	  assert(sym);
	  sym->set_label(":SymbolSection:");
	  assert(s);

	  auto lang = qucs::language_dispatcher["legacy_lib"];
	  assert(lang);

	  while(true){ itested();
		  cs.read_line();
		  if(cs.umatch("</Symbol>")){
			  break;
		  }else{ itested();
			  cs.skipbl();
			  lang->new__instance(cs, sym, sym->subckt());
			  trace2("symbolpaint", cs.fullstring(), sym->subckt()->size());
		  }
	  }

	  trace1("Symbol done", sym->numPorts());
	  // assert(!sym->numPorts()); // BUG not yet

	  trace1("find DOT", sym->label());
	  for(auto i : *sym->subckt()){ itested();
		  if(auto d = dynamic_cast<DEV_DOT*>(i)){
			  istream_t cs(istream_t::_STRING, d->s());
			  parse_dot(cs, sym);
		  }else{ itested();
		  }
	  }

	  s->push_back(sym);
  }
}d0;
Dispatcher<Command>::INSTALL p0(&qucs::command_dispatcher, "Symbol", &d0);
Dispatcher<Command>::INSTALL p1(&qucs::command_dispatcher, "Symbol>", &d0); // BUG
Dispatcher<Command>::INSTALL p2(&qucs::command_dispatcher, "<Symbol>", &d0); // ...
/*--------------------------------------------------------------------------*/
} // namespace
