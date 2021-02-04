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

/*!
  \class PolarDiagram
  \brief The PolarDiagram class implements the Polar diagram
*/

#if HAVE_CONFIG_H
# include <config.h>
#endif
#include <cmath>
#include <float.h>
#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#include "polardiagram.h"
#include "obsolete_paintings.h"


PolarDiagram::PolarDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  x1 = 10;     // position of label text
  y1 = 2;
  x2 = 200;    // initial size of diagram
  y2 = 200;
  x3 = 207;    // with some distance for right axes text
  Name = "Polar";

  Arcs.append(new struct Arc(0, y2, x2, y2, 0, 16*360, QPen(Qt::black,0)));
//  calcDiagram();
}

PolarDiagram::~PolarDiagram()
{
}

// ------------------------------------------------------------
void PolarDiagram::calcCoordinate(const double*, const double* yD, const double*,
				  float *px, float *py, Axis const*) const
{
  double yr = yD[0];
  double yi = yD[1];
  *px = float((yr/yAxis.up + 1.0)*double(x2)/2.0);
  *py = float((yi/yAxis.up + 1.0)*double(y2)/2.0);

  if(std::isfinite(*px))
    if(std::isfinite(*py))
      return;

  *px = *py = float(cx()) / 2.0;
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

  x3 = x2 + 7;
  createPolarDiagram(&yAxis);
  return 3;
}

// ------------------------------------------------------------
Element* PolarDiagram::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Polar");
  BitmapFile = (char *) "polar";

  if(getNewOne)  return new PolarDiagram();
  return 0;
}

#if 0
void Diagram::calcPolarAxisScale(Axis *Axis, double& numGrids,
				 double& GridStep, double& zD)
{ untested();
  if(Axis->autoScale) {  // auto-scale or user defined limits ?
    double Expo, Base;
    numGrids = floor(double(x2)/80.0); // minimal grid is 40 pixel
    Expo = floor(log10(Axis->max/numGrids));
    Base = Axis->max/numGrids/pow(10.0,Expo);// get first significant digit
    if(Base < 3.5) {       // use only 1, 2 and 5, which ever is best fitted
      if(Base < 1.5) Base = 1.0;
      else Base = 2.0;
    }
    else { untested();
      if(Base < 7.5) Base = 5.0;
      else { Base = 1.0; Expo++; }
    }
    GridStep = Base * pow(10.0,Expo); // grid distance in real values
    numGrids -= floor(numGrids - Axis->max/GridStep); // correct num errors
    Axis->up = GridStep*numGrids;

    zD = double(x2) / numGrids;   // grid distance in pixel
  }
  else {   // no auto-scale
    Axis->up = Axis->limit_max = fabs(Axis->limit_max);
    GridStep = Axis->step;
    zD = double(x2) / Axis->limit_max * Axis->step; // grid distance in pixel

    if(fabs(zD) < 2.0) {  // if grid too small, then no grid
      zD = double(x2);
      GridStep = Axis->step = Axis->up;
      numGrids = 1.0;
    }
    else numGrids = Axis->limit_max / Axis->step;
  }
}
#endif
#if 0 // from diagramdialog.
void Diagram::createPolarDiagram(Axis *Axis, int Mode)
{ untested();
  xAxis.low  = xAxis.min;
  xAxis.up   = xAxis.max;
  Axis->low = 0.0;
  if(fabs(Axis->min) > Axis->max)
    Axis->max = fabs(Axis->min);  // also fit negative values


  bool Above  = ((Mode & 1) == 1);  // paint upper half ?
  bool Below  = ((Mode & 2) == 2);  // paint lower half ?

  int i, z, tmp;
  if(Above)  i = y2;  else  i = y2>>1;
  if(Below)  z = 0;   else  z = y2>>1;
  // y line
  Lines.append(new Line(x2>>1, i, x2>>1, z, GridPen));

  int len  = 0;       // arc length
  int beta = 16*180;  // start angle
  if(Above)  { beta = 0;  len = 16*180; }
  if(Below)  len += 16*180;

  int phi, tPos;
  int tHeight = QucsSettings.font.pointSize() + 5;
  if(!Below)  tPos = (y2>>1) + 3;
  else  tPos = (y2>>1) - tHeight + 3;

  double Expo, Base, numGrids, GridStep, zD;
  if(xAxis.GridOn) { untested();
    calcPolarAxisScale(Axis, numGrids, GridStep, zD);

    double zDstep = zD;
    double GridNum  = 0.0;
    for(i=int(numGrids); i>1; i--) {    // create all grid circles
      z = int(zD);
      GridNum += GridStep;
      Texts.append(new Text(((x2+z)>>1)-10, tPos, misc::StringNiceNum(GridNum)));

      phi = int(16.0*180.0/pi*atan(double(2*tHeight)/zD));
      if(!Below)  tmp = beta + phi;
      else  tmp = beta;
      Arcs.append(new struct Arc((x2-z)>>1, (y2+z)>>1, z, z, tmp, len-phi,
			  GridPen));
      zD += zDstep;
    }
  }
  else {  // of  "if(GridOn)"
    Expo = floor(log10(Axis->max));
    Base = ceil(Axis->max/pow(10.0,Expo) - 0.01);
    Axis->up = Base * pow(10.0,Expo);  // separate Base * 10^Expo
  }

  // create outer circle
  Texts.append(new Text(x2-8, tPos, misc::StringNiceNum(Axis->up)));
  phi = int(16.0*180.0/pi*atan(double(2*tHeight)/double(x2)));
  if(!Below)  tmp = phi;
  else  tmp = 0;
  Arcs.append(new struct Arc(0, y2, x2, y2, tmp, 16*360-phi, QPen(Qt::black,0)));

  // get size of text using the screen-compatible metric
  FontMetrics metrics;
  QSize r = metrics.size(0, Texts.last()->s);  // width of text
  len = x2+r.width()-4;   // more space at the right
  if(len > x3)  x3 = len;
}
#endif
