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
class Model : public Command{
  virtual void do_it(istream_t& cs, SchematicModel* s){ untested();
	  auto fullstring = cs.fullString();
	  trace1("Symbol", fullstring);

	  std::string M;
	  std::string buf;

	  trace1("hack", M);
	  Symbol* sym = symbol_dispatcher.clone("symbolSection");
	  assert(sym);
	  sym->setLabel("symbolSection");
	  assert(s);

	  while(true){ untested();
		  cs.read_line();
		  trace1("Symbol", cs.fullString());
		  if(cs.is_end()){ untested();
			  break;
		  }else if(cs.umatch("</Symbol>")){ untested();
			  break;
		  }else{ untested();
			  cs.skipbl();
			  lang->parse_item(cs, sym->subckt());
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
