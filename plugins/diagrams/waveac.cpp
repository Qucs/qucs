/*
 * waveac.cpp - Implementation file for the temporal diagram
 *
 * Copyright (C) 2016, Nelson Martins, macedo.pp@gmail.com
 *		 2017, Alberto Silva, 1120196@isep.ipp.pt	
 *		
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif
#include <cmath>
#include <float.h>
#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#include "waveac.h"
#include "qucs.h"
#include "misc.h"
#include "some_font_stuff.h"

Waveac::Waveac(int _cx, int _cy) : Diagram(_cx, _cy)
{
  x1 = 10;      // position of label text
  y1 = y3 = 33;
  x2 = 240;    // initial size of diagram
  y2 = 160;
  x3 = 247;    // with some distance for text

  Name = "Waveac";
  calcDiagram();
}

Waveac::~Waveac()
{
}

// ------------------------------------------------------------
void Waveac::calcCoordinate(const double* xD, const double* yD, const double*,
                                 float *px, float *py, Axis const *pa) const
{
  double x  = *xD;
  double yr = *yD;

  *px = float((x-xAxis.low)/(xAxis.up-xAxis.low)*double(x2));
  *py = float((yr-pa->low)/(pa->up-pa->low)*double(y2));

  if(!std::isfinite(*px))  *px = 0.0;
  if(!std::isfinite(*py))  *py = 0.0;
}

// --------------------------------------------------------------
void Waveac::finishMarkerCoordinates(float& fCX, float& fCY) const
{
  if(!insideDiagram(fCX, fCY)) {
	  fCX = fCY = 0.0;
  }
}

// --------------------------------------------------------------
void Waveac::calcLimits()
{
  int i;
  double a, b, c;

  if(xAxis.autoScale) {// checks to preserve limit exchange (max < min)
    if(xAxis.log) {
      calcAxisLogScale(&xAxis, i, a, b, c, x2);
      xAxis.step = 1.0;
    }
    else  calcAxisScale(&xAxis, a, b, c, xAxis.step, double(x2));
    xAxis.limit_min = xAxis.low;
    xAxis.limit_max = xAxis.up;
  }

  if(yAxis.autoScale) {// checks to preserve limit exchange (max < min)
    if(yAxis.log) {
      calcAxisLogScale(&yAxis, i, a, b, c, y2);
      yAxis.step = 1.0;
    }
    else  calcAxisScale(&yAxis, a, b, c, yAxis.step, double(y2));
    yAxis.limit_min = yAxis.low;
    yAxis.limit_max = yAxis.up;
  }

  if(zAxis.autoScale) {// checks to preserve limit exchange (max < min)
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
int Waveac::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  double GridStep, corr, zD, zDstep, GridNum;
  // get size of text using the screen-compatible metric
  FontMetrics metrics;
  y1 = QucsSettings.font.pointSize() + 6;

  x1 = 10;      // position of label text
  x3 = x2 + 7;
  QString tmp;
  bool back = false;
  int  z, w, valid = 0;

  // =====  give "step" the right sign (if user made it wrong)  ==============
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
    if(xAxis.max*xAxis.min < 1e-200)  goto Frame;  // invalid
  }
  else  if(xAxis.limit_min*xAxis.limit_max < 1e-200)  goto Frame;  // invalid

  back = calcAxisLogScale(&xAxis, z, zD, zDstep, corr, x2);

  if(back) z = x2;
  while((z <= x2) && (z >= 0)) {    // create all grid lines
    if(xAxis.GridOn)  if(z < x2)  if(z > 0)
      Lines.prepend(new Line(z, y2, z, 0, GridPen));  // x grid

    if((zD < 1.5*zDstep) || (z == 0) || (z == x2)) {
      tmp = misc::StringNiceNum(zD);
      if(xAxis.up < 0.0)  tmp = '-'+tmp;
      w = metrics.width(tmp);  // width of text
      // center text horizontally under the x tick mark
      Texts.append(new Text(z-(w>>1), -y1, tmp));
      Lines.append(new Line(z, 5, z, -5, QPen(Qt::black,0)));  // x tick marks
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
  calcAxisScale(&xAxis, GridNum, zD, zDstep, GridStep, double(x2));

  double Expo;
  if(xAxis.up == 0.0)  Expo = log10(fabs(xAxis.up-xAxis.low));
  else  Expo = log10(fabs(xAxis.up));

  zD += 0.5;     // perform rounding
  z = int(zD);   //  "int(...)" implies "floor(...)"
  while((z <= x2) && (z >= 0)) {    // create all grid lines
    if(fabs(GridNum) < 0.01*pow(10.0, Expo)) GridNum = 0.0;// make 0 really 0
    tmp = misc::StringNiceNum(GridNum);
    w = metrics.width(tmp);  // width of text
    // center text horizontally under the x tick mark
    Texts.append(new Text(z-(w>>1), -y1, tmp)); // Text(x, y, str, ...)
    GridNum += GridStep;

    if(xAxis.GridOn)  if(z < x2)  if(z > 0)
      Lines.prepend(new Line(z, y2, z, 0, GridPen)); // x grid
    Lines.append(new Line(z, 5, z, -5, QPen(Qt::black,0)));   // x tick marks
    zD += zDstep;
    z = int(zD);
  }
}


  // ====  y grid  =======================================================
  if(zAxis.numGraphs > 0) if(calcYAxis(&zAxis, x2)) valid |= 2;
  if(yAxis.numGraphs > 0) if(calcYAxis(&yAxis, 0))  valid |= 1;


Frame:
  // outer frame
  Lines.append(new Line(0,  y2, x2, y2, QPen(Qt::black,0)));
  Lines.append(new Line(x2, y2, x2,  0, QPen(Qt::black,0)));
  Lines.append(new Line(0,   0, x2,  0, QPen(Qt::black,0)));
  Lines.append(new Line(0,  y2,  0,  0, QPen(Qt::black,0)));
  return valid;
}

void Waveac::calcData(Graph *g)
{
  double *pz = g->cPointsY;
  if(!pz)  return;
  if(g->numAxes() < 1) return;

  int i, z, j, Counter=2,m=0;
  sc = int(xAxis.up*freq[0])+1;//will determine how many points need to be created
  int Size = 51 * sc * g->countY + 10;
 
  if(xAxis.autoScale)  if(yAxis.autoScale)  if(zAxis.autoScale)
    Counter = -50000;

  double Dummy = 0.0;  // not used
  double *py = &Dummy;

  g->resizeScrPoints(Size);
  auto p = g->begin();
  auto p_end = g->begin();
  
  p_end += Size - 9;   // limit of buffer
  p->setStrokeEnd();
  ++p;
  assert(p!=g->end());


  Axis *pa;

  if(g->yAxisNo == 0)  pa = &yAxis;
  else  pa = &zAxis;
  
  double af=0.0;
  double A = 0.0;
  double *yp,xn,yn;
  //meaning it only works in AC 
  z = QString::compare(g->axis(0)->Var,"acfrequency",Qt::CaseInsensitive);
  if(z != 0)
  {
    sfreq = "0 Hz";
    p->setGraphEnd();
    return;
  }
  nfreqa=0;
  for(j=g->countY; j>0; j--) {  // every branch of curves
    if(!findmatch(g,m))//will find the y value of a certain frequency and curve
    {
      p->setGraphEnd();
      return;
    }
    m++;
    yp = g->gy;
    //with the points, it will calculate the Amplitude and phase of the sine wave
    af = atan2 (yp[1],yp[0]);
    A = sqrt(yp[1]*yp[1] +yp[0]*yp[0]);

    xn = 0.0;
    yn = A*sin(af);
    calcCoordinateP(&xn, &yn, py, p, pa);
    ++p;
    for(i = 1;i < sc*50;i++)
    {
      xn = i*xAxis.up/(sc*50);
      yn = A*sin(2*pi*freq[0]*xn + af);
      calcCoordinateP(&xn, &yn, py, p, pa);
      ++p;
      if(Counter >= 2)   // clipping only if an axis is manual
        clip(p);
    }
    if((p-3)->isStrokeEnd() && !(p-3)->isBranchEnd())
	p -= 3;  // no single point after "no stroke"
    else if((p-2)->isBranchEnd() && !(p-1)->isGraphEnd()) {
	if((!(p-1)->isPt()))
	  --p; // erase last hidden point
      
    }
    (p++)->setBranchEnd();
  }
  p->setGraphEnd();
  return;
  
}

// ------------------------------------------------------------
bool Waveac::insideDiagram(float x, float y) const
{
  return (regionCode(x, y) == 0);
}

// ------------------------------------------------------------
void Waveac::clip(Graph::iterator &p) const
{
  rectClip(p);
}

// ------------------------------------------------------------
Diagram* Waveac::newOne()
{
  return new Waveac();
}

// ------------------------------------------------------------
Element* Waveac::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("AC Temporal diagram");
  BitmapFile = (char *) "wave";

  if(getNewOne)  return new Waveac();
  return 0;
}
