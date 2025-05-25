/***************************************************************************
                                  wire.h
                                 --------
    begin                : Wed Sep 3 2003
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

#ifndef WIRE_H
#define WIRE_H

#include "viewpainter.h"
#include "element.h"
#include "components/component.h"    // because of struct Port
#include "wirelabel.h"

class QPainter;
class QString;


class Wire : public Conductor {
  //  TODO: would like to turn this into a weak pointer, but the pointer value
  //  is sometimes used as flag set while moving the full wire.
  mutable Node      *_port0{nullptr}, *_port1{nullptr};
public:
  Wire(int _x1=0, int _y1=0, int _x2=0, int _y2=0, Node *n1=0, Node *n2=0);
 ~Wire();

  int net_nodes()const {return 2;}
 // BUG. Port&
  Node*& ports(int i){
	  assert(i<net_nodes());
	  if(i==0){
		  return _port0;
	  }else{
		  return _port1;
	  }
  }

  // BUG: wire is not a Component
  Node*& ports(int i)const /*override*/ {
	  assert(i<net_nodes());
	  if(i==0){
		  return _port0;
	  }else{
		  return _port1;
	  }
  }

  void paint(ViewPainter*);
  void paintScheme(Schematic *s);
  void paintScheme(QPainter*);
  void setCenter(int, int, bool relative=false);
  void getCenter(int&, int&);
  bool getSelected(int, int);
  void setName(const QString&, const QString&, int delta_=0, int x_=0, int y_=0);

  // Setter functions for verilog parsing
  // x1, y1, x2, y2 are going to be private in the future
  virtual void set_qucs_x1(int x) { set_x1(x); }
  virtual void set_qucs_y1(int y) { set_y1(y); }
  virtual void set_qucs_x2(int x) { set_x2(x); }
  virtual void set_qucs_y2(int y) { set_y2(y); }
  virtual void set_attribute(std::string name, std::string value);
  virtual void apply_qucs_values() {}

  void    rotate();
  QString save();
  bool    load(const QString&);
  bool    isHorizontal();
};

#endif
