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

#include <painting.h>

/**
  *@author Michael Margraf
  */

class GraphicLine : public Painting  {
public: 
	GraphicLine();
	~GraphicLine();

  virtual void paintScheme(QPainter *p);
  virtual void getCenter(int& x, int &y);
  virtual void setCenter(int x, int y, bool relative=false);

  virtual GraphicLine* newOne();
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

  QPen   Pen;
};

#endif
