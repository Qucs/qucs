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
  x2 = 200;
  y2 = 200;

  GridOn = true;
  GridX = 40;
  GridY = 0;

  Name = "Polar";

  calcDiagram();
}

PolarDiagram::~PolarDiagram()
{
}

// --------------------------------------------------------------
void PolarDiagram::calcDiagram()
{
  int z;

  Lines.clear();
  Texts.clear();
  Arcs.clear();

  Arcs.append(new Arc(0, y2, x2, y2, 0, 16*360, QPen(QPen::black,1)));
  Texts.append(new Text(x2>>1, -15, QString::number(ymax)));

  Lines.append(new Line(x2>>1, y2, x2>>1, 0, QPen(QPen::lightGray,1)));  // y line
  Lines.append(new Line(0, y2>>1, x2, y2>>1, QPen(QPen::lightGray,1)));  // x line

  
  if(GridOn) {
    z=GridX;
    while(z < (x2>>1)) {
      Arcs.append(new Arc(z, y2-z, x2-(z<<1), y2-(z<<1), 0, 16*360, QPen(QPen::lightGray,1)));
      z += GridX;
    }
  }
}

// ------------------------------------------------------------
void PolarDiagram::calcData(Graph *g)
{
  int *p = g->Points;
  for(cPoint *cp = g->cPoints.first(); cp != 0; cp = g->cPoints.next()) {
    *(p++) = (x2>>1)+int(cp->yr/ymax*double(x2>>1));
    *(p++) = (y2>>1)+int(cp->yi/ymax*double(y2>>1));
  }

  calcDiagram();
}

// ------------------------------------------------------------
PolarDiagram* PolarDiagram::newOne()
{
  return new PolarDiagram();
}
