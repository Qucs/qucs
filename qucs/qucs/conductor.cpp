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
#include "conductor.h"
#include "node.h"
#include "wire.h"
#include "net.h"

#if 1
inline AdjConductorRange::AdjConductorRange(Conductor& c)
{
	_cond_begin = c.connectionsBegin();
	_cond_end = c.connectionsEnd();
}

Conductor* AdjConductorIterator::operator*()
{
	assert(_elt!=_end);
	auto c = dynamic_cast<Conductor*>(*_elt); // yikes
	assert(c);
	return c;
}

void AdjConductorIterator::skip()
{
	while(_elt!=_end){
		assert(intptr_t(*_elt)!=intptr_t(4)); // yes some weid code actually does that.
		if(dynamic_cast<Conductor const*>(*_elt)){
			break;
		}else{
			++_elt;
		}
	}
}

Conductor::~Conductor()
{
}




#endif
