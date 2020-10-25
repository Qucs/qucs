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
/*--------------------------------------------------------------------------*/
class Node;
/*--------------------------------------------------------------------------*/
class Port {
private:
public:
  Port(Port const& p) : _node(nullptr) {assert(!p._node);}
  explicit Port() : _node(nullptr) {}
  virtual ~Port(){ assert(!_node);}

public: // This makes Port behave like a pointer (and that's what it will be).
  Node const* operator->() const {return _node;}

public:
  QString const& netLabel() const;

public: // perhaps not here
  Node* value() const{return _node;}
  bool isConnected() const{return _node;}

private:
  // "connections" are bidirectional, this is taken care of in Symbol::(dis)connectNode
  friend class Symbol;
  virtual void connect(Node*);
  virtual void disconnect(Node*);

private:
  Node *_node;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
