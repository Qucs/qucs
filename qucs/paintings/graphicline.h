/***************************************************************************
                          graphicline.h  -  description
                             -------------------
    begin                : Mon Nov 24 2003
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

#ifndef GRAPHICLINE_H
#define GRAPHICLINE_H

#include "painting.h"

/**
  *@author Michael Margraf
  */

class GraphicLine : public Painting  {
public: 
  GraphicLine();
  ~GraphicLine();

  void paintScheme(QPainter *p);
  void getCenter(int& x, int &y);
  void setCenter(int x, int y, bool relative=false);

  GraphicLine* newOne();
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
};

#endif
