/***************************************************************************
                          component.h  -  description
                             -------------------
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

#include "viewpainter.h"
#include "element.h"
#include "node.h"


#include <qptrlist.h>
#include <qpen.h>
#include <qpoint.h>
#include <qwidget.h>

class QString;

/**
  *@author Michael Margraf
  */


class Component : public Element {
public:
  Component();
  virtual ~Component();

  virtual Component* newOne();
  virtual void recreate();
  void    paint(ViewPainter*);
  void    paintScheme(QPainter*);
  void    print(ViewPainter*);
  void    setCenter(int, int, bool relative=false);
  void    getCenter(int&, int&);
  void    TextSize(int&, int&);
  void    Bounding(int&, int&, int&, int&);
  void    entireBounds(int&, int&, int&, int&);
  bool    getSelected(int, int);
  void    rotate();
  void    mirrorX();  // mirror about X axis
  void    mirrorY();  // mirror about Y axis
  QString NetList();
  QString save();
  bool    load(const QString&);

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
  QString  Model, Name;
};


// prototype of independent function
Component* getComponentFromName(QString&);

#endif
