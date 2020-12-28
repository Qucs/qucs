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
	if(_node) {
		n->dec_ports();
		_node = nullptr;
	}else{
		unreachable(); // under construction
	}
}
/*--------------------------------------------------------------------------*/
#if 0 // not yet
void Port::new_node(const std::string& node_name, const Element* d)
{
  //assert(!_nnn); //BUG// fails on MUTUAL_L::expand after clone
  assert(d);

  NodeMap* Map = d->scope()->nodes();
  assert(Map);
  _nnn = Map->new_node(node_name);
///  _ttt = _nnn->user_number();
  assert(_nnn);
}
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
