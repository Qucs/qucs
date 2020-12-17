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
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
class PortSym : public Command{
	void do_it(istream_t& cs, SchematicModel* s) override{
	  assert(s);
	  auto fullstring = cs.fullString();
	  cs.reset();
	  trace1("PortSym", fullstring);
	  cs.skipbl();
	  cs.skip1('<');

	  std::string type;
	  int cx, cy, tx, ty;
	  cs >> type;
	  trace3("portcmd0", type, bool(cs), cs.cursor());
	  cs >> cx;
	  trace4("portcmd1", cx, bool(cs), cs.fullstring(), cs.cursor());
	  cs >> cy;
	  cs >> tx;
	  cs >> ty;

	  auto ps = painting_dispatcher.clone("PortSym");
	  ps->setPosition(pos_t(cx,cy));
	  s->pushBack(ps);
	  auto n = s->numPorts();
	  trace6("PortSymParams", type, cx, cy, tx, ty, n);

	  auto place = symbol_dispatcher.clone("place");
	  if(1){
		  // memory leak.
	  }else{ // later
		  s->pushBack(place);
	  }
	  assert(place);
	  place->setPosition(pos_t(cx,cy));
	  Node* node = place->connectNode(0, s->nodes());
	  assert(node);

	  // tmp hack.
	  trace1("portsym:setport", n);
	  s->setPort(n, node);
  }
}d0;
Dispatcher<Command>::INSTALL p(&command_dispatcher, ".PortSym", &d0);
/*--------------------------------------------------------------------------*/
} // namespace
