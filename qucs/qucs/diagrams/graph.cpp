/***************************************************************************
                          graph.cpp  -  description
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

#include "graph.h"

#include <math.h>
#include <stdlib.h>


Graph::Graph(const QString& _Line)
{
  Type = isGraph;

  Var    = _Line;
  countY = 0;    // no points in graph
  Points = 0;
  Thick  = numMode = 0;
  Color  = 0x0000ff;   // blue
  Style  = 0;    // solid line
  Precision  = 3;
  isSelected = false;
  yAxisNo = 0;   // left y axis

  Points = 0;
  cPointsY = 0;

  Markers.setAutoDelete(true);
  cPointsX.setAutoDelete(true);
}

Graph::~Graph()
{
  if(Points != 0) free(Points);
  if(cPointsY != 0) delete[] cPointsY;
}

// ---------------------------------------------------------------------
void Graph::paint(ViewPainter *p, int x0, int y0)
{
  if(Points == 0)
    return;

  if(isSelected) {
    p->Painter->setPen(QPen(QPen::darkGray,Thick+4));
    paintLines(p, x0, y0);

    p->Painter->setPen(QPen(QPen::white, Thick, Qt::SolidLine));
    paintLines(p, x0, y0);
    return;
  }

  // **** not selected ****
  p->Painter->setPen(QPen(QColor(Color), Thick, Qt::SolidLine));
  paintLines(p, x0, y0);
}

// ---------------------------------------------------------------------
void Graph::paintLines(ViewPainter *p, int x0, int y0)
{
  int x, y;
  int *pp = Points;

  if(*pp < 0)  pp++;
  for(int n1=countY; n1>0; n1--) {    // every branch of curves
    if(*pp >= 0)  p->drawPoint(x0+(*pp), y0-(*(pp+1)));
    while(*pp > -9) {   // until end of branch
      pp += 2;
      while(*pp > -2) {    // until end of stroke
	p->drawLine(x0+(*(pp-2)), y0-(*(pp-1)), x0+(*pp), y0-(*(pp+1)));
	pp += 2;
      }
      if(*pp < -9)  break;   // end of line ?
      else {
	x = x0 + *(pp-2);
	y = y0 - *(pp-1);
	if(*pp == -7) {    // paint star
	  p->drawLine(x-5, y, x+5, y);     // horizontal line
	  p->drawLine(x-4, y+4, x+4, y-4); // upper left to lower right
	  p->drawLine(x+4, y+4, x-4, y-4); // upper right to lower left
	}
	else if(*pp == -6)    // paint circle
	  p->drawEllipse(x-4, y-4, 8, 8);
	else if(*pp == -5) {  // paint arrow
	  p->drawLine(x, y, x, y0);
	  p->drawLine(x-4, y+7, x, y);
	  p->drawLine(x+4, y+7, x, y);
	}
      }
      pp++;
    }
    pp++;
  }
}

// ---------------------------------------------------------------------
QString Graph::save()
{
  QString s = "\t<\""+Var+"\" "+Color.name()+
	      " "+QString::number(Thick)+" "+QString::number(Precision)+
	      " "+QString::number(numMode)+" "+QString::number(Style)+
	      " "+QString::number(yAxisNo)+">";

  for(Marker *pm=Markers.first(); pm != 0; pm=Markers.next())
    s += "\n\t  "+pm->save();

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

  Var = s.section('"',1,1);  // Var

  QString n;
  n  = s.section(' ',1,1);    // Color
  Color.setNamedColor(n);
  if(!Color.isValid()) return false;

  n  = s.section(' ',2,2);    // Thick
  Thick = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',3,3);    // Precision
  Precision = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',4,4);    // numMode
  numMode = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',5,5);    // Style
  Style = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',6,6);    // yAxisNo
  if(n.isEmpty()) return true;   // backward compatible
  yAxisNo = n.toInt(&ok);
  if(!ok) return false;

  return true;
}

// -----------------------------------------------------------------------
// Checks if the coordinates x/y point to the graph. x/y are relative to
// diagram cx/cy. 5 is the precision the user must point onto the graph.
int Graph::getSelected(int x, int y)
{
  int *pp = Points;
  if(pp == 0) return -1;

  int A, zi;
  int dx, dx2, x1;
  int dy, dy2, y1;

  int countX = cPointsX.getFirst()->count;
  if(*pp < 0) {
    pp++;
    if(*pp < -9) {
      pp++;
      if(*pp < -10)  return -1;   // not even one point ?
    }
  }
  for(int z=0; z<countY; z++) {  // check every branch of curves
    zi = 0;
    do {
      x1 = *(pp++);  y1 = *(pp++);
      zi++;
      dx  = x - x1;
      dy  = y - y1;

      dx2 = (*pp);
      if(dx2 < -1) {
	if(dx2 < -9) break;
	pp++;
	if(dx2 < -2) {     // no line, but single point (e.g. star) ?
	  if(*pp < -9) break;
	  if(dx < -5) continue;
	  if(dx >  5) continue;
	  if(dy < -5) continue;
	  if(dy >  5) continue;
	  return z*countX;// + zi;   // points on graph
	}
	dx2 = *pp;
	if(dx2 < -9) break;
	zi -= 2;  // because of space (edge-values)
      }
      if(dx < -5) { if(x < dx2-5) continue; } // point between x coordinates ?
      else { if(x > 5) if(x > dx2+5) continue; }

      dy2 = (*(pp+1));
      if(dy < -5) { if(y < dy2-5) continue; } // point between y coordinates ?
      else { if(y > 5) if(y > dy2+5) continue; }

      dx2 -= x1;
      dy2 -= y1;

      A  = dx2*dy - dx*dy2;    // calculate the rectangle area spanned
      A *= A;                  // avoid the need for square root
      A -= 25*(dx2*dx2 + dy2*dy2);  // substract selectable area

      if(A <= 0)  return z*countX;// + zi;  // lies x/y onto the graph line ?
    } while(true);
    pp++;
  }

  return -1;
}

// -----------------------------------------------------------------------
// Creates a new graph and copies all the properties into it.
Graph* Graph::sameNewOne()
{
  Graph *pg = new Graph(Var);

  pg->Color = Color;
  pg->Thick = Thick;
  pg->Style = Style;

  pg->Precision = Precision;
  pg->numMode   = numMode;
  pg->yAxisNo   = yAxisNo;

  for(Marker *pm = Markers.first(); pm != 0; pm = Markers.next())
    pg->Markers.append(pm->sameNewOne(pg));

  return pg;
}
