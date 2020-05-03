/***************************************************************************
                                  wire.h
                                 --------
    copyright            : (C) 2003 by Michael Margraf
                               2018 Felix Salfelder / QUCS
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef WIRE_H
#define WIRE_H

#ifndef UNTANGLE_QT // BUG
# include "viewpainter.h"
#endif
#include "conductor.h"
#include "components/component.h"    // because of struct Port
#include "wirelabel.h"

class QPainter;
class QString;


class Wire : public Conductor {
public:
  Wire(int _x1=0, int _y1=0, int _x2=0, int _y2=0, Node *n1=0, Node *n2=0);
  ~Wire();

  Element* clone() const{
	  incomplete();
	  return nullptr;
  }
  Component* newOne(){
	  incomplete();
	  return nullptr;
  }

  void paint(ViewPainter*) const;
  void paintScheme(QPainter*) const;
  void setCenter(int, int, bool relative=false);
  void getCenter(int&, int&);
  bool getSelected(int, int);
  void setName(const QString&, const QString&, int delta_=0, int x_=0, int y_=0);

  void    rotate();
  QString save();

// private: not yet
  bool    obsolete_load(const QString&);

public:
  bool    isHorizontal() const { return (y1 == y2);}
  QRectF boundingRect() const;

public:
  void setPortByIndex(unsigned idx, Node* n){
	  assert(idx<2);
	  auto a=Ports.begin();
	  if(idx){
		  ++a;
	  }else{
	  }
	  *a = n;
  }
  Node* portByIndex(unsigned idx){
	  assert(idx<2);
	  assert(Ports.size()==2);
	  auto a=Ports.begin();
	  if(idx){
		  ++a;
	  }else{
	  }
	  return *a;
  }
  Node const* portByIndex(unsigned idx) const{
	  auto w=const_cast<Wire*>(this);
	  return w->portByIndex(idx);
  }

public: // adapt, move to Conductor.
  unsigned netNumber();
  bool hasNumber(){ return netNumber()!=-1u; }

public: // aliases. don't use
  Node* Port1(){
	  return portByIndex(0);
  }
  Node* Port2(){
	  return portByIndex(1);
  }

private:
  std::list<Node*> Ports;

public: // FIXME, these are still around.
	//int & cx__() { return cx; }
	//int & cy__() { return cy; }
	int & x1__() { return x1; }
	int & y1__() { return y1; }
	int & x2__() { return x2; }
	int & y2__() { return y2; }

public: // stuff used in mouseactions.
  void move1(int x, int y){ x1+=x; y1+=y; }
  void move2(int x, int y){ x2+=x; y2+=y; }
};

#endif
