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
#include "main.h"

#include <math.h>

#include <qmessagebox.h>


RectDiagram::RectDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  x1 = 10;      // position of label text
  y1 = y3 = 32;
  x2 = 240;     // initial size of diagram
  y2 = 160;
  x3 = 247;

  Name = "Rect";

  xAxis.log = yAxis.log = zAxis.log = false;
  calcDiagram();
}

RectDiagram::~RectDiagram()
{
}

// ------------------------------------------------------------
int RectDiagram::calcCoordinate(double* &xD, double* &yD,
				 int *px, int *py, Axis *pa)
{
//  int xc, yc;
  double x  = *(xD++);
  double yr = *(yD++);
  double yi = *(yD++);
  if(xAxis.log)
    *px = int(log10(x / xAxis.low)/log10(xAxis.up / xAxis.low)
		*double(x2) + 0.5);
  else  *px = int((x-xAxis.low)/(xAxis.up-xAxis.low)*double(x2) + 0.5);

  if(pa->log)
    *py = int(log10(sqrt(yr*yr + yi*yi)/fabs(pa->low)) /
		log10(pa->up/pa->low) * double(y2) + 0.5);
  else {
    if(fabs(yi) < 1e-250)  // preserve negative values if not complex number
      *py = int((yr-pa->low)/(pa->up-pa->low)*double(y2) + 0.5);
    else   // calculate magnitude of complex number
      *py = int((sqrt(yr*yr + yi*yi)-pa->low)/(pa->up-pa->low)
		*double(y2) + 0.5);
  }

  return regionCode(*px, *py);
}

// --------------------------------------------------------------
void RectDiagram::calcAxisScale(Axis *Axis, double& GridNum, double& zD,
				double& zDstep, double& GridStep, double Dist)
{
  double numGrids, Base, Expo, corr;

if(Axis->autoScale) {
  numGrids = floor(Dist/60.0);   // minimal grid is 60 pixel
  if(numGrids < 1.0) Base = Axis->max-Axis->min;
  else Base = (Axis->max-Axis->min)/numGrids;
  Expo = floor(log10(Base));
  Base = Base/pow(10.0,Expo);        // separate first significant digit
  if(Base < 3.5) {     // use only 1, 2 and 5, which ever is best fitted
    if(Base < 1.5) Base = 1.0;
    else Base = 2.0;
  }
  else {
    if(Base < 7.5) Base = 5.0;
    else { Base = 1.0; Expo++; }
  }
  GridStep = Base * pow(10.0,Expo);   // grid distance in real coordinates
  corr = floor((Axis->max-Axis->min)/GridStep - numGrids);
  if(corr < 0.0) corr++;
  numGrids += corr;     // correct rounding faults


  // upper y boundery ...........................
  zD = fabs(fmod(Axis->max, GridStep));// expand grid to upper diagram edge ?
  GridNum = zD/GridStep;
  if((1.0-GridNum) < 1e-10) GridNum = 0.0;  // fix rounding errors
  if(Axis->max <= 0.0) {
    if(GridNum < 0.3) { Axis->up += zD;  zD = 0.0; }
  }
  else  if(GridNum > 0.7)  Axis->up += GridStep-zD;
        else if(GridNum < 0.1)
	       if(GridNum*Dist >= 1.0)// more than 1 pixel above ?
		 Axis->up += 0.3*GridStep;  // beauty correction


  // lower y boundery ...........................
  zD = fabs(fmod(Axis->min, GridStep));// expand grid to lower diagram edge ?
  GridNum = zD/GridStep;
  if((1.0-GridNum) < 1e-10) zD = GridNum = 0.0;  // fix rounding errors
  if(Axis->min <= 0.0) {
    if(GridNum > 0.7) { Axis->low -= GridStep-zD;  zD = 0.0; }
    else if(GridNum < 0.1)
	   if(GridNum*Dist >= 1.0) { // more than 1 pixel above ?
	     Axis->low -= 0.3*GridStep;   // beauty correction
	     zD += 0.3*GridStep;
	   }
  }
  else {
    if(GridNum > 0.3) {
      zD = GridStep-zD;
      if(GridNum > 0.9) {
	if((1.0-GridNum)*Dist >= 1.0) { // more than 1 pixel above ?
	  Axis->low -= 0.3*GridStep;    // beauty correction
	  zD += 0.3*GridStep;
	}
      }
    }
    else { Axis->low -= zD;  zD = 0.0; }
  }

  GridNum = Axis->low + zD;
  zD /= (Axis->up-Axis->low)/Dist;
}
else {   // user defined limits
  zD = 0.0;
  Axis->low = GridNum = Axis->limit_min;
  Axis->up  = Axis->limit_max;
  GridStep  = Axis->step;
  Expo = floor(log10(Axis->step));
}
  zDstep = GridStep/(Axis->up-Axis->low)*Dist; // grid in pixel
}

// --------------------------------------------------------------
void RectDiagram::calcAxisLogScale(Axis *Axis, int& z, double& zD,
				   double& zDstep, double& corr, int len)
{
  double Base, Expo;

  if(Axis->autoScale) {
    Expo = floor(log10(Axis->max));
    Base = Axis->max/pow(10.0,Expo);
    if(Base > 3.0001) Axis->up = pow(10.0,Expo+1.0);
    else  if(Base < 1.0001) Axis->up = pow(10.0,Expo);
	  else Axis->up = 3.0 * pow(10.0,Expo);

    Expo = floor(log10(Axis->min));
    Base = Axis->min/pow(10.0,Expo);
    if(Base < 2.999) Axis->low = pow(10.0,Expo);
    else  if(Base > 9.999) Axis->low = pow(10.0,Expo+1.0);
	  else Axis->low = 3.0 * pow(10.0,Expo);

    corr = double(len) / log10(Axis->up / Axis->low);

    z = 0;
    zD = Axis->low;
    zDstep = pow(10.0,Expo);
  }
  else {
    Axis->low = Axis->limit_min;
    Axis->up  = Axis->limit_max;
    Expo = floor(log10(Axis->low));
    Base = ceil(Axis->low/pow(10.0,Expo));
    zD = Base * pow(10.0, Expo);
    zDstep = pow(10.0,Expo);

    corr = double(len) / log10(Axis->up / Axis->low);
    if(zD > 9.5*zDstep)  zDstep *= 10.0;
    z = int(corr*log10(zD / Axis->low) + 0.5); // int(..) implies floor(..)
  }
}

// --------------------------------------------------------------
bool RectDiagram::calcYAxis(Axis *Axis, int x0)
{
  int z;
  double GridStep, corr, zD, zDstep, GridNum;

  QSize r;
  QString tmp;
  QFontMetrics  metrics(QucsSettings.font);
  int maxWidth = 0;
  int Prec;
  char form;

  bool set_log = false;
if(Axis->log) {
  if(Axis->max*Axis->min <= 0.0)  return false;  // invalid
  if(Axis->max < 0.0) {   // for negative values
    corr = Axis->min;
    Axis->min = -Axis->max;
    Axis->max = -corr;
    corr = Axis->low;
    Axis->low = -Axis->up;
    Axis->up  = -corr;
    set_log = true;
  }

  calcAxisLogScale(Axis, z, zD, zDstep, corr, y2);

  if(set_log) z = y2;
  while((z <= y2) && (z >= 0)) {    // create all grid lines
    if(Axis->GridOn)  if(z < y2)  if(z > 0)
      Lines.append(new Line(0, z, x2, z, GridPen));  // y grid

    if((zD < 1.5*zDstep) || (z == 0)) {
      if(fabs(log10(zD)) < 3.0)  tmp = StringNum(zD);
      else  tmp = StringNum(zD, 'e',1);
      if(set_log)  tmp = '-'+tmp;

      r = metrics.size(0, tmp);  // width of text
      if(maxWidth < r.width()) maxWidth = r.width();
      if(x0 > 0)
        Texts.append(new Text(x0+7, z+6, tmp)); // text aligned left
      else
        Texts.append(new Text(-r.width()-7, z+6, tmp)); // text aligned right

      // y marks
      Lines.append(new Line(x0-5, z, x0+5, z, QPen(QPen::black,0)));
    }

    zD += zDstep;
    if(zD > 9.5*zDstep)  zDstep *= 10.0;
    z = int(corr*log10(zD / Axis->low) + 0.5); // int(..) implies floor(..)
    if(set_log)  z = y2 - z;
  }
  if(set_log) {   // set back values ?
    corr = Axis->min;
    Axis->min = -Axis->max;
    Axis->max = -corr;
    corr = Axis->low;
    Axis->low = -Axis->up;
    Axis->up  = -corr;
  }
}
else {  // not logarithmical
  calcAxisScale(Axis, GridNum, zD, zDstep, GridStep, double(y2));


  double Expo;
  if(Axis->up == 0.0)  Expo = log10(fabs(Axis->up-Axis->low));
  else  Expo = log10(fabs(Axis->up));
  if(fabs(Expo) < 3.0)  { form = 'g';  Prec = 3; }
  else  { form = 'e';  Prec = 0; }

  zD += 0.5;     // perform rounding
  z = int(zD);   //  "int(...)" implies "floor(...)"
  while(z <= y2) {  // create all grid lines
    if(fabs(GridNum) < 0.01*pow(10.0, Expo)) GridNum = 0.0;// make 0 really 0
    tmp = StringNum(GridNum, form, Prec);

    r = metrics.size(0, tmp);  // width of text
    if(maxWidth < r.width()) maxWidth = r.width();
    if(x0 > 0)
      Texts.append(new Text(x0+8, z+6, tmp));  // text aligned left
    else
      Texts.append(new Text(-r.width()-7, z+6, tmp));  // text aligned right
    GridNum += GridStep;

    if(Axis->GridOn)  if(z < y2)  if(z > 0)
      Lines.append(new Line(0, z, x2, z, GridPen));  // y grid
    Lines.append(new Line(x0-5, z, x0+5, z, QPen(QPen::black,0))); // y marks
    zD += zDstep;
    z = int(zD);
  }
} // of "if(ylog) ... else ..."
  if(x0 == 0)  x1 = maxWidth+14;
  else  x3 = x2+maxWidth+14;
  return true;
}

// --------------------------------------------------------------
int RectDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  if(fabs(xAxis.max-xAxis.min) < 1e-200) {
    xAxis.max += fabs(xAxis.max);
    xAxis.min -= fabs(xAxis.min);
  }
  if(fabs(yAxis.max-yAxis.min) < 1e-200) {
    yAxis.max += fabs(yAxis.max);
    yAxis.min -= fabs(yAxis.min);
  }
  if(fabs(zAxis.max-zAxis.min) < 1e-200) {
    zAxis.max += fabs(zAxis.max);
    zAxis.min -= fabs(zAxis.min);
  }
  if(yAxis.max == 0) if(yAxis.min == 0) {
    yAxis.max = 1;
    yAxis.min = -1;
  }
  if(zAxis.max == 0) if(zAxis.min == 0) {
    zAxis.max = 1;
    zAxis.min = -1;
  }
  if(xAxis.max == 0) if(xAxis.min == 0) {
    xAxis.max = 1;
    xAxis.min = -1;
  }
  xAxis.low = xAxis.min; xAxis.up = xAxis.max;
  yAxis.low = yAxis.min; yAxis.up = yAxis.max;
  zAxis.low = zAxis.min; zAxis.up = zAxis.max;


  int z;
  double GridStep, corr, zD, zDstep, GridNum;

  QString tmp;
  bool set_log = false;
  int  Prec, valid = 0;
  char form;

  // ====  x grid  =======================================================
if(xAxis.log) {
  if(xAxis.max*xAxis.min <= 0.0) goto Frame;  // invalid
  if(xAxis.max < 0.0) {
    corr = xAxis.min;
    xAxis.min = -xAxis.max;
    xAxis.max = -corr;
    corr = xAxis.low;
    xAxis.low = -xAxis.up;
    xAxis.up  = -corr;
    set_log = true;
  }

  calcAxisLogScale(&xAxis, z, zD, zDstep, corr, x2);

  if(set_log) z = x2;
  while((z <= x2) && (z >= 0)) {    // create all grid lines
    if(xAxis.GridOn)  if(z < x2)  if(z > 0)
      Lines.append(new Line(z, y2, z, 0, GridPen));  // x grid

    if((zD < 1.5*zDstep) || (z == 0) || (z == x2)) {
      if(fabs(log10(zD)) < 3.0)  tmp = StringNum(zD);
      else  tmp = StringNum(zD, 'e', 1);

      if(set_log)  Texts.append(new Text(z-10, -5, '-'+tmp));
      else  Texts.append(new Text(z-10, -5, tmp));

      Lines.append(new Line(z, 5, z, -5, QPen(QPen::black,0)));  // x marks
    }

    zD += zDstep;
    if(zD > 9.5*zDstep)  zDstep *= 10.0;
    z = int(corr*log10(zD / xAxis.low) + 0.5); // int(..) implies floor(..)
    if(set_log)  z = x2 - z;
  }
  if(set_log) {   // set back values ?
    corr = xAxis.min;
    xAxis.min = -xAxis.max;
    xAxis.max = -corr;
    corr = xAxis.low;
    xAxis.low = -xAxis.up;
    xAxis.up  = -corr;
  }
}
else {  // not logarithmical
  calcAxisScale(&xAxis, GridNum, zD, zDstep, GridStep, double(x2));

  double Expo;
  if(xAxis.up == 0.0)  Expo = log10(fabs(xAxis.up-xAxis.low));
  else  Expo = log10(fabs(xAxis.up));
  if(fabs(Expo) < 3.0)  { form = 'g';  Prec = 3; }
  else  { form = 'e';  Prec = 1; }

  zD += 0.5;     // perform rounding
  z = int(zD);   //  "int(...)" implies "floor(...)"
  while(z <= x2) {    // create all grid lines
    if(fabs(GridNum) < 0.01*pow(10.0, Expo)) GridNum = 0.0;// make 0 really 0
    Texts.append(new Text(z-10, -5, StringNum(GridNum, form, Prec)));
    GridNum += GridStep;

    if(xAxis.GridOn)  if(z < x2)  if(z > 0)
      Lines.append(new Line(z, y2, z, 0, GridPen)); // x grid
    Lines.append(new Line(z, 5, z, -5, QPen(QPen::black,0)));   // x marks
    zD += zDstep;
    z = int(zD);
  }
} // of "if(xlog) ... else ..."



  // ====  y grid  =======================================================
  if(zAxis.numGraphs > 0) if(calcYAxis(&zAxis, x2)) valid |= 1;
  if(yAxis.numGraphs > 0) if(calcYAxis(&yAxis, 0))  valid |= 2;
  if(valid == 0)  goto Frame;


  // outer frame
  Lines.append(new Line(0,  y2, x2, y2, QPen(QPen::black,0)));
  Lines.append(new Line(x2, y2, x2,  0, QPen(QPen::black,0)));
  Lines.append(new Line(0,   0, x2,  0, QPen(QPen::black,0)));
  Lines.append(new Line(0,  y2,  0,  0, QPen(QPen::black,0)));
  return valid;

Frame:
  // outer frame
  Lines.append(new Line(0,  y2, x2, y2, QPen(QPen::red,0)));
  Lines.append(new Line(x2, y2, x2,  0, QPen(QPen::red,0)));
  Lines.append(new Line(0,   0, x2,  0, QPen(QPen::red,0)));
  Lines.append(new Line(0,  y2,  0,  0, QPen(QPen::red,0)));
  return 0;
}

// ------------------------------------------------------------
RectDiagram* RectDiagram::newOne()
{
  return new RectDiagram();
}
