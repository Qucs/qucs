/***************************************************************************
    copyright            : (C) 2020 Felix Salfelder / QUCS team
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "port.h"
#include "node.h"

Port::Port(Port const& p)
	: _x(p._x), _y(p._y), Connection(nullptr)
{
}

QString const& Port::netLabel() const
{
	assert(Connection);
	return Connection->netLabel();
}
