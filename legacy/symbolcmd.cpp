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
#include "io.h"
#include "schematic_model.h"
#include "globals.h"
#include "painting.h"
#include "language.h"
#include "symbol.h"
#include "d_dot.h"
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
	SymbolSection() : SubcktBase() { untested();
		new_subckt();
	}
private:
	SymbolSection(SymbolSection const& p) : SubcktBase(p) { untested();
//		incomplete(); so what.
		new_subckt();
	}

	void setParameter(int, std::string const&){ untested();
	}

private: // Symbol
	bool is_device() const override { return false; }
	unsigned numPorts() const override{ untested();
		if(subckt()){ untested();
			return subckt()->numPorts();
		}else{ untested();
			unreachable();
			return 0;
		}
	}
	Port& port(unsigned) override {incomplete(); return static_hack_port;}
public:
	Symbol* clone() const override{ untested();
		return new SymbolSection(*this);
	}
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
		s->set_port_by_index(n, l);
	}else{
	}
}
/*--------------------------------------------------------------------------*/
class SymbolCommand : public Command{
	void do_it(istream_t& cs, SchematicModel* s) override{ untested();
	  auto fullstring = cs.fullString();
	  trace1("SymbolSection", fullstring);

	  Symbol* sc = symbolSection.clone(); // symbol_dispatcher.clone("symbolSection");
	  auto* sym = dynamic_cast<SubcktBase*>(sc);
	  assert(sym);
	  sym->setLabel(":SymbolSection:");
	  assert(s);

	  auto lang = language_dispatcher["legacy_lib"];
	  assert(lang);

	  while(true){ itested();
		  cs.read_line();
		  if(cs.umatch("</Symbol>")){ untested();
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
		  if(auto d = dynamic_cast<DEV_DOT*>(i)){ untested();
			  istream_t cs(istream_t::_STRING, d->s());
			  parse_dot(cs, sym);
		  }else{ itested();
		  }
	  }

	  s->pushBack(sym);
  }
}d0;
Dispatcher<Command>::INSTALL p0(&command_dispatcher, "Symbol", &d0);
Dispatcher<Command>::INSTALL p1(&command_dispatcher, "Symbol>", &d0); // BUG
Dispatcher<Command>::INSTALL p2(&command_dispatcher, "<Symbol>", &d0); // ...
/*--------------------------------------------------------------------------*/
} // namespace
