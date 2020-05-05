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

class ViewPainter;
class NodeMap;

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
  void setName(QString const& x){
	  incomplete();
//	  setLabel(x);
  }

  // BUG
  void setName(const QString&, const QString&, int x_=0, int y_=0);
  int cx() const{ return Element::cx_(); }
  int cy() const{ return Element::cy_(); }

  std::pair<int, int> position() const{
	  return _position;
  }

  void setState(int i){
	  State |= i;
  }
  bool hasState(int i) const{
	  return State & i;
  }

public:
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
private:
  const std::pair<int, int> _position;
};


#endif
