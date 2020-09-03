/***************************************************************************
    copyright            : (C) 2019, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sckt_proto.h"
bool SubcktProto::portExists(unsigned i) const
{
	trace3("sckt_proto::portExists", i, subckt(), this);
	assert(subckt());
	return i<subckt()->numPorts(); //?
}

unsigned SubcktProto::numPorts() const
{ untested();
	trace1("sckt_proto::numPorts", subckt()->numPorts());
	return subckt()->numPorts();
}

Net const* SubcktProto::portValue(unsigned i) const
{
	trace1("sckt_proto::portValue", subckt()->numPorts());
	// return node[i]->net()??
	return subckt()->portValue(i);
}

SubcktProto::SubcktProto(Element const* p)
	: SchematicSymbol(), _instance(p),
	sm(nullptr) // obsolete
{
	auto sym = dynamic_cast<Symbol const*>(p);
	assert(sym);
//	assert(sym->owner());
//	assert(dynamic_cast<Symbol const*>(sym->owner())); ///!!!
}
