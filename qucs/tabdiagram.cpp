/***************************************************************************
                          tabdiagram.cpp  -  description
                             -------------------
    begin                : Fri Oct 24 2003
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

#include "tabdiagram.h"


TabDiagram::TabDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  x2 = 300;
  y2 = 200;

  Name = "Tab";

  calcDiagram();    // calculate circles for smith chart with |r|=1
}

TabDiagram::~TabDiagram()
{
}

// ------------------------------------------------------------
// No data has to be calculated.
void TabDiagram::calcData(Graph *)
{
  calcDiagram();
}


// ------------------------------------------------------------
// calculates the text in the tabular
void TabDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  Lines.append(new Line(cx, cy-y2, cx+x2, cy-y2, QPen(QPen::black,1)));
  Lines.append(new Line(cx, cy-y2+15, cx+x2, cy-y2+15, QPen(QPen::black,1)));
  Lines.append(new Line(cx, cy-y2, cx, cy, QPen(QPen::black,1)));
  Lines.append(new Line(cx+x2, cy-y2, cx+x2, cy, QPen(QPen::black,1)));
  Lines.append(new Line(cx, cy, cx+x2, cy, QPen(QPen::black,1)));

  int y = cy-y2+30;
  Graph *g = Graphs.first();
  if(g == 0) return;
  
  Texts.append(new Text(cx+84, cy-y2+13, g->Line));
  for(cPoint *cp = g->cPoints.first(); cp != 0; cp = g->cPoints.next()) {
    Texts.append(new Text(cx+4, y, QString::number(cp->x)));
    Texts.append(new Text(cx+84, y, QString::number(cp->yr)+"/"+QString::number(cp->yi)));
    y += 14;
  }
}

// ------------------------------------------------------------
TabDiagram* TabDiagram::newOne()
{
  return new TabDiagram();
}
