/***************************************************************************
                              rect3ddiagram.cpp
                             -------------------
    begin                : Sat Mar 5 2005
    copyright            : (C) 2005 by Michael Margraf
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

#include "rect3ddiagram.h"
#include "main.h"

#include <math.h>


Rect3DDiagram::Rect3DDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  x1 = 10;      // position of label text
  y1 = y3 = 5;
  x2 = 200;    // initial size of diagram
  y2 = 200;
  x3 = 207;    // with some distance for right axes text

  rotX = 315;
  rotY = 0;
  rotZ = 225;

  Name = "Rect3D";
  calcDiagram();
}

Rect3DDiagram::~Rect3DDiagram()
{
}

// ------------------------------------------------------------
// Calculates the coefficients for 3D -> 2D transformation
void Rect3DDiagram::calcCoefficients()
{
  double rX = double(rotX) * M_PI/180.0;
  double rY = double(rotY) * M_PI/180.0;
  double rZ = double(rotZ) * M_PI/180.0;
  cxx =  cos(rY) * cos(rZ);
  cxy = -cos(rY) * sin(rZ);
  cxz =  sin(rY);
  cyx =  sin(rX) * sin(rY) * cos(rZ) + cos(rX) * sin(rZ);
  cyy = -sin(rX) * sin(rY) * sin(rZ) + cos(rX) * cos(rZ);
  cyz = -sin(rX) * cos(rY);
}

// ------------------------------------------------------------
double Rect3DDiagram::calcX_2D(double x, double y, double z)
{
  return cxx * x + cxy * y + cxz * z;
}

// ------------------------------------------------------------
double Rect3DDiagram::calcY_2D(double x, double y, double z)
{
  return cyx * x + cyy * y + cyz * z;
}

// ------------------------------------------------------------
void Rect3DDiagram::calcCoordinate(double* &xD, double* &zD, double* &yD,
				   int *px, int *py, Axis*)
{
  double x3D = *(zD++);
  double y3D = *(zD++);
  double z3D = zAxis.up - zAxis.low;
  if(fabs(y3D) < 1e-250)  // preserve negative values if not complex number
    z3D = (x3D - zAxis.low) / z3D;
  else
    z3D = (sqrt(x3D*x3D + y3D*y3D) - zAxis.low) / z3D;

  x3D = (*(xD++) - xAxis.low) / (xAxis.up - xAxis.low);
  y3D = (*yD - yAxis.low) / (yAxis.up - yAxis.low);

  *px = int(calcX_2D(x3D, y3D, z3D) * scaleX + 0.5) + xorig;
  *py = int(calcY_2D(x3D, y3D, z3D) * scaleY + 0.5) + yorig;
//qDebug("%g, %g, %g -> %d, %d", *(xD-1), *yD, *(zD-2), *px, *py);
/*  if(xAxis.log)
    *px = int(log10(x / xAxis.low)/log10(xAxis.up / xAxis.low)
		*double(x2) + 0.5);*/

/*  if(yAxis.log)
    *py = int(log10(y / yAxis.low)/log10(yAxis.up / yAxis.low)
		*double(y2) + 0.5);*/
}

// --------------------------------------------------------------
void Rect3DDiagram::calcLimits()
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
bool Rect3DDiagram::calcYAxis(Axis *Axis, int x0, int Begin, int End)
{
  int z;
  double GridStep, /*corr,*/ zD, zDstep, GridNum;

  QSize r;
  QString tmp;
  QFontMetrics  metrics(QucsSettings.font);
  int maxWidth = 0;
  int Prec;
  char form;

//  bool back = false;
if(Axis->log) {
/*  if(Axis->autoScale) {
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
  }*/
}
else {  // not logarithmical
  calcAxisScale(Axis, GridNum, zD, zDstep, GridStep, double(End-Begin));

  double Expo;
  if(Axis->up == 0.0)  Expo = log10(fabs(Axis->up-Axis->low));
  else  Expo = log10(fabs(Axis->up));
  if(fabs(Expo) < 3.0)  { form = 'g';  Prec = 3; }
  else  { form = 'e';  Prec = 0; }

  zD += 0.5 + double(Begin);
  z = int(zD);
//qDebug("******** %d -> %d", Begin, End);
  while((z <= End) && (z >= Begin)) {  // create all grid lines
//qDebug("%d, step: %g", z, zDstep);
    if(fabs(GridNum) < 0.01*pow(10.0, Expo)) GridNum = 0.0;// make 0 really 0
    tmp = StringNum(GridNum, form, Prec);

    r = metrics.size(0, tmp);  // width of text
    if(maxWidth < r.width()) maxWidth = r.width();
//    if(x0 > 0)
      Texts.append(new Text(x0+8, z+6, tmp));  // text aligned left
//    else
//      Texts.append(new Text(-r.width()-7, z+6, tmp));  // text aligned right
    GridNum += GridStep;

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
int Rect3DDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

//  QSize  r;
  double GridStep, /*corr,*/ zD, zDstep, GridNum;
  QFontMetrics  metrics(QucsSettings.font);

  x3 = x2 + 7;
  QString tmp;
//  bool back = false;
  int  /*Prec,*/ valid = 0;
//  char form;

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
/*  if(xAxis.autoScale) {
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
  }*/
}
else {  // not logarithmical

  // first calculate Axis.up and Axis.low
  calcAxisScale(&xAxis, GridNum, zD, zDstep, GridStep, double(x2));
  calcAxisScale(&yAxis, GridNum, zD, zDstep, GridStep, double(x2));
  calcAxisScale(&zAxis, GridNum, zD, zDstep, GridStep, double(x2));

  calcCoefficients();
  double XMIN_2D, XMAX_2D, YMIN_2D, YMAX_2D, x3D, y3D, z3D, x2D, y2D;
  int z;
  XMIN_2D = YMIN_2D = XMAX_2D = YMAX_2D = 0.0;  // origin is zero
  for(z=1; z<8; z++) {  // check 2D coordinates of all 8 corners of the quadrat
    if(z & 1) x3D = 1.0;  else x3D = 0.0;
    if(z & 2) y3D = 1.0;  else y3D = 0.0;
    if(z & 4) z3D = 1.0;  else z3D = 0.0;
    x2D = calcX_2D(x3D, y3D, z3D);
    y2D = calcY_2D(x3D, y3D, z3D);

    if(x2D < XMIN_2D)  XMIN_2D = x2D;
    if(x2D > XMAX_2D)  XMAX_2D = x2D;
    if(y2D < YMIN_2D)  YMIN_2D = y2D;
    if(y2D > YMAX_2D)  YMAX_2D = y2D;
  }
//qDebug("min 2D: %g, %g", XMIN_2D, YMIN_2D);
//qDebug("max 2D: %g, %g", XMAX_2D, YMAX_2D);

  scaleX = double(x2) / (XMAX_2D - XMIN_2D); // scaling 3D -> 2D transformation
  scaleY = double(y2) / (YMAX_2D - YMIN_2D);
  xorig  = -int(XMIN_2D * scaleX);   // position of origin
  yorig  = -int(YMIN_2D * scaleY);
//qDebug("orig: %d, %d", xorig, yorig);


  // paint coordinate cross
  int xi, yi, xtmp, ytmp;
  double *px, *py, *pz, val[2];
  val[0] = zAxis.low;  val[1] = 0.0;
  px = &xAxis.up;
  py = &yAxis.low;
  pz = val;
  calcCoordinate(px, pz, py, &xi, &yi, &zAxis);   // x axis
  Lines.append(new Line(xorig, yorig, xi, yi, QPen(QPen::black,0)));
//qDebug("x axis: %d, %d", xi, yi);

  px = &xAxis.up;
  py = &yAxis.up;
  pz = val;
  calcCoordinate(px, pz, py, &xtmp, &ytmp, &zAxis);  // xy area
  Lines.append(new Line(xi, yi, xtmp, ytmp, QPen(QPen::lightGray,0)));

  px = &xAxis.low;
  py = &yAxis.up;
  pz = val;
  calcCoordinate(px, pz, py, &xi, &yi, &zAxis);   // y axis
  Lines.append(new Line(xorig, yorig, xi, yi, QPen(QPen::black,0)));
//qDebug("y axis: %d, %d", xi, yi);
  Lines.append(new Line(xi, yi, xtmp, ytmp, QPen(QPen::lightGray,0)));

  val[0] = zAxis.up;
  px = &xAxis.low;
  py = &yAxis.up;
  pz = val;
  calcCoordinate(px, pz, py, &xtmp, &ytmp, &zAxis);  // yz area
  Lines.append(new Line(xi, yi, xtmp, ytmp, QPen(QPen::lightGray,0)));

  px = &xAxis.low;
  py = &yAxis.low;
  pz = val;
  calcCoordinate(px, pz, py, &xi, &yi, &zAxis);   // z axis
  Lines.append(new Line(xorig, yorig, xi, yi, QPen(QPen::black,0)));
//qDebug("z axis: %d, %d\n\n", xi, yi);
  Lines.append(new Line(xi, yi, xtmp, ytmp, QPen(QPen::lightGray,0)));


  // -----------------------------------------
  // write axis numbering
  if(calcYAxis(&zAxis, xtmp, ytmp-(yi-yorig), ytmp)) valid |= 1;
}

  // ====  y grid  =======================================================
//  if(zAxis.numGraphs > 0) if(calcYAxis(&zAxis, x2)) valid |= 1;
//  if(yAxis.numGraphs > 0) if(calcYAxis(&yAxis, 0))  valid |= 2;

//  return valid;
  return 0;
}

// ------------------------------------------------------------
Diagram* Rect3DDiagram::newOne()
{
  return new Rect3DDiagram();
}
