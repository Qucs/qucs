/***************************************************************************
                                  wire.h
                                 --------
    copyright            : (C) 2003 by Michael Margraf
                               2018, 2020 Felix Salfelder / QUCS
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
#include "components/component.h"    // because of struct Port
#include "wirelabel.h"
#include "platform.h"

class QPainter;
class QString;

class dummy {};


class Wire : public Symbol {
public:
  Wire(int _x1=0, int _y1=0, int _x2=0, int _y2=0); //  Node *n1=0, Node *n2=0);
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
  QString save(){unreachable(); return "";}

public: // Node xs
  bool hasNet() const { return _port0.connected(); }
  Net* getNet();
  Net const* getNet() const;

  QString const& netLabel() const;

private: // Symbol
  void setParameter(QString const& name, QString const& value);

private: // symbol Node stuff
  Node* connectNode(unsigned idx, NodeMap&) override;
  Node* disconnectNode(unsigned idx, NodeMap&) override;

private: // Symbol
  unsigned numPorts() const{ return 2; }

public:
  bool    isHorizontal() const { return (y1() == y2());}
  QRectF boundingRect() const;

public:
//  void setPortByIndex(unsigned idx, Node* n);
//  Node* portValue(unsigned idx);
  std::list<Node*>::iterator connectionsBegin();
  std::list<Node*>::iterator connectionsEnd(){
	  return _node_hack.end();
  }

private: // Symbol, internal port access.
  Port& port(unsigned i){
	  assert(i<2);
	  if(i==0){
		  return _port0;
	  }else{
		  return _port1;
	  }
  }

private:
  std::list<Node*> _node_hack;

public: // FIXME, these are still around. (from element)
	//int & cx__() { return cx; }
	//int & cy__() { return cy; }
	int & x1__() { return _port0.x; }
	int & y1__() { return _port0.y; }
	int & x2__() { return _port1.x; }
	int & y2__() { return _port1.y; }

	int const& x1() const { return _port0.x; }
	int const& y1() const { return _port0.y; }
	int const& x2() const { return _port1.x; }
	int const& y2() const { return _port1.y; }

	void setPos0(int x, int y){ x1__() = x; y1__() = y; }
	void setPos1(int x, int y){ x2__() = x; y2__() = y; }

public: // stuff used in mouseactions.
  void move1(int x, int y){ x1__()+=x; y1__()+=y; }
  void move2(int x, int y){ x2__()+=x; y2__()+=y; }

private:
	int & x1() { return _port0.x; }
	int & y1() { return _port0.y; }
	int & x2() { return _port1.x; }
	int & y2() { return _port1.y; }
private: // avoid access to obsolete Element members
  Port _port0;
  Port _port1;

private:
  QString nx, ny, delta;
  QString _netname;
};

#endif
