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
#include "sckt_base.h"
/*--------------------------------------------------------------------------*/
class QPainter;
class QString;
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
class Wires : public SubcktBase{
private:
	Port& port(unsigned){ throw "unreachable";}
};
/*--------------------------------------------------------------------------*/
class Wire : public Symbol, public Conductor {
private:
	explicit Wire(Wire const&);
	explicit Wire(pos_t const&, pos_t const&);

public:
	explicit Wire();
	~Wire();

	void getCenter(int&, int&);

private: // Element
	Wire* clone() const override{
		return new Wire(*this);
	}
	QDialog* schematicWidget(QucsDoc*) const override;
	void paint(ViewPainter*) const override;
	rect_t bounding_rect() const override;

private: // Conductor
	Symbol* newUnion(const Symbol*) const override;
	bool isNet(pos_t const&) const override;

private:
//	Symbol* newPort(const Place*) const;
	Symbol* newTee(const Wire*) const;
	Symbol* intersectPorts(const Symbol*) const;
	bool isInterior(pos_t const&) const;

private: // Symbol
	void setParameter(std::string const& name, std::string const& value) override;
	std::string paramValue(std::string const& name) const override;
	bool showLabel() const override{ return false; }
	void expand() override;
	unsigned numPorts() const override;
	pos_t portPosition(unsigned i) const{
		switch(i){
			case 0: return pos_t(0,0);
			case 1: return pos_t(abs(_scale), 0);
			default: unreachable();
			 return pos_t(0,0);
		}
	}

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
	void findScaleAndAngle(pos_t p1);
	Wire* extendTowards(pos_t const&) const;

	pos_t pP1() const;

public: // FIXME, these are still around. (from element)
	int x2() const { return pP1().first; }
	int y2() const { return pP1().second; }

private:
	Port _port0;
	Port _port1;
	std::string nx, ny, delta;
	std::string _netname;
	int _scale;
	bool _has_netname;
}w;
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "Wire", &w);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
Wire::Wire() : Symbol(),
    _port0(), _port1(),
    _scale(1.),
    _has_netname(false)
{
	assert(position() == pos_t(0, 0));
	// Symbol::setPosition(pos_t(0, 0)); // redundant?

	setTypeName("wire");
	setLabel("noname"); // BUG
}
/*--------------------------------------------------------------------------*/
Wire::Wire(Wire const& w)
  : Symbol(w),
    _port0(),
    _port1(),
    _scale(w._scale),
    _has_netname(w._has_netname)
{
	setLabel(w.label());
}
/*--------------------------------------------------------------------------*/
Wire::Wire(pos_t const& p0, pos_t const& p1)
  : Symbol(),
    _port0(),
    _port1(),
    _scale(1.),
    _has_netname(false)
{
	Symbol::setPosition(p0);

	pos_t pp1((p1 - p0).first, (p1 - p0).second);
	findScaleAndAngle(pp1);

	assert(_scale>0); // for now?
	assert(p0 == nodePosition(0));
	assert(p1 == nodePosition(1));

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
void Wire::findScaleAndAngle(pos_t p1)
{
	int x2 = getX(p1);
	int y2 = getY(p1);
	_scale = std::max(abs(x2), abs(y2));
	if(!_scale){
	}else if(x2 > 0){
		setAngle(0);
	}else if(x2<0){
		setAngle(180);
	}else if(y2>0){
		setAngle(270);
	}else if(y2<0){
		setAngle(90);
	}else{ untested();
	}

#if 0
	auto n0 = nodePosition(0);
	auto n1 = nodePosition(1);
	setLabel("wire_" + std::to_string(n0.first) +
			":"+ std::to_string(n0.second) +
			"_"+ std::to_string(n1.first) +
			":"+ std::to_string(n1.second));
#endif
}
/*--------------------------------------------------------------------------*/
Symbol* Wire::intersectPorts(Symbol const* s) const
{
	std::vector<pos_t> split;
	for(unsigned i=0; i<s->numPorts(); ++i){
		pos_t p = s->nodePosition(i);
		trace4("intersectPorts", i, s->nodePosition(i), s->label(), s->typeName());

		if(nodePosition(0) == p) {itested();
		}else if(nodePosition(1) == p) {itested();
		}else if(isInterior(p)) {
			trace1("found split", p);
			split.push_back(p);
		}
	}

	if(split.size()){
		trace2("intersectPorts", split.size(), split[0]);
		split.push_back(nodePosition(0));
		split.push_back(nodePosition(1));

		std::sort(split.begin(), split.end());
		Symbol *sckt = symbol_dispatcher.clone("subckt_proto");
		assert(sckt);
		sckt->new_subckt();
		SchematicModel* m = sckt->subckt();

		// TODO: ordering bug probably here as well.
		m->pushBack(s->clone());

		auto pp = split.begin();
		auto next = pp;
		++next;
		while(next!=split.end()){
			m->pushBack(new Wire(*pp, *next));
			trace3("intersectPorts new wire", *pp, *next, split.size());
			pp = next;
			++next;
		}
		return sckt;
	}else{itested();
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
	}else if(in_order(other, n0, n1) && _port0->numPorts()==1){ untested();
		trace3("extend2",other, n0, n1);
		return new Wire(other, n1);
	}else{
		return nullptr;
	}
}
/*--------------------------------------------------------------------------*/
Symbol* Wire::newTee(Wire const* o) const
{
	assert((angle() - o->angle() )%180);
	pos_t t0 = nodePosition(0);
	pos_t t1 = nodePosition(1);
	pos_t split(0, 0);
	Symbol* s = nullptr;
	Symbol const* teew = o;

	trace2("interior?", o->nodePosition(0), o->nodePosition(1));
	if(isInterior(o->nodePosition(0))){
		s = new Wires();
		split = o->nodePosition(0);
	}else if(isInterior(o->nodePosition(1))){ untested();
		s = new Wires();
		split = o->nodePosition(1);
	}else if(o->isInterior(nodePosition(0))){
		// BUG: call o->newUnion(this)
		s = new Wires();
		split = nodePosition(0);
		t0 = o->nodePosition(0);
		t1 = o->nodePosition(1);
		teew = this;
	}else if(o->isInterior(nodePosition(1))){ untested();
		// BUG: call o->newUnion(this)
		s = new Wires();
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
	trace3("Wire::newUnion(Symbol)", s->label(), nodePosition(0), nodePosition(1));
//	auto p = dynamic_cast<Place const*>(s);
	auto o = dynamic_cast<Wire const*>(s);
//	if(p){ untested();
//		return newPort(p);
//	}else
	if(o){
		trace2("Wire::newUnion(Wire)", o->nodePosition(0), o->nodePosition(1));
		trace2("Wire::newUnion(Wire)", _port0->degree(), _port1->degree());
		trace2("Wire::newUnion(Wire)", angle(), o->angle());
		
		if( (360 + angle() - o->angle() )%180 ){
			return newTee(o);
		}else if(isNet(o->nodePosition(0))){
			return extendTowards(o->nodePosition(1));
		}else if(isNet(o->nodePosition(1))){ untested();
			return extendTowards(o->nodePosition(0));
		}else{ untested();
		}
	}else{
		return intersectPorts(s);
	}
	return nullptr;
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
	//  int x1 = 0; // Wire::x1();
	//  int y1 = 0; // Wire::y1();

	{ itested();
		p->setPen(QPen(Qt::darkBlue,2));
		p->drawLine(0, 0, abs(_scale), 0);
		//    p->drawEllipse(x1-2, y1-2, 4, 4);
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
std::string Wire::paramValue(std::string const& n) const
{
	if(n=="symbol_scale"){ untested();
		return std::to_string(_scale);
	}else if(n=="deltax"){ untested();
		auto p = portPosition(1);
		return std::to_string(getX(p));
	}else if(n=="deltay"){ untested();
		auto p = portPosition(1);
		return std::to_string(getY(p));
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
	}else if(n=="$vflip"){ untested();
		Symbol::setParameter(n, v);
		_scale = abs(_scale) * atoi(v.c_str());
	}else if(n=="$angle"){ untested();
		Symbol::setParameter(n, v);
		trace4("new angle", angle(), cx(), cy(), _scale);
	}else if(n=="$hflip"){ untested();
		Symbol::setParameter(n, v);
		int tmp = atoi(v.c_str());
		assert(tmp==1);
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
		findScaleAndAngle(p1);
	}else if(n=="deltay"){
		int V = atoi(v.c_str());
		auto p1 = pP1();
		p1.second = V;
		if(V){
			p1.first = 0;
		}else{
		}
		findScaleAndAngle(p1);
	}else if(n=="netname"){
		_has_netname = (v != "");
		_netname = v;
	}else{
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
rect_t Wire::bounding_rect() const
{ itested();
  int N = 5;
  // assert(_scale>0); oops??
  //
  return rect_t(pos_t(-N,-N),pos_t(N+abs(_scale),N));
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
		}else if(n->netLabel() == n2->netLabel()){
			// nothing to do
		}else{
			message(QucsMsgWarning, ("possible label conflict. not sure what to do in "
					+ n->netLabel() + " vs " + n2->netLabel()).c_str());
		}
		trace2("wire addededge", n->netLabel(), n2->netLabel());
	}else{
	}

	nm.addEdge(this, n);
	trace3("Wire::connect", i, n->degree(), degree());

	if(_has_netname){
		trace2("wire override netlabel", n->netLabel(), _netname);
		n->setNetLabel(_netname);
	}else{
	}
	return n;
}
// ----------------------------------------------------------------
Node* Wire::disconnectNode(unsigned i, NodeMap& nm)
{
	trace3("Wire::disconnect", label(), i, degree());
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
/*--------------------------------------------------------------------------*/
bool Wire::isInterior(pos_t const& p) const
{
	if(p == nodePosition(0)){
		return false;
	}else if(p == nodePosition(1)){itested();
		return false;
	}else{
		return isNet(p);
	}
}
/*--------------------------------------------------------------------------*/
bool Wire::isNet(pos_t const& p) const
{
	int x = getX(p);
	int y = getY(p);
	x -= cx();
	y -= cy();
	trace2("isNet", x2(), y2());
	// return in_order(pos_t(0, 0), p-position(), pP1());
	return in_order(0, x, x2()) && in_order(0, y, y2());
}
/*--------------------------------------------------------------------------*/
pos_t Wire::pP1() const
{
	pos_t p(abs(_scale), 0);
	assert(!(angle()%90));
	rotate_after_mirror a(angle(), (1-hflip())/2, (1-vflip())/2);
	auto pp = a.apply(p);
	trace2("nodePosition post", pp, center());
	return pp;
}
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
