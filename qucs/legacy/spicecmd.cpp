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
// ignore "<Spice>" section. (use <Model> instead.)
class Model : public Command{
  virtual void do_it(istream_t& cs, SchematicModel* s){
	  auto fullstring = cs.fullString();
	  trace1("Model", fullstring);

	  std::string M;
	  std::string buf;
	  while(true){
		  cs.read_line();
		  trace1("Model", cs.fullString());
		  if(cs.is_end()){
			  break;
		  }else if(cs.umatch("</Spice>")){
			  break;
		  }else{
			  cs.skipbl();
			  M += cs.tail() + "\n";
		  }
	  }
#if 0 // this does not work (surprise!)
	  trace1("hack", M);
	  Symbol* textdef = symbol_dispatcher.clone("qucsatorScktHack");
	  assert(textdef);
	  textdef->setParameter("qucsatorsckthack", M);
	  textdef->setLabel(":qucsatorsckthack:");
	  assert(s);
	  s->pushBack(textdef);
#endif
  }
}d0;
Dispatcher<Command>::INSTALL p0(&command_dispatcher, "Spice", &d0);
Dispatcher<Command>::INSTALL p1(&command_dispatcher, "Spice>", &d0); // BUG
Dispatcher<Command>::INSTALL p2(&command_dispatcher, "<Spice>", &d0); // ...
/*--------------------------------------------------------------------------*/
} // namespace
