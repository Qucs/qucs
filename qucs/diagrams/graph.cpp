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

Graph::Graph(const QString& _Line)
{
  Line = _Line;

  count  = 0;    // no points in graph
  Points = 0;
  Thick  = 1;
  Color  = 0x0000ff;  // blue
  
  Points = 0;
  cPoints.setAutoDelete(true);
}

Graph::~Graph()
{
  if(Points != 0) delete[] Points;
}

// ---------------------------------------------------------------------
void Graph::paint(QPainter *p, int cx, int cy)
{
  int *pp = Points;
  if(pp == 0) return;
  p->setPen(QPen(QColor(Color), Thick, Qt::SolidLine));
  p->drawPoint(cx+(*pp), cy-(*(pp+1)));

  for(int n=count-1; n>0; n--) {
    p->drawLine(cx+(*pp), cy-(*(pp+1)), cx+(*(pp+2)), cy-(*(pp+3)));
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
