/***************************************************************************
                          rectdiagram.cpp  -  description
                             -------------------
    begin                : Thu Oct 2 2003
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

#include "rectdiagram.h"

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
  double *px = g->cPointsX;
  double *py = g->cPointsY;
  for(int z=g->count; z>0; z--) {
    *(p++) = int(((*(px++))-xlow)/(xup-xlow)*x2);
    // preserve negative values if not complex number
    if(fabs(*(py+1)) < 1e-250) {
      *(p++) = int(((*(py++))-ylow)/(yup-ylow)*y2);
      py++;   // do not use imaginary part
    }
    else {  // calculate magnitude of complex number
      *(p++) = int((sqrt((*py)*(*py) +
               (*(py+1))*(*(py+1)))-ylow)/(yup-ylow)*y2);
      py += 2;
    }
  }
}

// --------------------------------------------------------------
void RectDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  xlow = xmin;  xup = xmax;
  ylow = ymin;  yup = ymax;

  // ====  x grid  =======================================================
  double numGrids = floor(double(x2)/60.0);   // minimal grid is 60 pixel
  double Base = (xmax-xmin)/numGrids;
  double Expo = floor(log10(Base));
  Base = Base/pow(10.0,Expo);         // separate first significant digit
  if(Base < 3.5) {      // use only 1, 2 and 5, which ever is best fitted
    if(Base < 1.5) Base = 1.0;
    else Base = 2.0;
  }
  else {
    if(Base < 7.5) Base = 5.0;
    else { Base = 1.0; Expo++; }
  }
  double GridStep = Base * pow(10.0,Expo);  // distance between grids in real coordinates
  double corr = floor((xmax-xmin)/GridStep - numGrids);
  if(corr < 0.0) corr++;
  numGrids += corr;     // correct rounding faults


  double zD = GridStep-fmod(xmax, GridStep);
  if(zD/GridStep < 0.2)  xup = xmax+zD;   // expand grid to the right edge of diagram ?

  zD = fmod(xmin, GridStep); // expand grid to the left edge of diagram ?
  if(zD/GridStep < 0.2) { xlow = xmin-zD;  zD = 0.0; }
  else zD = GridStep-zD;

  double zDstep = GridStep/(xup-xlow)*double(x2);  // distance between grids in pixel
  double GridNum  = xlow + zD;
  zD /= (xup-xlow)/double(x2);

  int z = int(zD);
  while(z <= x2) {    // create all grid lines
    if(fabs(Expo) < 3.0)
      Texts.append(new Text(z-10, -17, QString::number(GridNum)));
    else
      Texts.append(new Text(z-10, -17, QString::number(GridNum, 'e', 0)));
    GridNum += GridStep;

    if(GridOn)  if(z < x2)  if(z > 0)
      Lines.append(new Line(z, y2, z, 0, QPen(QPen::lightGray,1)));   // x axis grid
    Lines.append(new Line(z, 5, z, -5, QPen(QPen::black,1)));   // x axis marks
    zD += zDstep;
    z = int(zD);
  }


  // ====  y grid  =======================================================
  numGrids = floor(double(y2)/60.0);   // minimal grid is 60 pixel
  Base = (ymax-ymin)/numGrids;
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
  GridStep = Base * pow(10.0,Expo);     // distance between grids in real coordinates
  corr = floor((ymax-ymin)/GridStep - numGrids);
  if(corr < 0.0) corr++;
  numGrids += corr;     // correct rounding faults


  zD = GridStep-fmod(ymax, GridStep);
  if(fabs(zD/GridStep) <= 0.2)  yup = ymax+zD; // expand grid to the upper edge of diagram ?

  zD = fmod(ymin, GridStep); // expand grid to the lower edge of the diagram ?
  GridNum = zD/GridStep;
  if(GridNum > 0.2) zD = GridStep-zD;
  else if(GridNum < -0.2) zD *= -1.0;
       else { ylow = ymin-zD;  zD = 0.0; }
//qDebug("zD: %g", zD);

  zDstep = GridStep/(yup-ylow)*double(y2);     // distance between grids in pixel
  GridNum  = ylow + zD;
  zD /= (yup-ylow)/double(y2);

  QRect r;
  QWidget w;
  QPainter p(&w);
  QString tmp;
  p.setFont(QFont("Helvetica",12, QFont::Light));

  int maxWidth = 0;
  z = int(zD);
  while(z <= y2) {    // create all grid lines
    if(fabs(Expo) < 3.0)  tmp = QString::number(GridNum);
    else QString::number(GridNum, 'e', 0);
    r = p.boundingRect(0,0,0,0,Qt::AlignAuto,tmp);      // get width of text
    if(maxWidth < r.width()) maxWidth = r.width();

    Texts.append(new Text(-r.width()-7, z-5, tmp));     // place text aligned right
    GridNum += GridStep;

    if(GridOn)  if(z < y2)  if(z > 0)
      Lines.append(new Line(0, z, x2, z, QPen(QPen::lightGray,1)));   // y axis grid
    Lines.append(new Line(-5, z, 5, z, QPen(QPen::black,1)));   // y axis marks
    zD += zDstep;
    z = int(zD);
  }
  x1 = maxWidth+14;


  // outer frame
  Lines.append(new Line(0,  y2, x2, y2, QPen(QPen::black,1)));
  Lines.append(new Line(x2, y2, x2,  0, QPen(QPen::black,1)));
  Lines.append(new Line(0,   0, x2,  0, QPen(QPen::black,1)));
  Lines.append(new Line(0,  y2,  0,  0, QPen(QPen::black,1)));
}

// ------------------------------------------------------------
RectDiagram* RectDiagram::newOne()
{
  return new RectDiagram();
}
