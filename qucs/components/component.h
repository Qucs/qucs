/***************************************************************************
                          component.h  -  description
                             -------------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef COMPONENT_H
#define COMPONENT_H

#include "../element.h"
#include "../node.h"


#include <qptrlist.h>
#include <qpen.h>
#include <qpoint.h>
#include <qwidget.h>

class QString;

/**
  *@author Michael Margraf
  */

struct Line {
  Line(int _x1, int _y1, int _x2, int _y2, QPen _style)
       : x1(_x1), y1(_y1), x2(_x2), y2(_y2), style(_style) {};
  int   x1, y1, x2, y2;
  QPen  style;
};

struct Arc {
  Arc(int _x, int _y, int _w, int _h, int _angle, int _arclen, QPen _style)
       : x(_x), y(_y), w(_w), h(_h), angle(_angle), arclen(_arclen), style(_style) {};
  int   x, y, w, h, angle, arclen;
  QPen  style;
};

struct Port {
  Port() {};
  Port(int _x, int _y) : x(_x), y(_y) {};
  int   x, y;
  Node *Connection;
};
  
struct Text {
  Text(int _x, int _y, const QString& _s) : x(_x), y(_y), s(_s) {};
  int   x, y;
  QString s;
};

struct Property {
  Property(const QString& _Name="", const QString& _Value="", bool _display=false, const QString& Desc="")
           : Name(_Name), Value(_Value), display(_display), Description(Desc) {};
  QString Name, Value;
  bool    display;   // show on schematic or not ?
  QString Description;
};

class Component : public Element {
public: 
	Component();
	virtual ~Component();

  virtual Component* newOne();
  void    paintScheme(QPainter *p);
  void    setCenter(int x, int y, bool relative=false);
  void    Bounding(int& _x1, int& _y1, int& _x2, int& _y2);
  void    entireBounds(int& _x1, int& _y1, int& _x2, int& _y2);
  void    paint(QPainter *p);
  void    rotate();
  void    mirrorX();  // mirror about X axis
  void    mirrorY();  // mirror about Y axis
  QString NetList();
  QString save();
  bool    load(const QString& s);

  // to hold track of the component appearance for saving and copying
  bool mirroredX;   // is it mirrored about X axis or not
  int  rotated;     // rotation angle divided by 90 degrees


  QPtrList<Line>     Lines;
  QPtrList<Arc>      Arcs;
  QPtrList<Port>     Ports;
  QPtrList<Text>     Texts;
  QPtrList<Property> Props;

  bool     isActive; // should it be used in simulation or not ?
  int      tx, ty;   // upper left corner of text (position)
  QString  Description;
  QString  Sign, Model, Name;
};

#endif
