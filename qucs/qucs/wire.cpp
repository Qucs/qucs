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

	void getCenter(int&, int&);

private: // Element
	Wire* clone() const override{ untested();
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
	pos_t portPosition(unsigned i) const{ itested();
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
	void updatePort();
	Wire* extendTowards(pos_t const&) const;

	pos_t pP1() const;
//		return _pp1;
//	}
//	void setP1(pos_t const& p){ untested();
//		_pp1 = p;
//	}

public: // FIXME, these are still around. (from element)
//	int x1() const { return _port0.x(); }
//	int y1() const { return _port0.y(); }
	int x2() const { return pP1().first; }
	int y2() const { return pP1().second; }

private:
	Port _port0;
	Port _port1;
	std::string nx, ny, delta;
	std::string _netname;

	int _scale;
}w;
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "Wire", &w);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
Wire::Wire() : Symbol(),
    _port0(), _port1(),
    _scale(1.)
{
	Symbol::setPosition(pos_t(0, 0)); // redundant?

	setTypeName("wire");
	setLabel("noname"); // BUG
}
/*--------------------------------------------------------------------------*/
Wire::Wire(Wire const& w)
  : Symbol(w), _port0(), _port1(), _scale(w._scale)
{ untested();
  setLabel(w.label());
}
/*--------------------------------------------------------------------------*/
Wire::Wire(pos_t const& p0, pos_t const& p1)
  : Symbol(),
	 _port0(),
    _port1(),
    _scale(1)
{ itested();

	Symbol::setPosition(p0);

	 pos_t pp1((p1 - p0).first, (p1 - p0).second),
	findScaleAndAngle(pp1);
	updatePort();

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
void Wire::findScaleAndAngle(pos_t p1)
{ untested();
  int x2 = getX(p1);
  int y2 = getY(p1);
  _scale = std::max(abs(x2), abs(y2));
  if(!_scale){ untested();
  }else if(x2 > 0){ untested();
    setAngle(0);
  }else if(x2<0){ untested();
    setAngle(180);
  }else if(y2>0){ untested();
    setAngle(270);
  }else if(y2<0){ untested();
    setAngle(90);
  }else{ untested();
  }
#ifndef NDEBUG
  trace2("dbg", angle(), _scale);
  auto x=x2;
  auto y=y2;
  updatePort();
  trace4("dbg same?", x, y, x2, y2);
  assert(x==x2);
  assert(y==y2);
#endif

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
#if 0
// places don't work like this.
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
#endif
/*--------------------------------------------------------------------------*/
Symbol* Wire::intersectPorts(Symbol const* s) const
{ untested();
	std::vector<pos_t> split;
	for(unsigned i=0; i<s->numPorts(); ++i){ untested();
		pos_t p = s->nodePosition(i);
		trace4("intersectPorts", i, s->nodePosition(i), s->label(), s->typeName());

		if(nodePosition(0) == p) {itested();
		}else if(nodePosition(1) == p) {itested();
		}else if(isInterior(p)) { untested();
			split.push_back(p);
		}
	}


	if(split.size()){ untested();
		trace2("intersectPorts", split.size(), split[0]);
		split.push_back(nodePosition(0));
		split.push_back(nodePosition(1));

		std::sort(split.begin(), split.end());
		Symbol *sckt = new SubcktBase(); // BUG: inherit.
		sckt->new_subckt();
		SchematicModel* m = sckt->subckt();

		// TODO: ordering bug probably here as well.
		m->pushBack(s->clone());

		auto pp = split.begin();
		auto next = pp;
		++next;
		while(next!=split.end()){ untested();
			m->pushBack(new Wire(*pp, *next));
			trace3("intersectPorts", *pp, *next, split.size());
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
{ untested();
	auto n0 = nodePosition(0);
	auto n1 = nodePosition(1);

	if(in_order(n0, other, n1)){ untested();
		return clone();
	}else if(in_order(other, n1, n0) && _port1->numPorts()==1){ untested();
		trace3("extend1", other, n1, n0);
		return new Wire(other, n0);
	}else if(in_order(other, n0, n1) && _port0->numPorts()==1){ untested();
		trace3("extend2",other, n0, n1);
		return new Wire(other, n1);
	}else{ untested();
		return nullptr;
	}
}
/*--------------------------------------------------------------------------*/
Symbol* Wire::newTee(Wire const* o) const
{ untested();
	assert((angle() - o->angle() )%180);
	pos_t t0 = nodePosition(0);
	pos_t t1 = nodePosition(1);
	pos_t split(0, 0);
	Symbol* s = nullptr;
	Symbol const* teew = o;

	trace2("interior?", o->nodePosition(0), o->nodePosition(1));
	if(isInterior(o->nodePosition(0))){ untested();
		s = new SubcktBase(); // BUG: inherit.
		split = o->nodePosition(0);
	}else if(isInterior(o->nodePosition(1))){ untested();
		s = new SubcktBase(); // BUG: inherit.
		split = o->nodePosition(1);
	}else if(o->isInterior(nodePosition(0))){ untested();
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
	}else{ untested();
	}

	if(s){ untested();
		trace1("building tee", split);
		s->new_subckt();
		SchematicModel* m = s->subckt();
		assert(m);

		// BUG BUG BUG. this depends on the order.
		m->pushBack(new Wire(t0, split));
		m->pushBack(teew->clone());
		m->pushBack(new Wire(t1, split));
	}else{ untested();
		trace1("no tee", split);
	}

	return s;
}
/*--------------------------------------------------------------------------*/
Symbol* Wire::newUnion(Symbol const* s) const
{ untested();
	trace3("Wire::newUnion(Symbol)", s->label(), nodePosition(0), nodePosition(1));
//	auto p = dynamic_cast<Place const*>(s);
	auto o = dynamic_cast<Wire const*>(s);
//	if(p){ untested();
//		return newPort(p);
//	}else
	if(o){ untested();
		trace2("Wire::newUnion(Wire)", o->nodePosition(0), o->nodePosition(1));
		trace2("Wire::newUnion(Wire)", _port0->degree(), _port1->degree());
		trace2("Wire::newUnion(Wire)", angle(), o->angle());
		
		if( (360 + angle() - o->angle() )%180 ){ untested();
			return newTee(o);
		}else if(isNet(o->nodePosition(0))){ untested();
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
QDialog* Wire::schematicWidget(QucsDoc* Doc) const
{ untested();
  trace0("Component::editElement");
  return new WireDialog(Doc); // memory leak?
}
/*--------------------------------------------------------------------------*/
void Wire::paint(ViewPainter *p) const
{itested();
  int x1 = 0; // Wire::x1();
  int y1 = 0; // Wire::y1();

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
void Wire::updatePort()
{ untested();
//  int x = dcos(angle()) * _scale;
//  int y = -dsin(angle()) * _scale;
  // setP1(pos_t(abs(_scale), 0));
}
/*--------------------------------------------------------------------------*/
std::string Wire::paramValue(std::string const& n) const
{ untested();
//  if(n=="$hflip"){ untested();
//    if (_scale<0){ untested();
//      incomplete();
//      return "-1";
//    }else{ untested();
//      return "1";
//    }
//  }else
	if(n=="symbol_scale"){ untested();
    return std::to_string(_scale);
//  }else if(n=="$vflip"){ untested();
//	  // vflip did not exist in legacy code.
//    return "1";
  }else if(n=="deltax"){ untested();
	  auto p = portPosition(1);
    return std::to_string(getX(p));
  }else if(n=="deltay"){ untested();
	  auto p = portPosition(1);
    return std::to_string(getY(p));
  }else if(n=="netname"){ untested();
    return _netname;
  }else{ untested();
    return Symbol::paramValue(n);
  }
}
// ----------------------------------------------------------------
void Wire::setParameter(std::string const& n, std::string const& v)
{ untested();
  if(n=="nx"){ untested();
    nx = v;
  }else if(n=="ny"){ untested();
    ny = v;
//  }else if(n=="$xposition"){ untested();
//    _cx = atoi(v.c_str());
//  }else if(n=="$yposition"){ untested();
//    _cy = atoi(v.c_str());
  }else if(n=="$vflip"){ untested();
    Symbol::setParameter(n, v);
    _scale = abs(_scale) * atoi(v.c_str());
    updatePort();
  }else if(n=="$angle"){ untested();
    Symbol::setParameter(n, v);
    updatePort();
    trace4("new angle", angle(), cx(), cy(), _scale);
  }else if(n=="$hflip"){ untested();
    Symbol::setParameter(n, v);
    int tmp = atoi(v.c_str());
    assert(tmp==1);
  }else if(n=="delta"){ untested();
    delta = v;
  }else if(n=="deltax"){ untested();
    int V = atoi(v.c_str());
    auto p1 = pP1();
    p1.first = V;
    if(V){ untested();
      p1.second = 0;
    }else{ untested();
    }
    findScaleAndAngle(p1);
    updatePort();
  }else if(n=="deltay"){ untested();
    int V = atoi(v.c_str());
    auto p1 = pP1();
    p1.second = V;
    if(V){ untested();
      p1.first = 0;
    }else{ untested();
    }
    findScaleAndAngle(p1);
    updatePort();
  }else if(n=="netname"){ untested();
    _netname = v;
  }else{ untested();
    Symbol::setParameter(n, v);
  }
}
// ----------------------------------------------------------------
unsigned Wire::numPorts() const
{ untested();
  return 2; /* really? */
}
// ----------------------------------------------------------------
void Wire::expand()
{ untested();
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
{ untested();
  assert(i<2);

  if(hasNet()){ untested();
  }else{ untested();
    nm.registerVertex(this);
  }
  assert(hasNet());

  Node* n = Symbol::connectNode(i, nm);

  if(Node* n2 = port((i+1)%2).value()){ untested();
    trace3("wire::connectnode", i, n, n2);
    trace2("wire addedge", n->netLabel(), n2->netLabel());

    // resolve conflicts more carefully...
    if(!n->hasNetLabel()){ untested();
      n->setNetLabel(n2->netLabel());
    }else if(!n2->hasNetLabel()){ untested();
      n2->setNetLabel(n->netLabel());
    }else{ untested();
      std::cerr << "possible label conflict. not sure what to do\n";
      std::cerr << n->netLabel() << " vs " << n2->netLabel() << "\n";
    }
    trace2("wire addededge", n->netLabel(), n2->netLabel());
    trace2("wire addededge2", n->netLabel(), n2->netLabel());
  }else{ untested();
  }

  nm.addEdge(this, n);
  trace3("Wire::connect", i, n->degree(), degree());

  if(_netname!=""){ untested();
    trace2("wire override netlabel", n->netLabel(), _netname);
    n->setNetLabel(_netname);
  }else{ untested();
  }
  return n;
}
// ----------------------------------------------------------------
Node* Wire::disconnectNode(unsigned i, NodeMap& nm)
{ itested();
  trace3("Wire::disconnect", label(), i, degree());
  assert(hasNet());
  Node* n = Symbol::disconnectNode(i, nm);
  trace3("Wire::disconnect", i, n->degree(), degree());

  //Conductor::removeEdge(n, nm);?
  nm.removeEdge(n, this);

  if(degree()){ itested();
  }else{ untested();
    nm.deregisterVertex(this);
  }

  return n;
}
/*--------------------------------------------------------------------------*/
bool Wire::isInterior(pos_t const& p) const
{ untested();
	if(p==nodePosition(0)){ untested();
		return false;
	}else if(p==nodePosition(1)){itested();
		return false;
	}else{ untested();
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
  trace2("isNet", x2(), y2());
  return in_order(0, x, x2()) && in_order(0, y, y2());
}
/*--------------------------------------------------------------------------*/
pos_t Wire::pP1() const
{ untested();
	pos_t p(abs(_scale), 0);
	assert(!(angle()%90));
	rotate_after_mirror a(angle(), (1-hflip())/2, (1-vflip())/2);
	auto pp = a.apply(p);
	trace2("nodePosition post", pp, center());
	return pp; //  + center();
}
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
