/***************************************************************************
                          filledrect.h  -  description
                             -------------------
    begin                : Thu May 20 2004
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

#ifndef FILLEDRECT_H
#define FILLEDRECT_H

#include "painting.h"

#include <qpen.h>
#include <qbrush.h>


/**
  *@author Michael Margraf
  */

class FilledRect : public Painting  {
public:
  FilledRect();
  ~FilledRect();

  void paintScheme(QPainter *p);
  void getCenter(int& x, int &y);
  void setCenter(int x, int y, bool relative=false);

  FilledRect* newOne();
  bool load(const QString& s);
  QString save();
  void paint(QPainter *p);
  void MouseMoving(int x, int y, int gx, int gy, QPainter *p, bool drawn);
  bool MousePressing();
  bool getSelected(int x, int y);
  void Bounding(int&, int&, int&, int&);

  void rotate();
  void mirrorX();
  void mirrorY();
  bool Dialog();

  QPen   Pen;
  QBrush Brush;    // filling style/color
};

#endif
