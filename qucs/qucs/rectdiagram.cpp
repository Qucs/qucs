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
  x2 = 240;
  y2 = 160;

  GridOn = true;
  GridX = 60;
  GridY = 40;

  Name = "Rect";

  xLabel = "x-Achse";
  yLabel = "y-Achse";

  calcDiagram();
}

RectDiagram::~RectDiagram()
{
}

// --------------------------------------------------------------
void RectDiagram::calcDiagram()
{
  int z;

  Lines.clear();
  Texts.clear();
  Arcs.clear();

  if(GridOn) {
    z=GridX;
    while(z < x2) {
      Lines.append(new Line(cx+z, cy-y2, cx+z, cy, QPen(QPen::lightGray,1)));   // x grid
      z += GridX;
    }
    z=y2-GridY;
    while(z > 0) {
      Lines.append(new Line(cx, cy-z, cx+x2, cy-z, QPen(QPen::lightGray,1)));   // y grid
      z -= GridY;
    }
  }

  z=0;
  while(z < x2) {
    Lines.append(new Line(cx+z, cy-5, cx+z, cy+5, QPen(QPen::black,1)));   // x axis marks
    z += GridX;
  }
  Texts.append(new Text(cx,   cy+15, QString::number(xg1)));
  Texts.append(new Text(cx+z, cy+15, QString::number(xg2)));

  z=y2-GridY;
  while(z >= 0) {
    Lines.append(new Line(cx-5, cy-z, cx+5, cy-z, QPen(QPen::black,1)));   // y axis marks
    z -= GridY;
  }

  // outer frame
  Lines.append(new Line(cx, cy-y2, cx+x2, cy-y2, QPen(QPen::black,1)));
  Lines.append(new Line(cx+x2, cy-y2, cx+x2, cy, QPen(QPen::black,1)));
  Lines.append(new Line(cx, cy, cx+x2, cy, QPen(QPen::black,1)));
  Lines.append(new Line(cx, cy-y2, cx, cy, QPen(QPen::black,1)));

  Texts.append(new Text(cx-20, cy-y2, QString::number(yg2)));
  Texts.append(new Text(cx-20, cy,    QString::number(yg1)));

/*  p->drawText(cx, cy+40, xLabel);
  p->save();
  p->rotate(270);
  p->drawText(-cy, cx-30, yLabel);
  p->restore();*/
}

// ------------------------------------------------------------
void RectDiagram::calcData(Graph *g)
{
  int *p = g->Points;
  for(cPoint *cp = g->cPoints.first(); cp != 0; cp = g->cPoints.next()) {
    *(p++) = cx+int((cp->x-xg1)/(xg2-xg1)*x2);
    *(p++) = cy-int((sqrt(cp->yr*cp->yr + cp->yi*cp->yi)-y1)/(yg2-yg1)*y2);
//QMessageBox::critical(0, "Error", QString::number(cp->x)+"  "+QString::number(cp->yr));
  }

  calcDiagram();
}

// ------------------------------------------------------------
RectDiagram* RectDiagram::newOne()
{
  return new RectDiagram();
}
