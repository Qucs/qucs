/***************************************************************************
                          graph.cpp  -  description
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

#include "graph.h"

#include <math.h>


Graph::Graph(const QString& _Line)
{
  Line = _Line;

  Type = isGraph;

  count  = 0;    // no points in graph
  Points = 0;
  Thick  = 1;
  Color  = 0x0000ff;  // blue
  isSelected = false;

  Points = 0;
  cPointsX = cPointsY = 0;
//  cPoints.setAutoDelete(true);
}

Graph::~Graph()
{
  if(Points != 0) delete[] Points;
}

// ---------------------------------------------------------------------
void Graph::paint(QPainter *p, int x0, int y0)
{
  int *pp = Points;
  if(pp == 0) {
    p->setPen(QPen(QColor(Color)));   // set color for xlabel text
    return;
  }

  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,Thick+4));
    p->drawPoint(x0+(*pp), y0-(*(pp+1)));

    for(int n=count-1; n>0; n--) {
      p->drawLine(x0+(*pp), y0-(*(pp+1)), x0+(*(pp+2)), y0-(*(pp+3)));
      pp += 2;
    }

    pp = Points;
    p->setPen(QPen(QPen::white, Thick, Qt::SolidLine));
    p->drawPoint(x0+(*pp), y0-(*(pp+1)));

    for(int n=count-1; n>0; n--) {
      p->drawLine(x0+(*pp), y0-(*(pp+1)), x0+(*(pp+2)), y0-(*(pp+3)));
      pp += 2;
    }
    p->setPen(QPen(QColor(Color)));   // set color for xlabel text
    return;
  }

  pp = Points;
  p->setPen(QPen(QColor(Color), Thick, Qt::SolidLine));
  p->drawPoint(x0+(*pp), y0-(*(pp+1)));

  for(int n=count-1; n>0; n--) {
    p->drawLine(x0+(*pp), y0-(*(pp+1)), x0+(*(pp+2)), y0-(*(pp+3)));
    pp += 2;
  }
}

// ---------------------------------------------------------------------
QString Graph::save()
{
  QString s = "      <"+Line+" "+Color.name()+" "+QString::number(Thick)+">";
  return s;
}

// ---------------------------------------------------------------------
bool Graph::load(const QString& _s)
{
  bool ok;
  QString s = _s;

  if(s.at(0) != '<') return false;
  if(s.at(s.length()-1) != '>') return false;
  s = s.mid(1, s.length()-2);   // cut off start and end character

  Line  = s.section(' ',0,0);    // Line

  QString n;
  n  = s.section(' ',1,1);    // Color
  Color.setNamedColor(n);
  if(!Color.isValid()) return false;

  n  = s.section(' ',2,2);    // Thick
  Thick = n.toInt(&ok);
  if(!ok) return false;

  return true;
}

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the graph. x/y are relative to diagram cx/cy
// 5 is the precision the user must point onto the graph.
int Graph::getSelected(int x, int y)
{
  int *pp = Points;
  if(pp == 0) return -1;

  int A;
  int dx, dx2, x1;
  int dy, dy2, y1;

  for(int n=1; n<count; n++) {   // count-1 runs (because need 2 points per run)
    x1 = *(pp++);  y1 = *(pp++);

    dx  = x - x1;
    dx2 = (*pp);
    if(dx < -5) { if(x < dx2-5) continue; }    // lies point between x coordinates ?
    else { if(x > dx2+5) continue; }

    dy  = y - y1;
    dy2 = (*(pp+1));
    if(dy < -5) { if(y < dy2-5) continue; }    // lies point between y coordinates ?
    else { if(y > dy2+5) continue; }

    dx2 -= x1;
    dy2 -= y1;

    A  = dx2*dy - dx*dy2;    // calculate the rectangle area spanned
    A *= A;                  // avoid the need for square root
    A -= 25*(dx2*dx2 + dy2*dy2);  // substract selectable area

    if(A <= 0)  return n;    // lies x/y onto the graph line ?
  }

  return -1;

/* ------------------------------------------------------------
   This algorithm is not bad, but not very fast.
   ------------------------------------------------------------
  double x_double, y_double, phi, sin_phi, cos_phi;
  int len, xn, yn;
  int x2, x1 = *(pp++);
  int y2, y1 = *(pp++);

  for(int n=1; n<count; n++) {   // count-1 runs (because need 2 points per run)
    x2 = (*pp) - x1;
    y2 = (*(pp+1)) - y1;

    x_double = double(x-x1);
    y_double = double(y-y1);

    phi = -atan2(double(y2), double(x2));
    sin_phi = sin(phi);
    cos_phi = cos(phi);
    len = int( sqrt(double( x2*x2 + y2*y2 )) )+5;

    xn = int(x_double*cos_phi - y_double*sin_phi);
    yn = int(x_double*sin_phi + y_double*cos_phi);

    // lies x/y onto the graph line ?
    if(xn > -5) if(xn < len) if(yn > -5) if(yn < 5)  return n;

    x1 = *(pp++);  y1 = *(pp++);
  }

  return -1;
  ------------------------------------------------------------ */
}
