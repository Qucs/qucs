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

#include "command.h"
#include "qucs_globals.h"
#include "language.h"
#include "io.h"

namespace{
/*--------------------------------------------------------------------------*/
class Get : public Command {
	void do_it(istream_t&, SchematicModel*) override;
}c;
static Dispatcher<Command>::INSTALL p0(&command_dispatcher, "get", &c);
/*--------------------------------------------------------------------------*/
void Get::do_it(istream_t& cmd, SchematicModel* sckt)
{
	std::string fn;
	cmd >> "get";
	cmd >> fn;

	trace1("get", fn);
	istream_t stream (istream_t::_WHOLE_FILE, fn);

	stream.read_line();

// TODO: any language.
// magic = stream.fullstring ...

	auto L = language_dispatcher["leg_sch"];
	assert(L);

	while(!stream.atEnd()){
		L->new__instance(stream, nullptr, sckt);
		stream.read_line();
	}
}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
