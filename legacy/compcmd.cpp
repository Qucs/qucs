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
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
using qucs::Symbol;
using qucs::SubcktBase;
/*--------------------------------------------------------------------------*/
class CompCommand : public Command{
	void do_it(istream_t& cs, ElementList* s) override{
		auto fullstring = cs.fullString();
		trace1("CompCommand", fullstring);
		assert(s);

		SubcktBase* sym = nullptr;
		auto p_ = s->find_("main");
		if(p_!=s->end()){
			sym = dynamic_cast<SubcktBase*>(*p_);
		}else{
			qucs::Component* sc = qucs::device_dispatcher.clone("subckt_proto");
			sym = dynamic_cast<SubcktBase*>(sc);
			assert(sym);
			sym->set_label("main");
			s->pushBack(sym);
			//sym->setOwner(..);
		}

		auto lang = qucs::language_dispatcher["legacy_lib"];
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
				if(d->typeName() == "Port"){ // BUG: use real ports.
					auto v = d->port_value(0);
					index_t kk = sym->numPorts();
					trace3("found a port", d->label(), v, kk);
				// subcirport seems to do this.
				//	sym->set_port_by_index(kk, d->label());
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
Dispatcher<Command>::INSTALL p0(&qucs::command_dispatcher, "Components", &d0);
Dispatcher<Command>::INSTALL p1(&qucs::command_dispatcher, "Components>", &d0); // BUG
Dispatcher<Command>::INSTALL p2(&qucs::command_dispatcher, "<Components>", &d0); // ...
/*--------------------------------------------------------------------------*/
} // namespace
