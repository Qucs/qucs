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
  y1 = y3 = 33;
  x2 = 240;    // initial size of diagram
  y2 = 160;
  x3 = 247;    // with some distance for right axes text

  Name = "Rect";
  calcDiagram();
}

RectDiagram::~RectDiagram()
{
}

// ------------------------------------------------------------
void RectDiagram::calcCoordinate(double* &xD, double* &yD,
				 int *px, int *py, Axis *pa)
{
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
}

// --------------------------------------------------------------
bool RectDiagram::calcAxisScale(Axis *Axis, double& GridNum, double& zD,
				double& zDstep, double& GridStep, double Dist)
{
  bool back=false;
  if(fabs(Axis->max-Axis->min) < 1e-200) { // if max = min, double difference
    Axis->max += fabs(Axis->max);
    Axis->min -= fabs(Axis->min);
  }
  if(Axis->max == 0) if(Axis->min == 0) {
    Axis->max = 1;
    Axis->min = -1;
  }
  Axis->low = Axis->min; Axis->up = Axis->max;

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
    else { Axis->low -= zD; zD = 0.0; }
  }

  GridNum = Axis->low + zD;
  zD /= (Axis->up-Axis->low)/Dist;
}
else {   // user defined limits
  zD = 0.0;
  Axis->low = GridNum = Axis->limit_min;
  Axis->up  = Axis->limit_max;
  if(Axis->limit_max < Axis->limit_min)
    back = true;
  GridStep  = Axis->step;
}

  zDstep = GridStep/(Axis->up-Axis->low)*Dist; // grid in pixel
  return back;
}

// --------------------------------------------------------------
bool RectDiagram::calcAxisLogScale(Axis *Axis, int& z, double& zD,
				   double& zDstep, double& corr, int len)
{
  if(fabs(Axis->max-Axis->min) < 1e-200) { // if max = min, double difference
    Axis->max += fabs(Axis->max);
    Axis->min -= fabs(Axis->min);
  }
  if(Axis->max == 0) if(Axis->min == 0) {
    Axis->max = 1;
    Axis->min = -1;
  }
  Axis->low = Axis->min; Axis->up = Axis->max;

  if(!Axis->autoScale) {
    Axis->low = Axis->limit_min;
    Axis->up  = Axis->limit_max;
  }


  bool mirror=false, mirror2=false;
  double tmp;
  if(Axis->up < 0.0) {   // for negative values
    tmp = Axis->low;
    Axis->low = -Axis->up;
    Axis->up  = -tmp;
    mirror = true;
  }

  double Base, Expo;
  if(Axis->autoScale) {
    if(mirror) {   // set back values ?
      tmp = Axis->min;
      Axis->min = -Axis->max;
      Axis->max = -tmp;
    }

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

    if(mirror) {   // set back values ?
      tmp = Axis->min;
      Axis->min = -Axis->max;
      Axis->max = -tmp;
    }
  }
  else {   // user defined limits
    if(Axis->up < Axis->low) {
      tmp = Axis->low;
      Axis->low = Axis->up;
      Axis->up  = tmp;
      mirror2 = true;
    }

    Expo = floor(log10(Axis->low));
    Base = ceil(Axis->low/pow(10.0,Expo));
    zD = Base * pow(10.0, Expo);
    zDstep = pow(10.0,Expo);
    if(zD > 9.5*zDstep)  zDstep *= 10.0;

    corr = double(len) / log10(Axis->up / Axis->low);
    z = int(corr*log10(zD / Axis->low) + 0.5); // int(..) implies floor(..)

    if(mirror2) {   // set back values ?
      tmp = Axis->low;
      Axis->low = Axis->up;
      Axis->up  = tmp;
    }
  }

  if(mirror) {   // set back values ?
    tmp = Axis->low;
    Axis->low = -Axis->up;
    Axis->up  = -tmp;
  }

  if(mirror == mirror2)  return false;
  else  return true;
}

// --------------------------------------------------------------
void RectDiagram::calcLimits()
{
  int i;
  double a, b, c;

  if(xAxis.autoScale) {// check before, to preserve limit exchange (max < min)
    if(xAxis.log) {
      calcAxisLogScale(&xAxis, i, a, b, c, x2);
      xAxis.step = 1.0;
    }
    else  calcAxisScale(&xAxis, a, b, c, xAxis.step, double(x2));
    xAxis.limit_min = xAxis.low;
    xAxis.limit_max = xAxis.up;
  }

  if(yAxis.autoScale) {// check before, to preserve limit exchange (max < min)
    if(yAxis.log) {
      calcAxisLogScale(&yAxis, i, a, b, c, y2);
      yAxis.step = 1.0;
    }
    else  calcAxisScale(&yAxis, a, b, c, yAxis.step, double(y2));
    yAxis.limit_min = yAxis.low;
    yAxis.limit_max = yAxis.up;
  }

  if(zAxis.autoScale) {// check before, to preserve limit exchange (max < min)
    if(zAxis.log) {
      calcAxisLogScale(&zAxis, i, a, b, c, y2);
      zAxis.step = 1.0;
    }
    else  calcAxisScale(&zAxis, a, b, c, zAxis.step, double(y2));
    zAxis.limit_min = zAxis.low;
    zAxis.limit_max = zAxis.up;
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

  bool back = false;
if(Axis->log) {
  if(Axis->autoScale) {
    if(Axis->max*Axis->min <= 0.0)  return false;  // invalid
  }
  else  if(Axis->limit_min*Axis->limit_max <= 0.0)  return false;  // invalid

  back = calcAxisLogScale(Axis, z, zD, zDstep, corr, y2);

  if(back) z = y2;
  while((z <= y2) && (z >= 0)) {    // create all grid lines
    if(Axis->GridOn)  if(z < y2)  if(z > 0)
      Lines.prepend(new Line(0, z, x2, z, GridPen));  // y grid

    if((zD < 1.5*zDstep) || (z == 0)) {
      if(fabs(log10(zD)) < 3.0)  tmp = StringNum(zD);
      else  tmp = StringNum(zD, 'e', 1);
      if(Axis->up < 0.0)  tmp = '-'+tmp;

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
    if(back) {
      z = int(corr*log10(zD / fabs(Axis->up)) + 0.5); // int() implies floor()
      z = y2 - z;
    }
    else
      z = int(corr*log10(zD / fabs(Axis->low)) + 0.5);// int() implies floor()
  }
}
else {  // not logarithmical
  back = calcAxisScale(Axis, GridNum, zD, zDstep, GridStep, double(y2));

  double Expo;
  if(Axis->up == 0.0)  Expo = log10(fabs(Axis->up-Axis->low));
  else  Expo = log10(fabs(Axis->up));
  if(fabs(Expo) < 3.0)  { form = 'g';  Prec = 3; }
  else  { form = 'e';  Prec = 0; }

  zD += 0.5;     // perform rounding
  z = int(zD);   //  "int(...)" implies "floor(...)"
  while((z <= y2) && (z >= 0)) {  // create all grid lines
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
      Lines.prepend(new Line(0, z, x2, z, GridPen));  // y grid
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

  int z;
  QSize  r;
  double GridStep, corr, zD, zDstep, GridNum;
  QFontMetrics  metrics(QucsSettings.font);

  x3 = x2 + 7;
  QString tmp;
  bool back = false;
  int  Prec, valid = 0;
  char form;

  // =====  give "step" the right sign  =============================
  xAxis.step = fabs(xAxis.step);
  if(xAxis.limit_min > xAxis.limit_max)
    xAxis.step *= -1.0;

  yAxis.step = fabs(yAxis.step);
  if(yAxis.limit_min > yAxis.limit_max)
    yAxis.step *= -1.0;

  zAxis.step = fabs(zAxis.step);
  if(zAxis.limit_min > zAxis.limit_max)
    zAxis.step *= -1.0;


  // ====  x grid  =======================================================
if(xAxis.log) {
  if(xAxis.autoScale) {
    if(xAxis.max*xAxis.min <= 0.0)  goto Frame;  // invalid
  }
  else  if(xAxis.limit_min*xAxis.limit_max <= 0.0)  goto Frame;  // invalid

  back = calcAxisLogScale(&xAxis, z, zD, zDstep, corr, x2);

  if(back) z = x2;
  while((z <= x2) && (z >= 0)) {    // create all grid lines
    if(xAxis.GridOn)  if(z < x2)  if(z > 0)
      Lines.prepend(new Line(z, y2, z, 0, GridPen));  // x grid

    if((zD < 1.5*zDstep) || (z == 0) || (z == x2)) {
      if(fabs(log10(zD)) < 3.0)  tmp = StringNum(zD);
      else  tmp = StringNum(zD, 'e', 1);
      r = metrics.size(0, tmp);  // width of text

      if(xAxis.up < 0.0)
        Texts.append(new Text(z-(r.width()>>1), -6, '-'+tmp));
      else
        Texts.append(new Text(z-(r.width()>>1), -6, tmp));

      Lines.append(new Line(z, 5, z, -5, QPen(QPen::black,0)));  // x marks
    }

    zD += zDstep;
    if(zD > 9.5*zDstep)  zDstep *= 10.0;
    if(back) {
      z = int(corr*log10(zD / fabs(xAxis.up)) + 0.5); // int() implies floor()
      z = x2 - z;
    }
    else
      z = int(corr*log10(zD / fabs(xAxis.low)) + 0.5);// int() implies floor()
  }
}
else {  // not logarithmical
  back = calcAxisScale(&xAxis, GridNum, zD, zDstep, GridStep, double(x2));

  double Expo;
  if(xAxis.up == 0.0)  Expo = log10(fabs(xAxis.up-xAxis.low));
  else  Expo = log10(fabs(xAxis.up));
  if(fabs(Expo) < 3.0)  { form = 'g';  Prec = 3; }
  else  { form = 'e';  Prec = 1; }

  zD += 0.5;     // perform rounding
  z = int(zD);   //  "int(...)" implies "floor(...)"
  while((z <= x2) && (z >= 0)) {    // create all grid lines
    if(fabs(GridNum) < 0.01*pow(10.0, Expo)) GridNum = 0.0;// make 0 really 0
    tmp = StringNum(GridNum, form, Prec);
    r = metrics.size(0, tmp);  // width of text
    Texts.append(new Text(z-(r.width()>>1), -6, tmp));
    GridNum += GridStep;

    if(xAxis.GridOn)  if(z < x2)  if(z > 0)
      Lines.prepend(new Line(z, y2, z, 0, GridPen)); // x grid
    Lines.append(new Line(z, 5, z, -5, QPen(QPen::black,0)));   // x marks
    zD += zDstep;
    z = int(zD);
  }
} // of "if(xlog) ... else ..."
  y1 = QucsSettings.font.pointSize() + 6;


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
