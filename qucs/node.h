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
class Node : public Element /* Object? */, public Conductor {
public:

private:
  Node(Node const&) = delete;
  Node(Node const&&) = delete;
private: // managed by NodeMap
  friend class NodeMap;
  explicit Node(std::pair<int, int>);
  ~Node();
public:
  Element* clone()const{
	  assert(false); // nodes are organised by NodeMap
	  return NULL; // new Node(*this);
  }

#if 0
  AdjNodeRange neighbours();
  void connectionsAppend(Element*){ // "connect"?
	  ++_degree;
  }
  void connectionsRemove(Element const*){
	  assert(_degree);
	  --_degree;
  }
  unsigned degree() const{
	  return _degree;
  }
//  Element* firstConnection() const{
//	  return Connections.getFirst();
//  }
//  Element* lastConnection() const{
//	  return Connections.getLast();
//  }
  void appendConnection(Element*){
	  ++_degree;
  }
  void prependConnection(Element*){
	  ++_degree;
  }
#endif
  void inc_ports(){ ++_ports; }
  void dec_ports(){ assert(_ports); --_ports; }
  bool has_ports() const{ return _ports; }

#if 0 /// hmmm... pair<begin end>?
	typedef std::list<Element /*const?*/ *> element_list_t;
  element_list_t const& connections() const{
	  return _conn;
  }
  element_list_t::iterator connectionsBegin() override{
	  return _conn.begin();
  }
  element_list_t::iterator connectionsEnd() override{
	  return _conn.end();
  }
#endif
// /  void setName(QString const&);
// /	  incomplete();
// ///	  setLabel(x);
// /  }

  // BUG
  void setName(const QString&, const QString&, int x_=0, int y_=0);
  int cx() const{ return Element::_cx; }
  int cy() const{ return Element::_cy; }

  std::pair<int, int> const& position() const{
	  return _position;
  }

  void setState(int i){
	  State |= i;
  }
  bool hasState(int i) const{
	  return State & i;
  }

public:
  // QString const& netLabel() const;
  // void setNetLabel(QString const&);
  // bool hasLabel() const;
  // bool hasNetLabel() const;

private: // element (BUG?)
  void paint(ViewPainter*) const{ }

public: // obsolete
//  bool  getSelected(int, int);
//  QRectF boundingRect() const override;

private:
  QString DType; // type of node (used by digital files)

public: // BUG
  int State;	 // remember some things during some operations, BUG


public: // protected coordinate abuse
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

private:
  // BUG: also stored in port?
  const std::pair<int, int> _position;
  unsigned _ports; // number of ports connecting to this node
};
/* ---------------------------------------------------------- */
/* ---------------------------------------------------------- */
#endif
