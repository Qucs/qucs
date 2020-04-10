/***************************************************************************
                                symbol.cpp
                                -----------
    copyright            : (C) 2016-2019 Felix Salfelder / QUCS team
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "symbol.h"
#include "schematic_model.h"
#include <assert.h>

// recreate schematic symbol. not sure why, maybe after parameter changes
// (why not just call "Symbol::create??!")
void Symbol::recreate(){
}

Symbol::~Symbol(){
	// disconnect();
}

QString const& Symbol::netLabel(unsigned i) const
{
	auto const* s=getScope();
	assert(s);
//	assert(hasPort(i));
	auto const& n=portValue(i);
	auto nn=n.number();
	
	return s->netLabel(nn);
}
