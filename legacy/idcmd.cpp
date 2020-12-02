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
#include "language.h"
#include "symbol.h"
#include "u_parameter.h"
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
class IdCommand : public Command{
	virtual void do_it(istream_t& cs, SchematicModel* scope){
		auto fullstring = cs.fullString();

		std::string type;
		int cx, cy, tx, ty;
		cs >> type;
		trace3("ID0", type, bool(cs), cs.cursor());
		cs >> cx;
		trace4("ID1", cx, bool(cs), cs.fullstring(), cs.cursor());
		cs >> cy;
		cs >> tx;
		cs >> ty;

		// from IDtext::load don't copy
		QString s = QString::fromStdString(fullstring);
		int i = 1;
		for(;;) {
			trace1("ID parameter", s);
			auto n = s.section('"', i,i);
			if(n.isEmpty())  break;

			// Parameter.append(new SubParameter(
			bool disp = (n.at(0) == '0') ? false : true;
			auto name = n.section('=', 1,1);
			auto def = n.section('=', 2,2);
			auto desc = n.section('=', 3,3);
			auto type = n.section('=', 4,4);

			assert(scope->params());

			//does not work, as intended. parameter values are not ordered.
			scope->params()->set(name.toStdString(), def.toStdString());

			trace5("ID parameter", disp, name, def, desc, type);

#if 0 // maybe this could work:
			auto p = painting_dispatcher.clone("parameter");
			p->set( name, def etc.)
				p->setParameter("position", i/2);

			scope->pushBack(p);
#endif

			i += 2;
		}

		auto ps = painting_dispatcher.clone(".ID");
		ps->setPosition(pos_t(cx,cy));
		scope->pushBack(ps);
	}
}d0;
Dispatcher<Command>::INSTALL p0(&command_dispatcher, ".ID", &d0);
/*--------------------------------------------------------------------------*/
} // namespace
