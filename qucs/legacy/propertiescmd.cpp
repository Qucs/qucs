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
#include "schematic_doc.h"
#include "globals.h"
#include "u_parameter.h"
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
class Properties : public Command{
  virtual void do_it(istream_t& cs, SchematicModel* s){
	  assert(s);
	  while(true){
		  cs.read_line();
		  trace1("Properties", cs.fullString());
		  if(cs.is_end()){
			  break;
		  }else if(cs.umatch("</Properties>")){
			  break;
		  }else{
			  cs.skipbl();
			  cs.skip1('<');
			  std::string value;

			  assert(s->params());
			  if(cs.umatch("View")){
				  cs >> "=";
				  cs >> value;
				  s->params()->set("ViewX1", value);
				  cs >> value;
				  s->params()->set("ViewY1", value);
				  cs >> value;
				  s->params()->set("ViewX2", value);
				  cs >> value;
				  s->params()->set("ViewY2", value);
				  cs >> value;
				  s->params()->set("ViewScale", value);
				  cs >> value;
				  s->params()->set("PosX", value);
				  value = cs.ctos(">", "=", ">");
				  s->params()->set("PosY", value);
			  }else if(cs.umatch("Grid")){
				  cs >> "=";
				  cs >> value;
			  }else{
				  std::string name;
				  cs >> name;
				  value = cs.ctos(">", "=", ">");
				  trace3("Prop", name, value, cs.fullstring());

#if 0
				  assert(s->owner());
				  s->owner()->setParameter(name, value);
#else

				  s->params()->set(name, value);
				  // // cast to qucsDoc?
				  // if(s->doc()){
				  //    s->doc()->setParameter(name, value);
				  // }else{
				  // }
#endif
			  }
		  }
	  }
  }
}d0;
Dispatcher<Command>::INSTALL p0(&command_dispatcher, "Properties", &d0);
Dispatcher<Command>::INSTALL p1(&command_dispatcher, "Properties>", &d0); // BUG
Dispatcher<Command>::INSTALL p2(&command_dispatcher, "<Properties>", &d0); // ...
/*--------------------------------------------------------------------------*/
} // namespace
