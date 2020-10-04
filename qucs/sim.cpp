/***************************************************************************
    copyright            : (C) 2015, 2020 by Felix Salfelder
    email                : felix@salfelder.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
#include "sim/sim.h"
#include "qucsdoc.h"
/* -------------------------------------------------------------------------------- */
Simulator::~Simulator()
{
	if(_doc){
		_doc->decSimulators();
		_doc = nullptr;
	}else{
	}
}
/* -------------------------------------------------------------------------------- */
void Simulator::attachDoc(QucsDoc* d)
{
	assert(!_doc);
	_doc = d;
	_doc->incSimulators();
	_data = _doc->qucsData(label().toStdString());
	init();
}
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
