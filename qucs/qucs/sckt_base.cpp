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

#include "sckt_base.h"
#include "schematic_model.h"
void SubcktBase::new_subckt()
{
	assert(!_subckt);
	_subckt = new SchematicModel(nullptr);
}
pos_t SubcktBase::portPosition(unsigned i) const{
	if(subckt()){
		return subckt()->portValue(i)->position();
	}else{
		unreachable();
		return pos_t(0,0);
	}
}
