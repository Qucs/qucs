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
#include "qio.h"

namespace{
/*--------------------------------------------------------------------------*/
class Get : public Command {
	void do_it(istream_t&, ElementList*) override;
}c;
static Dispatcher<Command>::INSTALL p0(&command_dispatcher, "get", &c);
/*--------------------------------------------------------------------------*/
void Get::do_it(istream_t& cmd, ElementList* sckt)
{
	std::string fn;
	cmd >> "get";
	cmd >> fn;


	std::string suffix;
	auto pos = fn.find_last_of(".");
	if(pos == std::string::npos){
		suffix = ".sch"; // wild guess
	}else{
		suffix = fn.substr(pos);
	}

	trace2("get", fn, suffix);

	istream_t stream (istream_t::_WHOLE_FILE, fn);
	stream.read_line();

// TODO: any language.
// magic = stream.fullstring ...

	auto L = language_dispatcher[suffix];
	assert(L);

	while(!stream.atEnd()){
		L->new__instance(stream, nullptr, sckt);
		stream.read_line();
	}
}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
