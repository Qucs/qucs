/***************************************************************************
                          rectdiagram.cpp  -  description
                             -------------------
    begin                : Thu Oct 2 2003
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

#include "rectdiagram.h"

RectDiagram::RectDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  dx = 240;
  dy = 160;

  GridOn = true;
  GridX = 60;
  GridY = 40;

  xLabel = "x-Achse";
  yLabel = "y-Achse";
}

RectDiagram::~RectDiagram()
{
}

// --------------------------------------------------------------
void RectDiagram::paint(QPainter *p)
{
  int z;

  p->setPen(QPen(Qt::black, 1, Qt::SolidLine));
  p->drawRect(cx, cy-dy, dx+1, dy+1);   // paint frame

  if(GridOn) {
    p->setPen(QPen(Qt::lightGray, 1, Qt::SolidLine));
    z=GridX;
    while(z < dx) {
      p->drawLine(cx+z, cy-dy, cx+z, cy);   // paint x grid
      z += GridX;
    }
    z=dy-GridY;
    while(z > 0) {
      p->drawLine(cx, cy-z, cx+dx, cy-z);   // paint y grid
      z -= GridY;
    }
  }

  p->setPen(QPen(Qt::black, 1, Qt::SolidLine));
  z=0;
  while(z < dx) {
    p->drawLine(cx+z, cy-5, cx+z, cy+5);   // paint x axis mark
    z += GridX;
  }
  z=dy-GridY;
  while(z >= 0) {
    p->drawLine(cx-5, cy-z, cx+5, cy-z);   // paint y axis mark
    z -= GridY;
  }

  p->drawText(cx, cy+20, xLabel);
  p->save();
  p->rotate(270);
  p->drawText(-cy, cx-15, yLabel);
  p->restore();
  
  for(Graph *pg = Graphs.first(); pg != 0; pg = Graphs.next())
    pg->paint(p);
}

// ------------------------------------------------------------
RectDiagram* RectDiagram::newOne()
{
  return new RectDiagram();
}
