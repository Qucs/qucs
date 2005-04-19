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
  y1 = y3 = 7;
  x2 = 200;    // initial size of diagram
  y2 = 200;
  x3 = 207;    // with some distance for right axes text

  rotX = 315;
  rotY = 0;
  rotZ = 225;

  Name = "Rect3D";
  // symbolic diagram painting
  Lines.append(new Line(0, 0, cx,  0, QPen(QPen::black,0)));
  Lines.append(new Line(0, 0,  0, cy, QPen(QPen::black,0)));
  Lines.append(new Line(0, 0, cx/2, cy/2, QPen(QPen::black,0)));
//  calcDiagram();
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
int Rect3DDiagram::calcAxis(Axis *Axis, int x, int xLen,
                                         int y, int yLen, bool Right)
{
//  int z;
  double GridStep, /*corr,*/ xD, yD, stepD, GridNum;

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
//qDebug("******** %d-%d  %d-%d", x, xLen, y, yLen);
  xD = sqrt(double(xLen*xLen + yLen*yLen));
//qDebug("Length: %g", xD);
  calcAxisScale(Axis, GridNum, yD, stepD, GridStep, xD);
  int count = int((xD - yD) / stepD) + 1;  // number of grids
//qDebug("count: %d", count);
  
  double Expo;
  if(Axis->up == 0.0)  Expo = log10(fabs(Axis->up-Axis->low));
  else  Expo = log10(fabs(Axis->up));
  if(fabs(Expo) < 3.0)  { form = 'g';  Prec = 3; }
  else  { form = 'e';  Prec = 0; }

  double phi = atan2(double(yLen), double(xLen));
  double xstepD = stepD * cos(phi);
  double ystepD = stepD * sin(phi);

  if(phi > 0.0)  phi -= M_PI/2.0;
  else  phi += M_PI/2.0;
  int gx = int(5.4 * cos(phi) + 0.5);  // short grid marker lines
  int gy = int(5.4 * sin(phi) + 0.5);

  xD = yD * sin(phi) + 0.5 + double(x);
  yD = yD * cos(phi) + 0.5 + double(y);
//qDebug("%d -> %g  (%g°)", x, xD, phi*180.0/M_PI);
  for(; count>0; count--) {
//qDebug("x/y: %d/%d", x, y);
    x = int(xD);
    y = int(yD);
    if(fabs(GridNum) < 0.01*pow(10.0, Expo)) GridNum = 0.0;// make 0 really 0
    tmp = StringNum(GridNum, form, Prec);

    r = metrics.size(0, tmp);  // width of text
    if(maxWidth < r.width()) maxWidth = r.width();
    if(Right)
      Texts.append(new Text(x+3+gx, y-6+gy, tmp)); // place text right
    else
      Texts.append(new Text(x-r.width()-2-gx, y-6-gy, tmp)); // place left
    GridNum += GridStep;

    // short grid marks
    Lines.append(new Line(x-gx, y-gy, x+gx, y+gy, QPen(QPen::black,0)));
    xD += xstepD;
    yD += ystepD;
  }
} // of "if(ylog) ... else ..."

  return maxWidth+5;
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

  scaleX = double(x2) / (XMAX_2D - XMIN_2D); // scaling 3D -> 2D transformation
  scaleY = double(y2) / (YMAX_2D - YMIN_2D);
  xorig  = -int(XMIN_2D * scaleX);   // position of origin
  yorig  = -int(YMIN_2D * scaleY);


  // paint coordinate cross
  int coord[12];
  double *px, *py, *pz, val[2];
  val[0] = zAxis.low;  val[1] = 0.0;
  px = &xAxis.up;
  py = &yAxis.low;
  pz = val;
  calcCoordinate(px, pz, py, &coord[0], &coord[1], &zAxis);   // x axis
  Lines.append(new Line(xorig, yorig, coord[0], coord[1], QPen(QPen::black,0)));

  px = &xAxis.up;
  py = &yAxis.up;
  pz = val;
  calcCoordinate(px, pz, py, &coord[2], &coord[3], &zAxis);  // xy area
  Lines.append(new Line(coord[0], coord[1], coord[2], coord[3], QPen(QPen::lightGray,0)));

  px = &xAxis.low;
  py = &yAxis.up;
  pz = val;
  calcCoordinate(px, pz, py, &coord[4], &coord[5], &zAxis);   // y axis
  Lines.append(new Line(xorig, yorig, coord[4], coord[5], QPen(QPen::black,0)));
  Lines.append(new Line(coord[4], coord[5], coord[2], coord[3], QPen(QPen::lightGray,0)));

  val[0] = zAxis.up;
  px = &xAxis.low;
  py = &yAxis.up;
  pz = val;
  calcCoordinate(px, pz, py, &coord[6], &coord[7], &zAxis);  // yz area
  Lines.append(new Line(coord[4], coord[5], coord[6], coord[7], QPen(QPen::lightGray,0)));

  px = &xAxis.low;
  py = &yAxis.low;
  pz = val;
  calcCoordinate(px, pz, py, &coord[8], &coord[9], &zAxis);   // z axis
  Lines.append(new Line(xorig, yorig, coord[8], coord[9], QPen(QPen::black,0)));
  Lines.append(new Line(coord[8], coord[9], coord[6], coord[7], QPen(QPen::lightGray,0)));

  val[0] = zAxis.up;
  px = &xAxis.up;
  py = &yAxis.low;
  pz = val;
  calcCoordinate(px, pz, py, &coord[10], &coord[11], &zAxis);  // xz area
  Lines.append(new Line(coord[0], coord[1], coord[10], coord[11], QPen(QPen::lightGray,0)));
  Lines.append(new Line(coord[8], coord[9], coord[10], coord[11], QPen(QPen::lightGray,0)));

  // ------------------------------------------------------------
  // Put axis labels into the text list.

  QSize s;
  Graph *pg;
  int x, y;
  int LineSpacing = metrics.lineSpacing();


  x = coord[4]-coord[2];
  y = coord[5]-coord[3];
  z = (int)sqrt(double(x*x) + double(y*y));
  double phi = atan2(double(y), double(x));
  double cos_phi = cos(phi);
  double sin_phi = sin(phi);
  
  y = calcAxis(&xAxis, coord[4], coord[2]-coord[4], // write axis numbering
                       coord[5], coord[3]-coord[5], true);
  if(y >= 0) valid |= 1;
  x = coord[2] + int(double(y)*sin_phi);
  y = coord[3] - int(double(y)*cos_phi);
  if(xAxis.Label.isEmpty()) {
    // write all x labels ----------------------------------------
    for(pg = Graphs.first(); pg != 0; pg = Graphs.next()) {
      DataX *pD = pg->cPointsX.getFirst();
      if(!pD) continue;
      x += int(double(LineSpacing)*sin_phi);
      y -= int(double(LineSpacing)*cos_phi);
      s = metrics.size(0, pD->Var);
      Texts.append(new Text(x+int(double((z-s.width())>>1)*cos_phi),
                            y+int(double((z-s.width())>>1)*sin_phi),
                            pD->Var, pg->Color, 12.0, cos_phi, sin_phi));
    }
  }
  else {
    x += int(double(LineSpacing)*sin_phi);
    y -= int(double(LineSpacing)*cos_phi);
    s = metrics.size(0, xAxis.Label);
    Texts.append(new Text(x+int(double((z-s.width())>>1)*cos_phi),
                          y+int(double((z-s.width())>>1)*sin_phi),
                          xAxis.Label, Qt::black, 12.0, cos_phi, sin_phi));
  }


  x = coord[2]-coord[0];
  y = coord[3]-coord[1];
  z = (int)sqrt(double(x*x) + double(y*y));
  phi = atan2(double(y), double(x));
  cos_phi = cos(phi);
  sin_phi = sin(phi);
  
  y = calcAxis(&yAxis, coord[0], coord[2]-coord[0], // write axis numbering
                       coord[1], coord[3]-coord[1], false);
  if(y >= 0) valid |= 2;
  x = coord[0] + int(double(y)*sin_phi);
  y = coord[1] - int(double(y)*cos_phi);
  if(yAxis.Label.isEmpty()) {
    // draw left y-label for all graphs ------------------------------
    for(pg = Graphs.first(); pg != 0; pg = Graphs.next()) {
      if(!pg->cPointsY)  continue;
      if(!valid) {
        delete[] pg->cPointsY;
        pg->cPointsY = 0;
        continue;
      }
      x += int(double(LineSpacing)*sin_phi);
      y -= int(double(LineSpacing)*cos_phi);
      s = metrics.size(0, pg->Var);
      Texts.append(new Text(x+int(double((z-s.width())>>1)*cos_phi),
                            y+int(double((z-s.width())>>1)*sin_phi),
                            pg->Var, pg->Color, 12.0, cos_phi, sin_phi));
    }
  }
  else {
    s = metrics.size(0, yAxis.Label);
    Texts.append(new Text(x+int(double((z-s.width())>>1)*cos_phi),
                          y+int(double((z-s.width())>>1)*sin_phi),
                          yAxis.Label, Qt::black, 12.0, cos_phi, sin_phi));
  }


  x = coord[6]-coord[4];
  y = coord[7]-coord[5];
  z = (int)sqrt(double(x*x) + double(y*y));
  phi = atan2(double(y), double(x));
  cos_phi = cos(phi);
  sin_phi = sin(phi);
  
  y = calcAxis(&zAxis, coord[4], coord[6]-coord[4], // write axis numbering
                       coord[5], coord[7]-coord[5], true);
  if(y >= 0) valid |= 4;
  x = coord[4] + int(double(y)*sin_phi);
  y = coord[5] - int(double(y)*cos_phi);
  if(zAxis.Label.isEmpty()) {
    // draw right y-label for all graphs ------------------------------
    for(pg = Graphs.first(); pg != 0; pg = Graphs.next()) {
      x += int(double(LineSpacing)*sin_phi);
      y -= int(double(LineSpacing)*cos_phi);
      if(pg->cPointsY) {
        s = metrics.size(0, pg->Var);
        Texts.append(new Text(x+int(double((z-s.width())>>1)*cos_phi),
                              y+int(double((z-s.width())>>1)*sin_phi),
                              pg->Var, pg->Color, 12.0, cos_phi, sin_phi));
      }
      else {     // if no data => <invalid>
        s = metrics.size(0, pg->Var+INVALID_STR);
        Texts.append(new Text(x+int(double((z-s.width())>>1)*cos_phi),
                              y+int(double((z-s.width())>>1)*sin_phi),
                              pg->Var+INVALID_STR, pg->Color, 12.0,
                              cos_phi, sin_phi));
      }
    }
  }
  else {
    x += int(double(LineSpacing)*sin_phi);
    y -= int(double(LineSpacing)*cos_phi);
    s = metrics.size(0, zAxis.Label);
    Texts.append(new Text(x+int(double((z-s.width())>>1)*cos_phi),
                          y+int(double((z-s.width())>>1)*sin_phi),
                          zAxis.Label, Qt::black, 12.0,
                          cos_phi, sin_phi));
  }

}  // of if(logarithmical) ... else ...
  
  return 3;
}

// ------------------------------------------------------------
void Rect3DDiagram::clip(int* &p)
{
  rectClip(p);
}

// ------------------------------------------------------------
Diagram* Rect3DDiagram::newOne()
{
  return new Rect3DDiagram();
}
