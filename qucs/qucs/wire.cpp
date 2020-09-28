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
#include "globals.h"
#include "geometry.h"
#include <QPainter>
#include "qt_compat.h" // geometry?

Wire::Wire()
   : _port0(0, 0), _port1(1, 0), _angle(0), _scale(1.)
{
  _cx = 0;
  _cy = 0;
  assert(_node_hack.empty());
  _node_hack.push_back(nullptr);
  _node_hack.push_back(nullptr);
  assert(_node_hack.size()==2);

  setType("wire");
  setLabel("noname"); // BUG
}

Wire::Wire(Wire const& w)
  : Symbol(w), _port0(w._port0), _port1(w._port1),
   _angle(w._angle), _scale(w._scale)
{
  assert(_node_hack.empty());
  _node_hack.push_back(nullptr);
  _node_hack.push_back(nullptr);
  assert(_node_hack.size()==2);

  setType(w.type()); // BUG. why not in Component?
  setLabel(w.label());
}

void Wire::findScaleAndAngle()
{
  _scale = std::max(abs(x2()), abs(y2()));
  if(x2() > 0){
    _angle = 0;
  }else if(x2()<0){
    _angle = 2;
  }else if(y1()>0){
    _angle = 3;
  }else{
    _angle = 1;
  }
}

// gaah. don't use this. (but it is used in a unit test.)
Wire::Wire(int _x1, int _y1, int _x2, int _y2)
  : Symbol(), _port0(0, 0),
    _port1(_x2-_x1, _y2-_y1),
    _angle(0), _scale(1)
{

  _cx = _x1;
  _cy = _y1;

  // not here.
  findScaleAndAngle();

  trace4("== created wire", _x1, _y1, _x2, _y2);
  assert(_x1 || _x2 || _y1 || _y2);
  trace2("created wire", _angle, _scale);

  updatePort();

  assert(cx() == _x1);
  assert(cy() == _y1);
  assert(x2() == _x2-_x1);
  assert(y2() == _y2-_y1);

  Type = isWire; // BUG
  assert(_node_hack.empty());
  _node_hack.push_back(nullptr);
  _node_hack.push_back(nullptr);
  assert(_node_hack.size()==2);

  setType("wire");
  setLabel("noname");
}

// not here.
static Wire w;
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "Wire", &w);

// ----------------------------------------------------------------
Wire::~Wire()
{
  assert(!port(0).isConnected());
  assert(!port(1).isConnected());
}

// ----------------------------------------------------------------
//
// // element??
void Wire::setCenter(int x, int y, bool relative)
{
  if(relative) {
    _cx += x;
    _cy += y;
//    if(Label) Label->setCenter(x, y, true);
  } else {
    _cx = x;
    _cy = y;
  }
}

// ----------------------------------------------------------------
void Wire::getCenter(int& x, int& y)
{
  unreachable();
  assert(false);
  x = (x1()+x2()) >> 1;
  y = (y1()+y2()) >> 1;
}

// ----------------------------------------------------------------
// Lie x/y on wire ? 5 is the precision the coordinates have to fit.
bool Wire::getSelected(int x_, int y_)
{ incomplete();

#if 0
  int x1 = x1__();
  int x2 = x2__();
  int y1 = y1__();
  int y2 = y2__();
  if(x1-5 <= x_) if(x2+5 >= x_) if(y1-5 <= y_) if(y2+5 >= y_)
    return true;
#endif

  return false;
}

// ----------------------------------------------------------------
void Wire::paint(ViewPainter *p) const
{
  int x1 = Wire::x1();
  int x2 = Wire::x2();
  int y1 = Wire::y1();
  int y2 = Wire::y2();

  { itested();
    p->setPen(QPen(Qt::darkBlue,2));
    p->drawLine(x1, y1, x2, y2);
    // p->drawEllipse(x1-2, y1-2, 4, 4);
  }

  Symbol::paint(p);
}

// ----------------------------------------------------------------
void Wire::setName(const QString&, const QString&, int, int, int)
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

// ----------------------------------------------------------------
void Wire::updatePort()
{
  assert(dsin(0) == 0);
  assert(dsin(1) == 1);
  assert(dsin(2) == 0);
  assert(dsin(3) == -1);

  assert(x1() == 0);
  assert(y1() == 0);
  x2() = dcos(_angle) * _scale;
  y2() = dsin(_angle) * _scale;
}

// ----------------------------------------------------------------
std::string Wire::getParameter(std::string const& n) const
{
  if(n=="$hflip"){ untested();
    if (_scale<0){
      incomplete();
      return "-1";
    }else{
      return "1";
    }
  }else if(n=="symbol_scale"){ untested();
    return std::to_string(_scale);
  }else if(n=="$angle"){ untested();
    return std::to_string(_angle*90);
  }else if(n=="$vflip"){ untested();
    return "1";
  }else if(n=="deltax"){
    return std::to_string(x2());
  }else if(n=="deltay"){
    return std::to_string(y2());
  }else{
    return Symbol::getParameter(n);
  }
}

// ----------------------------------------------------------------
void Wire::setParameter(std::string const& n, std::string const& v)
{
  if(n=="nx"){
    nx = v;
  }else if(n=="ny"){
    ny = v;
  }else if(n=="$xposition"){
    _cx = atoi(v.c_str());
  }else if(n=="$yposition"){
    _cy = atoi(v.c_str());
  }else if(n=="$hflip"){ untested();
    _scale = abs(_scale) * atoi(v.c_str());
    updatePort();
  }else if(n=="$angle"){ untested();
    _angle = atoi(v.c_str());
    assert(!(_angle % 90)); // for now.
    _angle/=90;
    _angle %=4;
    updatePort();
    trace4("new angle", _angle, _cx, _cy, _scale);
    trace4("new angle", x1(), y1(), x2(), y2());
  }else if(n=="$vflip"){ untested();
    // it does not matter.
    int tmp = atoi(v.c_str());
    if (tmp==-1){ untested();
    }else if(tmp==1){ untested();
    }else{ untested();
      unreachable();
    }
  }else if(n=="delta"){
    delta = v;
  }else if(n=="deltax"){
    int V = atoi(v.c_str());
    x2() = V;
    if(V){
      y2() = 0;
    }else{
    }
    findScaleAndAngle();
  }else if(n=="deltay"){
    int V = atoi(v.c_str());
    y2() = V;
    if(V){
      x2() = 0;
    }else{
    }
    findScaleAndAngle();
  }else if(n=="netname"){
    _netname = v;
//  }else if(n=="x0"){
//    x1() = atoi(v.c_str());
//  }else if(n=="y0"){
//    y1() = atoi(v.c_str());
//  }else if(n=="x1"){
//    x2() = atoi(v.c_str());
//  }else if(n=="y1"){
//    y2() = atoi(v.c_str());
  }else{ untested();
    Symbol::setParameter(n, v);
  }
}
// ----------------------------------------------------------------
QRectF Wire::boundingRect() const
{ itested();
  assert(x1() == 0);
  assert(y1() == 0);
  int N = 10;

  if(_angle==0 || _angle==1){
//    assert(x2()>=0);
//    assert(y2()>=0);
    return QRectF(-5, -5, x2()-x1()+N, y2()-y1()+N);
  }else{
//    assert(x2()<=0);
//    assert(y2()<=0);
    return QRectF(x2()-5, y2()-5, -x2()+x1()+N, -y2()+y1()+N);
  }
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
  assert(i<2);
  //Symbol::connectNode(i, l);
  Port const& pp = port(i);
  Port& mp = port(i);
  auto c = center();
  int cx = getX(c);
  int cy = getY(c);
  assert(cx==_cx);
  assert(cy==_cy);

  Node* n;
  trace3("wire::connectnode", cx, cy, i);
  trace2("wire::connectnode", pp.x_(), pp.y_());
  n = &l.at(cx+pp.x_(), cy+pp.y_());
  assert(n->hasNet());

  if(Node* n2 = port((i+1)%2).value()){
    trace3("wire::connectnode", i, n, n2);
    trace2("wire addedge", n->netLabel(), n2->netLabel());

    // resolve conflicts more carefully...
    if(!n->hasNetLabel()){
      n->setNetLabel(n2->netLabel());
    }else if(!n2->hasNetLabel()){
      n2->setNetLabel(n->netLabel());
    }else{
      std::cerr << "possible label conflict. not sure what to do\n";
      std::cerr << n->netLabel() << " vs " << n2->netLabel() << "\n";
    }
    trace2("wire addededge", n->netLabel(), n2->netLabel());

    l.addEdge(n, n2);
    trace2("wire addededge2", n->netLabel(), n2->netLabel());
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
