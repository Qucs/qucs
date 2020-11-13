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
class Properties : public Command{
  virtual void do_it(istream_t& cs, SchematicModel* s){
	  while(true){
		  cs.read_line();
		  trace1("Properties", cs.fullString());
		  if(cs.is_end()){
			  break;
		  }else if(cs.umatch("</Properties>")){
			  break;
		  }else{
			  cs.skipbl();
		  }
	  }
  }
}d0;
Dispatcher<Command>::INSTALL p0(&command_dispatcher, "Properties", &d0);
Dispatcher<Command>::INSTALL p1(&command_dispatcher, "Properties>", &d0); // BUG
Dispatcher<Command>::INSTALL p2(&command_dispatcher, "<Properties>", &d0); // ...
/*--------------------------------------------------------------------------*/
} // namespace
