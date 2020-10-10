/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2018, 2020 Felix Salfelder / QUCS team
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_PORT_H
#define QUCS_PORT_H

#include <QString> // BUG??
#include "trace.h"
#include "geometry.h"

class Node;

// TODO: move cruft to components
class Port {
private:
  // Port(Port const&){unreachable();}

public:
  Port(Port const&);
 // Port() {};
  explicit Port(int x, int y, bool _avail=true)
	  : _p(x, y),
	    avail(_avail), _node(nullptr)
  {
    Type=""; Name=""; };

public:
  int x_()const{return getX(_p);}
  int y_()const{return getY(_p);}
  pos_t const& position() const { return _p; }
  void setPosition(int x, int y) { _p=pos_t(x,y); }
  void setPosition(pos_t p) { _p=p; }

  // This makes Port behave like a pointer (and that's what it will be).
  Node const* operator->() const {return _node;}

  int x() const{ return getX(_p); }
  int y() const{ return getY(_p); }

public: // bug
//  int& x(){ return _x; }
//  int& y(){ return _y; }

public:
  QString const& netLabel() const;

public: // perhaps not here
  Node* value() const{return _node;}
  bool isConnected() const{return _node;}
  QString const& name() const{return Name;}

private:
  // "connections" are bidirectional, this is taken care of in Symbol::(dis)connectNode
  friend class Symbol;
  void connect(Node*);
  void disconnect(Node*);

private:
  pos_t _p;

public:
  bool  avail;
  QString Type; // BUG. type is "Port".
  QString Name; // BUG?

private:
  Node *_node;

};


#endif
