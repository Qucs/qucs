/***************************************************************************
                                 node.h
                                --------
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

class Node;

// TODO: move cruft to components
class Port {
private:
  // Port(Port const&){unreachable();}

public:
  Port(Port const&) = default; // BUG
 // Port() {};
  explicit Port(int _x, int _y, bool _avail=true)
	  : x(_x), y(_y), avail(_avail), Connection(nullptr)
  {
    Type=""; Name=""; };

public:
  int x_()const{return x;}
  int y_()const{return y;}
  std::pair<int, int> position() const { return std::make_pair(x,y); } // BUG
  void setPosition(int x_, int y_) { x=x_; y=y_; }

  // This makes Port behave like a pointer (and that's what it will be).
  Node const* operator->() const {return Connection;}

private:
  friend class Wire; // HACK
  int   x, y; // BUG. nodes have coordinates.
public:
  bool  avail;
  QString Type; // BUG. type is "Port".
  QString Name; // BUG?

private:
  Node *Connection;
public:
  QString const& netLabel() const;

public: // perhaps not here
  Node* value() const{return Connection;}
  bool connected() const{return Connection;}
  QString const& name() const{return Name;}

private:
  // "connections" are bidirectional, this is taken care of in Symbol::(dis)connectNode
  friend class Symbol;
  void connect(Node* n) { assert(!Connection); Connection = n; }
  void disconnect() { assert(Connection); Connection = NULL; }
};


#endif
