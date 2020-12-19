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
#include "globals.h"
#include "language.h"

namespace{

class Get : public Command {
	void do_it(istream_t&, SchematicModel*) override;
}c;
static Dispatcher<Command>::INSTALL p0(&command_dispatcher, "get", &c);

#if 0
void Get::load(istream_t& s, Object* c) const
{
// TODO: move stuff here that does not belong to leg_sch.
	auto l=doclang_dispatcher["leg_sch"];
	assert(l);
	auto L=dynamic_cast<SchematicLanguage const*>(l);
	assert(L);

	if(auto cc=dynamic_cast<SchematicSymbol*>(c)){
		while(!s.atEnd()){
			L->parse(s, cc); // BUG BUG only pass SchematicModel
			assert(s.atEnd()); // happens with legacy lang
		}
	}else if(auto cc=dynamic_cast<SubcktBase*>(c)){
		while(!s.atEnd()){
			L->parse(s, cc); // BUG BUG only pass SchematicModel
			assert(s.atEnd()); // happens with legacy lang
		}
	}else{
		unreachable();
	}

}
#endif

void Get::do_it(istream_t& cmd, SchematicModel* sckt)
{

	std::string fn;
	cmd >> "get";
	cmd >> fn;

	trace1("get", fn);
	istream_t stream (istream_t::_WHOLE_FILE, fn);

// TODO: any language.
	auto L = doclang_dispatcher["leg_sch"];
// 	assert(l);
// 	auto L=dynamic_cast<SchematicLanguage const*>(l);
	assert(L);

	stream.read_line();
	while(!stream.atEnd()){
		L->new__instance(stream, nullptr, sckt);
		stream.read_line();
	}
}
}
