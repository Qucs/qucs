/***************************************************************************
                               component.h
                              -------------
    begin                : Sat Aug 23 2003
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

#ifndef COMPONENT_H
#define COMPONENT_H

#include "../element.h"

#include <qptrlist.h>
#include <qpen.h>
#include <qpoint.h>
#include <qwidget.h>

class QucsDoc;
class ViewPainter;
class QString;


class Component : public Element {
public:
  Component();
  virtual ~Component();

  virtual Component* newOne();
  virtual void recreate(QucsDoc*);
  virtual QString NetList();
  virtual QString VHDL_Code();
  void    paint(ViewPainter*);
  void    paintScheme(QPainter*);
  void    print(ViewPainter*);
  void    setCenter(int, int, bool relative=false);
  void    getCenter(int&, int&);
  void    TextSize(int&, int&);
  void    Bounding(int&, int&, int&, int&);
  void    entireBounds(int&, int&, int&, int&, float);
  bool    getSelected(int, int);
  int     getTextSelected(int, int, float);
  void    rotate();
  void    mirrorX();  // mirror about X axis
  void    mirrorY();  // mirror about Y axis
  QString save();
  bool    load(const QString&);

  // to hold track of the component appearance for saving and copying
  bool mirroredX;   // is it mirrored about X axis or not
  int  rotated;     // rotation angle divided by 90 degrees


  QPtrList<Line>     Lines;
  QPtrList<struct Arc>      Arcs;
  QPtrList<Area>     Rects, Ellips;
  QPtrList<Port>     Ports;
  QPtrList<Text>     Texts;
  QPtrList<Property> Props;

  bool     isActive; // should it be used in simulation or not ?
  int      tx, ty;   // upper left corner of text (position)
  QString  Description;
  QString  Model, Name;

protected:
  int  analyseLine(const QString&);
  bool getIntegers(const QString&, int *i1=0, int *i2=0, int *i3=0,
		   int *i4=0, int *i5=0, int *i6=0);
  bool getPen(const QString&, QPen&, int);
  bool getBrush(const QString&, QBrush&, int);
  void performModification();
};

class GateComponent : public Component {
public:
  GateComponent();
  QString NetList();
  QString VHDL_Code();
  void recreate(QucsDoc*);
  void createSymbol();
};

// prototype of independent function
Component* getComponentFromName(QString&);

#endif
