/***************************************************************************
                          element.h  -  description
                             -------------------
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

#ifndef ELEMENT_H
#define ELEMENT_H

#include <qpen.h>
#include <qbrush.h>

class Node;
class QPainter;

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
      : x(_x), y(_y), w(_w), h(_h), angle(_angle),
	arclen(_arclen), style(_style) {};
  int   x, y, w, h, angle, arclen;
  QPen  style;
};

struct Area {
  Area(int _x, int _y, int _w, int _h, QPen _Pen,
	QBrush _Brush = QBrush(Qt::NoBrush))
	: x(_x), y(_y), w(_w), h(_h), Pen(_Pen), Brush(_Brush) {};
  int    x, y, w, h;
  QPen   Pen;
  QBrush Brush;    // filling style/color
};

struct Port {
  Port() {};
  Port(int _x, int _y) : x(_x), y(_y) {};
  int   x, y;
  Node *Connection;
};

struct Text {
  Text(int _x, int _y, const QString& _s, QColor _Color = QColor(0,0,0),
	float _Size = 10.0) : x(_x), y(_y), s(_s), Color(_Color),
	Size(_Size) {};
  int     x, y;
  QString s;
  QColor  Color;
  float   Size;
};

struct Property {
  Property(const QString& _Name="", const QString& _Value="",
	   bool _display=false, const QString& Desc="")
	 : Name(_Name), Value(_Value), display(_display), Description(Desc) {};
  QString Name, Value;
  bool    display;   // show on schematic or not ?
  QString Description;
};



enum ElementType {isDummy = 0,
		  isComponent = 0x0010, isGraph = 0x0020, isNode = 0x0040,
		  isMarker = 0x0080, isWire = 0x0100,
		  isPainting = 0x2000, isPaintingResize = 0x2001,
		  isHWireLabel = 0x4002, isVWireLabel  = 0x4004,
		  isNodeLabel  = 0x4008, isMovingLabel = 0x4010,
		  isHMovingLabel = 0x4020, isVMovingLabel = 0x4040,
		  isDiagram = 0x8000, isDiagramResize = 0x8001,
		  isDiagramScroll = 0x8002};


class Element {
public: 
  Element();
  virtual ~Element();

  virtual void paintScheme(QPainter*);
  virtual void setCenter(int, int, bool relative=false);
  virtual void getCenter(int&, int&);

  bool isSelected;

  ElementType  Type;    // whether it is Component, Wire, ...
  int  cx, cy, x1, y1, x2, y2;  // center and relative boundings
};

#endif
