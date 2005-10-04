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

#include <stdlib.h>
#include <math.h>
#include <float.h>


Rect3DDiagram::Rect3DDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  x1 = 10;      // position of label text
  y1 = y3 = 7;
  x2 = 200;    // initial size of diagram
  y2 = 200;
  x3 = 207;    // with some distance for right axes text

#ifdef HIDDENLINE
  Mem = pMem = 0;
#endif

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
  czx = -cos(rX) * sin(rY) * cos(rZ) + sin(rX) * sin(rZ);
  czy =  cos(rX) * sin(rY) * sin(rZ) + sin(rX) * cos(rZ);
  czz =  cos(rX) * cos(rY);
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
double Rect3DDiagram::calcZ_2D(double x, double y, double z)
{
  return czx * x + czy * y + czz * z;
}

// ------------------------------------------------------------
void Rect3DDiagram::calcCoordinate(double* &xD, double* &zD, double* &yD,
				   int *px, int *py, Axis*)
{
#ifdef HIDDENLINE
//qDebug("calcCoordinate ENTER");
  if(pMem == 0) {
  // needed for marker
#endif

  double x3D = *(zD++);
  double y3D = *(zD++);
  double z3D;
  if(zAxis.log) {
    z3D = sqrt(x3D*x3D + y3D*y3D);
/*    if(z3D <= 0.0)  clipping not yet correct implemented
      z3D = -1e5;   // "negative infinity"
    else*/
      z3D = log10(z3D / fabs(zAxis.low)) / log10(zAxis.up / zAxis.low);
  }
  else {
    if(fabs(y3D) > 1e-250) // preserve negative values if no complex number
      x3D = sqrt(x3D*x3D + y3D*y3D);
    z3D = (x3D - zAxis.low) / (zAxis.up - zAxis.low);
  }

  x3D = *(xD++);
  if(xAxis.log) {
    x3D /= xAxis.low;
/*    if(x3D <= 0.0)  clipping not yet correct implemented
      x3D = -1e5;   // "negative infinity"
    else*/
      x3D = log10(x3D) / log10(xAxis.up / xAxis.low);
  }
  else
    x3D = (x3D - xAxis.low) / (xAxis.up - xAxis.low);

  if(yAxis.log) {
    y3D = (*yD) / yAxis.low;
/*    if(y3D <= 0.0)  clipping not yet correct implemented
      y3D = -1e5;   // "negative infinity"
    else*/
      y3D = log10(y3D) / log10(yAxis.up / yAxis.low);
  }
  else
    y3D = (*yD - yAxis.low) / (yAxis.up - yAxis.low);

  *px = int(calcX_2D(x3D, y3D, z3D) * scaleX + 0.5) + xorig;
  *py = int(calcY_2D(x3D, y3D, z3D) * scaleY + 0.5) + yorig;

#ifdef HIDDENLINE
  }  // of "if(Mem == 0)"
  else {
  // used in "calcData"

//qDebug("calc: %p: %d, %d", pMem, pMem->x, pMem->y);
  *px = pMem->x;
  *py = pMem->y;
  pMem++;
  }
//qDebug("calcCoordinate LEAVE");
#endif
}

// ------------------------------------------------------------
// For this diagram, this function is actual not for clipping, but
// for sorting out unnesseccary and invisible points.
void Rect3DDiagram::clip(int* &p)
{
#ifdef HIDDENLINE
//qDebug("NEU");
//qDebug("NEU %p: %d, %d, %d, %d", p, *(p-3), *(p-2), *(p-1), *p);
  p -= 4;
  pMem -= 2;
  int No = pMem->No + 1;
  if(*(p+1) < 0) {  // only one point accessible ?
    p += 2;
    pMem++;
  }

  pMem--;
  do {  // are there more points for this line ?
    pMem++;
//qDebug("   calc %d: %d, %d", pMem->No, pMem->x, pMem->y);
    if(pMem->done & 11) {   // point is grid point ?
//qDebug("   0");
      if(pMem->done & 4) { // point invisible ?
        if(pMem > Mem) if(((pMem-1)->done & 12) == 0) { //if( *(p-1) >= 0 ) {
          *(p++) = pMem->x;
          *(p++) = pMem->y;
          *(p++) = -2;
//qDebug("   1");
        }
//qDebug("   2");
        continue;
      }
    }

//qDebug("   3");
    *(p++) = pMem->x;
    *(p++) = pMem->y;
    if(pMem->done & 4) {   // point invisible ?
      *(p++) = -2;
//qDebug("   4");
    }

  } while(pMem->No < No);

  if(pMem->done & 8)   // last point of branch ?
    if(*(p-1) == -2)  p--;
//qDebug("   ende %p: %d, %d, %d, %d", p, *(p-3), *(p-2), *(p-1), *p);
  pMem++;
#else
  rectClip(p);
#endif
}

#ifdef HIDDENLINE
// ------------------------------------------------------------
void Rect3DDiagram::calcCoordinate3D(double x, double y, double zr, double zi,
					tPoint3D *p, tPointZ *pz, int& No)
{
  if(zAxis.log) {
    zr = sqrt(zr*zr + zi*zi);
/*    if(zr <= 0.0)  clipping not yet correct implemented
      zr = -1e5;   // "negative infinity"
    else*/
      zr = log10(zr / fabs(zAxis.low)) / log10(zAxis.up / zAxis.low);
  }
  else {
    if(fabs(zi) > 1e-250) // preserve negative values if no complex number
      zr = sqrt(zr*zr + zi*zi);
    zr = (zr - zAxis.low) / (zAxis.up - zAxis.low);
  }

  if(xAxis.log) {
    x /= xAxis.low;
/*    if(x <= 0.0)  clipping not yet correct implemented
      x = -1e5;   // "negative infinity"
    else*/
      x = log10(x) / log10(xAxis.up / xAxis.low);
  }
  else
    x = (x - xAxis.low) / (xAxis.up - xAxis.low);

  if(yAxis.log) {
    y = y / yAxis.low;
/*    if(y <= 0.0)  clipping not yet correct implemented
      y = -1e5;   // "negative infinity"
    else*/
      y = log10(y) / log10(yAxis.up / yAxis.low);
  }
  else
    y = (y - yAxis.low) / (yAxis.up - yAxis.low);

  p->x  = int(calcX_2D(x, y, zr) * scaleX + 0.5) + xorig;
  p->y  = int(calcY_2D(x, y, zr) * scaleY + 0.5) + yorig;
  p->No = pz->No = No++;
  p->done = 0;
  pz->z = float(calcZ_2D(x, y, zr));
}

// --------------------------------------------------------------
bool Rect3DDiagram::isHidden(int x, int y, tBound *Bounds)
{
  if( ((Bounds+x)->max >= y) && ((Bounds+x)->min <= y) )
    return true;
  else {
    if( (Bounds+x)->max < y )  (Bounds+x)->max = y;
    if( (Bounds+x)->min > y )  (Bounds+x)->min = y;
  }

  return false;
}

// --------------------------------------------------------------
// Enlarge memory block if neccessary.
void Rect3DDiagram::enlargeMemoryBlock(tPoint3D* &MemEnd)
{
  if(pMem >= MemEnd) {
//qDebug("1: %p -> %p", pMem, MemEnd);
    int Size = MemEnd - Mem + 256;
//qDebug("2: %d", Size);
    MemEnd = Mem;
//qDebug("3: %p", Mem);
    Mem  = (tPoint3D*)realloc(Mem, Size*sizeof(tPoint3D));
//qDebug("4: %p", Mem);
    pMem += Mem - MemEnd;
//qDebug("5");
    MemEnd = Mem + Size - 5;
  }
}

// --------------------------------------------------------------
// Calculate all 3D points of the line between point "p" and "EndPoint".
// Parameters:   p        - pointer on 3D coordinate of line start point
//               EndPoint - pointer on 3D coordinate of line end point
//               Bounds   - pointer on memory block for occupied area
//               MemEnd   - pointer where memory block ends
void Rect3DDiagram::calcLine(tPoint3D* p, tPoint3D *EndPoint,
				tBound *Bounds, tPoint3D* &MemEnd)
{
  int No_ = p->No;  // needed because "p" may change (realloc)
  int x1_ = p->x, y1_ = p->y;
  int x2_ = EndPoint->x, y2_ = EndPoint->y;
//qDebug("enter: %d/%d -> %d/%d, No: %d", x1_, y1_, x2_, y2_, p->No);

  bool wasHidden = isHidden(x1_, y1_, Bounds);
  if(wasHidden) {
    p->done |= 4;   // mark as hidden
//    qDebug("first hidden");
  }

  int ax_ = 0, ay_ = 0;
  int ix_, iy_, dx_, dy_, of_;

  if(x2_ >= x1_) {
    dx_ = x2_ - x1_;
    ix_ = 1;
  }
  else {
    dx_ = x1_ - x2_;
    ix_ = -1;
  }

  if(y2_ >= y1_) {
    dy_ = y2_ - y1_;
    iy_ = 1;
  }
  else {
    dy_ = y1_ - y2_;
    iy_ = -1;
  }

  if(dx_ < dy_) {
    of_ = dx_;   // exchange dx and dy
    dx_ = dy_;
    dy_ = of_;

    ax_ = iy_;
    ay_ = ix_;
    ix_ = iy_ = 0;
  }

  of_ = dx_ >> 1;
  for(int i=dx_; i>1; i--) {   // calculate each point of the line
    x1_ += ix_;
    y1_ += ax_;
    of_ += dy_;
    if(of_ > dx_) {
      of_ -= dx_;
      x1_ += ay_;
      y1_ += iy_;
    }
    
    if( isHidden(x1_, y1_, Bounds) != wasHidden ) {
      wasHidden = !wasHidden;
      if(i == dx_) if(!wasHidden) { // start point often hidden uneccessarily
        // here "p" can be used as it is only executed once before "realloc"
        p->done &= -5;   // mark as not hidden
        continue;
      }
//qDebug("  hidden split: %d/%d  (%d, %d)", x1_, y1_, (Bounds+x1_)->min, (Bounds+x1_)->max);
      pMem->x  = x1_;
      pMem->y  = y1_;
      pMem->No = No_;
      pMem->done = 0;
      if(wasHidden) pMem->done = 4;   // mark as hidden
      pMem++;
//qDebug("weiter");
      // Enlarge memory block if neccessary.
      enlargeMemoryBlock(MemEnd);   // this may make "p" invalid
//qDebug("weiter2");
    }
  }

//  if(isHidden(EndPoint->x, EndPoint->y, Bounds)) {
    isHidden(EndPoint->x, EndPoint->y, Bounds);   // set hidden buffer
    if(wasHidden) {  // last point same as second last
//qDebug("Line No: %d", p->No);
      if((EndPoint->done & 1) == 0) {
        EndPoint->done |= 4;   // mark as hidden
//qDebug("yes");
      }
/*      pMem->x  = x1_;
      pMem->y  = y1_;
      pMem->No = p->No;
      pMem->done = 4;   // mark as hidden
      pMem++;
    }*/
  }
}

// --------------------------------------------------------------
// Removes the invisible parts of the graph.
void Rect3DDiagram::removeHiddenLines(Graph *g, tBound *Bounds)
{
qDebug("removeHiddenLines ANFANG");
  double *px;
  double *pz = g->cPointsY;
  if(!pz)  return;
  if(g->cPointsX.count() < 1) return;

  int Size = g->cPointsX.getFirst()->count * g->countY;
  tPointZ *zMem = (tPointZ*)malloc( (Size+2)*sizeof(tPointZ) );
  Mem = (tPoint3D*)malloc( 2*(Size+2)*sizeof(tPoint3D) );

  pMem = Mem;
  tPointZ *zp = zMem;

  double Dummy = 0.0;  // number for 1-dimensional data in 3D cartesian
  double *py = &Dummy;
  if(g->countY > 1)  py = g->cPointsX.at(1)->Points;

  // calculate coordinates of all lines
  int i, j, z, dx, dy, No=0;
  dy = g->cPointsX.at(1)->count;
  for(i=g->countY-1; i>=0; i--) {   // y coordinates
    px = g->cPointsX.getFirst()->Points;
    calcCoordinate3D(*(px++), *py, *(pz++), *(pz++), pMem++, zp++, No);
    
    for(j=g->cPointsX.getFirst()->count-1; j>0; j--)   // x coordinates
      calcCoordinate3D(*(px++), *py, *(pz++), *(pz++), pMem++, zp++, No);

    (pMem-1)->done |= 8;  // mark as "last in line"
    py++;
    if((i % dy) == 0)
      py = g->cPointsX.at(1)->Points;
  }

  // calculate points of cross lines
  // "zp" points to unused memory, the values are ignored
  if(g->countY > 1) {
    pz = g->cPointsY;
    for(j=g->countY/g->cPointsX.at(1)->count; j>0; j--) { // every plane
      DataX *pD = g->cPointsX.first();
      px = pD->Points;
      dx = pD->count;
      pD = g->cPointsX.next();
      dy = pD->count;
      for(i=g->cPointsX.getFirst()->count; i>0; i--) {  // every branch
        py = pD->Points;
        calcCoordinate3D(*px, *(py++), *pz, *(pz+1), pMem++, zp, No);
        pz += 2*dx;  // next z coordinate
        for(z=dy-1; z>0; z--) {  // every point
          calcCoordinate3D(*px, *(py++), *pz, *(pz+1), pMem++, zp, No);
          pz += 2*dx;  // next z coordinate
        }
        (pMem-1)->done |= 8;  // mark as "last in line"
        px++;   // next x coordinate
        pz -= 2*dx*dy - 2;  // next z coordinate
      }
      pz += 2*dx*(dy-1);
    }
  }
qDebug("Size 1: %d, %d, %d", Size, pMem-Mem, zp-zMem);


#if 1
  qDebug("##########################################");
  for(tPoint3D *p=Mem; p<pMem; p++)
    qDebug("xyPoints:  %d/%d - %d - %d", p->x, p->y, p->No, p->done);
  qDebug("------------------------------------------");
  for(tPointZ *p=zMem; p-zMem<Size; p++)
    qDebug("zPoints:   %g - %d", p->z, p->No);
#endif


  // Calculate the z-coordinate of all polygons by building the
  // sum of the z-coordinates of all of its 4 corners.
  zp = zMem;
  dx = g->cPointsX.getFirst()->count;
  dy = g->cPointsX.at(1)->count;
  for(i=g->countY-1; i>=0; i--) {   // all branches
    if(i % dy) {
      for(j=dx-1; j>0; j--) {   // x coordinates
        zp->z += (zp+1)->z + (zp+dx)->z + (zp+dx+1)->z;
        zp++;
      }
      zp->z = -FLT_MAX;  // last one not needed
      zp++;
    }
    else   // last line not needed
      for(j=dx; j>0; j--) {   // x coordinates
        zp->z = -FLT_MAX;  // last one not needed
        zp++;
      }
  }
qDebug("Size 2: %d, %d, %d", Size, pMem-Mem, zp-zMem);

#if 0
  qDebug("-------------------------- 4 z addition");
  for(tPointZ *p=zMem; p-zMem<Size; p++)
    qDebug("zPoints:   %g - %d", p->z, p->No);
#endif


  // bubble sort algorithm -> sort z-coordinates (greatest first)
  tPointZ vPZ;
  for(i=1; i<=Size; i++) {
    zp = zMem;
    for(j=0; j<(Size-i); j++) {
      if(zp->z < (zp+1)->z) {
	vPZ = *zp;
	*zp = *(zp+1);
	*(zp+1) = vPZ;
      }
      zp++;
    }
  }

#if 0
  qDebug("--------------------------- z sorting");
  for(tPointZ *p=zMem; p-zMem<Size; p++)
    qDebug("zPoints:   %g - %d", p->z, p->No);
#endif

  tPoint3D *p, *MemEnd = Mem + 2*Size - 5;   // limit of buffer
//  enlargeMemoryBlock(MemEnd);
  // look for hidden lines
  zp = zMem;
  dx = g->cPointsX.getFirst()->count;
  dy = g->cPointsX.at(1)->count;
  do {

    // go through all 4 lines of polygon
    p  = Mem + zp->No;  // polygon corner coordinates
    if((p->done & 1) == 0) {
      p->done |= 1;
      calcLine(p, p+1, Bounds, MemEnd);  // may make "p" invalid (realloc)
    }
    p += dx;  // polygon corner coordinates
    if((p->done & 1) == 0) {
      p->done |= 1;
      calcLine(p, p+1, Bounds, MemEnd);  // may make "p" invalid (realloc)
    }

    // now the cross grid lines
    p  = Mem + Size;  // polygon corner coordinates
    z  = zp->No % (dx * dy);
    p += (z % dx) * dy + (z / dx) + (zp->No - z);  // map from xy to yx
    if((p->done & 1) == 0) {
      p->done |= 1;
      calcLine(p, p+1, Bounds, MemEnd);  // may make "p" invalid (realloc)
    }
    p += dy;       // polygon corner coordinates
    if((p->done & 1) == 0) {
      p->done |= 1;
      calcLine(p, p+1, Bounds, MemEnd);  // may make "p" invalid (realloc)
    }

    zp++;   // next polygon
  } while(zp->z > -FLT_MAX);


#if 1
  qDebug("--------------------------- hidden lines %d", pMem-Mem);
  for(tPoint3D *p=Mem; p<pMem; p++)
    qDebug("xyPoints:  %d/%d - %d - %d", p->x, p->y, p->No, p->done);
#endif

  free(zMem);

  // bubble sort algorithm -> sort "No" (least one first)
  Size = pMem - Mem;
  tPoint3D v3D;
  for(i=1; i<=Size; i++) {
    p = Mem;
    for(j=0; j<(Size-i); j++) {
      if(p->No > (p+1)->No) {
	v3D = *p;
	*p = *(p+1);
	*(p+1) = v3D;
      }
      p++;
    }
  }


#if 1
  qDebug("--------------------------- last sorting %d", pMem-Mem);
  for(tPoint3D *p=Mem; p<pMem; p++)
    qDebug("xyPoints:  %d/%d - %d - %d", p->x, p->y, p->No, p->done);
  qDebug("\n");
#endif
qDebug("removeHiddenLines ENDE");
}
#endif

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
  double GridStep, corr, yD, stepD, GridNum, Expo;
  double xstepD, ystepD;

  QString tmp;
  QFontMetrics  metrics(QucsSettings.font);
  int maxWidth = 0;
  int count, gx, gy, w;

  double xD = sqrt(double(xLen*xLen + yLen*yLen));
  double phi = atan2(double(yLen), double(xLen));

  if(phi > 0.0)  Expo = phi - M_PI/2.0;
  else  Expo = phi + M_PI/2.0;
  gx = int(5.4 * cos(Expo) + 0.5);  // short grid marker lines
  gy = int(5.4 * sin(Expo) + 0.5);


if(Axis->log) {

  bool back = calcAxisLogScale(Axis, w, yD, stepD, corr, int(xD));

  double upD  = Axis->up;
  if(yD > 1.5*stepD)  yD = 10.0*stepD;  // always start at power of 10
  if(back) {
    upD  = Axis->low;
    phi += M_PI;
    xD   = 0.0;
  }

  ystepD = corr * log10(yD / fabs(Axis->low));
  while(ystepD <= xD) {  // create all grid lines

    tmp = StringNiceNum(yD);
    if(Axis->up < 0.0)  tmp = '-'+tmp;
    w = metrics.width(tmp);  // width of text
    if(maxWidth < w) maxWidth = w;

    xLen = int(ystepD * cos(phi) + 0.5) + x;
    yLen = int(ystepD * sin(phi) + 0.5) + y;
    if(Right)
      Texts.append(new Text(xLen+3+gx, yLen-6+gy, tmp));
    else
      Texts.append(new Text(xLen-w-2-gx, yLen-6-gy, tmp));

    // short grid marks
    Lines.append(new Line(xLen-gx, yLen-gy, xLen+gx, yLen+gy,
                          QPen(QPen::black,0)));
    yD *= 10.0;
    ystepD += corr;
  }

}
else {  // not logarithmical
  calcAxisScale(Axis, GridNum, yD, stepD, GridStep, xD);
  count = int((xD - yD) / stepD) + 1;  // number of grids
  
  xstepD = stepD * cos(phi);
  ystepD = stepD * sin(phi);
  xD = yD * cos(phi) + 0.5 + double(x);
  yD = yD * sin(phi) + 0.5 + double(y);

  if(Axis->up == 0.0)  Expo = log10(fabs(Axis->up-Axis->low));
  else  Expo = log10(fabs(Axis->up));

  for(; count>0; count--) {
    x = int(xD);
    y = int(yD);
    if(fabs(GridNum) < 0.01*pow(10.0, Expo)) GridNum = 0.0; // make 0 really 0
    tmp = StringNiceNum(GridNum);

    w = metrics.width(tmp);  // width of text
    if(maxWidth < w) maxWidth = w;
    if(Right)
      Texts.append(new Text(x+3+gx, y-6+gy, tmp)); // place text right
    else
      Texts.append(new Text(x-w-2-gx, y-6-gy, tmp)); // place left
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

  double GridStep, corr, zD, zDstep, GridNum;
  double phi, cos_phi, sin_phi;
  QFontMetrics  metrics(QucsSettings.font);

  x3 = x2 + 7;
  QString tmp;
  int valid = 0;
  int x, y, w, LineSpacing;
#ifdef HIDDENLINE
  tBound *Bounds;
#endif

  // =====  calculate transformation coefficients from rotation angles ===
  calcCoefficients();


  // =====  check position of axes in 2D rectangle =======================
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


  // =====  give "step" the right sign  ==================================
  xAxis.step = fabs(xAxis.step);
  if(xAxis.limit_min > xAxis.limit_max)
    xAxis.step *= -1.0;

  yAxis.step = fabs(yAxis.step);
  if(yAxis.limit_min > yAxis.limit_max)
    yAxis.step *= -1.0;

  zAxis.step = fabs(zAxis.step);
  if(zAxis.limit_min > zAxis.limit_max)
    zAxis.step *= -1.0;


  // =====  calculate Axis.up and Axis.low  ==============================
  if(xAxis.log) {
    if(xAxis.autoScale) {
      if(xAxis.max*xAxis.min <= 0.0)  goto Frame;  // invalid
    }
    else  if(xAxis.limit_min*xAxis.limit_max <= 0.0)  goto Frame;  // invalid
    calcAxisLogScale(&xAxis, z, zD, zDstep, corr, x2);
  }
  else  calcAxisScale(&xAxis, GridNum, zD, zDstep, GridStep, double(x2));
    
  if(yAxis.log) {
    if(yAxis.autoScale) {
      if(yAxis.max*yAxis.min <= 0.0)  goto Frame;  // invalid
    }
    else  if(yAxis.limit_min*yAxis.limit_max <= 0.0)  goto Frame;  // invalid
    calcAxisLogScale(&yAxis, z, zD, zDstep, corr, x2);
  }
  else  calcAxisScale(&yAxis, GridNum, zD, zDstep, GridStep, double(x2));

  if(zAxis.log) {
    if(zAxis.autoScale) {
      if(zAxis.max*zAxis.min <= 0.0)  goto Frame;  // invalid
    }
    else  if(zAxis.limit_min*zAxis.limit_max <= 0.0)  goto Frame;  // invalid
    calcAxisLogScale(&zAxis, z, zD, zDstep, corr, x2);
  }
  else  calcAxisScale(&zAxis, GridNum, zD, zDstep, GridStep, double(x2));


  // =====  paint coordinate cross  ====================================
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



  DataX *pD;
  Graph *pg;
  LineSpacing = metrics.lineSpacing();

  // =====  x axis labels  =============================================
  x = coord[4]-coord[2];
  y = coord[5]-coord[3];
  z = (int)sqrt(double(x*x) + double(y*y));
  phi = atan2(double(y), double(x));
  cos_phi = cos(phi);
  sin_phi = sin(phi);
  
  y = calcAxis(&xAxis, coord[4], -x, coord[5], -y, true); // axis numbering
  if(y >= 0) valid |= 1;
  x = coord[2] + int(double(y)*sin_phi);
  y = coord[3] - int(double(y)*cos_phi);
  if(xAxis.Label.isEmpty()) {
    // write all x labels ----------------------------------------
    for(pg = Graphs.first(); pg != 0; pg = Graphs.next()) {
      if(!pg->cPointsY)  continue;
      if(!valid) continue;
      pD = pg->cPointsX.getFirst();
      if(!pD) continue;
      x += int(double(LineSpacing)*sin_phi);
      y -= int(double(LineSpacing)*cos_phi);
      w = metrics.width(pD->Var);
      Texts.append(new Text(x+int(double((z-w)>>1)*cos_phi),
                            y+int(double((z-w)>>1)*sin_phi),
                            pD->Var, pg->Color, 12.0, cos_phi, sin_phi));
    }
  }
  else {
    x += int(double(LineSpacing)*sin_phi);
    y -= int(double(LineSpacing)*cos_phi);
    w = metrics.width(xAxis.Label);
    Texts.append(new Text(x+int(double((z-w)>>1)*cos_phi),
                          y+int(double((z-w)>>1)*sin_phi),
                          xAxis.Label, Qt::black, 12.0, cos_phi, sin_phi));
  }


  // =====  y axis labels  =============================================
  x = coord[2]-coord[0];
  y = coord[3]-coord[1];
  z = (int)sqrt(double(x*x) + double(y*y));
  phi = atan2(double(y), double(x));
  cos_phi = cos(phi);
  sin_phi = sin(phi);
  
  y = calcAxis(&yAxis, coord[0], x, coord[1], y, false); // write axis numbering
  if(y >= 0) valid |= 2;
  x = coord[0] + int(double(y)*sin_phi);
  y = coord[1] - int(double(y)*cos_phi);
  if(yAxis.Label.isEmpty()) {
    // draw y-label for all graphs ------------------------------
    for(pg = Graphs.first(); pg != 0; pg = Graphs.next()) {
      if(!valid) {
        delete[] pg->cPointsY;
        pg->cPointsY = 0;
      }
      if(!pg->cPointsY)  continue;
      pD = pg->cPointsX.at(1);
      if(!pD) continue;
      x += int(double(LineSpacing)*sin_phi);
      y -= int(double(LineSpacing)*cos_phi);
      w = metrics.width(pD->Var);
      Texts.append(new Text(x+int(double((z-w)>>1)*cos_phi),
                            y+int(double((z-w)>>1)*sin_phi),
                            pD->Var, pg->Color, 12.0, cos_phi, sin_phi));
    }
  }
  else {
    x += int(double(LineSpacing)*sin_phi);
    y -= int(double(LineSpacing)*cos_phi);
    w = metrics.width(yAxis.Label);
    Texts.append(new Text(x+int(double((z-w)>>1)*cos_phi),
                          y+int(double((z-w)>>1)*sin_phi),
                          yAxis.Label, Qt::black, 12.0, cos_phi, sin_phi));
  }


  // =====  z axis labels  =============================================
  x = coord[6]-coord[4];
  y = coord[7]-coord[5];
  z = (int)sqrt(double(x*x) + double(y*y));
  phi = atan2(double(y), double(x));
  cos_phi = cos(phi);
  sin_phi = sin(phi);
  
  y = calcAxis(&zAxis, coord[4], x, coord[5], y, true); // write axis numbering
  if(y >= 0) valid |= 4;
  x = coord[4] + int(double(y)*sin_phi);
  y = coord[5] - int(double(y)*cos_phi);
  if(zAxis.Label.isEmpty()) {
    // draw z-label for all graphs ------------------------------
    for(pg = Graphs.first(); pg != 0; pg = Graphs.next()) {
      x += int(double(LineSpacing)*sin_phi);
      y -= int(double(LineSpacing)*cos_phi);
      if(pg->cPointsY) {
        w = metrics.width(pg->Var);
        Texts.append(new Text(x+int(double((z-w)>>1)*cos_phi),
                              y+int(double((z-w)>>1)*sin_phi),
                              pg->Var, pg->Color, 12.0, cos_phi, sin_phi));
      }
      else {     // if no data => <invalid>
        w = metrics.width(pg->Var+INVALID_STR);
        Texts.append(new Text(x+int(double((z-w)>>1)*cos_phi),
                              y+int(double((z-w)>>1)*sin_phi),
                              pg->Var+INVALID_STR, pg->Color, 12.0,
                              cos_phi, sin_phi));
      }
    }
  }
  else {
    x += int(double(LineSpacing)*sin_phi);
    y -= int(double(LineSpacing)*cos_phi);
    w = metrics.width(zAxis.Label);
    Texts.append(new Text(x+int(double((z-w)>>1)*cos_phi),
                          y+int(double((z-w)>>1)*sin_phi),
                          zAxis.Label, Qt::black, 12.0,
                          cos_phi, sin_phi));
  }


#ifdef HIDDENLINE
  // to store max and min values for hidden coordinates
  Bounds = (tBound*)malloc(x2 * sizeof(tBound));
  for(w=x2-1; w>=0; w--) {
    (Bounds+w)->max = INT_MIN;
    (Bounds+w)->min = INT_MAX;
  }

  for(Graph *pg = Graphs.first(); pg!=0; pg = Graphs.next())
    removeHiddenLines(pg, Bounds);

  free(Bounds);
#endif
  return 3;


Frame:   // jump here if error occurred (e.g. impossible log boundings)
  Lines.append(new Line(0,  y2, x2, y2, QPen(QPen::black,0)));
  Lines.append(new Line(x2, y2, x2,  0, QPen(QPen::black,0)));
  Lines.append(new Line(0,   0, x2,  0, QPen(QPen::black,0)));
  Lines.append(new Line(0,  y2,  0,  0, QPen(QPen::black,0)));
  return 0;
}

// ------------------------------------------------------------
bool Rect3DDiagram::insideDiagram(int x, int y)
{
  return (regionCode(x, y) == 0);
}

// ------------------------------------------------------------
Diagram* Rect3DDiagram::newOne()
{
  return new Rect3DDiagram();
}

// ------------------------------------------------------------
Element* Rect3DDiagram::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("3D-Cartesian");
  BitmapFile = "rect3d";

  if(getNewOne)  return new Rect3DDiagram();
  return 0;
}
