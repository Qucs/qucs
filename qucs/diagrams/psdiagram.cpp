/***************************************************************************
                        psdiagram.cpp  -  description
                             -------------------
    begin                : Sat Nov 06 2004
    copyright            : (C) 2004 by Michael Margraf
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

#include "psdiagram.h"
#include "main.h"

#include <math.h>

// mixed polar and smith diagram
PSDiagram::PSDiagram(int _cx, int _cy, bool _polarUp)
			: Diagram(_cx, _cy)
{
  x1 = 10;       // position of label text
  y1 = y3 = 2;
  x2 = 200;      // initial size of diagram
  x3 = 207;
  y2 = 200;
  if(_polarUp)  Name = "PS";  // polar diagram upper half ?
  else  Name = "SP";

  calcDiagram();
}

PSDiagram::~PSDiagram()
{
}

// ------------------------------------------------------------
void PSDiagram::calcCoordinate(double* &, double* &yD,
			       int *px, int *py, Axis *pa)
{
  double yr = *(yD++);
  double yi = *(yD++);
  *px = (x2>>1)+int(yr/pa->up*double(x2>>1) + 0.5);
  *py = (y2>>1)+int(yi/pa->up*double(y2>>1) + 0.5);
}

// --------------------------------------------------------------
void PSDiagram::calcLimits()
{
  int i;
  double a, b;
  Axis *polarAxis, *smithAxis;

  if(Name == "PS") {
    smithAxis = &yAxis;
    polarAxis = &zAxis;
  }
  else {
    polarAxis = &yAxis;
    smithAxis = &zAxis;
  }

  calcSmithAxisScale(smithAxis, i, i);
  smithAxis->limit_min = 0.0;
  smithAxis->step = double(i);
  smithAxis->limit_max = smithAxis->up;

  calcPolarAxisScale(polarAxis, a, polarAxis->step, b);
  polarAxis->limit_min = 0.0;
  polarAxis->limit_max = polarAxis->up;

}

// --------------------------------------------------------------
int PSDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  x3 = x2 + 7;
  if(Name == "PS") {   // if polar not in upper half -> mirror
    createSmithChart(&yAxis, 5);
    createPolarDiagram(&zAxis, 1);
  }
  else {
    createSmithChart(&zAxis, 3);
    createPolarDiagram(&yAxis, 2);
  }

  // x line
  Lines.append(new Line(0, y2>>1, x2, y2>>1, QPen(QPen::black,0)));
  return 1;
}

// ------------------------------------------------------------
PSDiagram* PSDiagram::newOne()
{
  return new PSDiagram();
}
