/***************************************************************************
                          rectangle.h  -  description
                             -------------------
    begin                : Sat Nov 22 2003
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

#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "painting.h"

#include <qpen.h>


/**
  *@author Michael Margraf
  */

class Rectangle : public Painting  {
public: 
	Rectangle();
	~Rectangle();

  virtual void paintScheme(QPainter *p);
  virtual void getCenter(int& x, int &y);
  virtual void setCenter(int x, int y, bool relative=false);

  virtual Rectangle* newOne();
  virtual bool load(const QString& s);
  virtual QString save();
  virtual void paint(QPainter *p);
  virtual void MouseMoving(int x, int y, int gx, int gy, QPainter *p, bool drawn);
  virtual bool MousePressing();
  virtual bool getSelected(int x, int y);
  virtual void Bounding(int&, int&, int&, int&);

  virtual void rotate();
  virtual void mirrorX();
  virtual void mirrorY();
  virtual bool Dialog();

  QPen  Pen;
};

#endif
