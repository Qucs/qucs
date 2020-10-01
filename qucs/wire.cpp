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

// ----------------------------------------------------------------
Wire::Wire()
   : _port0(0, 0), _port1(1, 0), _angle(0), _scale(1.)
{ untested();
  _cx = 0;
  _cy = 0;

  setType("wire");
  setLabel("noname"); // BUG
}
// ----------------------------------------------------------------
Wire::Wire(Wire const& w)
  : Symbol(w), _port0(w._port0), _port1(w._port1),
   _angle(w._angle), _scale(w._scale)
{ untested();
  setLabel(w.label());
}
// ----------------------------------------------------------------
void Wire::findScaleAndAngle()
{ untested();
  _scale = std::max(abs(x2()), abs(y2()));
  if(x2() > 0){ untested();
    _angle = 0;
  }else if(x2()<0){ untested();
    _angle = 2;
  }else if(y1()>0){ untested();
    _angle = 3;
  }else{ untested();
    _angle = 1;
  }
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
{ untested();
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
// ----------------------------------------------------------------
void Wire::getCenter(int& x, int& y)
{ untested();
  unreachable();
  assert(false);
  x = (x1()+x2()) >> 1;
  y = (y1()+y2()) >> 1;
}
// ----------------------------------------------------------------
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
{ untested();
  int x1 = Wire::x1();
  int x2 = Wire::x2();
  int y1 = Wire::y1();
  int y2 = Wire::y2();

  { itested();
    p->setPen(QPen(Qt::darkBlue,2));
    p->drawLine(x1, y1, x2, y2);
    // p->drawEllipse(x1-2, y1-2, 4, 4);
  }

  if(degree()){
    p->setPen(QPen(Qt::green,2));
   p->drawEllipse(x2/2-2, y2/2-2, 4, 4);
  }

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

  assert(x1() == 0);
  assert(y1() == 0);
  x2() = dcos(_angle) * _scale;
  y2() = dsin(_angle) * _scale;
}
// ----------------------------------------------------------------
std::string Wire::getParameter(std::string const& n) const
{ untested();
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
  }else if(n=="deltax"){ untested();
    return std::to_string(x2());
  }else if(n=="deltay"){ untested();
    return std::to_string(y2());
  }else{ untested();
    return Symbol::getParameter(n);
  }
}

// ----------------------------------------------------------------
void Wire::setParameter(std::string const& n, std::string const& v)
{ untested();
  if(n=="nx"){ untested();
    nx = v;
  }else if(n=="ny"){ untested();
    ny = v;
  }else if(n=="$xposition"){ untested();
    _cx = atoi(v.c_str());
  }else if(n=="$yposition"){ untested();
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
  }else if(n=="delta"){ untested();
    delta = v;
  }else if(n=="deltax"){ untested();
    int V = atoi(v.c_str());
    x2() = V;
    if(V){ untested();
      y2() = 0;
    }else{ untested();
    }
    findScaleAndAngle();
  }else if(n=="deltay"){ untested();
    int V = atoi(v.c_str());
    y2() = V;
    if(V){ untested();
      x2() = 0;
    }else{ untested();
    }
    findScaleAndAngle();
  }else if(n=="netname"){ untested();
    _netname = v;
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

  if(_angle==0 || _angle==1){ itested();
    return QRectF(-5, -5, x2()-x1()+N, y2()-y1()+N);
  }else{ itested();
    return QRectF(x2()-5, y2()-5, -x2()+x1()+N, -y2()+y1()+N);
  }
}
// ----------------------------------------------------------------
#if 0
std::list<Node*>::iterator Wire::connectionsBegin()
{ untested();
  trace1("DBG", _node_hack.size());
  assert(_node_hack.size()==2);
  auto a = _node_hack.begin();
  *a = _port0.value();
  ++a;
  *a = _port1.value();
  trace2("wireconnections", _port0.value(), _port1.value());

  return _node_hack.begin();
}
#endif
// ----------------------------------------------------------------
Node* Wire::connectNode(unsigned i, NodeMap& nm)
{ untested();
  assert(i<2);

  if(hasNet()){
  }else{
    nm.registerVertex(this);
  }
  assert(hasNet());

  Node* n = Symbol::connectNode(i, nm); // maybe use this?

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

  // addEdge(n, nm);?
  nm.addEdge(this, n);
  trace3("Wire::connect", i, n->degree(), degree());

  //n->connectionsAppend(this);

  if(_netname!=""){ untested();
    trace2("wire override netlabel", n->netLabel(), _netname);
    n->setNetLabel(QString::fromStdString(_netname));
  }else{ untested();
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
// ----------------------------------------------------------------
#if 0
Net* Wire::net()
{ untested();
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
#endif
// ----------------------------------------------------------------
// vim:ts=8:sw=2:et
