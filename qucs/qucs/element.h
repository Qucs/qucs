/***************************************************************************
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
  *
  * The Element class is also defined here which is a superclass
  * of every component symbol.
  *
  */

#ifndef QUCS_ELEMENT_H
#define QUCS_ELEMENT_H
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifndef UNTANGLE_QT // later
//# include <QBrush>
//# include <QDebug>
#endif

#include <assert.h>
#include "object.h"
#include "io_trace.h"
#include "port.h"
#include "qt_compat.h"

class Node;
class QucsDoc;
class QPainter;
class WireLabel;
class SchematicDoc;
class SchematicModel;
class Symbol;
class QDialog;

# include <QPen> // BUG
struct Line {
	Line(Line const&) = default;
  Line(int _x1, int _y1, int _x2, int _y2, QPen _style)
       : x1(_x1), y1(_y1), x2(_x2), y2(_y2), style(_style) {};
  int   x1, y1, x2, y2;
  QPen  style;
};

struct Arc {
	Arc(Arc const&) = default;
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

// does not work. move to legacy stuff.
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
//#define isNode             0x0040
#define isMarker           0x0080
//#define isWire             0x0100

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
// will be gone soon.
class Component;
class Label;
class WireLabel;
class Diagram;
class Painting;
class Graph;
class Marker;
class Node;
class ViewPainter;
/*--------------------------------------------------------------------------*/
class SchematicModel;
/*--------------------------------------------------------------------------*/
class Element : public Object {
public:
  friend class ElementGraphics;
  typedef rect_t Rect;
protected:
  Element(Element const&);
public:
  Element();
  Element(int cx, int cy) : _center(cx, cy) { unreachable(); }
  virtual ~Element();

public: // make old variables accessible
	int const& cx() const { return _center.first; }
	int const& cy() const { return _center.second; }

	int const& x1_() const { return x1; }
	int const& y1_() const { return y1; }
//	int const& x2_() const { return x2; }
//	int const& y2_() const { return y2; }
	void snapToGrid(SchematicDoc& s);

	void setObsoleteType(int t){
		Type = t;
	}
	virtual rect_t bounding_rect() const;

public: // other stuff
  virtual bool showLabel() const{ return true; }
  //virtual bool showParam(int i) const{ return true; } // later

  void setCenter(pos_t const& c){ _center = c; }
//  virtual void setCenter(int x, int y, bool relative=false);
  virtual void getCenter(int&, int&) const; // BUG
  virtual void paint(ViewPainter*) const = 0;
  virtual QDialog* schematicWidget(QucsDoc*) const { return nullptr; }

  // really?
  virtual QWidget* newWidget() {return nullptr;}

  // BUG: not virtual
  virtual pos_t center()const;


public:
	virtual Element* clone()const = 0;
	virtual Element* clone_instance()const{
		return clone();
	}
//  virtual QString const& name() const{return Name;}
//  void setName(QString const& n){ Name = n; }
  virtual QString const& description() const{return incomplete_description;}
  virtual char const* iconBasename() const{return nullptr;}

public: // BUG
  int  Type;    // whether it is Component, Wire, ...

public: // set protected variables. don't use
//   void obsolete_set(std::string name, int value){
// 	  incomplete();
// 	  if(name == "cx"){
// 		  cx = value;
// 	  }else if(name == "cy"){
// 		  cy = value;
// 	  }
//   }
	virtual bool legacyTransformHack() const{
		return false;
	}

private:
   pos_t _center; // BUG: store in symbol?

protected:
  int x1, y1;
public:
  
  // BUG; abused in taskElement
//  mutable int x2, y2;  // center and relative boundings

  // create a declaration, e.g. subcircuit definition or include directive
  virtual Symbol const* proto(SchematicModel const*) const{return nullptr;}
  SchematicModel const* scope() const;
  virtual SchematicModel* scope();

public: // friend ElementGraphics?
  void attachToModel();
  void detachFromModel();

protected: //BUG

public:
  Element const* owner() const{return _owner;}
  Element* mutable_owner() const{return _owner;}

protected:
  Element* owner(){ return _owner;}

public:
  void setOwner(Element* e) { _owner=e;}

private:
  Element* _owner; // should probably be const all the way
}; // Element

inline SchematicModel const* Element::scope() const
{
	auto e=const_cast<Element*>(this);
	return e->scope();
}
/*--------------------------------------------------------------------------*/
class Component;
class TaskElement;
class WireLabel;
class Diagram;
class Painting;
class Graph;
class Marker;
/*--------------------------------------------------------------------------*/
Component const* component(Element const*);
WireLabel const* wireLabel(Element const*);
Diagram const* diagram(Element const*);
Painting const* painting(Element const*);
Graph const* graph(Element const*);
/*--------------------------------------------------------------------------*/
Component* component(Element*);
TaskElement* command(Element*);
inline Element*& element(Element*& x){return x;}
WireLabel* wireLabel(Element*);
Diagram* diagram(Element*);
Painting* painting(Element*);
Graph* graph(Element*);
Marker* marker(Element*);
Node* node(Element*);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
