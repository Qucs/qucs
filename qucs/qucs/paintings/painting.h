/***************************************************************************
                          painting.h  -  description
                             -------------------
    begin                : Sat Nov 22 2003
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

#ifndef PAINTING_H
#define PAINTING_H

#include "../element.h"

#include <qpainter.h>

/**
  *@author Michael Margraf
  */

class Painting : public Element  {
public: 
  Painting();
  ~Painting();

  virtual void getCenter(int&, int &) {};
  virtual bool getSelected(int, int) { return false; };

  virtual Painting* newOne();
  virtual bool load(const QString&) { return true; };
  virtual QString save();
  virtual void paint(QPainter*) {};
  virtual void MouseMoving(int, int, int, int, QPainter*, bool) {};
  virtual bool MousePressing() { return false; };
  virtual void Bounding(int&, int&, int&, int&) {};
  virtual bool ResizeTouched(int, int) { return false; };
  virtual void MouseResizeMoving(int, int, QPainter*) {};

  virtual void rotate() {};
  virtual void mirrorX() {};
  virtual void mirrorY() {};
  virtual bool Dialog() { return false; };

  int  State;   // state for different mouse operations
};

#endif
