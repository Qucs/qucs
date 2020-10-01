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
class Node : public Element /* Object? */ {
public:
	typedef std::list<Element /*const?*/ *> element_list_t;

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
#if 0 /// hmmm... pair<begin end>?
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
  QString const& netLabel() const;
  void setNetLabel(QString const&);

  bool hasLabel() const;
  bool hasNetLabel() const;
private: // element (BUG?)
  void paint(ViewPainter*) const{ }

public: // obsolete
//  bool  getSelected(int, int);
//  QRectF boundingRect() const override;

private:
  unsigned _degree;

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
public: // internal. here?
  bool hasNet() const { return _net; }
  Net* net() {assert(_net); return _net; }
  Net* newNet(NetList&);
  Net const* net() const {assert(_net);  return _net; }
  void setNet(Net* x){_net = x; }
  void attachNet(Net* x);
  void detachNet(Net* x);
  bool visited(unsigned lvl) const {return lvl == _visit;}
  void visit(unsigned lvl){ _visit = lvl; }

private:
  // BUG: also stored in port?
  const std::pair<int, int> _position;
private:
  Net* _net;
  unsigned _visit; // keep track of what has been done
};
/* ---------------------------------------------------------- */
class AdjNodeIterator{
public:
	typedef std::list<Element*>::iterator elt_iter;
	typedef std::list<Node*>::iterator node_iter;
public:
	AdjNodeIterator(elt_iter b, elt_iter e);
public:
	AdjNodeIterator(AdjNodeIterator const& o)
		: _wire(o._wire),
		  _wend(o._wend),
		  _node(o._node),
		  _nend(o._nend){
			  assert(o.is_valid());
			  assert(is_valid());
	}
public:
	Node* operator*();
	
	AdjNodeIterator& operator++();
	bool operator==(AdjNodeIterator const& o);
	bool operator!=(AdjNodeIterator const& o){
		return !(*this == o);
	}
private:
	void next();
	bool is_valid() const;
	void skip();
private:
	elt_iter _wire;
	elt_iter _wend;
	node_iter _node;
	node_iter _nend;
	
public:
	friend class AdjNodeRange;
};
/* ---------------------------------------------------------- */
// could use pair?
class AdjNodeRange{
public:
	explicit AdjNodeRange(Node& c);
public:
	AdjNodeRange(AdjNodeRange const&o)
		: _begin(o._begin),
		  _end(o._end){
		assert(_begin.is_valid());
		assert(_end.is_valid());
	}
public:
	AdjNodeIterator begin(){
		return _begin;
	}
	AdjNodeIterator end(){
		return _end;
	}

private:
	AdjNodeIterator _begin;
	AdjNodeIterator _end;
};
/* ---------------------------------------------------------- */
#endif
