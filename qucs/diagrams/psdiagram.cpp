/***************************************************************************
                        psdiagram.cpp  -  description
                             -------------------
    begin                : Sat Nov 06 2004
    copyright            : (C) 2004 by Michael Margraf
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

#include "psdiagram.h"
#include "main.h"

#include <math.h>

// mixed polar and smith diagram
PSDiagram::PSDiagram(int _cx, int _cy, bool _polarUp)
			: Diagram(_cx, _cy)
{
  x1 = x3 = 10;  // position of label text
  y1 = y3 = 16;
  x2 = 200;      // initial size of diagram
  y2 = 200;
  polarUp = _polarUp;  // polar diagram upper half ?
  Name = "PS";

  calcDiagram();
}

PSDiagram::~PSDiagram()
{
}

// ------------------------------------------------------------
void PSDiagram::calcCoordinate(double, double yr, double yi,
				  int *px, int *py, Axis* pa)
{
  *px = (x2>>1)+int(yr/pa->up*double(x2>>1) + 0.5);
  *py = (y2>>1)+int(yi/pa->up*double(y2>>1) + 0.5);
}

// --------------------------------------------------------------
int PSDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  // x and y line
  Lines.append(new Line(x2>>1, y2, x2>>1, y2>>1, GridPen));
  Lines.append(new Line(0, y2>>1, x2, y2>>1, GridPen));

  double phi, Expo, Base;
  xAxis.low = ylAxis.low = yrAxis.low = 0.0;
  xAxis.up  = xAxis.max;
  if(fabs(ylAxis.min) > ylAxis.max)
    ylAxis.max = fabs(ylAxis.min);  // also fit negative values
  if(fabs(yrAxis.min) > yrAxis.max)
    yrAxis.max = fabs(yrAxis.min);  // also fit negative values

  QFontMetrics  metrics(QucsSettings.font);
  int tHeight = metrics.lineSpacing();

  Axis *pAxis = &yrAxis;   // axis for polar diagram
  Axis *sAxis = &ylAxis;   // axis for smith diagram
  // ..........................................................
  // polar half
  if(xAxis.GridOn) {

    double numGrids = floor(double(x2)/80.0); // minimal grid is 40 pixel
    Expo = floor(log10(pAxis->max/numGrids));
    Base = pAxis->max/numGrids/pow(10.0,Expo); // get first significant digit
    if(Base < 3.5) {       // use only 1, 2 and 5, which ever is best fitted
      if(Base < 1.5) Base = 1.0;
      else Base = 2.0;
    }
    else {
      if(Base < 7.5) Base = 5.0;
      else { Base = 1.0; Expo++; }
    }
    double GridStep = Base * pow(10.0,Expo); // grid distance in real values
    numGrids -= floor(numGrids - pAxis->max/GridStep); // correct num errors
    pAxis->up = GridStep*numGrids;
    double zD = double(x2) / numGrids;   // grid distance in pixel


    int z;
    double zDstep = zD;
    double GridNum  = 0.0;
    for(int i=int(numGrids); i>1; i--) {    // create all grid circles
      z = int(zD);
      GridNum += GridStep;
      if(fabs(Expo) < 3.0)
        Texts.append(new Text(((x2+z)>>1)-10, (y2>>1)+tHeight,
			 StringNum(GridNum)));
      else
        Texts.append(new Text(((x2+z)>>1)-10, (y2>>1)+tHeight,
			 StringNum(GridNum, 'e', 0)));

      phi = 16.0*180.0/M_PI*atan(double(2*(tHeight+3))/zD);
      Arcs.append(new Arc((x2-z)>>1, (y2+z)>>1, z, z, int(phi), 16*180,
			  GridPen));
      zD += zDstep;
    }
  }
  else {
    Expo = floor(log10(pAxis->max));
    Base = ceil(pAxis->max/pow(10.0,Expo) - 0.01);
    pAxis->up = Base * pow(10.0,Expo);  // separate Base * 10^Expo
  }



  // ..........................................................
  // smith half
  sAxis->low = 0.0;
  if(sAxis->max > 1.01) sAxis->up = 1.05*sAxis->max;
  else sAxis->up = 1.0;

if(xAxis.GridOn) {
  int dx2 = x2>>1;

  int GridX = 4;    // number of arcs with re(z)=const
  int GridY = 4;    // number of arcs with im(z)=const

  double im, n_cos, n_sin, real, real1, real2, root;
  double rMAXq = sAxis->up*sAxis->up;
  int    theta, beta, m, x, y;

  // ....................................................
  // draw arcs with im(z)=const

  for(m=1; m<GridY; m++) {
    n_sin = M_PI*double(m)/double(GridY);
    n_cos = cos(n_sin);
    n_sin = sin(n_sin);
    im = (1-n_cos)/n_sin * pow(sAxis->up,0.7); // up^0.7 is beauty correction
    x  = int((1-im)/sAxis->up*dx2);
    y  = int(im/sAxis->up*x2);

    if(sAxis->up <= 1.0) {       // Smith chart with |r|=1
      beta  = int(16.0*180.0*atan2(n_sin-im,n_cos-1)/M_PI);
      if(beta<0) beta += 16*360;
      theta = 16*270-beta;
    }
    else {         // Smith chart with |r|>1
      im = 1/im;
      real  = (rMAXq+1)/(rMAXq-1);
      root =  real*real - im*im-1;
      if(root<0) {       // circle lies completely within the Smith chart ?
        beta = 0;        // yes, ...
        theta = 16*360;  // ... draw whole circle
      }
      else {
	// calculate both intersections with most outer circle
	real1 =  sqrt(root)-real;
	real2 = -sqrt(root)-real;

	root  = (real1+1)*(real1+1) + im*im;
	n_cos = (real1*real1 + im*im - 1) / root;
	n_sin = 2*im / root;
	beta  = int(16.0*180.0*atan2(n_sin-1/im,n_cos-1)/M_PI);
	if(beta<0) beta += 16*360;


        root  = (real2+1)*(real2+1) + im*im;
        n_cos = (real2*real2 + im*im - 1) / root;
        n_sin = 2*im / root;
        theta  = int(16.0*180.0*atan2(n_sin-1/im,n_cos-1)/M_PI);
        if(theta<0) theta += 16*360;
        theta = theta - beta;   // arc length
        if(theta < 0) theta = 16*360+theta;
      }
    }

    Arcs.append(new Arc(dx2+x, dx2+y, y, y, beta, theta, GridPen));
//    Arcs.append(new Arc(dx2+x, dx2, y, y, 16*360-beta-theta, theta, GridPen));
  }

  // ....................................................
  // draw  arcs with Re(z)=const
  for(m=1; m<GridX; m++) {
    im = m*(sAxis->up+1)/GridX - sAxis->up;
    x  = int(im/sAxis->up*double(dx2));
    im = (1-im);
    y  = int(im/sAxis->up*double(dx2));    // diameter

    Arcs.append(new Arc(dx2+x, dx2+(y>>1), y, y, 16*180, 16*360, GridPen));
/*        if abs(abs(r)-1) > 0.4      // do not draw if to close to most outer circle (beauty correction)
        */

    if(sAxis->up > 1.0) {  // draw arcs on the rigth-handed side ?
      im = (rMAXq-1)/(im*(im/2+1)) - 1;
      if(im>=1)
        Arcs.append(new Arc(dx2+x+y, dx2+(y>>1), y, y, 16*180, 16*360,
				GridPen));
      else {
        beta  = int(16.0*180.0/M_PI*acos(im));
        theta = 2*(16*180-beta);
//        Arcs.append(new Arc(dx2+x+y, dx2+(y>>1), y, y, beta, theta, GridPen));
        Arcs.append(new Arc(dx2+x+y, dx2+(y>>1), y, y, 16*180, theta, GridPen));
      }
    }
  }


  // horizontal line Im(r)=0
//  Lines.append(new Line(0, dx2, x2, dx2, GridPen));

  // ....................................................
  if(sAxis->up > 1.0) {  // draw circle with |r|=1 ?
    x = int(x2/sAxis->up);
    Arcs.append(new Arc(dx2-(x>>1), dx2+(x>>1), x, x, 16*180, 16*360,
			QPen(QPen::black,0)));

    // vertical line Re(r)=1 (visible only if |r|>1)
    x = int(x2/sAxis->up)>>1;
    y = int(sqrt(rMAXq-1)/sAxis->up*dx2);
//    Lines.append(new Line(dx2+x, dx2+y, dx2+x, dx2-y, GridPen));
    Lines.append(new Line(dx2+x, dx2+y, dx2+x, dx2, GridPen));

    Texts.append(
	new Text(0, QucsSettings.font.pointSize()+4, StringNum(sAxis->up)));
  }
}  // of if(GridOn)  =  end of smith


  // ............................................................
  // create outer circle
  if(fabs(Expo) < 3.0)
    Texts.append(new Text(x2-10, (y2>>1)+tHeight, StringNum(pAxis->up)));
  else
    Texts.append(new Text(x2-10, (y2>>1)+tHeight,
			  StringNum(pAxis->up, 'e', 0)));
  phi = 16.0*180.0/M_PI*atan(double(2*(tHeight+3))/double(x2));
  Arcs.append(new Arc(0, y2, x2, y2, int(phi), 16*180,
			QPen(QPen::black,0)));

  return 1;
}

// ------------------------------------------------------------
PSDiagram* PSDiagram::newOne()
{
  return new PSDiagram();
}
