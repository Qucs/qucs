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
  virtual void do_it(istream_t& cs, SchematicModel* s){
	  auto fullstring = cs.fullString();
	  trace1("SymbolSection", fullstring);

	  std::string M;

	  trace1("hack", M);
	  Symbol* sc = symbolSection.clone(); // symbol_dispatcher.clone("symbolSection");
	  auto* sym = dynamic_cast<SubcktBase*>(sc);
	  assert(sym);
	  sym->setLabel(":SymbolSection:");
	  assert(s);

	  auto lang = doclang_dispatcher["legacy_lib"];
	  assert(lang);

	  while(true){
		  trace1("Symbol Command", cs.fullString());
		  cs.read_line();
		  trace1("Symbol Command", cs.fullString());
		  ///if(cs.is_end()){ untested();
		  ///   // bug. matches newlines...??
		  ///   break;
		  ///}else
		  if(cs.umatch("</Symbol>")){
			  break;
		  }else{
			  cs.skipbl();
			  lang->new__instance(cs, sym, sym->subckt());
			  trace2("symbolpaint", cs.fullstring(), sym->subckt()->size());
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
