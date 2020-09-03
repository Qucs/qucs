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
#include "wire.h"
#include "node.h"// bug. conductor.h
#include "nodemap.h"

#include <QPainter>

Wire::Wire(int _x1, int _y1, int _x2, int _y2)
	: _port0(_x1, _y1), _port1(_x2, _y2)
{
  cx = 0;
  cy = 0;

  Type = isWire; // BUG
  assert(_node_hack.empty());
  _node_hack.push_back(nullptr);
  _node_hack.push_back(nullptr);
  assert(_node_hack.size()==2);

  setType("wire");
  setLabel("noname");
}

Wire::~Wire()
{
  assert(!port(0).connected());
  assert(!port(1).connected());
}

// ----------------------------------------------------------------
void Wire::rotate()
{
  int x1 = x1__();
  int x2 = x2__();
  int y1 = y1__();
  int y2 = y2__();
  int xm, ym, tmp;

  xm = (x1+x2) >> 1;
  ym = (y1+y2) >> 1;

  tmp = x1;
  x1__()  = xm + y1  - ym;
  y1__()  = ym - tmp + xm;

  tmp = x2;
  x2__()  = xm + y2  - ym;
  y2__()  = ym - tmp + xm;

#if 0
  if(Label) {
    tmp = Label->cx_();
    Label->moveTo(xm + Label->cy_() - ym, ym - tmp + xm);
    if(Label->Type == isHWireLabel){
		 // Label->setV(); // something like that, don't abuse type.
		 Label->Type = isVWireLabel;
	 } else {
		 Label->Type = isHWireLabel;
	 }
  }else{
	  unreachable(); /// ?!?!
  }
#endif
}

// ----------------------------------------------------------------
void Wire::setCenter(int x, int y, bool relative)
{
  if(relative) {
    x1() += x;  x2() += x;
    y1() += y;  y2() += y;
//    if(Label) Label->setCenter(x, y, true);
  }
  else {
    x1() = x;  x2() = x;
    y1() = y;  y2() = y;
  }
}

// ----------------------------------------------------------------
void Wire::getCenter(int& x, int& y)
{
  x = (x1()+x2()) >> 1;
  y = (y1()+y2()) >> 1;
}

// ----------------------------------------------------------------
// Lie x/y on wire ? 5 is the precision the coordinates have to fit.
bool Wire::getSelected(int x_, int y_)
{ incomplete();

  int x1 = x1__();
  int x2 = x2__();
  int y1 = y1__();
  int y2 = y2__();
  if(x1-5 <= x_) if(x2+5 >= x_) if(y1-5 <= y_) if(y2+5 >= y_)
    return true;

  return false;
}

// ----------------------------------------------------------------
void Wire::paintScheme(QPainter *p) const
{
  p->drawLine(x1(), y1(), x2(), y2());
//  if(Label)
//    if((Label->Type == isHWireLabel) || (Label->Type == isHWireLabel))
//    if(Label->Type == isHWireLabel)
//      Label->paintScheme(p);
}

// ----------------------------------------------------------------
void Wire::paint(ViewPainter *p) const
{
  int x1 = Wire::x1();
  int x2 = Wire::x2();
  int y1 = Wire::y1();
  int y2 = Wire::y2();

  if(isSelected()){ untested();
    p->Painter->setPen(QPen(Qt::darkGray,6));
    p->drawLine(x1, y1, x2, y2);
    p->Painter->setPen(QPen(Qt::lightGray,2));
    p->drawLine(x1, y1, x2, y2);
  }else{ itested();
    p->Painter->setPen(QPen(Qt::darkBlue,2));
    p->drawLine(x1, y1, x2, y2);
  }
}


// ----------------------------------------------------------------
void Wire::setName(const QString& Name_, const QString& Value_, int delta_, int x_, int y_)
{
#if 0 // not here. this is graphics stuff.
  qDebug() << "Wirelabelparse?!" << Name_;
  if(Name_.isEmpty() && Value_.isEmpty()) {
    if(Label) delete Label;
    Label = 0;
  }else if(!Label) {
    if(isHorizontal())
      Label = new WireLabel(Name_, x1+delta_, y1, x_, y_, isHWireLabel);
    else
      Label = new WireLabel(Name_, x1, y1+delta_, x_, y_, isVWireLabel);
    Label->pOwner = this;
    Label->initValue = Value_;
  }else{
    Label->setName(Name_); // ?!
    Label->setLabel(Name_);
  }
#endif
}

void Wire::setParameter(std::string const& name, std::string const& value)
{
  if(name=="nx"){
    nx = value;
  }else if(name=="ny"){
    ny = value;
  }else if(name=="delta"){
    delta = value;
  }else if(name=="netname"){
    _netname = value;
  }else{ untested();
  }
}
//
// ----------------------------------------------------------------
QRectF Wire::boundingRect() const
{ itested();
  return QRectF(x1()-5, y1()-5, x2()-x1()+10, y2()-y1()+10);
}
// ----------------------------------------------------------------
// // -> conductor.cpp.
#if 0
unsigned Wire::netNumber() const
{
  if (portValue(0)){
    assert(portValue(1));
    assert(portValue(0)->netNumber() == portValue(1)->netNumber());
    return portValue(0)->netNumber();
  }else{
    return -1u;
  }
}
// ----------------------------------------------------------------
void Wire::setPortByIndex(unsigned idx, Node* n)
{
  assert(idx<2);
  auto a=Ports.begin();
  if(idx){
    ++a;
  }else{
  }
  *a = n;
}
// ----------------------------------------------------------------
Node* Wire::portValue(unsigned idx)
{
  assert(idx<2);
  assert(Ports.size()==2);
  auto a=Ports.begin();
  if(idx){
    ++a;
  }else{
  }
  auto n = prechecked_cast<Node*>(*a);
  assert(n || !*a);
  return n;
}
#endif
std::list<Node*>::iterator Wire::connectionsBegin()
{
  trace1("DBG", _node_hack.size());
  assert(_node_hack.size()==2);
  auto a = _node_hack.begin();
  *a = _port0.value();
  ++a;
  *a = _port1.value();
  trace2("wireconnections", _port0.value(), _port1.value());

  return _node_hack.begin();
}
// ----------------------------------------------------------------
// // fishy. involve base case?
Node* Wire::connectNode(unsigned i, NodeMap&l)
{
  //Symbol::connectNode(i, l);
  Port const& pp = port(i);
  Port& mp = port(i);
  Node* n = &l.at(pp.x_()+cx_(), pp.y_()+cy_());
  assert(n->hasNet());

  if(Node* other_node = port((i+1)%2).value()){
    trace3("wire::connectnode", i, n, other_node);
    trace2("wire addedge", n->netLabel(), other_node->netLabel());

    // BUG. label may go the wrong way
    // resolve conflicts more carefully...
    l.addEdge(n, other_node);
  }else{
  }

  n->connectionsAppend(this);
  mp.connect(n);

  if(_netname!=""){
    trace2("wire override netlabel", n->netLabel(), _netname);
    n->setNetLabel(QString::fromStdString(_netname));
  }else{
  }
  return n;
}
// ----------------------------------------------------------------
// // fishy. involve base case?
Node* Wire::disconnectNode(unsigned i, NodeMap&nm)
{
  trace1("wire::disconnectNode", i);
  Node* n = Symbol::disconnectNode(i, nm);

  if(Node* other_node = port((i+1)%2).value()){
    nm.postRemoveEdge(n, other_node);
  }else{
  }
  return n;
}
// ----------------------------------------------------------------
// ----------------------------------------------------------------
Net* Wire::net()
{
  assert(_port0.value());
  assert(_port1.value());
  assert(_port0.value()->net() == _port1.value()->net());
  return _port0.value()->net();
}
// ----------------------------------------------------------------
Net const* Wire::net() const
{ untested();
  assert(_port0.value());
  return _port0.value()->net();
}
// ----------------------------------------------------------------
// vim:ts=8:sw=2:et
