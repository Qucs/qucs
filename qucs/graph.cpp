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

Graph::Graph()
{
  count = 0;    // no points in graph
  Thick = 1;
  Color = QColor(0,0,255);
}

Graph::~Graph()
{
}

// ---------------------------------------------------------------------
void Graph::paint(QPainter *p)
{
//  QPoint *p1 = Points.first();
//  if(p1 == 0) return;

  int *pp = Points;
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
