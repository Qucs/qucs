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

#include <qwidget.h>


TabDiagram::TabDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  x1 = 10;     // position of label text
  y1 = 20;
  x2 = 300;    // initial size of diagram
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
//  calcDiagram();
}


// ------------------------------------------------------------
// calculates the text in the tabular
void TabDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  Lines.append(new Line(0, y2, x2, y2, QPen(QPen::black,1)));
  Lines.append(new Line(0, y2-15, x2, y2-15, QPen(QPen::black,1)));
  Lines.append(new Line(0, y2, 0, 0, QPen(QPen::black,1)));
  Lines.append(new Line(x2, y2, x2, 0, QPen(QPen::black,1)));
  Lines.append(new Line(0, 0, x2, 0, QPen(QPen::black,1)));

  Graph *g = Graphs.first();
  if(g == 0) return;

  QWidget w;
  QPainter p(&w);
  p.setFont(QFont("Helvetica",12, QFont::Light));
  QString Str;
  QRect r;
  int colWidth=0, x=8, y = y2-30;
  
  // ................................................
  Str = g->IndepVar;
  r = p.boundingRect(0,0,0,0,Qt::AlignAuto,Str);      // get width of text
  if(r.width() > colWidth) {
    colWidth = r.width();
    if((x+colWidth) >= x2) {    // enough space for text ?
      Lines.append(new Line(x2-6, y2-4, x2+7, y2-4, QPen(QPen::red,2)));  // mark lack of space
      Lines.append(new Line(x2,   y2-7, x2+6, y2-4, QPen(QPen::red,2)));  // with a small arrow
      Lines.append(new Line(x2,   y2-1, x2+6, y2-4, QPen(QPen::red,2)));
      return;
    }
  }
  Texts.append(new Text( 4, y2-13, Str));   // write independent variable

  for(cPoint *cp = g->cPoints.first(); cp != 0; cp = g->cPoints.next()) {
    Str = QString::number(cp->x);
    r = p.boundingRect(0,0,0,0,Qt::AlignAuto,Str);      // get width of text
    if(r.width() > colWidth) {
      colWidth = r.width();
      if((x+colWidth) >= x2) {    // enough space for text ?
        Lines.append(new Line(x2-6, y-4, x2+7, y-4, QPen(QPen::red,2)));  // mark lack of space
        Lines.append(new Line(x2,   y-7, x2+6, y-4, QPen(QPen::red,2)));  // with a small arrow
        Lines.append(new Line(x2,   y-1, x2+6, y-4, QPen(QPen::red,2)));
        return;
      }
    }
    
    Texts.append(new Text( x, y, Str));
    y -= 14;
    if(y < 0) break;
  }
  x += colWidth+10;
  Lines.append(new Line(x-8, y2, x-8, 0, QPen(QPen::black,2)));

  // ................................................
  for(; g!=0; g = Graphs.next()) {    // write all dependent variables
    int y = y2-30;
    colWidth = 0;

    Str = g->Line;
    r = p.boundingRect(0,0,0,0,Qt::AlignAuto,Str);      // get width of text
    if(r.width() > colWidth) {
      colWidth = r.width();
      if((x+colWidth) >= x2) {    // enough space for text ?
        Lines.append(new Line(x2-6, y2-4, x2+7, y2-4, QPen(QPen::red,2)));  // mark lack of space
        Lines.append(new Line(x2,   y2-7, x2+6, y2-4, QPen(QPen::red,2)));  // with a small arrow
        Lines.append(new Line(x2,   y2-1, x2+6, y2-4, QPen(QPen::red,2)));
        return;
      }
    }
    Texts.append(new Text(x, y2-13, Str));

    for(cPoint *cp = g->cPoints.first(); cp != 0; cp = g->cPoints.next()) {
      Str = QString::number(cp->yr)+"/"+QString::number(cp->yi);
      r = p.boundingRect(0,0,0,0,Qt::AlignAuto,Str);      // get width of text
      if(r.width() > colWidth) {
        colWidth = r.width();
        if((x+colWidth) >= x2) {    // enough space for text ?
          Lines.append(new Line(x2-6, y-4, x2+7, y-4, QPen(QPen::red,2)));  // mark lack of space
          Lines.append(new Line(x2,   y-7, x2+6, y-4, QPen(QPen::red,2)));  // with a small arrow
          Lines.append(new Line(x2,   y-1, x2+6, y-4, QPen(QPen::red,2)));
          return;
        }
      }

      Texts.append(new Text(x, y, Str));
      y -= 14;
      if(y < 0) break;
    }
    x += colWidth+10;
    Lines.append(new Line(x-8, y2, x-8, 0, QPen(QPen::black,1)));
  }
}

// ------------------------------------------------------------
TabDiagram* TabDiagram::newOne()
{
  return new TabDiagram();
}
