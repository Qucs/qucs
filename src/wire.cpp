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
#include "conductor.h"
#include "qucs_globals.h"
#include "geometry.h"
#include "element_list.h"
#include <QPainter>
#include "qt_compat.h" // geometry?
#include "l_compar.h"
#include "widget.h"

#include "viewpainter.h"
#include "platform.h"
#include "symbol.h"
#include "sckt_base.h"
/*--------------------------------------------------------------------------*/
class QPainter;
class QString;
/*--------------------------------------------------------------------------*/
static const QColor color(Qt::darkBlue);
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
using qucs::Conductor;
using qucs::Doc;
using qucs::ElementList;
using qucs::Widget;
using qucs::Symbol;
using qucs::SubcktBase;
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
	Widget* schematicWidget(Doc*) const override;
	void paint(ViewPainter*) const override;
	rect_t bounding_rect() const override;

	std::string dev_type() const override{ return "wire"; }

private: // Conductor
	SubcktBase* newUnion(const Symbol*) const override;
	bool isNet(pos_t const&) const override;

private:
//	Symbol* newPort(const Place*) const;
	SubcktBase* newTee(const Wire*) const;
	SubcktBase* intersectPorts(const Symbol*) const;
	bool isInterior(pos_t const&) const;

private: // Symbol
	index_t param_count() const override { return Symbol::param_count() + 3; }
	void set_param_by_name(std::string const& name, std::string const& value) override;
	std::string param_value_by_name(std::string const& name) const override;
	std::string param_name(index_t i) const override;
	std::string param_value(index_t i) const override;
	bool param_is_printable(index_t i) const override;

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
	void set_port_by_index(index_t, std::string const&);

private: // Node/Net stuff
	void connectNode(index_t idx);
	void disconnectNode(index_t idx);

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
	SubcktBase* extendTowards(pos_t const&) const;

	pos_t pP1() const;

public: // FIXME, these are still around. (from element)
	int x2() const { return pP1().first; }
	int y2() const { return pP1().second; }

private:
	Port _port0;
	Port _port1;
	int _tx{0};
	int _ty{0};
  	std::string delta;
	int _scale;
	std::string _netname;
	bool _has_netname;
}w;
static Dispatcher<Symbol>::INSTALL p(&qucs::symbol_dispatcher, "Wire", &w);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
Wire::Wire() : Symbol(),
    _port0(), _port1(),
    _scale(1.),
    _has_netname(false)
{
	assert(position() == pos_t(0, 0));
	set_label("noname"); // BUG
}
/*--------------------------------------------------------------------------*/
Wire::Wire(Wire const& w)
  : Symbol(w),
    _port0(),
    _port1(),
	 _tx(w._tx),
	 _ty(w._ty),
    _scale(w._scale),
    _netname(w._netname),
    _has_netname(w._has_netname)
{
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
	set_label("noname");
	findScaleAndAngle(pp1);

	// assert(_scale>0); // for now?
	assert(p0 == nodePosition(0));
	assert(p1 == nodePosition(1));
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
	set_label("wire_" + std::to_string(n0.first) +
			":"+ std::to_string(n0.second) +
			"_"+ std::to_string(n1.first) +
			":"+ std::to_string(n1.second));
#endif
}
/*--------------------------------------------------------------------------*/
SubcktBase* Wire::intersectPorts(Symbol const* s) const
{
	std::vector<pos_t> split;
	for(unsigned i=0; i<s->numPorts(); ++i){
		pos_t p = s->nodePosition(i);
		trace4("intersectPorts", i, s->nodePosition(i), s->label(), s->dev_type());

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
		Component *c = qucs::device_dispatcher.clone("subckt_proto");
		auto sckt = prechecked_cast<SubcktBase*>(c);
		assert(sckt);
		sckt->new_subckt();
		ElementList* m = sckt->subckt();

		// TODO: ordering bug probably here as well.
		m->push_back(s->clone());

		auto pp = split.begin();
		auto next = pp;
		++next;
		while(next!=split.end()){
			m->push_back(new Wire(*pp, *next));
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
SubcktBase* Wire::extendTowards(pos_t const& other) const
{
	trace3("Wire::extendTowards", getX(other), getY(other), short_label());
	auto n0 = nodePosition(0);
	auto n1 = nodePosition(1);

//	trace2("extendTowards", _port0.value(), _port1.value());

	// HACK
//	assert(_port0.isConnected());
//	assert(_port1.isConnected());
	unsigned np0 = 2; // hack
	if(_port0.isConnected()){
		np0 = _port0->numPorts();
	}else{ untested();
	}
	unsigned np1 = 2; // hack
	if(_port1.isConnected()){
		np1 = _port1->numPorts();
	}else{ untested();
	}


	// trace2("extendTowards", _port0->numPorts(), _port1->numPorts());
	Wire* w = nullptr;

	if(in_order(n0, other, n1)){
		w = clone();
	}else if(in_order(other, n1, n0) && np1==2){
		trace3("extend1", other, n1, n0);
		w = new Wire(other, n0);
	}else if(in_order(other, n0, n1) && np0==2){ untested();
		trace3("extend2",other, n0, n1);
		w = new Wire(other, n1);
	}else{
	}

	if(w){
		Component *s = qucs::device_dispatcher.clone("subckt_proto");
		auto sckt = prechecked_cast<SubcktBase*>(s);
		assert(sckt);
		sckt->new_subckt();
		assert(sckt->subckt());
		sckt->subckt()->push_back(w);
		return sckt;
	}else{
		return nullptr;
	}
}
/*--------------------------------------------------------------------------*/
SubcktBase* Wire::newTee(Wire const* o) const
{
	assert((angle() - o->angle() )%180);
	pos_t t0 = nodePosition(0);
	pos_t t1 = nodePosition(1);
	pos_t split(0, 0);
	SubcktBase* s = nullptr;
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
		ElementList* m = s->subckt();
		assert(m);

		// BUG BUG BUG. this depends on the order.
		m->push_back(new Wire(t0, split));
		m->push_back(teew->clone());
		m->push_back(new Wire(t1, split));
	}else{
		trace1("no tee", split);
	}

	return s;
}
/*--------------------------------------------------------------------------*/
SubcktBase* Wire::newUnion(Symbol const* s) const
{
	trace3("Wire::newUnion(Symbol)", s->label(), nodePosition(0), nodePosition(1));
//	auto p = dynamic_cast<Place const*>(s);
	auto o = dynamic_cast<Wire const*>(s);
//	if(p){ untested();
//		return newPort(p);
//	}else
	SubcktBase* ret = nullptr;
	if(o){
		trace2("Wire::newUnion(Wire)", o->nodePosition(0), o->nodePosition(1));
//		trace2("Wire::newUnion(Wire)", _port0->degree(), _port1->degree());
		trace2("Wire::newUnion(Wire)", angle(), o->angle());
		
		if( (360 + angle() - o->angle() )%180 ){
			ret = newTee(o);
		}else if(isNet(o->nodePosition(0))){
			ret = extendTowards(o->nodePosition(1));
		}else if(isNet(o->nodePosition(1))){ untested();
			ret = extendTowards(o->nodePosition(0));
		}else{ untested();
		}
	}else{
		ret = intersectPorts(s);
	}
	if(ret){
		trace1("newUnion", ret->subckt()->size());
	}else{
	}
	return ret;
}
/*--------------------------------------------------------------------------*/
// NOT HERE ? //
Widget* Wire::schematicWidget(Doc* Doc) const
{ untested();
	trace0("Wire::schematicWidget");

	Object const* w = qucs::widget_dispatcher["WireDialog"];
	auto ww = dynamic_cast<Widget const*>(w);

	assert(ww);
	Widget* clone = ww->clone();
	assert(clone);
	return clone;
}
/*--------------------------------------------------------------------------*/
void Wire::paint(ViewPainter *p) const
{itested();
	//  int x1 = 0; // Wire::x1();
	//  int y1 = 0; // Wire::y1();

	{ itested();
		p->setPen(QPen(color, 2));
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
    Label->set_label(Name_);
  }
}
#endif
/*--------------------------------------------------------------------------*/
std::string Wire::param_name(index_t i) const
{
	switch (int(Wire::param_count()) - 1 - i) {
	case 0:
		return "$tx";
	case 1:
		return "$ty";
	case 2:
		return "netname";
	default:
		return Symbol::param_name(i);
	}
}
/*--------------------------------------------------------------------------*/
std::string Wire::param_value(index_t i) const
{
	switch (int(Wire::param_count()) - 1 - i) {
	case 0:
		return std::to_string(_tx);
	case 1:
		return std::to_string(_ty);
	case 2:
		return _netname;
	default:
		return Symbol::param_value(i);
	}
}
/*--------------------------------------------------------------------------*/
bool Wire::param_is_printable(index_t i) const
{
	switch (int(Wire::param_count()) - 1 - i) {
	case 0:
	case 1:
	case 2:
		return _has_netname;
	default:
		return Symbol::param_is_printable(i);
	}
}
/*--------------------------------------------------------------------------*/
std::string Wire::param_value_by_name(std::string const& n) const
{
	if(n=="symbol_scale"){ untested();
		return std::to_string(_scale);
	}else if(n=="$tx"){ untested();
		return std::to_string(_tx);
	}else if(n=="$ty"){ untested();
		return std::to_string(_ty);
	}else if(n=="deltax"){ untested();
		auto p = portPosition(1);
		return std::to_string(getX(p));
	}else if(n=="deltay"){ untested();
		auto p = portPosition(1);
		return std::to_string(getY(p));
	}else if(n=="$param_display"){ untested();
		return std::to_string(31*_has_netname);
	}else if(n=="$param_hidename"){ untested();
		return std::to_string(31);
	}else if(n=="netname"){ untested();
		return _netname;
	}else{
		return Symbol::param_value_by_name(n);
	}
}
// ----------------------------------------------------------------
void Wire::set_param_by_name(std::string const& n, std::string const& v)
{
	if(n=="$tx"){itested();
		_tx = atoi(v.c_str());
	}else if(n=="$ty"){itested();
		_ty = atoi(v.c_str());
	}else if(n=="$vflip"){ untested();
		Symbol::set_param_by_name(n, v);
		_scale = abs(_scale) * atoi(v.c_str());
	}else if(n=="$angle"){ untested();
		Symbol::set_param_by_name(n, v);
		trace4("new angle", angle(), cx(), cy(), _scale);
	}else if(n=="$hflip"){ untested();
		Symbol::set_param_by_name(n, v);
		int tmp = atoi(v.c_str());
		assert(tmp==1);
	}else if(n=="delta"){itested();
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
	}else if(n=="netname"){itested();
		_has_netname = (v != "");
		_netname = v;
	}else{
		Symbol::set_param_by_name(n, v);
	}
}
// ----------------------------------------------------------------
unsigned Wire::numPorts() const
{
  return 2; /* really? */
}
// ----------------------------------------------------------------
void Wire::expand()
{itested();
  // stash NodeLabels as subdevices.
  // just not sure where exactly.
  if (_netname != ""){itested();
    new_subckt();
    Symbol* n = qucs::symbol_dispatcher.clone("NodeLabel");
    n->set_label(_netname);
    // n->setParameters("$xposition");
    // n->setParameters("$yposition");
    // n->setParameters("dx", 5);
    // n->setParameters("dy", 5);
    assert(n);
    trace1("Wire::expand", _netname);
    subckt()->push_back(n);
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
void Wire::set_port_by_index(index_t i, std::string const& value)
{
	assert(i<2);
	if(port_value(i)=="(null)"){
	}else if(port_value(i)==""){ untested();
	}else if(value==""){
		disconnectNode(i);
	}else{ untested();
	}

	Symbol::set_port_by_index(i, value);

	if(value==""){
		assert(port_value(i)=="(null)");
	}else{
		trace2("wire::spbi", i, value);
		connectNode(i);
	}
}
// ----------------------------------------------------------------
void Wire::connectNode(index_t i)
{
	assert(scope());
	assert(scope()->nodes());
	auto& nm = *scope()->nodes();

	if(hasNet()){
	}else{
		nm.registerVertex(this);
	}
	assert(hasNet());
	Node* n = port(i).value();

	if(Node* n2 = port((i+1)%2).value()){
		trace3("wire::connectnode", i, n, n2);
		trace2("wire addedge", n->netLabel(), n2->netLabel());

		// resolve conflicts more carefully...
		if(!n->hasNetLabel()){
			n->setNetLabel(n2->netLabel());
		}else if(!n2->hasNetLabel()){itested();
			n2->setNetLabel(n->netLabel());
		}else if(n->netLabel() == n2->netLabel()){
			// nothing to do
		}else{
			message(qucs::MsgWarning, ("possible label conflict. not sure what to do in "
					+ n->netLabel() + " vs " + n2->netLabel()).c_str());
		}
		trace2("wire addededge", n->netLabel(), n2->netLabel());
	}else{
	}

	nm.addEdge(this, n);
	trace3("Wire::connect added edge", i, n->degree(), degree());

	if(_has_netname){itested();
		trace2("wire override netlabel", n->netLabel(), _netname);
		n->setNetLabel(_netname);
	}else{
	}
}
// ----------------------------------------------------------------
void Wire::disconnectNode(index_t i)
{
	assert(scope());
	assert(scope()->nodes());
	auto& nm = *scope()->nodes();

	trace3("Wire::disconnect", label(), i, degree());
	if(!hasNet()){ untested();
		unreachable(); // happens after bogus connect calls
		return;
	}else{
	}
	Node* n = port(i).value();
	assert(n);
	trace4("Wire::disconnect", i, n->degree(), degree(), n->netLabel());

	nm.removeEdge(n, this);

	if(degree()){
	}else{
		nm.deregisterVertex(this);
	}
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
