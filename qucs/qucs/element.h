/***************************************************************************
                                 element.h
                                -----------
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

/** \file element.h
  * \brief Defines drawing elements for schematics
  *
  * element.h contains definitions of various drawing elements used
  * used to render schematics and the schematic symbols. The following
  * structs are defined to hold information on various drawing types:
  *
  *    Line
  *    Arc
  *    Area
  *    Port
  *    Text
  *    Property
  *
  * The Element class is also defined here which is a superclass
  * of every component symbol.
  *
  */

#ifndef ELEMENT_H
#define ELEMENT_H

#include <QPen>
#include <QBrush>
#include <QDebug>
#include <assert.h>
#include "io_trace.h"
#include "object.h"
#include "io_trace.h"

class Node;
class QPainter;
class WireLabel;
class Schematic;

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
  Port(int _x, int _y, bool _avail=true) : x(_x), y(_y), avail(_avail) {
    Type=""; Name=""; Connection=0;};
  int   x, y;
  bool  avail;
  QString Type;
  QString Name;
  Node *Connection;
};

struct Text {
  Text(int _x, int _y, const QString& _s, QColor _Color = QColor(0,0,0),
	float _Size = 10.0, float _mCos=1.0, float _mSin=0.0)
	: x(_x), y(_y), s(_s), Color(_Color), Size(_Size),
	  mSin(_mSin), mCos(_mCos) { over = under = false; };
  int	  x, y;
  QString s;
  QColor  Color;
  float	  Size, mSin, mCos; // font size and rotation coefficients
  bool	  over, under;      // text attributes
};

struct Property {
  Property(const QString& _Name="", const QString& _Value="",
	   bool _display=false, const QString& Desc="")
	 : Name(_Name), Value(_Value), display(_display), Description(Desc) {};
  QString Name, Value;
  bool    display;   // show on schematic or not ?
  QString Description;
};


// valid values for Element.Type
// The 4 least significant bits of each value are reserved for special
// additionals !!!
#define isDummyElement    0
#define isSpecialMask    -16

#define isComponent        0x30000
#define isComponentText    0x30002
#define isAnalogComponent  0x10000
#define isDigitalComponent 0x20000

#define isGraph            0x0020
#define isNode             0x0040
#define isMarker           0x0080
#define isWire             0x0100

#define isPainting         0x2000
#define isPaintingResize   0x2001

#define isLabel            0x4000
#define isHWireLabel       0x4020
#define isVWireLabel       0x4040
#define isNodeLabel        0x4080
#define isMovingLabel      0x4001
#define isHMovingLabel     0x4002
#define isVMovingLabel     0x4004

#define isDiagram          0x8000
#define isDiagramResize    0x8001
#define isDiagramHScroll   0x8002
#define isDiagramVScroll   0x8003


/** \class Element
  * \brief Superclass of all schematic drawing elements
  *
  *
  */

class NetLang;
class ViewPainter;

static QString incomplete_description="incomplete_description";

class Element : public Object {
public:
  Element();
  virtual ~Element();

  virtual void paintScheme(Schematic *) const; // obsolete?
  virtual void paintScheme(QPainter *) const; // obsolete?
  virtual void draw(QPainter&) { incomplete(); }
  virtual void setCenter(int, int, bool relative=false);
  virtual void getCenter(int&, int&);
  virtual void paint(ViewPainter*); // BUG: const

  virtual void editElement(){}
  virtual void MPressElement(){}
  virtual void tAC(QTextStream&, Schematic*, QStringList&, int&, int,
		  NetLang const&){untested();}
public:
  // should be pure, but several elments are incomplete.
  virtual Object* newOne()const{
	  incomplete();
	  qDebug() << label() << "lacks clone\n";
	  return NULL;
  }
//  { unreachable(); return 0 /*NULL, actually*/;}
  virtual QString name() const{return "incomplete_name";}
  virtual QString const& description() const{return incomplete_description;}
  virtual char const* iconBasename() const{return nullptr;}

//private: FIXME
  bool isSelected;
  int  Type;    // whether it is Component, Wire, ...
  int  cx, cy, x1, y1;
  
  // BUG; abused in Command
  mutable int x2, y2;  // center and relative boundings
}; // Element


/** \class Conductor
  * \brief label for Node and Wire classes
  *
  */
class Conductor : public Element {
public:
  WireLabel *Label;
};

class Component;
class Command;
class Wire;
class WireLabel;
class Diagram;
class Painting;
class Graph;
class Marker;

Component* component(Element*);
Command* command(Element*);
inline Element*& element(Element*& x){return x;}
Wire* wire(Element*);
WireLabel* wireLabel(Element*);
Diagram* diagram(Element*);
Painting* painting(Element*);
Graph* graph(Element*);
Marker* marker(Element*);
Node* node(Element*);
#endif
