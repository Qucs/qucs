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
/*--------------------------------------------------------------------------*/
#include <string>
#include <assert.h>
/*--------------------------------------------------------------------------*/
class Node;
class Element;
class pos_t;
/*--------------------------------------------------------------------------*/
class Port {
private:
public:
  Port(Port const* p) : _node(p->_node) {assert(p);}
  Port(Port const& p) : _node(nullptr) {assert(!p._node);}
  explicit Port() : _node(nullptr) {}
  virtual ~Port();

public: // This makes Port behave like a pointer (and that's what it is).
  Node const* operator->() const {return _node;}

public:
  std::string const& netLabel() const;
  std::string const& nodeLabel() const;

public: // perhaps not here
  Node* value() const{return _node;}
  bool isConnected() const{return _node;} // has_node()?
  void new_node(const std::string&, const Element*);
  void new_node(const pos_t&, const Element*);

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
