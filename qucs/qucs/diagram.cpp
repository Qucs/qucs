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

// --------------------------------------------------------------------------
bool Diagram::loadVarData(const QString& var, const QString& fileName)
{
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
      if(var == tmp) break;
    }
  }

  if(stream.atEnd()) {
    file.close();
    return false;   // return if data name was not found
  }

  tmp = Line.section(' ', 2, 2);
  tmp.remove('>');
  int n = loadIndepVarData(tmp, fileName);    // get independent variable

  Graph *g = Graphs.current();
  g->Line = var;
  int   *p = g->Points;

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
        p++;
        *(p++) = cy-int(x);
      }
      else {
        tmp0 = tmp.mid(i-1);  // imaginary part
        tmp0.remove('j');
        y = tmp0.toDouble(&ok);
        tmp = tmp.left(i-1);  // real part
        x = tmp.toDouble(&ok);
        p++;
        *(p++) = cy-int(sqrt(x*x+y*y));
      }
//QMessageBox::critical(0, "Error", QString::number(x)+"  "+QString::number(y));
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
  
  Graph *g = new Graph();   // create a new graph
  Graphs.append(g);

  int *p = new int[2*n];    // create memory for points
  g->Points = p;
  g->count  = n;

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
      *(p++) = cx+int(x);
      p++;

      i = Line.find(' ');
    }
    Line = stream.readLine();
  }

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
