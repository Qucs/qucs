/***************************************************************************
                                ellipse.h
                               -----------
    begin                : Sun Nov 23 2003
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

#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "painting.h"

#include <qpen.h>


class Ellipse : public Painting  {
public:
  Ellipse(bool _filled=false);
 ~Ellipse();

  void paintScheme(QPainter*);
  void getCenter(int&, int&);
  void setCenter(int, int, bool relative=false);

  Painting* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  static Element* info_filled(QString&, char* &, bool getNewOne=false);
  bool load(const QString&);
  QString save();
  void paint(ViewPainter*);
  void MouseMoving(QPainter*, int, int, int, int, QPainter*, int, int, bool);
  bool MousePressing();
  bool getSelected(int, int);
  void Bounding(int&, int&, int&, int&);
  bool ResizeTouched(int, int, int);
  void MouseResizeMoving(int, int, QPainter*);

  void rotate();
  void mirrorX();
  void mirrorY();
  bool Dialog();

  QPen  Pen;
  QBrush Brush;    // filling style/color
  bool  filled;    // filled or not (for "getSelected" etc.)
};

#endif
