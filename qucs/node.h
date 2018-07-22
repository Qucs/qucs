/***************************************************************************
                                 node.h
                                --------
    begin                : Sat Sep 20 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NODE_H
#define NODE_H

#include "element.h"
#include "qt_compat.h"

class ViewPainter;


class Node : public Conductor {
public:
  Node(int, int);
  ~Node();
  Element* clone()const{
	  return new Node(*this);
  }

  void connectionsAppend(Element* e){
	  Connections.append(e);
  }
  void connectionsRemove(Element* e){
	  Connections.removeRef(e);
  }
  unsigned connectionsCount() const{
	  return Connections.count();
  }
  Q3PtrList<Element> const& connections() const{
	  return Connections;
  }
  void setName(QString const& x){
//	  // BUG
	  Element::Name = x;
  }

  // BUG
  void setName(const QString&, const QString&, int x_=0, int y_=0);
  // QString const& name() const{ return Name; }

  void setState(int i){
	  State |= i;
  }
  bool hasState(int i) const{
	  return State & i;
  }

public: // obsolete
  bool  getSelected(int, int);

private: //Element overrides
  void  paint(ViewPainter*);

public: // BUG. does weird reverse iteration
  Q3PtrList<Element> Connections;

private:
  // QString Name;  //Element?
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
};

#endif
