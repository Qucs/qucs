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
      Lines.append(new Line(z, y2, z, 0, QPen(QPen::lightGray,1)));   // x grid
      z += GridX;
    }
    z=y2-GridY;
    while(z > 0) {
      Lines.append(new Line(0, z, x2, z, QPen(QPen::lightGray,1)));   // y grid
      z -= GridY;
    }
  }

  z=0;
  while(z < x2) {
    Lines.append(new Line(z, 5, z, -5, QPen(QPen::black,1)));   // x axis marks
    z += GridX;
  }
  Texts.append(new Text(0, -15, QString::number(xmin)));
  Texts.append(new Text(x2, -15, QString::number(xmax)));

  z=y2-GridY;
  while(z >= 0) {
    Lines.append(new Line(-5, z, 5, z, QPen(QPen::black,1)));   // y axis marks
    z -= GridY;
  }

  // outer frame
  Lines.append(new Line(0,  y2, x2, y2, QPen(QPen::black,1)));
  Lines.append(new Line(x2, y2, x2,  0, QPen(QPen::black,1)));
  Lines.append(new Line(0,   0, x2,  0, QPen(QPen::black,1)));
  Lines.append(new Line(0,  y2,  0,  0, QPen(QPen::black,1)));

  Texts.append(new Text(-20, y2, QString::number(ymax)));
  Texts.append(new Text(-20, 0,  QString::number(ymin)));
}

// ------------------------------------------------------------
void RectDiagram::calcData(Graph *g)
{
  int *p = g->Points;
  for(cPoint *cp = g->cPoints.first(); cp != 0; cp = g->cPoints.next()) {
    *(p++) = int((cp->x-xmin)/(xmax-xmin)*x2);
    *(p++) = int((sqrt(cp->yr*cp->yr + cp->yi*cp->yi)-ymin)/(ymax-ymin)*y2);
  }
}

// ------------------------------------------------------------
RectDiagram* RectDiagram::newOne()
{
  return new RectDiagram();
}
