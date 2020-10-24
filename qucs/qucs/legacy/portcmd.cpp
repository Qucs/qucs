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
  virtual void do_it(istream_t& cs, SchematicModel* s){
	  auto fullstring = cs.fullString();
	  trace1("PortSym", fullstring);


	  std::string type;
	  int cx, cy, tx, ty;
	  cs >> type;
	  cs >> cx;
	  cs >> cy;
	  cs >> tx;
	  cs >> ty;

	  auto ps = painting_dispatcher.clone("PortSym");
	  ps->setCenter(pos_t(cx,cy));
	  s->pushBack(ps);
	  auto n = s->numPorts();
	  trace6("PortSym", type, cx, cy, tx, ty, n);

	  auto place = symbol_dispatcher.clone("place");
	  assert(place);
	  place->setCenter(pos_t(cx,cy));
	  Node* node = place->connectNode(0, s->nodes());
	  assert(node);

	  trace1("portsym:setport", n);
	  s->setPort(n, node);
  }
}d0;
Dispatcher<Command>::INSTALL p(&command_dispatcher, ".PortSym", &d0);
/*--------------------------------------------------------------------------*/
} // namespace
