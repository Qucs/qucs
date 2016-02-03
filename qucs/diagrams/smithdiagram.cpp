/***************************************************************************
                          smithdiagram.cpp  -  description
                             -------------------
    begin                : Sat Oct 18 2003
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

#if HAVE_CONFIG_H
# include <config.h>
#endif
#include <QtGui>
#include <math.h>
#include <float.h>
#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#include "smithdiagram.h"
#include "main.h"


SmithDiagram::SmithDiagram(int _cx, int _cy, bool ImpMode) : Diagram(_cx, _cy)
{
  x1 = 10;     // position of label text
  y1 = 2;
  x2 = 200;    // initial size of diagram
  y2 = 200;
  y3 = 0;
  x3 = 207;    // with some distance for right axes text
  if(ImpMode)  Name = "Smith";  // with impedance circles
  else  Name = "ySmith";        // with admittance circles

  Arcs.append(new struct Arc(0, y2, x2, y2, 0, 16*360, QPen(Qt::black,0)));
//  calcDiagram();    // calculate circles for smith chart with |r|=1
}

SmithDiagram::~SmithDiagram()
{
}

// ------------------------------------------------------------
// calculate the screen coordinates for the graph data
void SmithDiagram::calcCoordinate(double* &, double* &yD, double* &,
                                  float *px, float *py, Axis*)
{
  double yr = *(yD++);
  double yi = *(yD++);
  *px = float((yr/yAxis.up + 1.0)*double(x2)/2.0);
  *py = float((yi/yAxis.up + 1.0)*double(y2)/2.0);

  if(isfinite(*px))
    if(isfinite(*py))
      return;

  *px = *py = float(cx) / 2.0;
}

// ------------------------------------------------------------
void SmithDiagram::calcLimits()
{
  int a;
  calcSmithAxisScale(&yAxis, a, a);
  yAxis.limit_min = 0.0;
  yAxis.step = double(a);
  yAxis.limit_max = yAxis.up;
}

// ------------------------------------------------------------
// calculate the circles and arcs of the smith chart
int SmithDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  x3 = x2 + 7;
  if(Name.at(0) == 'y')  createSmithChart(&yAxis, 6);
  else  createSmithChart(&yAxis);

  // outer most circle
  Arcs.append(new Arc(0, x2, x2, x2, 0, 16*360, QPen(Qt::black,0)));

  // horizontal line Im(r)=0
  Lines.append(new Line(0, x2>>1, x2, x2>>1, GridPen));

  return 3;
}

// ------------------------------------------------------------
Diagram* SmithDiagram::newOne()
{
  return new SmithDiagram();
}

// ------------------------------------------------------------
Element* SmithDiagram::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Smith Chart");
  BitmapFile = (char *) "smith";

  if(getNewOne)  return new SmithDiagram();
  return 0;
}

// ------------------------------------------------------------
Element* SmithDiagram::info_y(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Admittance Smith");
  BitmapFile = (char *) "ysmith";

  if(getNewOne)  return new SmithDiagram(0, 0, false);
  return 0;
}
