/***************************************************************************
                          diagram.cpp  -  description
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

#include "diagram.h"

#include <math.h>
#include <float.h>

#include <qtextstream.h>
#include <qmessagebox.h>


Diagram::Diagram(int _cx, int _cy)
{
  cx = _cx;  cy = _cy;
  x1 = 0;  y1 = 0;

  ymin = xmin = DBL_MAX;
  ymax = ymax = -DBL_MAX;

  Type = isDiagram;
  isSelected = false;
  
  Graphs.setAutoDelete(true);
  Arcs.setAutoDelete(true);
  Lines.setAutoDelete(true);
  Texts.setAutoDelete(true);
}

Diagram::~Diagram()
{
}

// ------------------------------------------------------------
void Diagram::paint(QPainter *p)
{
  for(Line *pl = Lines.first(); pl != 0; pl = Lines.next()) {    // paint all lines
    p->setPen(pl->style);
    p->drawLine(cx+pl->x1, cy-pl->y1, cx+pl->x2, cy-pl->y2);
  }

  for(Arc *pa = Arcs.first(); pa != 0; pa = Arcs.next()) {    // paint all arcs
    p->setPen(pa->style);
    p->drawArc(cx+pa->x, cy-pa->y, pa->w, pa->h, pa->angle, pa->arclen);
  }

  if(Name[0] != 'T') {   // no graph within tabulars
    Graph *pg;
    QRect r;
    int delta=20;

    if(xLabel.isEmpty())
      for(pg = Graphs.first(); pg != 0; pg = Graphs.next()) {   // draw all graphs
        pg->paint(p, cx, cy);
        r = p->boundingRect(0,0,0,0,Qt::AlignAuto,pg->IndepVar);    // get width of text
        p->drawText(cx+((x2-r.width())>>1), cy+delta, pg->IndepVar);
        delta += r.height();
      }
    else {
      for(pg = Graphs.first(); pg != 0; pg = Graphs.next())   // draw all graphs
        pg->paint(p, cx, cy);
      r = p->boundingRect(0,0,0,0,Qt::AlignAuto,xLabel);    // get width of text
      p->setPen(QColor(0,0,0));
      p->drawText(cx+((x2-r.width())>>1), cy+delta, xLabel);
    }

    delta = 10;
    p->save();
    p->rotate(270);
    if(yLabel.isEmpty()) {
      for(pg = Graphs.first(); pg != 0; pg = Graphs.next()) {   // draw y-label for all graphs
        p->setPen(pg->Color);
        r = p->boundingRect(0,0,0,0,Qt::AlignAuto,pg->Line);    // get width of text
        p->drawText(-cy+((y2-r.width())>>1), cx-delta, pg->Line);
        delta += r.height();
      }
    }
    else {
        r = p->boundingRect(0,0,0,0,Qt::AlignAuto,yLabel);    // get width of text
        p->setPen(QColor(0,0,0));
        p->drawText(-cy+((y2-r.width())>>1), cx-delta, yLabel);
    }
    p->restore();
  }
  
  p->setPen(QPen(QPen::black,1));
  for(Text *pt = Texts.first(); pt != 0; pt = Texts.next())      // write whole text
    p->drawText(cx+pt->x, cy-pt->y, pt->s);

  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,3));
    p->drawRoundRect(cx-5, cy-y2-5, x2+10, y2+10);
  }
}

// ------------------------------------------------------------
void Diagram::paintScheme(QPainter *p)
{
  p->drawRect(cx, cy-y2, x2, y2);
}

// ------------------------------------------------------------
void Diagram::calcDiagram()
{
}

// ------------------------------------------------------------
void Diagram::calcData(Graph *)
{
}

// -------------------------------------------------------
void Diagram::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  _x1 = cx;
  _y1 = cy-y2;
  _x2 = cx+x2;
  _y2 = cy;
}

// --------------------------------------------------------------------------
void Diagram::loadGraphData(const QString& defaultDataSet)
{
  ymin = xmin = DBL_MAX;
  ymax = xmax = -DBL_MAX;

  for(Graph *pg = Graphs.first(); pg != 0; pg = Graphs.next())
    loadVarData(defaultDataSet);    // load data and determine max and min values
//QMessageBox::critical(0, "Error", QString::number(xmin)+"  "+QString::number(xmax)+" * "+QString::number(ymin)+"  "+QString::number(ymax));
//QMessageBox::critical(0, "Error", Name);

  calcDiagram();

  for(Graph *pg = Graphs.first(); pg != 0; pg = Graphs.next())
    calcData(pg);   // calculate graph coordinates
}

// --------------------------------------------------------------------------
bool Diagram::loadVarData(const QString& fileName)
{
  Graph *g = Graphs.current();
  if(g->Points != 0) delete[] g->Points;
  if(g->Line.isEmpty()) return false;

  QFile file(fileName);
  if(!file.open(IO_ReadOnly)) {
    QMessageBox::critical(0, "Error", "Cannot load dataset: "+fileName);
    return false;
  }

  QString Line, tmp, tmp0;
  QTextStream stream(&file);

  while(!stream.atEnd()) {    // look for variable name in data file
    Line = stream.readLine();
    if(Line.left(4) == "<dep") {
      tmp = Line.section(' ', 1, 1);
      if(g->Line == tmp) break;
    }
  }

  if(stream.atEnd()) {
    file.close();
    return false;   // return if data name was not found
  }

  tmp = Line.section(' ', 2, 2);
  tmp.remove('>');
  g->IndepVar = tmp;
/*  int n =*/ loadIndepVarData(tmp, fileName);    // get independent variable

//  g->Line = var;
//  int   *p = g->Points;
  cPoint *p = g->cPoints.first();

  int i;
  bool ok;
  double x, y;
  Line = stream.readLine();
  while(Line.left(2) != "</") {
    Line = Line.stripWhiteSpace();
    Line += ' ';
    i = Line.find(' ');
    while(i > 0) {    // extract all numbers in the line
      tmp  = Line.left(i);
      Line = Line.mid(i+1);

      i = tmp.find('j');
      if(i < 0) {
        x = tmp.toDouble(&ok);
        y = 0;
      }
      else {
        tmp0 = tmp.mid(i-1);  // imaginary part
        tmp0.remove('j');
        y = tmp0.toDouble(&ok);
        tmp = tmp.left(i-1);  // real part
        x = tmp.toDouble(&ok);
      }
      p->yr = x;
      p->yi = y;
      y = sqrt(x*x+y*y);
      if(y > ymax) ymax = y;
      if(y < ymin) ymin = y;
      p = g->cPoints.next();
      i = Line.find(' ');
    }
    Line = stream.readLine();
  }

  file.close();
  return true;
}

// --------------------------------------------------------------------------
// Reads the data of an independent variable. Returns the numberof points.
int Diagram::loadIndepVarData(const QString& var, const QString& fileName)
{
  QFile file(fileName);
  if(!file.open(IO_ReadOnly)) {
    QMessageBox::critical(0, "Error", "Cannot load dataset: "+fileName);
    return 0;
  }

  QString Line, tmp;
  QTextStream stream(&file);

  while(!stream.atEnd()) {    // look for variable name in data file
    Line = stream.readLine();
    if(Line.left(6) == "<indep") {
      tmp = Line.section(' ', 1, 1);
      if(var == tmp) break;
    }
  }

  if(stream.atEnd()) {
    file.close();
    return 0;   // return if data name was not found
  }

  bool ok;
  tmp = Line.section(' ', 2, 2);  // get number of points
  tmp.remove('>');
  int n = tmp.toInt(&ok);
  
  Graph *g = Graphs.current();

//  if(g->Points != 0) delete[] g->Points;
  int *p = new int[2*n];    // create memory for points
  g->Points = p;
  g->count  = n;
  g->cPoints.clear();

  int i;
  double x;
  Line = stream.readLine();
  while(Line.left(2) != "</") {
    Line = Line.stripWhiteSpace();
    Line += ' ';
    i = Line.find(' ');
    while(i > 0) {    // extract all numbers in the line
      tmp  = Line.left(i);
      Line = Line.mid(i+1);

      x = tmp.toDouble(&ok);  // get number
      g->cPoints.append(new cPoint(x,0,0));
      if(x > xmax) xmax = x;
      if(x < xmin) xmin = x;

      i = Line.find(' ');
    }
    Line = stream.readLine();
  }

  file.close();
  return n;
}

// ------------------------------------------------------------
void Diagram::setCenter(int x, int y, bool relative)
{
  if(relative) {
    cx += x;  cy += y;
  }
  else {
    cx = x;  cy = y;
  }
}

// ------------------------------------------------------------
Diagram* Diagram::newOne()
{
  return new Diagram();
}

// ------------------------------------------------------------
QString Diagram::save()
{
  QString s = "   <"+Name+" "+QString::number(cx)+" "+QString::number(cy)+" ";
  s += QString::number(x2)+" "+QString::number(y2)+" ";
  if(GridOn) s+= "1 ";
  else s += "0 ";
  s += QString::number(GridX)+" "+QString::number(GridY) + " \""+xLabel+"\" \""+yLabel+"\">\n";

  for(Graph *p=Graphs.first(); p!=0; p=Graphs.next())
    s += p->save()+"\n";

  s += "   </"+Name+">";
  return s;
}

// ------------------------------------------------------------
bool Diagram::load(const QString& Line, QTextStream *stream)
{
  bool ok;
  QString s = Line;

  if(s.at(0) != '<') return false;
  if(s.at(s.length()-1) != '>') return false;
  s = s.mid(1, s.length()-2);   // cut off start and end character

  QString n;
  n  = s.section(' ',1,1);    // cx
  cx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',2,2);    // cy
  cy = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',3,3);    // x2
  x2 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',4,4);    // y2
  y2 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',5,5);    // GridOn
  if(n.toInt(&ok) == 1) GridOn = true;
  else GridOn = false;
  if(!ok) return false;

  n  = s.section(' ',6,6);    // GridX
  GridX = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',7,7);    // GridY
  GridY = n.toInt(&ok);
  if(!ok) return false;

  xLabel = s.section('"',1,1);    // xLabel
  yLabel = s.section('"',3,3);    // yLabel

  // .......................................................
  // load graphs of the diagram
  while(!stream->atEnd()) {
    s = stream->readLine();
    s = s.stripWhiteSpace();
    if(s == ("</"+Name+">")) return true;  // found end tag ?
    Graph *g = new Graph();
    if(!g->load(s)) return false;
    Graphs.append(g);
  }

  return false;   // end tag missing
}
