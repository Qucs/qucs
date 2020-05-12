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
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "painting.h"

#include <QPen>


class Ellipse : public Painting  {
public:
  Ellipse(bool _filled=false);
 ~Ellipse();

  void paintScheme(SchematicDoc*) const;
  void getCenter(int&, int&);
  void setCenter(int, int, bool relative=false);

  Element* clone() const {return new Ellipse(*this);}
  static Element* info(QString&, char* &, bool getNewOne=false);
  static Element* info_filled(QString&, char* &, bool getNewOne=false);
  bool load(const QString&);
  QString save();
  QString saveCpp();
  QString saveJSON();
  void paint(ViewPainter*);
  void MouseMoving(SchematicDoc*, int, int, int, int, SchematicDoc*, int, int, bool);
  bool MousePressing();
  bool getSelected(float, float, float);
  bool resizeTouched(float, float, float);
  void MouseResizeMoving(int, int, SchematicDoc*);

  void rotate();
  void mirrorX();
  void mirrorY();
  bool Dialog();

  QPen  Pen;
  QBrush Brush;    // filling style/color
  bool  filled;    // filled or not (for "getSelected" etc.)
};

#endif
