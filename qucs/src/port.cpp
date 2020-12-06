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
/*--------------------------------------------------------------------------*/
Port::~Port()
{
//	assert(!_node); not yet.
	if(_node){
		// memory leak
		unreachable();
	}else{
	}
}
/*--------------------------------------------------------------------------*/
std::string const& Port::netLabel() const
{
	assert(_node);
	return _node->netLabel();
}
/*--------------------------------------------------------------------------*/
void Port::connect(Node* n)
{
	assert(!_node);
	_node = n;
	_node->inc_ports();
}
/*--------------------------------------------------------------------------*/
void Port::disconnect(Node* n)
{
	assert(_node);
	n->dec_ports();

	_node = nullptr;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
