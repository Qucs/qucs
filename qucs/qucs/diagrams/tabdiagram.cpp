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

#include <math.h>


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
void TabDiagram::calcData(Graph *g)
{
  if(g->Points != 0) {
    delete[] g->Points;    // memory is of no use in this diagram type
    g->Points = 0;
  }
//  calcDiagram();
}


// ------------------------------------------------------------
// calculates the text in the tabular
void TabDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  // outer frame
  Lines.append(new Line(0, y2, x2, y2, QPen(QPen::black,0)));
  Lines.append(new Line(0, y2, 0, 0, QPen(QPen::black,0)));
  Lines.append(new Line(x2, y2, x2, 0, QPen(QPen::black,0)));
  Lines.append(new Line(0, 0, x2, 0, QPen(QPen::black,0)));
  Lines.append(new Line(0, y2-16, x2, y2-16, QPen(QPen::black,2)));

  Graph *g = Graphs.first();
  if(g == 0) return;

  QWidget w;
  QPainter p(&w);
  p.setFont(QFont("Helvetica",12, QFont::Light));
  QString Str;
  QRect r;
  int colWidth=0, x=8, y = y2-30;

  // ................................................
  double *py, *px;
  int counting = g->countX1 * g->countX2, lastCount = 1;
  for(DataX *pD = g->cPointsX.last(); pD!=0; pD = g->cPointsX.prev()) {
    Str = pD->Var;
    r = p.boundingRect(0,0,0,0,Qt::AlignAuto,Str);      // get width of text
    if(r.width() > colWidth) {
      colWidth = r.width();
      if((x+colWidth) >= x2) {    // enough space for text ?
        // mark lack of space with a small arrow
        Lines.append(new Line(x2-6, y2-4, x2+7, y2-4, QPen(QPen::red,2)));
        Lines.append(new Line(x2,   y2-7, x2+6, y2-4, QPen(QPen::red,2)));
        Lines.append(new Line(x2,   y2-1, x2+6, y2-4, QPen(QPen::red,2)));
        return;
      }
    }
    Texts.append(new Text(x-4, y2-13, Str));  // write independent variable

    if(pD->count != 0) {
    y = y2-30;
    counting /= pD->count;   // how many rows to be skipped
    for(int z1=0; z1<lastCount; z1++) {
      px = pD->Points;
      for(int z=pD->count; z>0; z--) {
        if(y < 0) break;
        Str = QString::number(*(px++));
        r = p.boundingRect(0,0,0,0,Qt::AlignAuto,Str); // get width of text
        if(r.width() > colWidth) {
          colWidth = r.width();
          if((x+colWidth) >= x2) {    // enough space for text ?
            // mark lack of space with a small arrow
            Lines.append(new Line(x2-6, y-4, x2+7, y-4, QPen(QPen::red,2)));
            Lines.append(new Line(x2,   y-7, x2+6, y-4, QPen(QPen::red,2)));
            Lines.append(new Line(x2,   y-1, x2+6, y-4, QPen(QPen::red,2)));
            return;
          }
        }

        Texts.append(new Text( x, y, Str));
        y -= 14*counting;
      }
      if(pD == g->cPointsX.getFirst())
        Lines.append(new Line(0, y+12, x2, y+12, QPen(QPen::black,0)));
    }
    lastCount = pD->count; }
    x += colWidth+10;
    Lines.append(new Line(x-8, y2, x-8, 0, QPen(QPen::black,0)));
  }
  Lines.current()->style = QPen(QPen::black,2);

  // ................................................
  for(; g!=0; g = Graphs.next()) {    // write all dependent variables
    y = y2-30;
    colWidth = 0;

    Str = g->Var;
    r = p.boundingRect(0,0,0,0,Qt::AlignAuto,Str);      // get width of text
    if(r.width() > colWidth) {
      colWidth = r.width();
      if((x+colWidth) >= x2) {    // enough space for text ?
        // mark lack of space with a small arrow
        Lines.append(new Line(x2-6, y2-4, x2+7, y2-4, QPen(QPen::red,2)));
        Lines.append(new Line(x2,   y2-7, x2+6, y2-4, QPen(QPen::red,2)));
        Lines.append(new Line(x2,   y2-1, x2+6, y2-4, QPen(QPen::red,2)));
        return;
      }
    }
    Texts.append(new Text(x, y2-13, Str));


    py = g->cPointsY;
    for(int z=g->countX1 * g->countX2; z>0; z--) {
      if(y < 0) break;
      if(fabs(*(py+1)) > 1e-250) {
        Str = QString::number(*(py+1));
        if(Str.at(0) == '-') { Str.at(0) = 'j'; Str = '-'+Str; }
        else Str = "+j"+Str;
        Str = QString::number(*py)+Str;
      }
      else Str = QString::number(*py);
      py += 2;

      r = p.boundingRect(0,0,0,0,Qt::AlignAuto,Str);      // get width of text
      if(r.width() > colWidth) {
        colWidth = r.width();
        if((x+colWidth) >= x2) {    // enough space for text ?
          // mark lack of space with a small arrow
          Lines.append(new Line(x2-6, y-4, x2+7, y-4, QPen(QPen::red,2)));
          Lines.append(new Line(x2,   y-7, x2+6, y-4, QPen(QPen::red,2)));
          Lines.append(new Line(x2,   y-1, x2+6, y-4, QPen(QPen::red,2)));
          return;
        }
      }

      Texts.append(new Text(x, y, Str));
      y -= 14;
    }
    x += colWidth+10;
    Lines.append(new Line(x-8, y2, x-8, 0, QPen(QPen::black,0)));
  }
}

// ------------------------------------------------------------
TabDiagram* TabDiagram::newOne()
{
  return new TabDiagram();
}
