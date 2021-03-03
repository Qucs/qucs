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
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
class Model : public Command{
  virtual void do_it(istream_t& cs, ElementList* s) {
	  auto fullstring = cs.fullString();
	  trace1("Model", fullstring);

	  std::string M;
	  std::string buf;
	  while(true){
		  cs.read_line();
		  trace1("Model", cs.fullString());
		  if(cs.is_end()){
			  break;
		  }else if(cs.umatch("</Model>")){
			  break;
		  }else{
			  cs.skipbl();
			  M += cs.tail() + "\n";
		  }
	  }
#if 1
	  trace1("hack", M);
	  Symbol* textdef = qucs::symbol_dispatcher.clone("qucsatorScktHack");
	  assert(textdef);
	  textdef->set_param_by_name("qucsatorsckthack", M);
	  textdef->set_label(":qucsatorsckthack:");
	  assert(s);
	  s->push_back(textdef);
#endif
  }
}d0;
Dispatcher<Command>::INSTALL p0(&qucs::command_dispatcher, "Model", &d0);
Dispatcher<Command>::INSTALL p1(&qucs::command_dispatcher, "Model>", &d0); // BUG
Dispatcher<Command>::INSTALL p2(&qucs::command_dispatcher, "<Model>", &d0); // ...
/*--------------------------------------------------------------------------*/
} // namespace
