/***************************************************************************
                          polardiagram.cpp  -  description
                             -------------------
    begin                : Fri Oct 17 2003
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

#include "polardiagram.h"
#include "main.h"

#include <math.h>


PolarDiagram::PolarDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  x1 = 10;     // position of label text
  y1 = 16;
  x2 = 200;    // initial size of diagram
  y2 = 200;
  Name = "Polar";

  calcDiagram();
}

PolarDiagram::~PolarDiagram()
{
}

// ------------------------------------------------------------
void PolarDiagram::calcCoordinate(double* &, double* &yD,
				  int *px, int *py, Axis*)
{
  double yr = *(yD++);
  double yi = *(yD++);
  *px = (x2>>1)+int(yr/yAxis.up*double(x2>>1) + 0.5);
  *py = (y2>>1)+int(yi/yAxis.up*double(y2>>1) + 0.5);
}

// --------------------------------------------------------------
void PolarDiagram::calcLimits()
{
  double a, b;
  calcPolarAxisScale(&yAxis, a, yAxis.step, b);
  yAxis.limit_min = 0.0;
  yAxis.limit_max = yAxis.up;
}

// --------------------------------------------------------------
int PolarDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  // x line
  Lines.append(new Line(0, y2>>1, x2, y2>>1, GridPen));

  createPolarDiagram(&yAxis);
  return 1;
}

// ------------------------------------------------------------
PolarDiagram* PolarDiagram::newOne()
{
  return new PolarDiagram();
}
