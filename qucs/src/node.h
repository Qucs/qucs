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
// CHECK: hierarchy
// // maybe: Place : Conductor?
class Node : public Object, public Conductor {
private:
  Node(Node const&) = delete;
  Node(Node const&&) = delete;

private: // managed by NodeMap
  friend class NodeMap;
  explicit Node(pos_t);
  ~Node();

public:
  void inc_ports(){ ++_ports; }
  void dec_ports(){ assert(_ports); --_ports; }
  Object::Type type() const override {return Object::Type::Node;}

  // possibly wrong.
//  void connect(Port&);
//  void disconnect(Port&);

public:
  bool hasPorts() const{ return _ports; }
  unsigned numPorts() const{ return _ports; }

  // BUG
//  void setName(const QString&, const QString&, int x_=0, int y_=0);

  pos_t const& position() const{
	  return _position;
  }
  bool isNet(pos_t const&) const override;

private:
  // QString DType; // type of node (used by digital files)

public: // protected coordinate abuse

private:
  const pos_t _position;
  unsigned _ports; // number of ports connecting to this node
};
/* ---------------------------------------------------------- */
/* ---------------------------------------------------------- */
#endif
