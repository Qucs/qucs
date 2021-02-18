/***************************************************************************
    copyright            : (C) 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// Node in a circuit
// A Port connects to a Node, Connected nodes form a Net

#ifndef QUCS_NODE_H
#define QUCS_NODE_H

#include "conductor.h"
#include "platform.h"
#include "object.h"
/*--------------------------------------------------------------------------*/
class ViewPainter;
class NodeMap;
class NetList;
class AdjNodeRange;
/*--------------------------------------------------------------------------*/
typedef unsigned index_t;
/*--------------------------------------------------------------------------*/
class Node : public Object, public Conductor {
private:
  Node(Node const&) = delete;
  Node(Node const&&) = delete;

private: // managed by NodeMap
  friend class NodeMap;
  explicit Node(std::string const& name, index_t n);
  ~Node();

public:
  void inc_ports(){ ++_ports; }
  void dec_ports(){ assert(_ports); --_ports; }

public:
  bool hasPorts() const{ return _ports; }
  unsigned numPorts() const{ return _ports; }

private:
  const index_t _index;
  unsigned _ports; // number of ports connecting to this node
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
