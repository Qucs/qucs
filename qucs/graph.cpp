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
  Color  = QColor(0,0,255);
  
  Points = 0;
  cPoints.setAutoDelete(true);
}

Graph::~Graph()
{
  if(Points != 0) delete[] Points;
}

// ---------------------------------------------------------------------
void Graph::paint(QPainter *p)
{
//  QPoint *p1 = Points.first();
//  if(p1 == 0) return;

  int *pp = Points;
  if(pp == 0) return;
  p->setPen(QPen(Color, Thick, Qt::SolidLine));
  p->drawPoint(*pp, *(pp+1));

//  for(QPoint *p2 = Points.next(); p2 != 0; p2 = Points.next()) {
//    p->drawLine(*p1, *p2);
//    p1 = p2;
//  }
  for(int n=count-1; n>0; n--) {
    p->drawLine(*pp, *(pp+1), *(pp+2), *(pp+3));
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
