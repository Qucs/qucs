/***************************************************************************
                          graphicline.h  -  description
                             -------------------
    begin                : Mon Nov 24 2003
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

#ifndef GRAPHICLINE_H
#define GRAPHICLINE_H

#include "painting.h"

/**
  *@author Michael Margraf
  */

class GraphicLine : public Painting  {
public: 
  GraphicLine(int cx_=0, int cy_=0, int x2_=0, int y2_=0,
		QPen Pen_=QPen(QColor()));
  ~GraphicLine();

  void paintScheme(QPainter*);
  void getCenter(int&, int&);
  void setCenter(int, int, bool relative=false);

  GraphicLine* newOne();
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

  QPen   Pen;
};

#endif
