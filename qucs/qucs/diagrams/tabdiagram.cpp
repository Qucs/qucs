/***************************************************************************
                          tabdiagram.cpp  -  description
                             -------------------
    begin                : Fri Oct 24 2003
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

#include "tabdiagram.h"
#include "main.h"

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
// calculates the text in the tabular
int TabDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  QSize r;
  QFontMetrics  metrics(QucsSettings.font);
  int tHeight = metrics.lineSpacing();
  QString Str;
  int colWidth=0, x=8, y = y2-tHeight-6;

  // outer frame
  Lines.append(new Line(0, y2, x2, y2, QPen(QPen::black,0)));
  Lines.append(new Line(0, y2, 0, 0, QPen(QPen::black,0)));
  Lines.append(new Line(x2, y2, x2, 0, QPen(QPen::black,0)));
  Lines.append(new Line(0, 0, x2, 0, QPen(QPen::black,0)));
  Lines.append(new Line(0, y+2, x2, y+2, QPen(QPen::black,2)));

  Graph *g = Graphs.first();
  if(g == 0) return 0;
  if(g->cPointsX.isEmpty()) return 0;

  // ................................................
  double *py, *px;
  int counting, lastCount = 1;
  counting = g->cPointsX.getFirst()->count * g->countY;
  for(DataX *pD = g->cPointsX.last(); pD!=0; pD = g->cPointsX.prev()) {
    Str = pD->Var;
    r = metrics.size(0, Str); // width of text
    if(r.width() > colWidth) {
      colWidth = r.width();
      if((x+colWidth) >= x2) {    // enough space for text ?
        // mark lack of space with a small arrow
        Lines.append(new Line(x2-6, y2-4, x2+7, y2-4, QPen(QPen::red,2)));
        Lines.append(new Line(x2,   y2-7, x2+6, y2-4, QPen(QPen::red,2)));
        Lines.append(new Line(x2,   y2-1, x2+6, y2-4, QPen(QPen::red,2)));
        return 0;
      }
    }
    Texts.append(new Text(x-4, y2-2, Str)); // independent variable
    if(pD->count != 0) {
    y = y2-tHeight-5;
    counting /= pD->count;   // how many rows to be skipped
    for(int z1=0; z1<lastCount; z1++) {
      px = pD->Points;
      for(int z=pD->count; z>0; z--) {
	if(y < tHeight) break;
	Str = StringNum(*(px++));
	r = metrics.size(0, Str);  // width of text
	if(r.width() > colWidth) {
          colWidth = r.width();
          if((x+colWidth) >= x2) {    // enough space for text ?
            // mark lack of space with a small arrow
            Lines.append(new Line(x2-6, y-4, x2+7, y-4, QPen(QPen::red,2)));
            Lines.append(new Line(x2,   y-7, x2+6, y-4, QPen(QPen::red,2)));
            Lines.append(new Line(x2,   y-1, x2+6, y-4, QPen(QPen::red,2)));
            return 0;
          }
        }

        Texts.append(new Text( x, y, Str));
        y -= tHeight*counting;
      }
      if(pD == g->cPointsX.getFirst())
        Lines.append(new Line(0, y, x2, y, QPen(QPen::black,0)));
    }
    lastCount *= pD->count; }
    x += colWidth+15;
    Lines.append(new Line(x-8, y2, x-8, 0, QPen(QPen::black,0)));
  }
  Lines.current()->style = QPen(QPen::black,2);

  // ................................................
  for(; g!=0; g = Graphs.next()) {    // write all dependent variables
    y = y2-tHeight-5;
    colWidth = 0;

    Str = g->Var;
    r = metrics.size(0, Str); // width of text
    if(r.width() > colWidth) {
      colWidth = r.width();
      if((x+colWidth) >= x2) {    // enough space for text ?
        // mark lack of space with a small arrow
        Lines.append(new Line(x2-6, y2-4, x2+7, y2-4, QPen(QPen::red,2)));
        Lines.append(new Line(x2,   y2-7, x2+6, y2-4, QPen(QPen::red,2)));
        Lines.append(new Line(x2,   y2-1, x2+6, y2-4, QPen(QPen::red,2)));
        return 0;
      }
    }
    Texts.append(new Text(x, y2-2, Str));  // dependent variable


    py = g->cPointsY;
    if (!g->cPointsX.getFirst()) return 0;
    for(int z=g->cPointsX.getFirst()->count * g->countY; z>0; z--) {
      if(y < tHeight) break;
      switch(g->numMode) {
        case 0: Str = complexRect(*py, *(py+1), g->Precision);
	        break;
        case 1: Str = complexDeg(*py, *(py+1), g->Precision);
	        break;
        case 2: Str = complexRad(*py, *(py+1), g->Precision);
	        break;
      }
      py += 2;

      r = metrics.size(0, Str);  // width of text
      if(r.width() > colWidth) {
        colWidth = r.width();
        if((x+colWidth) >= x2) {    // enough space for text ?
          // mark lack of space with a small arrow
          Lines.append(new Line(x2-6, y-4, x2+7, y-4, QPen(QPen::red,2)));
          Lines.append(new Line(x2,   y-7, x2+6, y-4, QPen(QPen::red,2)));
          Lines.append(new Line(x2,   y-1, x2+6, y-4, QPen(QPen::red,2)));
          return 0;
        }
      }

      Texts.append(new Text(x, y, Str));
      y -= tHeight;
    }
    x += colWidth+15;
    Lines.append(new Line(x-8, y2, x-8, 0, QPen(QPen::black,0)));
  }

  return 1;
}

// ------------------------------------------------------------
TabDiagram* TabDiagram::newOne()
{
  return new TabDiagram();
}
