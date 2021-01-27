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
#include "schematic_model.h"
#include "qucs_globals.h"
#include "painting.h"
#include "language.h"
#include "symbol.h"
#include "dot.h"
#include "sckt_base.h"
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
Port static_hack_port;
/*--------------------------------------------------------------------------*/
// stuff paintings in here
// rename and move to library when ready?
class SymbolSection : public SubcktBase{
public:
	SymbolSection() : SubcktBase() {
		new_subckt();
	}
	~SymbolSection(){
		for(auto pp : _ports){
			delete pp;
		}
	}
private:
	SymbolSection(SymbolSection const& p) : SubcktBase(p) {
//		incomplete(); so what.
		new_subckt();
	}

	void setParameter(int, std::string const&){ untested();
	}

private: // Sckt
	bool makes_own_scope()const override { return true;}
	SchematicModel* scope() override{
		return subckt();
	}
private: // Symbol
	bool is_device() const override { return false; }
	unsigned numPorts() const override{
		if(subckt()){
			return subckt()->numPorts();
		}else{ untested();
			unreachable();
			return 0;
		}
	}
	Port& port(unsigned i) override {
		assert(scope());
		if(_ports.size() > i){
		}else{
			_ports.resize(i+1);
		}

		if(_ports[i]){
		}else{
			_ports[i] = new Port();
		}

		return *_ports[i];
	}
public:
	Symbol* clone() const override{
		return new SymbolSection(*this);
	}
private: // FIXME. base
	std::vector<Port*> _ports;
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
		trace3("pd", n, l, cs.fullstring());
		s->set_port_by_index(n, l);
	}else{
	}
}
/*--------------------------------------------------------------------------*/
class SymbolCommand : public Command{
	void do_it(istream_t& cs, SchematicModel* s) override{
	  auto fullstring = cs.fullString();
	  trace1("SymbolSection", fullstring);

	  Symbol* sc = symbolSection.clone(); // symbol_dispatcher.clone("symbolSection");
	  auto* sym = dynamic_cast<SubcktBase*>(sc);
	  assert(sym);
	  sym->setLabel(":SymbolSection:");
	  assert(s);

	  auto lang = languageDispatcher["legacy_lib"];
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

	  s->pushBack(sym);
  }
}d0;
Dispatcher<Command>::INSTALL p0(&commandDispatcher, "Symbol", &d0);
Dispatcher<Command>::INSTALL p1(&commandDispatcher, "Symbol>", &d0); // BUG
Dispatcher<Command>::INSTALL p2(&commandDispatcher, "<Symbol>", &d0); // ...
/*--------------------------------------------------------------------------*/
} // namespace
