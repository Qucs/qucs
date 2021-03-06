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
#include "parameter.h"
#include "dot.h"
#include <QString>
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
class IdCommand : public Command{
	void do_it(istream_t& cs, ElementList* scope) override{
		auto fullstring = cs.fullString();

		std::string type;
		int cx, cy; // , tx, ty;
//		cs >> type;
		cs >> cx;
		cs >> cy;
		trace4("ID1", fullstring, type, cx, cy);

		std::string label = cs.ctos(">", "=", ">");

		// from IDtext::load. don't copy
		QString s = QString::fromStdString(fullstring);
		trace2("ID parameter", s, label);
		int i = 1;
		for(;;) {
			auto n = s.section('"', i,i);
			if(n.isEmpty())  break;

			// Parameter.append(new SubParameter(
			bool disp = (n.at(0) == '0') ? false : true;
			auto name = n.section('=', 1,1).toStdString();
			auto def = n.section('=', 2,2).toStdString();
			auto desc = n.section('=', 3,3).toStdString();
			auto type = n.section('=', 4,4).toStdString();

			assert(scope->params());

			//does not work, as intended. parameter values are not ordered.
			scope->params()->set(name, def);

			trace5("ID parameter", disp, name, def, desc, type);

			auto dot = new DEV_DOT();
			// is this a portparameter?
			dot->set("parameter " + name + " " + def); //TODO: desc & type.

			trace2("push DOT", name, def);
			scope->push_back(dot);

			i += 2;
		}

		auto ps = qucs::element_dispatcher.clone(".ID");
		assert(ps);
		trace3("ID push", label, cx, cy);
		ps->setPosition(pos_t(cx,cy));
		ps->set_label(label);
		scope->push_back(ps);
	}
}d0;
Dispatcher<Command>::INSTALL p0(&qucs::command_dispatcher, ".ID", &d0);
/*--------------------------------------------------------------------------*/
} // namespace
