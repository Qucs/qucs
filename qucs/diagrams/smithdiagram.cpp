/***************************************************************************
                          smithdiagram.cpp  -  description
                             -------------------
    begin                : Sat Oct 18 2003
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

#include "smithdiagram.h"

#include <math.h>


SmithDiagram::SmithDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  x1 = 10;     // position of label text
  y1 = 16;
  x2 = 200;    // initial size of diagram
  y2 = 200;

  GridOn = true;
  Name = "Smith";

  calcDiagram();    // calculate circles for smith chart with |r|=1
}

SmithDiagram::~SmithDiagram()
{
}

// ------------------------------------------------------------
// calculate the screen coordinates for the graph data
void SmithDiagram::calcData(Graph *g)
{
  int *p = g->Points;
//  if(p == 0) return;
  for(cPoint *cp = g->cPoints.first(); cp != 0; cp = g->cPoints.next()) {
    *(p++) = (x2>>1)+int(cp->yr/ymax*double(x2>>1));
    *(p++) = (y2>>1)+int(cp->yi/ymax*double(y2>>1));
  }
}


// ------------------------------------------------------------
// calculate the circles and arcs of the smith chart
void SmithDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  xlow = ylow = 0.0;
  if(ymax > 1.01) xup = yup *= 1.05;
  else xup = yup = 1.0;

if(GridOn) {
  int dx2 = x2>>1;

  int GridX = 4;    // number of arcs with re(z)=const
  int GridY = 4;    // number of arcs with im(z)=const

  double im, n_cos, n_sin, real, real1, real2, root, rMAXq = xup*xup;
  int    theta, beta, m, x, y;

  // ....................................................
  // draw arcs with im(z)=const

  for(m=1; m<GridY; m++) {
    n_sin = M_PI*double(m)/double(GridY);
    n_cos = cos(n_sin);
    n_sin = sin(n_sin);
    im = (1-n_cos)/n_sin * pow(xup,0.7);  // xup^0.7 is beauty correction
    x  = int((1-im)/xup*dx2);
    y  = int(im/xup*x2);

    if(xup <= 1.0) {       // Smith chart with |r|=1
      beta  = int(16*180*atan2(n_sin-im,n_cos-1)/M_PI);
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
        real1 =  sqrt(root)-real;   // calculate both intersections with most outer circle
        real2 = -sqrt(root)-real;

        root  = (real1+1)*(real1+1) + im*im;
        n_cos = (real1*real1 + im*im - 1) / root;
        n_sin = 2*im / root;
        beta  = int(16*180*atan2(n_sin-1/im,n_cos-1)/M_PI);
        if(beta<0) beta += 16*360;


        root  = (real2+1)*(real2+1) + im*im;
        n_cos = (real2*real2 + im*im - 1) / root;
        n_sin = 2*im / root;
        theta  = int(16*180*atan2(n_sin-1/im,n_cos-1)/M_PI);
        if(theta<0) theta += 16*360;
        theta = theta - beta;   // arc length
        if(theta < 0) theta = 16*360+theta;
      }
    }

    Arcs.append(new Arc(dx2+x, dx2+y, y, y, beta, theta, QPen(QPen::lightGray,1)));
    Arcs.append(new Arc(dx2+x, dx2  , y, y, 16*360-beta-theta, theta, QPen(QPen::lightGray,1)));
  }

  // ....................................................
  // draw  arcs with Re(z)=const
  for(m=1; m<GridX; m++) {
    im = m*(xup+1)/GridX - xup;
    x  = int(im/xup*double(dx2));
    im = (1-im);
    y  = int(im/xup*double(dx2));    // diameter

    Arcs.append(new Arc(dx2+x, dx2+(y>>1), y, y, 0, 16*360, QPen(QPen::lightGray,1)));
/*        if abs(abs(r)-1) > 0.4      // do not draw if to close to most outer circle (beauty correction)
        */

    if(xup > 1.0) {    // draw arcs on the rigth-handed side ?
      im = (rMAXq-1)/(im*(im/2+1)) - 1;
      if(im>=1)
        Arcs.append(new Arc(dx2+x+y, dx2+(y>>1), y, y, 0, 16*360, QPen(QPen::lightGray,1)));
      else {
        beta  = int(16*180/M_PI*acos(im));
        theta = 2*(16*180-beta);
        Arcs.append(new Arc(dx2+x+y, dx2+(y>>1), y, y, beta, theta, QPen(QPen::lightGray,1)));
      }
    }
  }


  // horizontal line Im(r)=0
  Lines.append(new Line(0, dx2, x2, dx2, QPen(QPen::lightGray,1)));

  // ....................................................
  if(xup > 1.0) {  // draw circle with |r|=1 ?
    x = int(x2/xup);
    Arcs.append(new Arc(dx2-(x>>1), dx2+(x>>1), x, x, 0, 16*360, QPen(QPen::black,1)));

    // vertical line Re(r)=1 (visible only if |r|>1)
    x = int(x2/xup)>>1;
    y = int(sqrt(rMAXq-1)/xup*dx2);
    Lines.append(new Line(dx2+x, dx2+y, dx2+x, dx2-y, QPen(QPen::lightGray,1)));

    Texts.append(new Text(0, 4, QString::number(xup)));
  }
}  // of if(GridOn)

  Arcs.append(new Arc(0, x2, x2, x2, 0, 16*360, QPen(QPen::black,1)));  // outer most circle
}

// ------------------------------------------------------------
SmithDiagram* SmithDiagram::newOne()
{
  return new SmithDiagram();
}
