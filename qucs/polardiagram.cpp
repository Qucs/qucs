/***************************************************************************
                          polardiagram.cpp  -  description
                             -------------------
    begin                : Fri Oct 17 2003
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

#include "polardiagram.h"


PolarDiagram::PolarDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  dx = 200;
  dy = 200;

  GridOn = true;
  GridX = 40;
  GridY = 0;

  Name = "Polar";
}

PolarDiagram::~PolarDiagram()
{
}

// --------------------------------------------------------------
void PolarDiagram::paint(QPainter *p)
{
  int z;

  p->setPen(QPen(Qt::black, 1, Qt::SolidLine));
  p->drawArc(cx, cy-dy, dx, dy, 0, 16*360);
  p->drawText(cx+(dx>>1), cy+15, QString::number(y2));

  p->setPen(QPen(Qt::lightGray, 1, Qt::SolidLine));
  p->drawLine(cx+(dx>>1), cy-dy, cx+(dx>>1), cy);   // paint y line
  p->drawLine(cx, cy-(dy>>1), cx+dx, cy-(dy>>1));   // paint x line

  
  if(GridOn) {
    z=GridX;
    while(z < (dx>>1)) {
      p->drawArc(cx+z, cy-dy+z, dx-(z<<1), dy-(z<<1), 0, 16*360);
      z += GridX;
    }
  }

  for(Graph *pg = Graphs.first(); pg != 0; pg = Graphs.next())
    pg->paint(p);
}

// ------------------------------------------------------------
void PolarDiagram::calcData(Graph *g)
{
  int *p = g->Points;
  for(cPoint *cp = g->cPoints.first(); cp != 0; cp = g->cPoints.next()) {
    *(p++) = cx+(dx>>1)+int(cp->yr/y2*double(dx>>1));
    *(p++) = cy-(dy>>1)-int(cp->yi/y2*double(dy>>1));
  }
}

// ------------------------------------------------------------
PolarDiagram* PolarDiagram::newOne()
{
  return new PolarDiagram();
}
