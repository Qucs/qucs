/***************************************************************************
                             timingdiagram.cpp
                            -------------------
    begin                : Sat Oct 22 2005
    copyright            : (C) 2005 by Michael Margraf
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

#include "timingdiagram.h"
#include "main.h"

#include <math.h>


TimingDiagram::TimingDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  x1 = 0;    // no extension to select area
  y1 = 0;
  x2 = x3 = 300;  // initial size of diagram
  y2 = 200;
  Name = "Time";
  xAxis.limit_min = 0.0;  // scroll bar position (needs to be saved in file)

  calcDiagram();
}

TimingDiagram::~TimingDiagram()
{
}

// ------------------------------------------------------------
void TimingDiagram::paint(ViewPainter *p)
{
  // paint all lines
  for(Line *pl = Lines.first(); pl != 0; pl = Lines.next()) {
    p->Painter->setPen(pl->style);
    p->drawLine(cx+pl->x1, cy-pl->y1, cx+pl->x2, cy-pl->y2);
  }

  p->Painter->setPen(Qt::black);
  // write whole text
  for(Text *pt = Texts.first(); pt != 0; pt = Texts.next())
    p->drawText(pt->s, cx+pt->x, cy-pt->y);


  if(y1 > 0) {  // paint scroll bar ?
    int   x, y, dx, dy;
    QPointArray Points;
    // draw scroll bar
    p->fillRect(cx+yAxis.numGraphs, cy+2, zAxis.numGraphs, 14, Qt::gray);

    // draw frame for scroll bar
    p->Painter->setPen(QPen(QPen::black,0));
    p->drawLine(cx+xAxis.numGraphs, cy, cx+xAxis.numGraphs, cy+17);
    p->drawLine(cx+xAxis.numGraphs+17, cy, cx+xAxis.numGraphs+17, cy+17);
    p->drawLine(cx+xAxis.numGraphs, cy+17, cx+x2, cy+17);
    p->drawLine(cx+x2, cy, cx+x2, cy+17);
    p->drawLine(cx+x2-17, cy, cx+x2-17, cy+17);

    // draw the arrows above and below the scroll bar
    p->Painter->setBrush(QBrush(Qt::gray));

    p->map(cx+xAxis.numGraphs+3,  cy+3, x, y);
    p->map(cx+xAxis.numGraphs+14, cy+14, dx, dy);
    Points.setPoints(3, x, (y+dy)>>1, dx, y, dx, dy);
    p->Painter->drawConvexPolygon(Points);

    dx -= x;
    p->map(cx+x2-3,  cy+3, x, y);
    Points.setPoints(3, x, (y+dy)>>1, x-dx, y, x-dx, dy);
    p->Painter->drawConvexPolygon(Points);

    p->Painter->setBrush(QBrush(Qt::NoBrush));
  }


  if(isSelected) {
    p->Painter->setPen(QPen(QPen::darkGray,3));
    p->drawRect(cx-5, cy-y2-5, x2+10, y2+10);
    p->Painter->setPen(QPen(QPen::darkRed,2));
    p->drawResizeRect(cx, cy-y2);  // markers for changing the size
    p->drawResizeRect(cx, cy);
    p->drawResizeRect(cx+x2, cy-y2);
    p->drawResizeRect(cx+x2, cy);
  }
}

// ------------------------------------------------------------
int TimingDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();

  y1 = 0;  // no scroll bar
  x3 = x2;
  QFontMetrics  metrics(QucsSettings.font);
  int tHeight = metrics.lineSpacing();
  QString Str;
  int colWidth=0, x=4, y = y2-tHeight-6, xStart = 0, z;

  int NumAll=0;   // how many values per row
  int NumLeft=0;  // how many values could not be written
  int invisibleCount = 0;  // how many values are invisible
  
  // outer frame
  Lines.append(new Line(0, y2, x2, y2, QPen(QPen::black,0)));
  Lines.append(new Line(0, y2, 0, 0, QPen(QPen::black,0)));
  Lines.append(new Line(x2, y2, x2, 0, QPen(QPen::black,0)));
  Lines.append(new Line(0, 0, x2, 0, QPen(QPen::black,0)));
  Lines.append(new Line(0, y+2, x2, y+2, QPen(QPen::black,0)));

  Graph *firstGraph;
  Graph *g = Graphs.first();
  if(g == 0) {  // no variables specified in diagram ?
    Str = QObject::tr("no variables");
    colWidth = checkColumnWidth(Str, metrics, colWidth, x, y2);
    if(colWidth >= 0)
      Texts.append(new Text(x, y2-2, Str)); // independent variable
    return 0;
  }


  double *px;
  while(g->cPointsX.isEmpty()) {  // any graph with data ?
    g = Graphs.next();
    if(g == 0) break;
  }
  if(g == 0) {
    Str = QObject::tr("no data");
    colWidth = checkColumnWidth(Str, metrics, colWidth, x, y2);
    if(colWidth < 0)  return 0;
    Texts.append(new Text(x, y2-2, Str));
    return 0;
  }
  firstGraph = g;

if(!g->cPointsX.isEmpty()) {
  // ................................................
  if(g->cPointsX.count() > 1) {
    Str = QObject::tr("wrong dependency");
    colWidth = checkColumnWidth(Str, metrics, colWidth, x, y2);
    if(colWidth >= 0)
      Texts.append(new Text(x, y2-2, Str)); // independent variable
    return 0;
  }


  // first, write name of independent variable
  DataX *pD = g->cPointsX.getFirst();
  NumAll = pD->count;
  Str = pD->Var;
  colWidth = checkColumnWidth(Str, metrics, colWidth, x, y2);
  if(colWidth < 0)  return 1;
  Texts.append(new Text(x, y2-2, Str));
  

  y -= 2;
  // write all dependent variable names to get width of first column
  for(g = Graphs.first(); g!=0; g = Graphs.next()) {
    if(y < tHeight)  break;
    Str = g->Var;
    colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
    if(colWidth < 0)  return 1;
    Texts.append(new Text(x, y, Str));  // dependent variable
    y -= tHeight;
  }
  x += colWidth + 13;
  xAxis.numGraphs = x -6;
  Lines.append(new Line(x-6, y2, x-6, 0, QPen(QPen::black,0)));
  xStart = x;


  invisibleCount = NumAll - (x2-xAxis.numGraphs)/40;
  if(invisibleCount <= 0)  xAxis.limit_min = 0.0;  // longer than needed
  else {
    NumLeft = invisibleCount - int(xAxis.limit_min);
    if(invisibleCount < int(xAxis.limit_min))
      xAxis.limit_min = double(invisibleCount); // adjust limit of scroll bar
  }


  // write independent variable values (usually time)
  y = y2-tHeight-4;
  px = pD->Points;
  z = int(xAxis.limit_min);
  px += z;
  z = pD->count - z;
  for( ; z>0; z--) {
    Str = num2str(*(px++));
    colWidth = metrics.width(Str);  // width of text
    if(x+colWidth+2 >= x2)  break;

    Texts.append(new Text( x, y2-2, Str));
    Lines.append(new Line(x+5, y, x+5, y-3, QPen(QPen::black,0)));
    x += 40;
  }

}  // of "if no data in graphs"


  // ................................................
  // work on all dependent variables
  QPen Pen;
  int  yLast;
  y = y2-tHeight-8;
  for(g = Graphs.first(); g!=0; g = Graphs.next()) {
    if(y < tHeight) {
      // mark lack of space with a small arrow
      Lines.append(new Line(4, 6, 4, -7, QPen(QPen::red,2)));
      Lines.append(new Line(1, 0, 4, -7, QPen(QPen::red,2)));
      Lines.append(new Line(7, 0, 4, -7, QPen(QPen::red,2)));
      break;
    }

    x = xStart + 5;
    colWidth = 0;

    if(g->cPointsY) {
      px = g->cPointsY;

      if(sameDependencies(g, firstGraph)) {

        Pen = QPen(g->Color, g->Thick);  // default is solid line
        switch(g->Style) {
          case 1: Pen.setStyle(Qt::DashLine); break;
          case 2: Pen.setStyle(Qt::DotLine);  break;
        }

        z = int(xAxis.limit_min);
        px += 2*z;

        yLast = 0;
        if(z > 0)  yLast += 2; // vertical line before first value ?
        if(*(px-yLast) > 0.5)  // high or low ?
          yLast = 2;
        else
          yLast = tHeight - 4;

        z = g->cPointsX.getFirst()->count - z;
        for( ; z>0; z--) {
          if(*px > 0.5) {  // high or low ?
            if(yLast > 2)
              Lines.append(new Line(x, y-tHeight+4, x, y-2, Pen));
            yLast = 2;
          }
          else {
            if(yLast < 3)
              Lines.append(new Line(x, y-tHeight+4, x, y-2, Pen));
            yLast = tHeight - 4;
          } 

          if(x+40 >= x2) break;
          Lines.append(new Line(x, y-yLast, x+40, y-yLast, Pen));

          x += 40;
          px += 2;
        }
      }  // of "if(sameDeps)"
      else {
        Str = QObject::tr("wrong dependency");
        colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
        if(colWidth < 0)  goto funcEnd;
        Texts.append(new Text(x, y, Str));
      }
    }
    else {   // no data in graph
      Str = QObject::tr("no data");
      colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
      if(colWidth < 0)  goto funcEnd;
      Texts.append(new Text(x, y, Str));
    }
    y -= tHeight;
  }


funcEnd:
  if(invisibleCount > 0) {  // could all values be displayed ?
    y1 = 18;   // extend the select area to the bottom
    x  = (x2 - xAxis.numGraphs - 37);
    z  = int(xAxis.limit_min);
    if(NumLeft < 0) NumLeft = 0;
    y  = NumAll - NumLeft - z;

    // length of scroll bar
    zAxis.numGraphs = x * y / NumAll;
    if(zAxis.numGraphs < 3)  zAxis.numGraphs = 3;

    // position of scroll bar in pixel
    yAxis.numGraphs = x * z / NumAll + xAxis.numGraphs + 19;

    xAxis.limit_max = double(y);
  }

  return 1;
}

// ------------------------------------------------------------
bool TimingDiagram::scroll(int clickPos)
{
  if(y1 <= 0) return false;   // no scroll bar ?
  double tmp = xAxis.limit_min;

  int x = cx;
  if(clickPos > (cx+x2-20)) {  // scroll one value to the right ?
    xAxis.limit_min++;
  }
  else {
    x += xAxis.numGraphs + 20;
    if(clickPos < x) {  // scroll bar one value to the left ?
      if(xAxis.limit_min <= 0)  return false;
      xAxis.limit_min--;
    }
    else {
      x = cx + yAxis.numGraphs;
      if(clickPos < x) {  // scroll bar one page to the left ?
        xAxis.limit_min -= xAxis.limit_max;
        if(xAxis.limit_min < 0.0)  xAxis.limit_min = 0.0;
      }
      else {
        x += zAxis.numGraphs;
        if(clickPos > x)   // one page to the right ?
          xAxis.limit_min += xAxis.limit_max;
      }
    }
  }

  calcDiagram();
  if(tmp == xAxis.limit_min)  return false;   // did anything change ?
  return true;
}

// ------------------------------------------------------------
Diagram* TimingDiagram::newOne()
{
  return new TimingDiagram();
}

// ------------------------------------------------------------
Element* TimingDiagram::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Timing Diagram");
  BitmapFile = "timing";

  if(getNewOne)  return new TimingDiagram();
  return 0;
}
