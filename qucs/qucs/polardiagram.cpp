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

#include <math.h>


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
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  double Expo = floor(log10(ymax));
  double Base = ceil(ymax/pow(10,Expo) - 0.05);
  ymax = Base * pow(10,Expo);
  if(fabs(Expo) < 3) Texts.append(new Text(x2-10, (y2>>1)-12, QString::number(ymax)));
  else Texts.append(new Text(x2-10, (y2>>1)-12, QString::number(ymax, 'e', 0)));
  Arcs.append(new Arc(0, y2, x2, y2, 0, 16*352, QPen(QPen::black,1)));

  Lines.append(new Line(x2>>1, y2, x2>>1, 0, QPen(QPen::lightGray,1)));  // y line
  Lines.append(new Line(0, y2>>1, x2, y2>>1, QPen(QPen::lightGray,1)));  // x line

  
  if(GridOn) {
    int z;
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
}

// ------------------------------------------------------------
PolarDiagram* PolarDiagram::newOne()
{
  return new PolarDiagram();
}
