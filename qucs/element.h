/***************************************************************************
                          element.h  -  description
                             -------------------
    begin                : Sat Sep 20 2003
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

#ifndef ELEMENT_H
#define ELEMENT_H

#include <qpainter.h>

class Node;

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



enum ElementType {isDummy, isComponent, isWire, isHWireLabel, isVWireLabel, isNodeLabel, isMovingLabel,
                  isDiagram, isGraph, isPainting, isNode};


class Element {
public: 
  Element();
  virtual ~Element();

  virtual void paintScheme(QPainter *p);
  virtual void setCenter(int x, int y, bool relative=false);

  bool isSelected;

  ElementType  Type;    // whether it is Component, Wire, ...
  int  cx, cy, x1, y1, x2, y2;  // center and relative boundings
};

#endif
