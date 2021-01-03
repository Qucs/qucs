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

#include "output.h"
/* -------------------------------------------------------------------------------- */
SimOutputDir::~SimOutputDir()
{
	for(auto i : _d){
		detach(&i.second);
	}
	_d.clear();
}
/* -------------------------------------------------------------------------------- */
SimOutputDir::SimOutputDir(const SimOutputDir& s) : _d(s._d)
{ untested();
	incomplete();
}
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
