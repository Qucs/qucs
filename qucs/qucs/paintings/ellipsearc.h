/***************************************************************************
                         ellipsearc.h  -  description
                             -------------------
    begin                : Thu Sep 9 2004
    copyright            : (C) 2004 by Michael Margraf
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

#ifndef ELLIPSEARC_H
#define ELLIPSEARC_H

#include "painting.h"

#include <qpen.h>


/**
  *@author Michael Margraf
  */

class EllipseArc : public Painting  {
public:
  EllipseArc();
  ~EllipseArc();

  void paintScheme(QPainter*);
  void getCenter(int&, int&);
  void setCenter(int, int, bool relative=false);

  EllipseArc* newOne();
  bool load(const QString&);
  QString save();
  void paint(ViewPainter*);
  void MouseMoving(int, int, int, int, QPainter*, bool);
  bool MousePressing();
  bool getSelected(int, int);
  void Bounding(int&, int&, int&, int&);
  bool ResizeTouched(int, int);
  void MouseResizeMoving(int, int, QPainter*);

  void rotate();
  void mirrorX();
  void mirrorY();
  bool Dialog();

  QPen  Pen;
  int   Angle, ArcLen;   // start angle and arc length
};

#endif
