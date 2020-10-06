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
#include "nodemap.h"
#include "wiredialog.h"
#include "globals.h"
#include "geometry.h"
#include "schematic_model.h"
#include <QPainter>
#include "qt_compat.h" // geometry?
#include "l_compar.h"

#include "viewpainter.h"
#include "wirelabel.h"
#include "platform.h"
#include "symbol.h"

class QPainter;
class QString;

class dummy {};

namespace{

class Wire : public Symbol, public Conductor {
private:
  Wire(Wire const&);

public:
  explicit Wire();
  explicit Wire(int _x1, int _y1, int _x2, int _y2);
  ~Wire();

  Element* clone() const override{
	  return new Wire(*this);
  }
  QDialog* schematicWidget(QucsDoc*) const override;

  Component* newOne(){
	  assert(false);
	  return nullptr;
  }

  void paint(ViewPainter*) const override;
  void setCenter(int, int, bool relative=false);
  void getCenter(int&, int&);
//  bool getSelected(int, int);
  void setName(const QString&, const QString&, int delta_=0, int x_=0, int y_=0);
  QString save(){unreachable(); return "";}

private: // Conductor
  bool isNet(int, int) const override;

private:
  void findScaleAndAngle();

public: // Node xs
//  bool hasNet() const { return _port0.isConnected(); }
#if 0
  Net* net();
  Net const* net() const;
#endif

  QString const& netLabel() const;

private: // Symbol
  void setParameter(std::string const& name, std::string const& value) override;
  std::string getParameter(std::string const& name) const override;
  bool showLabel() const override{ return false; }

private: // symbol Node stuff
  Node* connectNode(unsigned idx, NodeMap&) override;
  Node* disconnectNode(unsigned idx, NodeMap&) override;

private: // Symbol
  void expand() override;
  unsigned numPorts() const;

public:
  bool    isHorizontal() const { return (y1() == y2());}
  QRectF boundingRect() const;

public:
//  void setPortByIndex(unsigned idx, Node* n);
//  Node* portValue(unsigned idx);

private: // Symbol, internal port access.
  Port& port(unsigned i){
	  assert(i<2);
	  if(i==0){
		  return _port0;
	  }else{
		  return _port1;
	  }
  }


private: // FIXME, these are still around. (from element)
//	int & x1__() { return _port0.x; }
//	int & y1__() { return _port0.y; }
//	int & x2__() { return _port1.x; }
//	int & y2__() { return _port1.y; }

	void updatePort();

public: // FIXME, these are still around. (from element)

	int x1() const { return _port0.x(); }
	int y1() const { return _port0.y(); }
	int x2() const { return _port1.x(); }
	int y2() const { return _port1.y(); }

// 	void setPos0(int x, int y){ x1__() = x; y1__() = y; }
// 	void setPos1(int x, int y){ x2__() = x; y2__() = y; }

public: // stuff used in mouseactions.
//  void move1(int x, int y){ unreachable(); } // {x1__()+=x; y1__()+=y; }
//  void move2(int x, int y){ unreachable(); } // {x2__()+=x; y2__()+=y; }

private:
	int & x1() { return _port0.x(); }
	int & y1() { return _port0.y(); }
	int & x2() { return _port1.x(); }
	int & y2() { return _port1.y(); }
private: // avoid access to obsolete Element members
  Port _port0;
  Port _port1;

private:
  std::string nx, ny, delta;
  std::string _netname;

  int _angle;
  int _scale;
};

// ----------------------------------------------------------------
Wire::Wire()
   : _port0(0, 0), _port1(1, 0), _angle(0), _scale(1.)
{
  _cx = 0;
  _cy = 0;

  setTypeName("wire");
  setLabel("noname"); // BUG
}
// ----------------------------------------------------------------
Wire::Wire(Wire const& w)
  : Symbol(w), _port0(w._port0), _port1(w._port1),
   _angle(w._angle), _scale(w._scale)
{
  setLabel(w.label());
}
// ----------------------------------------------------------------
void Wire::findScaleAndAngle()
{
  _scale = std::max(abs(x2()), abs(y2()));
  if(!_scale){ untested();
  }else if(x2() > 0){
    _angle = 0;
  }else if(x2()<0){ untested();
    _angle = 2;
  }else if(y2()>0){ untested();
    _angle = 1;
  }else if(y2()<0){
    _angle = 3;
  }
#ifndef NDEBUG
  trace2("dbg", _angle, _scale);
  auto x=x2();
  auto y=y2();
  updatePort();
  trace4("dbg", x, y, x2(), y2());
  assert(x==x2());
  assert(y==y2());
#endif
}

// gaah. don't use this. (but it is used in a unit test.)
Wire::Wire(int _x1, int _y1, int _x2, int _y2)
  : Symbol(), _port0(0, 0),
    _port1(_x2-_x1, _y2-_y1),
    _angle(0), _scale(1)
{ untested();

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

  setTypeName("wire");
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
void Wire::setCenter(int x, int y, bool relative)
{ untested();
//  Symbol::setCenter(x, y, relative);
  if(relative) { untested();
    _cx += x;
    _cy += y;
//    if(Label) Label->setCenter(x, y, true);
  } else { untested();
    _cx = x;
    _cy = y;
  }
}
/*--------------------------------------------------------------------------*/
QDialog* Wire::schematicWidget(QucsDoc* Doc) const
{ untested();
  trace0("Component::editElement");
  return new WireDialog(Doc); // memory leak?
}
/*--------------------------------------------------------------------------*/
#if 0
// Lie x/y on wire ? 5 is the precision the coordinates have to fit.
bool Wire::getSelected(int , int )
{ incomplete();
  unreachable();

  int x1 = x1__();
  int x2 = x2__();
  int y1 = y1__();
  int y2 = y2__();
  if(x1-5 <= x_) if(x2+5 >= x_) if(y1-5 <= y_) if(y2+5 >= y_)
    return true;

  return false;
}
#endif

// ----------------------------------------------------------------
void Wire::paint(ViewPainter *p) const
{itested();
  int x1 = Wire::x1();
  int x2 = Wire::x2();
  int y1 = Wire::y1();
  int y2 = Wire::y2();

  { itested();
    p->setPen(QPen(Qt::darkBlue,2));
    p->drawLine(x1, y1, x2, y2);
    // p->drawEllipse(x1-2, y1-2, 4, 4);
  }

#if 0
  if(degree()){
    p->setPen(QPen(Qt::green,2));
   p->drawEllipse(x2/2-2, y2/2-2, 4, 4);
  }
#endif

  Symbol::paint(p);
}
// ----------------------------------------------------------------
void Wire::setName(const QString&, const QString&, int, int, int)
{ untested();
#if 0 // not here. this is graphics stuff.
  qDebug() << "Wirelabelparse?!" << Name_;
  if(Name_.isEmpty() && Value_.isEmpty()) { untested();
    if(Label) delete Label;
    Label = 0;
  }else if(!Label) { untested();
    if(isHorizontal())
      Label = new WireLabel(Name_, x1+delta_, y1, x_, y_, isHWireLabel);
    else
      Label = new WireLabel(Name_, x1, y1+delta_, x_, y_, isVWireLabel);
    Label->pOwner = this;
    Label->initValue = Value_;
  }else{ untested();
    Label->setName(Name_); // ?!
    Label->setLabel(Name_);
  }
#endif
}
// ----------------------------------------------------------------
void Wire::updatePort()
{ untested();
  assert(dsin(0) == 0);
  assert(dsin(1) == 1);
  assert(dsin(2) == 0);
  assert(dsin(3) == -1);
  assert(dcos(0) == 1);
  assert(dcos(1) == 0);
  assert(dcos(2) == -1);
  assert(dcos(3) == 0);

  assert(x1() == 0);
  assert(y1() == 0);
  x2() = dcos(_angle) * _scale;
  y2() = dsin(_angle) * _scale;
}
// ----------------------------------------------------------------
std::string Wire::getParameter(std::string const& n) const
{
  if(n=="$hflip"){ untested();
    if (_scale<0){ untested();
      incomplete();
      return "-1";
    }else{ untested();
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
  }else if(n=="netname"){
    return _netname;
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
    updatePort();
  }else if(n=="deltay"){
    int V = atoi(v.c_str());
    y2() = V;
    if(V){
      x2() = 0;
    }else{
    }
    findScaleAndAngle();
    updatePort();
  }else if(n=="netname"){
    _netname = v;
  }else{ untested();
    Symbol::setParameter(n, v);
  }
}
// ----------------------------------------------------------------
unsigned Wire::numPorts() const
{
  return 2; /* really? */
}
// ----------------------------------------------------------------
void Wire::expand()
{
  // stash NodeLabels as subdevices.
  // just not sure where exactly.
  if (_netname != ""){ untested();
    new_subckt();
    Symbol* n = symbol_dispatcher.clone("NodeLabel");
    n->setLabel(_netname);
    // n->setParameters("$xposition");
    // n->setParameters("$yposition");
    // n->setParameters("dx", 5);
    // n->setParameters("dy", 5);
    assert(n);
    trace1("Wire::expand", _netname);
    subckt()->pushBack(n);
  }else{
  }
}
// ----------------------------------------------------------------
QRectF Wire::boundingRect() const
{ itested();
  assert(x1() == 0);
  assert(y1() == 0);
  int N = 10;
  assert(_scale>0);

  if(_angle==0 || _angle==1){ itested();
    return QRectF(-5, -5, x2()+N, y2()+N);
  }else{ itested();
    return QRectF(x2()-5, y2()-5, -x2()+N, -y2()+N);
  }
}
// ----------------------------------------------------------------
Node* Wire::connectNode(unsigned i, NodeMap& nm)
{
  assert(i<2);

  if(hasNet()){
  }else{
    nm.registerVertex(this);
  }
  assert(hasNet());

  Node* n = Symbol::connectNode(i, nm);

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
    trace2("wire addededge2", n->netLabel(), n2->netLabel());
  }else{
  }

  nm.addEdge(this, n);
  trace3("Wire::connect", i, n->degree(), degree());

  if(_netname!=""){
    trace2("wire override netlabel", n->netLabel(), _netname);
    n->setNetLabel(QString::fromStdString(_netname));
  }else{
  }
  return n;
}
// ----------------------------------------------------------------
// // fishy. involve base case?
Node* Wire::disconnectNode(unsigned i, NodeMap& nm)
{ untested();
  assert(hasNet());
  Node* n = Symbol::disconnectNode(i, nm);
  trace3("Wire::disconnect", i, n->degree(), degree());

  //Conductor::removeEdge(n, nm);?
  nm.removeEdge(n, this);

  if(degree()){
  }else{
    nm.deregisterVertex(this);
  }

  return n;
}
// ----------------------------------------------------------------
bool Wire::isNet(int x, int y) const
{ untested();
  x -= cx();
  y -= cy();
  return in_order(x1(), x, x2()) && in_order(y1(), y, y2());
}
// ----------------------------------------------------------------
}
// ----------------------------------------------------------------
// ----------------------------------------------------------------
// vim:ts=8:sw=2:et
