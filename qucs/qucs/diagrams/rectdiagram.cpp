/***************************************************************************
                          rectdiagram.cpp  -  description
                             -------------------
    begin                : Thu Oct 2 2003
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

#include "rectdiagram.h"
#include "../main.h"

#include <math.h>

#include <qmessagebox.h>


RectDiagram::RectDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  x1 = 10;     // position of label text
  y1 = 32;
  x2 = 240;    // initial size of diagram
  y2 = 160;

  GridOn = true;
  Name = "Rect";

  xLabel = "";
  yLabel = "";

  calcDiagram();
}

RectDiagram::~RectDiagram()
{
}

// ------------------------------------------------------------
void RectDiagram::calcData(Graph *g)
{
  int *p = g->Points;
//  if(p == 0) return;
  double *px;
  double *py = g->cPointsY;
  for(int i=g->countY; i>0; i--) {
    px = g->cPointsX.getFirst()->Points;
    for(int z=g->cPointsX.getFirst()->count; z>0; z--) {
      *(p++) = int(((*(px++))-xlow)/(xup-xlow)*double(x2) + 0.5);
      // preserve negative values if not complex number
      if(fabs(*(py+1)) < 1e-250) {
	*(p++) = int(((*(py++))-ylow)/(yup-ylow)*double(y2) + 0.5);
	py++;   // do not use imaginary part
      }
      else {  // calculate magnitude of complex number
	*(p++) = int((sqrt((*py)*(*py) +
		(*(py+1))*(*(py+1)))-ylow)/(yup-ylow)*double(y2) + 0.5);
        py += 2;
      }
    }
  }
}

// --------------------------------------------------------------
void RectDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  if(fabs(xmax-xmin) < 1e-200) { xmax += fabs(xmax); xmin -= fabs(xmin); }
  if(fabs(ymax-ymin) < 1e-200) { ymax += fabs(ymax); ymin -= fabs(ymin); }
  if(ymax == 0) if(ymin == 0) { ymax = 1; ymin = -1; }
  if(xmax == 0) if(xmin == 0) { xmax = 1; xmin = -1; }
  xlow = xmin;  xup = xmax;
  ylow = ymin;  yup = ymax;


  int z;
  double numGrids, Base, Expo, GridStep, corr, zD, zDstep, GridNum;
  // ====  x grid  =======================================================
  numGrids = floor(double(x2)/60.0);   // minimal grid is 60 pixel
  if(numGrids < 1.0) Base = xmax-xmin;
  else Base = (xmax-xmin)/numGrids;
  Expo = floor(log10(Base));
  Base = Base/pow(10.0,Expo);         // separate first significant digit
  if(Base < 3.5) {      // use only 1, 2 and 5, which ever is best fitted
    if(Base < 1.5) Base = 1.0;
    else Base = 2.0;
  }
  else {
    if(Base < 7.5) Base = 5.0;
    else { Base = 1.0; Expo++; }
  }
  GridStep = Base * pow(10.0,Expo); // grid distance (real coordinates)
  corr = floor((xmax-xmin)/GridStep - numGrids);
  if(corr < 0.0) corr++;
  numGrids += corr;     // correct rounding faults


  // upper x boundery ...........................
  zD = fabs(fmod(xmax, GridStep)); // expand grid to upper edge of diagram ?
  GridNum = zD/GridStep;
  if((1.0-GridNum) < 1e-10) GridNum = 0.0;  // fix rounding errors
  if(xmax <= 0.0) {
    if(GridNum < 0.3) { xup += zD;  zD = 0.0; }
  }
  else  if(GridNum > 0.7)  xup += GridStep-zD;
        else if(GridNum < 0.1)
	       if(GridNum*double(x2) >= 1.0)// more than 1 pixel above ?
		 xup += 0.3*GridStep;  // beauty correction


  // lower x boundery ...........................
  zD = fabs(fmod(xmin, GridStep)); // expand grid to lower edge of diagram ?
  GridNum = zD/GridStep;
  if((1.0-GridNum) < 1e-10) zD = GridNum = 0.0;  // fix rounding errors
  if(xmin <= 0.0) {
    if(GridNum > 0.7) { xlow -= GridStep-zD;  zD = 0.0; }
    else if(GridNum < 0.1)
	   if(GridNum*double(x2) >= 1.0) { // more than 1 pixel above ?
	     xlow -= 0.3*GridStep;    // beauty correction
	     zD   += 0.3*GridStep;
	   }
  }
  else {
    if(GridNum > 0.3) {
      zD = GridStep-zD;
      if(GridNum > 0.9) {
	if((1.0-GridNum)*double(x2) >= 1.0) { // more than 1 pixel above ?
	  xlow -= 0.3*GridStep;    // beauty correction
	  zD   += 0.3*GridStep;
	}
      }
    }
    else { xlow -= zD;  zD = 0.0; }
  }


  zDstep = GridStep/(xup-xlow)*double(x2);  // grid distance in pixel
  GridNum  = xlow + zD;
  zD /= (xup-xlow)/double(x2);

  zD += 0.5;     // perform rounding
  z = int(zD);   //  "int(...)" implies "floor(...)"
  while(z <= x2) {    // create all grid lines
    if(fabs(GridNum) < 0.01*pow(10.0, Expo)) GridNum = 0.0;// make 0 really 0
    if(fabs(Expo) < 3.0)
      Texts.append(new Text(z-10, -17, QString::number(GridNum)));
    else
      Texts.append(new Text(z-10, -17, QString::number(GridNum, 'e', 1)));
    GridNum += GridStep;

    if(GridOn)  if(z < x2)  if(z > 0)
      Lines.append(new Line(z, y2, z, 0, QPen(QPen::lightGray,0))); // x grid
    Lines.append(new Line(z, 5, z, -5, QPen(QPen::black,0)));   // x marks
    zD += zDstep;
    z = int(zD);
  }


  // ====  y grid  =======================================================
  numGrids = floor(double(y2)/60.0);   // minimal grid is 60 pixel
  if(numGrids < 1.0) Base = ymax-ymin;
  else Base = (ymax-ymin)/numGrids;
  Expo = floor(log10(Base));
  Base = Base/pow(10.0,Expo);        // separate first significant digit
  if(Base < 3.5) {            // use only 1, 2 and 5, which ever is best fitted
    if(Base < 1.5) Base = 1.0;
    else Base = 2.0;
  }
  else {
    if(Base < 7.5) Base = 5.0;
    else { Base = 1.0; Expo++; }
  }
  GridStep = Base * pow(10.0,Expo);   // grid distance in real coordinates
  corr = floor((ymax-ymin)/GridStep - numGrids);
  if(corr < 0.0) corr++;
  numGrids += corr;     // correct rounding faults


  // upper y boundery ...........................
  zD = fabs(fmod(ymax, GridStep)); // expand grid to upper edge of diagram ?
  GridNum = zD/GridStep;
  if((1.0-GridNum) < 1e-10) GridNum = 0.0;  // fix rounding errors
  if(ymax <= 0.0) {
    if(GridNum < 0.3) { yup += zD;  zD = 0.0; }
  }
  else  if(GridNum > 0.7)  yup += GridStep-zD;
        else if(GridNum < 0.1)
	       if(GridNum*double(y2) >= 1.0)// more than 1 pixel above ?
		 yup += 0.3*GridStep;  // beauty correction


  // lower y boundery ...........................
  zD = fabs(fmod(ymin, GridStep)); // expand grid to lower edge of diagram ?
  GridNum = zD/GridStep;
  if((1.0-GridNum) < 1e-10) zD = GridNum = 0.0;  // fix rounding errors
  if(ymin <= 0.0) {
    if(GridNum > 0.7) { ylow -= GridStep-zD;  zD = 0.0; }
    else if(GridNum < 0.1)
	   if(GridNum*double(y2) >= 1.0) { // more than 1 pixel above ?
	     ylow -= 0.3*GridStep;    // beauty correction
	     zD   += 0.3*GridStep;
	   }
  }
  else {
    if(GridNum > 0.3) {
      zD = GridStep-zD;
      if(GridNum > 0.9) {
	if((1.0-GridNum)*double(y2) >= 1.0) { // more than 1 pixel above ?
	  ylow -= 0.3*GridStep;    // beauty correction
	  zD   += 0.3*GridStep;
	}
      }
    }
    else { ylow -= zD;  zD = 0.0; }
  }

  zDstep = GridStep/(yup-ylow)*double(y2); // distance between grids in pixel
  GridNum  = ylow + zD;
  zD /= (yup-ylow)/double(y2);

  QRect r;
  QString tmp;
  QFontMetrics  metrics(QucsSettings.font);

  int maxWidth = 0;
  zD += 0.5;     // perform rounding
  z = int(zD);   //  "int(...)" implies "floor(...)"
  while(z <= y2) {    // create all grid lines
    if(fabs(GridNum) < 0.01*pow(10.0, Expo)) GridNum = 0.0;// make 0 really 0
    if(fabs(Expo) < 3.0)  tmp = QString::number(GridNum);
    else tmp = QString::number(GridNum, 'e',1);
    r = metrics.boundingRect(0,0,0,0, Qt::AlignAuto, tmp); // width of text
    if(maxWidth < r.width()) maxWidth = r.width();

    Texts.append(new Text(-r.width()-7, z-5, tmp));  // text aligned right
    GridNum += GridStep;

    if(GridOn)  if(z < y2)  if(z > 0)
      Lines.append(new Line(0, z, x2, z, QPen(QPen::lightGray,0))); // y grid
    Lines.append(new Line(-5, z, 5, z, QPen(QPen::black,0)));   // y marks
    zD += zDstep;
    z = int(zD);
  }
  x1 = maxWidth+14;

  // outer frame
  Lines.append(new Line(0,  y2, x2, y2, QPen(QPen::black,0)));
  Lines.append(new Line(x2, y2, x2,  0, QPen(QPen::black,0)));
  Lines.append(new Line(0,   0, x2,  0, QPen(QPen::black,0)));
  Lines.append(new Line(0,  y2,  0,  0, QPen(QPen::black,0)));
}

// ------------------------------------------------------------
RectDiagram* RectDiagram::newOne()
{
  return new RectDiagram();
}
