/*
 * phasordiagram.cpp - Implementation file for the phasor diagram
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


#include <QMessageBox>

#include "phasordiagram.h"
#include "qucs.h"
#include "misc.h"

PhasorDiagram::PhasorDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  x1 = 10;     // position of label text
  y1 = 2;
  x2 = 200;    // initial size of diagram
  y2 = 200;
  x3 = 207;    // with some distance for text
  Name = "Phasor";
  
  calcDiagram();
}

PhasorDiagram::~PhasorDiagram()
{
}
// --------------------------------------------------------------
void PhasorDiagram::finishMarkerCoordinates(float& fCX, float& fCY) const
{
  if(!insideDiagram(fCX, fCY)) {
	  fCX = fCY = 0.0;
  }
}

// ------------------------------------------------------------
void PhasorDiagram::calcCoordinatePh(const double* yD,float *px, float *py, Axis const *pa,Axis const *pt) const
{
 double yr = yD[0];
 double yi = yD[1];
 	

  *px = float((yr - pt->low) / (pt->up - pt->low) * double(x2));
  *py = float((yi - pa->low) / (pa->up - pa->low) * double(y2));
 
  if(std::isfinite(*px))
    if(std::isfinite(*py))
      return;

  *px = *py = 0.0;
 
}
// --------------------------------------------------------------
void PhasorDiagram::calcLimits()
{
  double a, b,c;
  calcAxisScale(&xAxis, a, b, c, xAxis.step, double(x2));
  xAxis.limit_min = xAxis.low;
  xAxis.limit_max = xAxis.up;
  calcAxisScale(&yAxis, a, b, c, yAxis.step, double(y2));
  yAxis.limit_min = yAxis.low;
  yAxis.limit_max = yAxis.up;
  calcAxisScale(&zAxis, a, b, c, zAxis.step, double(y2));
  zAxis.limit_min = zAxis.low;
  zAxis.limit_max = zAxis.up;
}

// --------------------------------------------------------------
int PhasorDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

 double GridStep, zD, zDstep, GridNum;
  // get size of text using the screen-compatible metric
  QFontMetrics metrics(QucsSettings.font, 0);
  y1 = QucsSettings.font.pointSize() + 6;

  x1 = 10;      // position of label text
  x3 = x2 + 7;
  QString tmp;
  int  z, valid = 0;

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

  calcAxisScale(&xAxis, GridNum, zD, zDstep, GridStep, double(x2));

  zD += 0.5;     // perform rounding
  z = int(zD);   //  "int(...)" implies "floor(...)"
  while((z <= x2) && (z >= 0)) {    // create all grid lines
    if(xAxis.GridOn)  if(z < x2)  if(z > 0)
      Lines.prepend(new Line(z, y2, z, 0, GridPen)); // x grid
    Lines.append(new Line(z, 5, z, -5, QPen(Qt::black,0)));   // x tick marks
    zD += zDstep;
    z = int(zD);
  }
  
  if(xAxis.up >= 0.0) if(xAxis.low <= 0.0) {  // paint origin cross ?
    z = int(double(x2) * fabs(xAxis.low / (xAxis.up-xAxis.low)) + 0.5);
    Lines.append(new Line(z, 0, z, y2, QPen(Qt::black,0)));
  }
  // ====  y grid  =======================================================
  if(zAxis.numGraphs > 0) if(calcYAxis(&zAxis, x2)) valid |= 2;
  if(yAxis.numGraphs > 0) if(calcYAxis(&yAxis, 0)) {
    valid |= 1;
    if(yAxis.up >= 0.0) if(yAxis.low <= 0.0) {  // paint origin cross ?
      z = int(double(y2) * fabs(yAxis.low / (yAxis.up-yAxis.low)) + 0.5);
      Lines.append(new Line(0, z, x2, z, QPen(Qt::black,0)));
    }
  }

  calcRestAxis(&xAxisV,&yAxisV,&zAxisV);
  calcRestAxis(&xAxisI,&yAxisI,&zAxisI);
  calcRestAxis(&xAxisP,&yAxisP,&zAxisP);
  calcRestAxis(&xAxisZ,&yAxisZ,&zAxisZ);

  // outer frame
  Lines.append(new Line(0,  y2, x2, y2, QPen(Qt::black,0)));
  Lines.append(new Line(x2, y2, x2,  0, QPen(Qt::black,0)));
  Lines.append(new Line(0,   0, x2,  0, QPen(Qt::black,0)));
  Lines.append(new Line(0,  y2,  0,  0, QPen(Qt::black,0)));
  return valid;
}
// ------------------------------------------------------------
/*this function will compare the global limits with the specific limit of the graph
  to represent vector in a way that is all visible */
void PhasorDiagram::calcRestAxis(Axis *xA, Axis *yA , Axis *zA) 
{
       
    if(xAxis.max > 3.0 * xA->max)
    {
      xA->up  = 3.0 * xA->max * xAxis.up / xAxis.max;   
      xA->low = 3.0 * xA->min * xAxis.low / xAxis.min;
      yA->up  = 3.0 * yA->max * yAxis.up / yAxis.max;   
      yA->low = 3.0 * yA->min * yAxis.low / yAxis.min;
      zA->up  = 3.0 * zA->max * zAxis.up / zAxis.max;   
      zA->low = 3.0 * zA->min * zAxis.low / zAxis.min;
    }
    else
    {
      xA->up  = xAxis.up;   
      xA->low = xAxis.low;
      yA->up  = yAxis.up;   
      yA->low = yAxis.low;
      zA->up  = zAxis.up;   
      zA->low = zAxis.low;
    }
}
// ------------------------------------------------------------

void PhasorDiagram::calcData(Graph *g)
{
  double *pz = g->cPointsY;
  if(!pz)  return;
  if(g->numAxes() < 1) return;

  int z, i;
  int Size = ((2*(g->count(0)) + 1)*nfreqt * g->countY) + 10;

  g->resizeScrPoints(Size);
  auto p = g->begin();
  auto p_end = g->begin();
  
  p_end += Size - 9;   // limit of buffer
  p->setStrokeEnd();
  ++p;
  assert(p!=g->end());

  Axis *pa,*pt;
  findaxisA(g);
  pt = xAxisA;
  if(g->yAxisNo == 0)  pa = yAxisA;
  else  pa = zAxisA;

  double yf[] = {0.0,0.0};//points of the beginning of the vector 
  float xn,yn;
  float f1,f2; 
  char a=0; 
  double *yp;

  //meaning that it only works in AC 
  z = QString::compare(g->axis(0)->Var,"acfrequency",Qt::CaseInsensitive); 
  if(z != 0 )
  {
    sfreq = "0 Hz";
    p->setGraphEnd();
    return;
  }
  for(z=0;z<nfreqt;z++)
  {
    nfreqa=z;
    for(i=0;i<g->countY; i++) {  // every branch of curves
      if(!findmatch(g,i))//will find the y value of a certain frequency and curve
      {  
        p->setGraphEnd();
	return;
      }
	 
      yp = g->gy;
      calcCoordinatePh(yf, &f1, &f2, pa, pt);//coordinates of the beginning of vector(0,0)
      p->setScr(f1, f2);
      if(!insideDiagramPh(p,&xn,&yn)) a=1;  // within diagram ?
	      		
      ++p;
      calcCoordinatePh(yp, &f1, &f2, pa,pt);//coordinates of the end of vector
      p->setScr(f1, f2);
      /*if the point(0,0) of vector isn't inside diagram, it will try to find new 
        coordinates close to the old coordinates that are within the diagram*/
      if(a==1)
      {
        if(newcoordinate(p,&xn,&yn)) (p-1)->setScr(xn, yn);//saves if a new point is found
	else //it will not create a vector
	{
          --p;
	  continue;  
	}
      }
      if(insideDiagramPh(p,&xn,&yn)) p++;   // if final point of vector is within diagram
      else//it will find a new one closer to the old ones
      {
        if(newcoordinate(p,&xn,&yn))//saves if a new point is found
        {
	  (p++)->setScr(xn, yn);
          (p++)->setScrX(-1);
        }
        else --p;//it will not create a vector
      }
      (p++)->setBranchEnd();
    }
  }
  p->setGraphEnd();
  return;
}

Diagram* PhasorDiagram::newOne()
{
  return new PhasorDiagram();
}

// ------------------------------------------------------------
Element* PhasorDiagram::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("AC Phasorial diagram");
  BitmapFile = (char *) "phasor";

  if(getNewOne)  return new PhasorDiagram();
  return 0;
}

