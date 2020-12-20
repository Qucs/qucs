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
// stuff paintings in here
// rename and move to library when ready?
class SymbolSection : public SubcktBase{
public:
	SymbolSection() : SubcktBase() {
		new_subckt();
	}
private:
	SymbolSection(SymbolSection const& p) : SubcktBase(p) {
//		incomplete(); so what.
		new_subckt();
	}

	void setParameter(int, std::string const&){
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
	Port& port(unsigned) override {incomplete(); throw "incomplete";}
public:
	Symbol* clone() const override{
		return new SymbolSection(*this);
	}
}symbolSection;
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

	  auto lang = language_dispatcher["legacy_lib"];
	  assert(lang);

	  while(true){
		  cs.read_line();
		  if(cs.umatch("</Symbol>")){
			  break;
		  }else{
			  cs.skipbl();
			  lang->new__instance(cs, sym, sym->subckt());
			  trace2("symbolpaint", cs.fullstring(), sym->subckt()->size());
		  }
	  }

	  s->pushBack(sym);

	  trace1("find DOT", sym->label());
	  for(auto i : *sym->subckt()){
		  if(auto d = dynamic_cast<DEV_DOT*>(i)){
			  trace1("DOT incomplete", d->s());
//			  sym->setParam(k, name); //?
		  }else{
		  }
	  }
  }
}d0;
Dispatcher<Command>::INSTALL p0(&command_dispatcher, "Symbol", &d0);
Dispatcher<Command>::INSTALL p1(&command_dispatcher, "Symbol>", &d0); // BUG
Dispatcher<Command>::INSTALL p2(&command_dispatcher, "<Symbol>", &d0); // ...
/*--------------------------------------------------------------------------*/
} // namespace
