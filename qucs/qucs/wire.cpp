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
#include "place.h"
#include "sckt_base.h"

class QPainter;
class QString;

namespace{

class Wires : public SubcktBase{
	//...
};

class Wire : public Symbol, public Conductor {
private:
	Wire(Wire const&);
	explicit Wire(pos_t const&, pos_t const&);

public:
	explicit Wire();
	~Wire();

	void setCenter(int, int, bool relative=false);
	void getCenter(int&, int&);

private: // Element
	Wire* clone() const override{
		return new Wire(*this);
	}
	QDialog* schematicWidget(QucsDoc*) const override;
	void paint(ViewPainter*) const override;
	QRectF boundingRect() const override;

private: // Conductor
	Symbol* newUnion(const Symbol*) const override;
	bool isNet(pos_t const&) const override;

private:
	Symbol* newPort(const Place*) const;
	Symbol* newTee(const Wire*) const;
	Symbol* intersectPorts(const Symbol*) const;
	bool isInterior(pos_t const&) const;

private: // Symbol
	void setParameter(std::string const& name, std::string const& value) override;
	std::string paramValue(std::string const& name) const override;
	bool showLabel() const override{ return false; }
	void expand() override;
	unsigned numPorts() const override;

private: // symbol Node stuff
	Node* connectNode(unsigned idx, NodeMap&) override;
	Node* disconnectNode(unsigned idx, NodeMap&) override;

public: // needed?
	QString const& netLabel() const;

private: // Symbol, internal port access.
	Port& port(unsigned i){
		assert(i<2);
		if(i==0){
			return _port0;
		}else{
			return _port1;
		}
	}

private:
	void findScaleAndAngle();
	void updatePort();
	Wire* extendTowards(pos_t const&) const;

	pos_t pP1() const{
		return _port1.position();
	}
	void setP1(pos_t const& p){
		_port1.setPosition(p);
	}

public: // FIXME, these are still around. (from element)
	int x1() const { return _port0.x(); }
	int y1() const { return _port0.y(); }
	int x2() const { return _port1.x(); }
	int y2() const { return _port1.y(); }

private:
	Port _port0;
	Port _port1;
	std::string nx, ny, delta;
	std::string _netname;

	int _angle;
	int _scale;
}w;
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "Wire", &w);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
Wire::Wire()
   : _port0(0, 0), _port1(1, 0), _angle(0), _scale(1.)
{
  _cx = 0;
  _cy = 0;

  setTypeName("wire");
  setLabel("noname"); // BUG
}
/*--------------------------------------------------------------------------*/
Wire::Wire(Wire const& w)
  : Symbol(w), _port0(w._port0), _port1(w._port1),
   _angle(w._angle), _scale(w._scale)
{
  setLabel(w.label());
}
/*--------------------------------------------------------------------------*/
Wire::Wire(pos_t const& p0, pos_t const& p1)
  : Symbol(), _port0(0, 0),
    _port1((p1 - p0).first, (p1 - p0).second),
    _angle(0), _scale(1)
{ itested();

	_cx = getX(p0);
	_cy = getY(p0);
	trace2("new Wire", p0, p1);
	trace2("new Wire", _cx, _cy);

	findScaleAndAngle();
	updatePort();

	trace3("new Wire", _scale, _cx, _cy);
	assert(_scale>0); // for now?

	// assert(cx() == _x1);
	// assert(cy() == _y1);
	// assert(x2() == _x2-_x1);
	// assert(y2() == _y2-_y1);

	setTypeName("wire");
	setLabel("noname");
}
/*--------------------------------------------------------------------------*/
Wire::~Wire()
{
	assert(!port(0).isConnected());
	assert(!port(1).isConnected());
}
/*--------------------------------------------------------------------------*/
void Wire::findScaleAndAngle()
{
  _scale = std::max(abs(x2()), abs(y2()));
  if(!_scale){
  }else if(x2() > 0){
    _angle = 0;
  }else if(x2()<0){
    _angle = 2;
  }else if(y2()>0){
    _angle = 1;
  }else if(y2()<0){
    _angle = 3;
  }
#ifndef NDEBUG
  trace2("dbg", _angle, _scale);
  auto x=x2();
  auto y=y2();
  updatePort();
  trace4("dbg same?", x, y, x2(), y2());
  assert(x==x2());
  assert(y==y2());
#endif
}
/*--------------------------------------------------------------------------*/
Symbol* Wire::newPort(Place const* pl) const
{ untested();
	pos_t where = pl->nodePosition(0);

	if(nodePosition(0) == where) { untested();
	}else if(nodePosition(1) == where) { untested();
	}else if(isNet(where)) { untested();
		Symbol *s = new SubcktBase(); // BUG: inherit.
		s->new_subckt();
		SchematicModel* m = s->subckt();
		m->pushBack( new Wire(nodePosition(0), where));
		m->pushBack( new Wire(nodePosition(1), where));

		return s;
	}else{ untested();
	}
	return nullptr;
}
/*--------------------------------------------------------------------------*/
Symbol* Wire::intersectPorts(Symbol const* s) const
{
	std::vector<pos_t> split;
	for(unsigned i=0; i<s->numPorts(); ++i){
		pos_t p = s->nodePosition(i);

		if(nodePosition(0) == p) { untested();
		}else if(nodePosition(1) == p) { untested();
		}else if(isInterior(p)) {
			split.push_back(p);
		}
	}


	if(split.size()){
		trace2("intersectPorts", split.size(), split[0]);
		split.push_back(nodePosition(0));
		split.push_back(nodePosition(1));

		std::sort(split.begin(), split.end());
		Symbol *sckt = new SubcktBase(); // BUG: inherit.
		sckt->new_subckt();
		SchematicModel* m = sckt->subckt();
		m->pushBack(s->clone());

		auto pp = split.begin();
		auto next = pp;
		++next;
		while(next!=split.end()){
			m->pushBack(new Wire(*pp, *next));
			trace2("intersectPorts", *pp, *next);
			pp = next;
			++next;
		}
		return sckt;
	}else{ untested();
	}
	return nullptr;
}
/*--------------------------------------------------------------------------*/
Wire* Wire::extendTowards(pos_t const& other) const
{
	auto n0 = nodePosition(0);
	auto n1 = nodePosition(1);

	if(in_order(n0, other, n1)){
		return clone();
	}else if(in_order(other, n1, n0) && _port1->numPorts()==1){
		trace3("extend1", other, n1, n0);
		return new Wire(other, n0);
	}else if(in_order(other, n0, n1) && _port0->numPorts()==1){
		trace3("extend2",other, n0, n1);
		return new Wire(other, n1);
	}else{
		return nullptr;
	}
}
/*--------------------------------------------------------------------------*/
Symbol* Wire::newTee(Wire const* o) const
{
	assert((_angle - o->_angle )%2);
	pos_t t0 = nodePosition(0);
	pos_t t1 = nodePosition(1);
	pos_t split(0, 0);
	Symbol* s = nullptr;
	Symbol const* teew = o;

	trace2("interior?", o->nodePosition(0), o->nodePosition(1));
	if(isInterior(o->nodePosition(0))){
		s = new SubcktBase(); // BUG: inherit.
		split = o->nodePosition(0);
	}else if(isInterior(o->nodePosition(1))){
		s = new SubcktBase(); // BUG: inherit.
		split = o->nodePosition(1);
	}else if(o->isInterior(nodePosition(0))){
		// BUG: call o->newUnion(this)
		s = new SubcktBase(); // BUG: inherit.
		split = nodePosition(0);
		t0 = o->nodePosition(0);
		t1 = o->nodePosition(1);
		teew = this;
	}else if(o->isInterior(nodePosition(1))){ untested();
		// BUG: call o->newUnion(this)
		s = new SubcktBase(); // BUG: inherit.
		split = nodePosition(1);
		t0 = o->nodePosition(0);
		t1 = o->nodePosition(1);
		teew = this;
	}else{
	}

	if(s){
		trace1("building tee", split);
		s->new_subckt();
		SchematicModel* m = s->subckt();
		assert(m);

		// BUG BUG BUG. this depends on the order.
		m->pushBack(new Wire(t0, split));
		m->pushBack(teew->clone());
		m->pushBack(new Wire(t1, split));
	}else{
		trace1("no tee", split);
	}

	return s;
}
/*--------------------------------------------------------------------------*/
Symbol* Wire::newUnion(Symbol const* s) const
{
	auto p = dynamic_cast<Place const*>(s);
	auto o = dynamic_cast<Wire const*>(s);
	if(p){ untested();
		return newPort(p);
	}else if(o){
		trace2("Wire::newUnion(Wire)", nodePosition(0), nodePosition(1));
		trace2("Wire::newUnion(Wire)", _port0->degree(), _port1->degree());
		
		if( (_angle - o->_angle )%2 ){
			return newTee(o);
		}else if(isNet(o->nodePosition(0))){
			return extendTowards(o->nodePosition(1));
		}else if(isNet(o->nodePosition(1))){ untested();
			return extendTowards(o->nodePosition(0));
		}else{ untested();
		}
	}else{ untested();
		return intersectPorts(s);
	}
	return nullptr;
}
/*--------------------------------------------------------------------------*/
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
  if(degree()){ untested();
    p->setPen(QPen(Qt::green,2));
   p->drawEllipse(x2/2-2, y2/2-2, 4, 4);
  }
#endif

  Symbol::paint(p);
}
// ----------------------------------------------------------------
#if 0 // not here. this is graphics stuff.
void Wire::setName(const QString&, const QString&, int, int, int)
{ untested();
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
}
#endif
/*--------------------------------------------------------------------------*/
void Wire::updatePort()
{
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
  int x = dcos(_angle) * _scale;
  int y = dsin(_angle) * _scale;
  setP1(pos_t(x, y));
}
/*--------------------------------------------------------------------------*/
std::string Wire::paramValue(std::string const& n) const
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
  }else if(n=="$angle"){
    return std::to_string(_angle*90);
  }else if(n=="$vflip"){ untested();
    return "1";
  }else if(n=="deltax"){
    return std::to_string(x2());
  }else if(n=="deltay"){
    return std::to_string(y2());
  }else if(n=="netname"){ untested();
    return _netname;
  }else{
    return Symbol::paramValue(n);
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
    }else{
      unreachable();
    }
  }else if(n=="delta"){
    delta = v;
  }else if(n=="deltax"){
    int V = atoi(v.c_str());
    auto p1 = pP1();
    p1.first = V;
    if(V){
      p1.second = 0;
    }else{
    }
    setP1(p1);
    findScaleAndAngle();
    updatePort();
  }else if(n=="deltay"){
    int V = atoi(v.c_str());
    auto p1 = pP1();
    p1.second = V;
    if(V){
      p1.first = 0;
    }else{
    }
    setP1(p1);
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
  if (_netname != ""){
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
  }else{ untested();
  }
}
// ----------------------------------------------------------------
QRectF Wire::boundingRect() const
{ itested();
  assert(x1() == 0);
  assert(y1() == 0);
  int N = 10;
  // assert(_scale>0); oops??

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
Node* Wire::disconnectNode(unsigned i, NodeMap& nm)
{ itested();
  assert(hasNet());
  Node* n = Symbol::disconnectNode(i, nm);
  trace3("Wire::disconnect", i, n->degree(), degree());

  //Conductor::removeEdge(n, nm);?
  nm.removeEdge(n, this);

  if(degree()){ itested();
  }else{
    nm.deregisterVertex(this);
  }

  return n;
}
/*--------------------------------------------------------------------------*/
bool Wire::isInterior(pos_t const& p) const
{
	if(p==nodePosition(0)){
		return false;
	}else if(p==nodePosition(1)){ untested();
		return false;
	}else{
		return isNet(p);
	}
}
/*--------------------------------------------------------------------------*/
bool Wire::isNet(pos_t const& p) const
{itested();
  int x = getX(p);
  int y = getY(p);
  x -= cx();
  y -= cy();
  return in_order(x1(), x, x2()) && in_order(y1(), y, y2());
}
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
