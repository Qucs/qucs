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

#include <qtextstream.h>
#include <qmessagebox.h>


Diagram::Diagram(int _cx, int _cy) : cx(_cx), cy(_cy)
{
  Graphs.setAutoDelete(true);
}

Diagram::~Diagram()
{
}

// ------------------------------------------------------------
void Diagram::paint(QPainter *)
{
}

// ------------------------------------------------------------
void Diagram::paintScheme(QPainter *p)
{
  p->drawRect(cx, cy-dy, dx, dy);
}

// ------------------------------------------------------------
void Diagram::calcData(Graph *)
{
}

// --------------------------------------------------------------------------
bool Diagram::loadVarData(const QString& fileName)
{
  Graph *g = Graphs.current();
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
/*  int n =*/ loadIndepVarData(tmp, fileName);    // get independent variable

//  g->Line = var;
//  int   *p = g->Points;
  cPoint *p = g->cPoints.first();

  int i;
  bool ok;
  double x, y, ymin=1e45, ymax=-1e45;
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
//        p++;
//        *(p++) = cy-int(x);
      }
      else {
        tmp0 = tmp.mid(i-1);  // imaginary part
        tmp0.remove('j');
        y = tmp0.toDouble(&ok);
        tmp = tmp.left(i-1);  // real part
        x = tmp.toDouble(&ok);
//        p++;
//        *(p++) = cy-int(sqrt(x*x+y*y));
      }
      p->yr = x;
      p->yi = y;
      y = sqrt(x*x+y*y);
      if(y > ymax) ymax = y;
      if(y < ymin) ymin = y;
      p = g->cPoints.next();
//QMessageBox::critical(0, "Error", QString::number(x)+"  "+QString::number(y));
      i = Line.find(' ');
    }
    Line = stream.readLine();
  }

  y1 = ymin; y2 = ymax;
//QMessageBox::critical(0, "Error", QString::number(ymin)+"  "+QString::number(ymax));

  file.close();

  calcData(g);
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

  int *p = new int[2*n];    // create memory for points
  g->Points = p;
  g->count  = n;
  g->cPoints.clear();

  int i;
  double x, xmin=1e45, xmax=-1e45;
  Line = stream.readLine();
  while(Line.left(2) != "</") {
    Line = Line.stripWhiteSpace();
    Line += ' ';
    i = Line.find(' ');
    while(i > 0) {    // extract all numbers in the line
      tmp  = Line.left(i);
      Line = Line.mid(i+1);

      x = tmp.toDouble(&ok);  // get number
//      *(p++) = cx+int(x);
//      p++;
      g->cPoints.append(new cPoint(x,0,0));
      if(x > xmax) xmax = x;
      if(x < xmin) xmin = x;

      i = Line.find(' ');
    }
    Line = stream.readLine();
  }

  x1 = xmin; x2 = xmax;

  file.close();
  return n;
}

// ------------------------------------------------------------
void Diagram::setCenter(int _cx, int _cy)
{
  cx = _cx;
  cy = _cy;
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
  s += QString::number(dx)+" "+QString::number(dy)+" ";
  if(GridOn) s+= "1 ";
  else s += "0 ";
  s += QString::number(GridX)+" "+QString::number(GridY)+">\n";

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

  n  = s.section(' ',3,3);    // dx
  dx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',4,4);    // dy
  dy = n.toInt(&ok);
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

  // .......................................................
  while(!stream->atEnd()) {
    s = stream->readLine();
    s = s.stripWhiteSpace();
    if(s == ("</"+Name+">")) return true;  // found end tag
    Graph *g = new Graph();
    if(!g->load(s)) return false;
    Graphs.append(g);
    n = ((QFile*)stream->device())->name();
    n.replace(QString(".sch"),QString(".dat"));
    loadVarData(n);
//QMessageBox::critical(0, "Error", n);
  }

  return false;   // end tag missing
}
