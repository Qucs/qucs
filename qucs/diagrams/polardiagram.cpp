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

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "polardiagram.h"

#include <math.h>


PolarDiagram::PolarDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  x1 = 10;     // position of label text
  y1 = 16;
  x2 = 200;    // initial size of diagram
  y2 = 200;

  GridOn = true;
  Name = "Polar";

  calcDiagram();
}

PolarDiagram::~PolarDiagram()
{
}

// ------------------------------------------------------------
void PolarDiagram::calcData(Graph *g)
{
  int *p = g->Points;
//  if(p == 0) return;
  double *py = g->cPointsY;
  for(int z=g->count; z>0; z--) {
    *(p++) = (x2>>1)+int((*(py++))/xup*double(x2>>1));
    *(p++) = (y2>>1)+int((*(py++))/yup*double(y2>>1));
  }
}

// --------------------------------------------------------------
void PolarDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  // x and y line
  Lines.append(new Line(x2>>1, y2, x2>>1, 0, QPen(QPen::lightGray,1)));
  Lines.append(new Line(0, y2>>1, x2, y2>>1, QPen(QPen::lightGray,1)));

  double phi, Expo, Base;
  xlow = ylow = 0.0;
  if(fabs(ymin) > ymax)  ymax = fabs(ymin);  // also fit negative values


  if(GridOn) {

    double numGrids = floor(double(x2)/80.0); // minimal grid is 40 pixel
    Expo = floor(log10(ymax/numGrids));
    Base = ymax/numGrids/pow(10.0,Expo); // separate first significant digit
    if(Base < 3.5) {       // use only 1, 2 and 5, which ever is best fitted
      if(Base < 1.5) Base = 1.0;
      else Base = 2.0;
    }
    else {
      if(Base < 7.5) Base = 5.0;
      else { Base = 1.0; Expo++; }
    }
    double GridStep = Base * pow(10.0,Expo);     // distance between grids in real coordinates
    numGrids -= floor(numGrids - ymax/GridStep); // corrects rounding faults
    xup = yup = GridStep*numGrids;
    double zD = double(x2) / numGrids;     // distance between grids in pixel

/*  ---------------------------------------------------------------------
    ---- This algorithm is not bad but creates unpleasant grid steps ----
    ---------------------------------------------------------------------
    double numGrids = floor(double(x2)/80);      // minimal grid is 40 pixel
    Expo = floor(log10(ymax/numGrids));
    Base = floor(ymax/numGrids/pow(10,Expo)+0.5);   // separate first significant digit
    double GridStep = Base * pow(10,Expo);     // distance between grids in real coordinates
    numGrids -= floor((GridStep*numGrids-ymax) / GridStep);     // correct rounding faults
    xup = yup = GridStep*numGrids;
    double zD = double(x2) / numGrids;     // distance between grids in pixel
    --------------------------------------------------------------------- */

    int z;
    double zDstep = zD;
    double GridNum  = 0.0;
    for(int i=int(numGrids); i>1; i--) {    // create all grid circles
      z = int(zD);
      GridNum += GridStep;
      if(fabs(Expo) < 3.0)
        Texts.append(new Text(((x2+z)>>1)-10, (y2>>1)-12, QString::number(GridNum)));
      else
        Texts.append(new Text(((x2+z)>>1)-10, (y2>>1)-12, QString::number(GridNum, 'e', 0)));

      phi = 16.0*180.0/M_PIl*atan(30.0/zD);    // 2*(text height+3) / circle radius
      Arcs.append(new Arc((x2-z)>>1, (y2+z)>>1, z, z, 0, 16*360-int(phi), QPen(QPen::lightGray,1)));
      zD += zDstep;
    }
  }
  else {
    Expo = floor(log10(ymax));
    Base = ceil(ymax/pow(10.0,Expo) - 0.01);
    xup  = yup  = Base * pow(10.0,Expo);       // separate number into Base * 10^Expo
  }

  // create outer circle
  if(fabs(Expo) < 3.0)
    Texts.append(new Text(x2-10, (y2>>1)-12, QString::number(xup)));
  else
    Texts.append(new Text(x2-10, (y2>>1)-12, QString::number(xup, 'e', 0)));
  phi = 16.0*180.0/M_PIl*atan(30.0/double(x2));    // (text height+3) / circle radius
  Arcs.append(new Arc(0, y2, x2, y2, 0, 16*360-int(phi), QPen(QPen::black,1)));
}

// ------------------------------------------------------------
PolarDiagram* PolarDiagram::newOne()
{
  return new PolarDiagram();
}
