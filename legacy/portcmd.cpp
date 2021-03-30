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
#include "symbol.h"
#include "element_list.h"
#include "qucs_globals.h"
#include "painting.h"
#include "dot.h"
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
static const std::string port = ".port_";
/*--------------------------------------------------------------------------*/
class PortSym : public Command{
	void do_it(istream_t& cs, ElementList* s) override{
		assert(s);
		auto fullstring = cs.fullString();
		cs.reset();
		trace1("PortSym", fullstring);

		cs.skipbl();
		cs.skip1('<');

		std::string type;
		int cx, cy, n, ty;
		cs >> type;
		trace3("portcmd0", type, bool(cs), cs.cursor());
		cs >> cx;
		trace4("portcmd1", cx, bool(cs), cs.fullstring(), cs.cursor());
		cs >> cy;
		cs >> n;
		cs >> ty;
		assert(n);
		std::string portname = ":port" + std::to_string(n);

		auto dot = new DEV_DOT();
		dot->set(port + " x=" + std::to_string(cx)
		              + " y=" + std::to_string(cy)
		              + " n=" + std::to_string(n-1)
		              + " label=" + portname);
		s->push_back(dot);

		auto ps = qucs::element_dispatcher.clone("PortSym");
		assert(ps);
		ps->setPosition(pos_t(cx,cy));
		s->push_back(ps);

		auto place = qucs::symbol_dispatcher.clone("place");
		assert(place);
		// not yet. place->set_port_by_index(0, "incomplete");
		place->setPosition(pos_t(cx,cy));

		if(1){
			place->set_label(portname);
			s->push_back(place);
		}
//		Node* node = place->connectNode(0, s->nodes());
//		assert(node);

#if 0
		// tmp hack.
		{
			auto n = s->numPorts();
			trace1("portsym:setport", n);
			s->setPort(n, node);
		}
#endif
	}
}d0;
Dispatcher<Command>::INSTALL p0(&qucs::command_dispatcher, ".PortSym", &d0);
/*--------------------------------------------------------------------------*/
} // namespace
