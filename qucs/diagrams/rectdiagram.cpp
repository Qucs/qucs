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
  Name = "Rect";

  xlog = ylog = false;
  calcDiagram();
}

RectDiagram::~RectDiagram()
{
}

// ------------------------------------------------------------
void RectDiagram::calcCoordinate(double x, double yr, double yi,
				 int *px, int *py)
{
  if(xlog)  *px = int(log10(x / xlow)/log10(xup / xlow)*double(x2) + 0.5);
  else  *px = int((x-xlow)/(xup-xlow)*double(x2) + 0.5);

  if(ylog)
    *py = int(log10(sqrt(yr*yr + yi*yi)/fabs(ylow)) / log10(yup/ylow)
		    *double(y2) + 0.5);
  else {
    if(fabs(yi) < 1e-250)  // preserve negative values if not complex number
      *py = int((yr-ylow)/(yup-ylow)*double(y2) + 0.5);
    else   // calculate magnitude of complex number
      *py = int((sqrt(yr*yr + yi*yi)-ylow)/(yup-ylow)*double(y2) + 0.5);
  }
}

// --------------------------------------------------------------
bool RectDiagram::calcDiagram()
{
  if(!Lines.isEmpty()) Lines.clear();
  if(!Texts.isEmpty()) Texts.clear();
  if(!Arcs.isEmpty()) Arcs.clear();

  if(fabs(xmax-xmin) < 1e-200) { xmax += fabs(xmax); xmin -= fabs(xmin); }
  if(fabs(ymax-ymin) < 1e-200) { ymax += fabs(ymax); ymin -= fabs(ymin); }
  if(ymax == 0) if(ymin == 0) { ymax = 1; ymin = -1; }
  if(xmax == 0) if(xmin == 0) { xmax = 1; xmin = -1; }
  xlow = xmin;  xup = xmax;
  ylow = ymin;  yup = ymax;


  int z=0;
  double numGrids, Base, Expo, GridStep, corr, zD, zDstep, GridNum;

  QSize r;
  QString tmp;
  QFontMetrics  metrics(QucsSettings.font);
  int maxWidth = 0;
  bool set_log = false;

  // ====  x grid  =======================================================
if(xlog) {
  if(xmax*xmin <= 0.0) goto Frame;  // invalid
  if(xmax < 0.0) {
    corr = xmin;
    xmin = -xmax;
    xmax = -corr;
    corr = xlow;
    xlow = -xup;
    xup  = -corr;
    set_log = true;
  }

  Expo = floor(log10(xmax));
  Base = xmax/pow(10.0,Expo);
  if(Base > 3.0001) xup = pow(10.0,Expo+1.0);
  else  if(Base < 1.0001) xup = pow(10.0,Expo);
	else xup = 3.0 * pow(10.0,Expo);

  Expo = floor(log10(xmin));
  Base = xmin/pow(10.0,Expo);
  if(Base < 2.999) xlow = pow(10.0,Expo);
  else  if(Base > 9.999) xlow = pow(10.0,Expo+1.0);
	else xlow = 3.0 * pow(10.0,Expo);

  corr = double(x2) / log10(xup / xlow);

  zD = xlow;
  zDstep = pow(10.0,Expo);
  if(set_log) z = x2;
  while((z <= x2) && (z >= 0)) {    // create all grid lines
    if(GridOn)  if(z < x2)  if(z > 0)
      Lines.append(new Line(z, y2, z, 0, GridPen));  // x grid

    if((zD < 1.5*zDstep) || (z == 0) || (z == x2)) {
      if(fabs(Expo) < 3.0)  tmp = QString::number(zD);
      else  tmp = QString::number(zD, 'e', 1);

      if(set_log)  Texts.append(new Text(z-10, -17, '-'+tmp));
      else  Texts.append(new Text(z-10, -17, tmp));

      Lines.append(new Line(z, 5, z, -5, QPen(QPen::black,0)));  // x marks
    }

    zD += zDstep;
    if(zD > 9.5*zDstep)  zDstep *= 10.0;
    z = int(corr*log10(zD / xlow) + 0.5); // "int(...)" implies "floor(...)"
    if(set_log)  z = x2 - z;
  }
  if(set_log) {   // set back values ?
    corr = xmin;
    xmin = -xmax;
    xmax = -corr;
    corr = xlow;
    xlow = -xup;
    xup  = -corr;
  }
}
else {  // not logarithmical
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
      Lines.append(new Line(z, y2, z, 0, GridPen)); // x grid
    Lines.append(new Line(z, 5, z, -5, QPen(QPen::black,0)));   // x marks
    zD += zDstep;
    z = int(zD);
  }
} // of "if(xlog) ... else ..."



  // ====  y grid  =======================================================
  set_log = false;
if(ylog) {
  if(ymax*ymin <= 0.0) goto Frame;  // invalid
  if(ymax < 0.0) {
    corr = ymin;
    ymin = -ymax;
    ymax = -corr;
    corr = ylow;
    ylow = -yup;
    yup  = -corr;
    set_log = true;
  }

  Expo = floor(log10(ymax));
  Base = ymax/pow(10.0,Expo);
  if(Base > 3.0001) yup = pow(10.0,Expo+1.0);
  else  if(Base < 1.0001) yup = pow(10.0,Expo);
	else yup = 3.0 * pow(10.0,Expo);

  Expo = floor(log10(ymin));
  Base = ymin/pow(10.0,Expo);
  if(Base < 2.999) ylow = pow(10.0,Expo);
  else  if(Base > 9.999) ylow = pow(10.0,Expo+1.0);
	else ylow = 3.0 * pow(10.0,Expo);

  corr = double(y2) / log10(yup / ylow);

  z = 0;
  zD = ylow;
  zDstep = pow(10.0,Expo);
  if(set_log) z = y2;
  while((z <= y2) && (z >= 0)) {    // create all grid lines
    if(GridOn)  if(z < y2)  if(z > 0)
      Lines.append(new Line(0, z, x2, z, GridPen));  // y grid

    if((zD < 1.5*zDstep) || (z == 0)) {
      if(fabs(Expo) < 3.0)  tmp = QString::number(zD);
      else  tmp = QString::number(zD, 'e',1);
      if(set_log)  tmp = '-'+tmp;

      r = metrics.size(0, tmp);  // width of text
      if(maxWidth < r.width()) maxWidth = r.width();
      Texts.append(new Text(-r.width()-7, z-5, tmp));  // text aligned right

      Lines.append(new Line(-5, z, 5, z, QPen(QPen::black,0)));  // y marks
    }

    zD += zDstep;
    if(zD > 9.5*zDstep)  zDstep *= 10.0;
    z = int(corr*log10(zD / ylow) + 0.5); // "int(...)" implies "floor(...)"
    if(set_log)  z = y2 - z;
  }
  if(set_log) {   // set back values ?
    corr = ymin;
    ymin = -ymax;
    ymax = -corr;
    corr = ylow;
    ylow = -yup;
    yup  = -corr;
  }
}
else {  // not logarithmical
  numGrids = floor(double(y2)/60.0);   // minimal grid is 60 pixel
  if(numGrids < 1.0) Base = ymax-ymin;
  else Base = (ymax-ymin)/numGrids;
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

  zD += 0.5;     // perform rounding
  z = int(zD);   //  "int(...)" implies "floor(...)"
  while(z <= y2) {    // create all grid lines
    if(fabs(GridNum) < 0.01*pow(10.0, Expo)) GridNum = 0.0;// make 0 really 0
    if(fabs(Expo) < 3.0)  tmp = QString::number(GridNum);
    else tmp = QString::number(GridNum, 'e',1);
    r = metrics.size(0, tmp);  // width of text
    if(maxWidth < r.width()) maxWidth = r.width();

    Texts.append(new Text(-r.width()-7, z-5, tmp));  // text aligned right
    GridNum += GridStep;

    if(GridOn)  if(z < y2)  if(z > 0)
      Lines.append(new Line(0, z, x2, z, GridPen));  // y grid
    Lines.append(new Line(-5, z, 5, z, QPen(QPen::black,0)));   // y marks
    zD += zDstep;
    z = int(zD);
  }
} // of "if(xlog) ... else ..."
  x1 = maxWidth+14;


  // outer frame
  Lines.append(new Line(0,  y2, x2, y2, QPen(QPen::black,0)));
  Lines.append(new Line(x2, y2, x2,  0, QPen(QPen::black,0)));
  Lines.append(new Line(0,   0, x2,  0, QPen(QPen::black,0)));
  Lines.append(new Line(0,  y2,  0,  0, QPen(QPen::black,0)));
  return true;

Frame:
  // outer frame
  Lines.append(new Line(0,  y2, x2, y2, QPen(QPen::red,0)));
  Lines.append(new Line(x2, y2, x2,  0, QPen(QPen::red,0)));
  Lines.append(new Line(0,   0, x2,  0, QPen(QPen::red,0)));
  Lines.append(new Line(0,  y2,  0,  0, QPen(QPen::red,0)));
  return false;
}

// ------------------------------------------------------------
RectDiagram* RectDiagram::newOne()
{
  return new RectDiagram();
}
