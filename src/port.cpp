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
#include "element.h"
#include "schematic_model.h"
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
const std::string nullabel="(null)";
std::string const& Port::nodeLabel() const
{
	if(_node){
		return _node->label();
	}else{
		// unreachable(); useful for debugging
		return nullabel;
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
void Port::new_node(const std::string& node_name, Element const* d)
{
  assert(d);

  SchematicModel const* scope = d->scope();
  if(scope){
	  NodeMap* Map = scope->nodes();
	  assert(Map);
	  if(_node){
		  _node->dec_ports();
	  }else{
	  }
	  _node = Map->new_node(node_name);

	  if(_node){
		  _node->inc_ports();
	  }else{
	  }
	///  _ttt = _nnn->user_number();
  }else{
	  // cannot have nodes without scope, can we?!
	  trace2("new node no scope", d->label(), node_name);
	  assert(false);
	  unreachable();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
