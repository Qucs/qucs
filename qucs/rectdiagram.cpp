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

#include <math.h>

#include <qmessagebox.h>


RectDiagram::RectDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  dx = 240;
  dy = 160;

  GridOn = true;
  GridX = 60;
  GridY = 40;

  Name = "Rect";

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
  p->drawText(cx,   cy+15, QString::number(x1));
  p->drawText(cx+z, cy+15, QString::number(x2));

  z=dy-GridY;
  while(z >= 0) {
    p->drawLine(cx-5, cy-z, cx+5, cy-z);   // paint y axis mark
    z -= GridY;
  }
  p->drawText(cx-20, cy-dy, QString::number(y2));
  p->drawText(cx-20, cy, QString::number(y1));

  p->drawText(cx, cy+40, xLabel);
  p->save();
  p->rotate(270);
  p->drawText(-cy, cx-30, yLabel);
  p->restore();
  
  for(Graph *pg = Graphs.first(); pg != 0; pg = Graphs.next())
    pg->paint(p);
}

// ------------------------------------------------------------
void RectDiagram::calcData(Graph *g)
{
  int *p = g->Points;
  for(cPoint *cp = g->cPoints.first(); cp != 0; cp = g->cPoints.next()) {
    *(p++) = cx+int((cp->x-x1)/(x2-x1)*dx);
    *(p++) = cy-int((sqrt(cp->yr*cp->yr + cp->yi*cp->yi)-y1)/(y2-y1)*dy);
//QMessageBox::critical(0, "Error", QString::number(cp->x)+"  "+QString::number(cp->yr));
  }
}

// ------------------------------------------------------------
RectDiagram* RectDiagram::newOne()
{
  return new RectDiagram();
}
