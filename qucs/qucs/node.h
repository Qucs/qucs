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
//#include "wire.h" // bug

class ViewPainter;
class NodeMap;
class NetList;
class AdjNodeRange;

class Node : public Conductor, public Element /* Object? */ {
private:
  Node(Node const&){ unreachable(); }
  Node(Node const&&){ unreachable(); }
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
  void connectionsAppend(Element* e){ // "connect"?
	  Connections.append(e);
  }
  void connectionsRemove(Element const* ee){
     Element* e=const_cast<Element*>(ee);
	  Connections.removeRef(e);
  }
  unsigned connectionsCount() const{
	  return Connections.count();
  }
  Element* firstConnection(){
	  return Connections.first();
  }
  Element* lastConnection(){
	  return Connections.last();
  }
  void appendConnection(Element* e){
	  return Connections.append(e);
  }
  void prependConnection(Element* e){
	  return Connections.prepend(e);
  }
#if 1 /// hmmm... pair<begin end>?
  Q3PtrList<Element> const& connections() const{
	  return Connections;
  }
  std::list<Element*>::iterator connectionsBegin(){
	  return Connections.begin();
  }
  std::list<Element*>::iterator connectionsEnd(){
	  return Connections.end();
  }
#endif
// /  void setName(QString const&);
// /	  incomplete();
// ///	  setLabel(x);
// /  }

  // BUG
  void setName(const QString&, const QString&, int x_=0, int y_=0);
  int cx() const{ return Element::cx_(); }
  int cy() const{ return Element::cy_(); }

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

  bool hasLabel() const{
	  // possibly "one of the connections is a label?"
	  incomplete();
	  return false;
  }

private: // element (BUG?)
  void paint(ViewPainter*) const{ }

public: // obsolete
  bool  getSelected(int, int);

private: //Element overrides
  void  paint(ViewPainter*);

private: // BUG. does weird reverse iteration
  Q3PtrList<Element> Connections;

private:
  // QString Name;  //Element?
  QString DType; // type of node (used by digital files)

public: // BUG
  int State;	 // remember some things during some operations, BUG

  QRectF boundingRect() const;

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
  Net* getNet() {assert(_net); return _net; }
  Net* newNet(NetList&);
  Net const* getNet() const {assert(_net);  return _net; }
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
