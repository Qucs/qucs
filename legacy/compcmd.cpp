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
	index_t numPorts() const override{ untested();
		if(subckt()){ untested();
			return subckt()->numPorts();
		}else{ untested();
			unreachable();
			return 0;
		}
	}
	Port& port(unsigned) override {incomplete(); throw "incomplete";}

private: // BUG? a SubcktBase is a Painting...
	virtual rect_t bounding_rect() const override{ unreachable(); return rect_t();}
	virtual void paint(ViewPainter*) const override{ unreachable(); }

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
		assert(s);

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
			s->pushBack(sym);
			//sym->setOwner(..);
		}

		auto lang = languageDispatcher["legacy_lib"];
		assert(lang);

		while(true){
			cs.read_line();
			if(cs.umatch("</Components>")){
				break;
			}else{
				cs.skipbl();
				trace2("compcmd", cs.fullstring(), sym->scope()->size());
				lang->new__instance(cs, sym, sym->scope());
			}
		}

		trace1("find DOT", sym->label());
		for(auto i : *sym->scope()){
			if(auto d = dynamic_cast<Symbol*>(i)){
				if(d->typeName() == "Port"){
					auto v = d->port_value(0);
					index_t kk = sym->numPorts();
					trace3("found a port", d->label(), v, kk);
					// sym->set_port_by_index(kk, d->label());
				}else{
				}
			}else if(auto d = dynamic_cast<DEV_DOT*>(i)){ untested();
				trace1("DOT incomplete", d->s());
				//			  sym->setParam(k, name); //?
			}else{
			}
		}
	}
}d0;
Dispatcher<Command>::INSTALL p0(&commandDispatcher, "Components", &d0);
Dispatcher<Command>::INSTALL p1(&commandDispatcher, "Components>", &d0); // BUG
Dispatcher<Command>::INSTALL p2(&commandDispatcher, "<Components>", &d0); // ...
/*--------------------------------------------------------------------------*/
} // namespace
