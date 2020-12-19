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
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
	// FIXME: use proto
class Section : public SubcktBase{
public:
	Section() : SubcktBase() {
		new_subckt();
	}
private:
	Section(Section const& p) : SubcktBase(p) { untested();
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
	Port& port(unsigned) override {incomplete(); throw "incomplete";}
public: // same as in sckt_proto
	SchematicModel* scope(){ untested(); return subckt();}
	SchematicModel const* scope() const{ untested(); return subckt();}
public:
	Symbol* clone() const override{ untested();
		return new Section(*this);
	}
}mainSection;
/*--------------------------------------------------------------------------*/
class CompCommand : public Command{
	void do_it(istream_t& cs, SchematicModel* s) override{
		auto fullstring = cs.fullString();
		trace1("CompCommand", fullstring);

		SubcktBase* sym = nullptr;
		auto p_ = s->find_("main");
		if(p_!=s->end()){
			sym = dynamic_cast<SubcktBase*>(*p_);
		}else{
			// "headless" mode
			// create main, but no project.
			//  (defer expansion of components that need a project)
//			Symbol* sc = mainSection.clone();
			Symbol* sc = symbol_dispatcher.clone("subckt_proto");

			sym = dynamic_cast<SubcktBase*>(sc);
			assert(sym);
			sym->setLabel("main");
			//sym->setOwner(..);
			s->pushBack(sym);
			assert(s);
		}

		auto lang = language_dispatcher["legacy_lib"];
		assert(lang);

		while(true){
			cs.read_line();
			if(cs.umatch("</Components>")){
				break;
			}else if(cs.umatch("</Wires>")){
				break;
			}else{
				cs.skipbl();
				trace2("compcmd", cs.fullstring(), sym->scope()->size());
				lang->new__instance(cs, sym, sym->scope());
			}
		}

		trace1("find DOT", sym->label());
		for(auto i : *sym->scope()){
			if(auto d = dynamic_cast<DEV_DOT*>(i)){ untested();
				trace1("DOT incomplete", d->s());
				//			  sym->setParam(k, name); //?
			}else{
			}
		}
	}
}d0;
Dispatcher<Command>::INSTALL p0(&command_dispatcher, "Components|Wires", &d0);
Dispatcher<Command>::INSTALL p1(&command_dispatcher, "Components>|Wires>", &d0); // BUG
Dispatcher<Command>::INSTALL p2(&command_dispatcher, "<Components>|<Wires>", &d0); // ...
/*--------------------------------------------------------------------------*/
} // namespace
