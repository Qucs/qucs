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
#include "netlist.h"
#include "net.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
Conductor::~Conductor()
{
	if(_adj.size()){
		unreachable();
		incomplete();
	}else if(_net){
		unreachable();
		incomplete();
	}else{
	}
}
/*--------------------------------------------------------------------------*/
Conductor::Conductor()
   : _net(nullptr),
     _visit(0)
{
}
/*--------------------------------------------------------------------------*/
std::string const& Conductor::netLabel() const
{
	assert(_net);
	return _net->label();
}
/*--------------------------------------------------------------------------*/
void Conductor::setNetLabel(std::string const& l)
{
  assert(_net);
  _net->setLabel(l);
}
/*--------------------------------------------------------------------------*/
bool Conductor::hasNetLabel() const
{
  if(_net){
    return _net->hasLabel();
  }else{ untested();
    // reachable??
    return false;
  }
}
/*--------------------------------------------------------------------------*/
#if 0
bool Node::hasLabel() const
{
  unreachable(); //???
  if(_net){
    return _net->hasLabel();
  }else{ untested();
    // reachable??
    return false;
  }
}
#endif
/*--------------------------------------------------------------------------*/
void Conductor::detachNet(Net* n)
{
  assert(_net == n);
  assert(_net);
  _net->dec_nodes();
  // if(!_net->size()){
  //   delete _net;
  // }else{
  // }
  _net = nullptr;
}
// ----------------------------------------------------------------
Net* Conductor::newNet(NetList& nl)
{
  assert(!_net);
  _net = nl.newNet();
  assert(_net);
  _net->inc_nodes();
  return _net;
}
// ----------------------------------------------------------------
void Conductor::attachNet(Net* n)
{
  trace2("Conductor::att", this, n);
  assert(n);
  assert(!_net);
  _net = n;
  _net->inc_nodes();
}
/*--------------------------------------------------------------------------*/
