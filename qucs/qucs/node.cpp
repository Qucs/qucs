/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "node.h"
#include "wire.h"
#include "viewpainter.h" //?
#include "wirelabel.h"
#include "net.h"
#include "netlist.h"

#include <QPainter>

#if 0
Node::Node(int _x, int _y) : Conductor(), Element()
{ untested();
  unreachable();
}
#endif

Node::Node(std::pair<int, int> pos)
  :Conductor(), Element(),
   _position(pos),
   _net(nullptr),
   _visit(0)
{
  trace1("Node::Node", this);
  // Label = nullptr; // BUG
//  Type  = isNode;
  State = 0;
  DType = "";

  // BUG:
//  Element::cx = _x;
//  Element::cy = _y;
}

Node::~Node()
{ untested();
  trace1("~Node", this);
  assert(!connectionsCount());
  assert(!_net);
}

// -------------------------------------------------------------
void Node::paint(ViewPainter *p)
{ untested();
  switch(Connections.count()) {
    case 1:  if(hasLabel())
               p->fillRect(cx()-2, cy()-2, 4, 4, Qt::darkBlue); // open but labeled
             else { untested();
               p->Painter->setPen(QPen(Qt::red,1));  // node is open
               p->drawEllipse(cx()-4, cy()-4, 8, 8);
             }
             return;
    case 2:  if(Connections.getFirst()->Type == isWire)
               if(Connections.getLast()->Type == isWire) return;
             p->fillRect(cx()-2, cy()-2, 4, 4, Qt::darkBlue);
             break;
    default: p->Painter->setBrush(Qt::darkBlue);  // more than 2 connections
	     p->Painter->setPen(QPen(Qt::darkBlue,1));
	     p->drawEllipse(cx()-3, cy()-3, 6, 6);
	     p->Painter->setBrush(Qt::NoBrush);
             break;
  }
}
// ----------------------------------------------------------------
bool Node::getSelected(int x_, int y_)
{ untested();
  if(cx()-5 <= x_) if(cx()+5 >= x_) if(cy()-5 <= y_) if(cy()+5 >= y_)
    return true;

  return false;
}
// ----------------------------------------------------------------
QString const& Node::netLabel() const
{
  assert(_net);
  return _net->label();
}
// ----------------------------------------------------------------
void Node::setNetLabel(QString const& l)
{
  assert(_net);
  _net->setLabel(l);
}
// ----------------------------------------------------------------
bool Node::hasNetLabel() const
{
  if(_net){
    return _net->hasLabel();
  }else{ untested();
    // reachable??
    return false;
  }
}
// ----------------------------------------------------------------
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
// ----------------------------------------------------------------

// BUG: does not set Name
// what is a "Name"??
void Node::setName(const QString& , const QString& , int, int )
{ untested();
  incomplete();
#if 0
  if(Name_.isEmpty() && Value_.isEmpty()) { untested();
    if(Label) delete Label;
    Label = 0;
    return;
  }

  if(!Label) Label = new WireLabel(Name_, cx(), cy(), x_, y_, isNodeLabel);
  else Label->setName(Name_);
  Label->pOwner = this;
  Label->initValue = Value_;
#endif
}

// ----------------------------------------------------------------
QRectF Node::boundingRect() const
{ untested();
  QRectF b(cx()-4,cy()-4,8,8);
  return b;
}
// ----------------------------------------------------------------
AdjNodeRange::AdjNodeRange(Node& n)
  : _begin(AdjNodeIterator(n.connectionsBegin(), n.connectionsEnd())),
    _end(AdjNodeIterator(n.connectionsEnd(), n.connectionsEnd()))
{
  trace2("AdjNodeRange", n.connectionsCount(), &n);
}
// ----------------------------------------------------------------
bool AdjNodeIterator::operator==(AdjNodeIterator const& o)
{
  assert(is_valid());
  assert(o.is_valid());
  if(_wire!=o._wire){
    return false;
  }else if(_wire==_wend && o._wire==o._wend){
    return true;
  }else{
    return _node==o._node;
  }
}
// ----------------------------------------------------------------
AdjNodeIterator& AdjNodeIterator::operator++()
{
  assert(is_valid());
  next();
  skip();
  assert(is_valid());
  return *this;
}
// ----------------------------------------------------------------
AdjNodeIterator::AdjNodeIterator(elt_iter b, elt_iter e)
  :_wire(b), _wend(e)
{
  if (b==e){
      _node = _nend; // needed?
  }else if(auto w=dynamic_cast<Wire*>(*_wire)){
    _node = w->connectionsBegin();
    _nend = w->connectionsEnd();
  }else{
    // there is a wire, but it is null.
  }
  skip();
  assert(is_valid());
}
// ----------------------------------------------------------------
void Node::detachNet(Net* n)
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
Net* Node::newNet(NetList& nl)
{
  assert(!_net);
  _net = nl.newNet();
  assert(_net);
  _net->inc_nodes();
  return _net;
}
// ----------------------------------------------------------------
void Node::attachNet(Net* n)
{
  trace2("Node::att", this, n);
  assert(n);
  assert(!_net);
  _net = n;
  _net->inc_nodes();
}
// ----------------------------------------------------------------
AdjNodeRange Node::neighbours()
{
  return AdjNodeRange(*this);
}
// ----------------------------------------------------------------
bool AdjNodeIterator::is_valid() const
{
  if(_wire==_wend){
    return true;
  }else if(*_wire==nullptr){ untested();
    // some Connections are not wires.
    return false;
  }else if(_node==_nend){
    return false;
  }else if(*_node==nullptr){
    // fishy wire-connection iterator FIXME
    return false;
  }else{
    return true;
  }
}
// ----------------------------------------------------------------
void AdjNodeIterator::skip()
{
  while(!is_valid()){
    next();
  }
}
// ----------------------------------------------------------------
void AdjNodeIterator::next()
{
  if(_node != _nend){
    ++_node;
  }else if(_wire!=_wend){
    ++_wire;

    if(_wire==_wend){
      _nend = _node;
    }else if(auto w=dynamic_cast<Wire*>(*_wire)){
      _node = w->connectionsBegin();
      _nend = w->connectionsEnd();
    }else{ untested();
      _nend = _node;
    }
  }else{
  }
}
// ----------------------------------------------------------------
Node* AdjNodeIterator::operator*()
{
  assert(is_valid());
  assert(_wire!=_wend);
  assert(_node!=_nend);
  Node* n = prechecked_cast<Node*>(*_node);
  assert(n);
  return n;
}
// ----------------------------------------------------------------
// ----------------------------------------------------------------
// vim:ts=8:sw=2:et
