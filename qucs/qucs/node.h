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
#include "qt_compat.h"
#include "platform.h"

class ViewPainter;
class NodeMap;
class NetList;
class AdjNodeRange;

// TODO: fix hierarchy
// // maybe: Place : Conductor?
class Node : public Object, public Conductor {
public:

private:
  Node(Node const&) = delete;
  Node(Node const&&) = delete;
private: // managed by NodeMap
  friend class NodeMap;
  explicit Node(std::pair<int, int>);
  ~Node();
public:

  void inc_ports(){ ++_ports; }
  void dec_ports(){ assert(_ports); --_ports; }
  bool has_ports() const{ return _ports; }

  // BUG
  void setName(const QString&, const QString&, int x_=0, int y_=0);

  std::pair<int, int> const& position() const{
	  return _position;
  }

  void setState(int i){
	  State |= i;
  }
  bool hasState(int i) const{
	  return State & i;
  }

private:
  // QString DType; // type of node (used by digital files)

public: // protected coordinate abuse
#if 0
  void markUnChecked(){
	  y1=0;
  }
  void markChecked(){
	  y1=1;
  }
  void reset_something(){
	  x1=0;
  }
  void set_something(int x){
	  x1|=x;
  }
#endif

private:
  const std::pair<int, int> _position;
  unsigned _ports; // number of ports connecting to this node
};
/* ---------------------------------------------------------- */
/* ---------------------------------------------------------- */
#endif
