/***************************************************************************
    copyright            : (C) 2018, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QUCS_SCHEMATIC_SYMBOL_H
#define QUCS_SCHEMATIC_SYMBOL_H
/*--------------------------------------------------------------------------*/
#include "sckt_base.h"
/*--------------------------------------------------------------------------*/
// maybe later.
class SchematicNetlist : public SubcktBase{
protected:
	SchematicSymbol(SchematicSymbol const& p);
public:

	explicit SchematicSymbol();
	~SchematicSymbol();

public
	std::string const& netName(std::string const& nodeName) const {
		incomplete();
		auto m = s->scope();
		assert(m);
		auto n = m->nodes();
		assert(n);

		std::string p = s->port_value(k);
		return n->netName(p);
	}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
