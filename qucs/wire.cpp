/***************************************************************************
                          wire.cpp  -  description
                             -------------------
    begin                : Wed Sep 3 2003
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

#include "wire.h"


Wire::Wire(int _x1, int _y1, int _x2, int _y2, Node *n1, Node *n2, const QString& _Name)
{
  cx = 0;
  cy = 0;
  x1 = _x1;
  y1 = _y1;
  x2 = _x2;
  y2 = _y2;
  Port1 = n1;
  Port2 = n2;
  Name  = _Name;

  nx=0; ny=0; delta=0;

  Type = isWire;
  isSelected = false;
}

Wire::~Wire()
{
}

// ----------------------------------------------------------------
void Wire::paintScheme(QPainter *p)
{
  p->drawLine(x1, y1, x2, y2);
}

// ----------------------------------------------------------------
void Wire::rotate()
{
  int xm, ym, tmp;

  xm = (x1+x2) >> 1;
  ym = (y1+y2) >> 1;

  tmp = x1;
  x1  = xm + y1  - ym;
  y1  = ym - tmp + xm;

  tmp = x2;
  x2  = xm + y2  - ym;
  y2  = ym - tmp + xm;
}

// ----------------------------------------------------------------
void Wire::setCenter(int x, int y, bool relative)
{
  if(relative) {
    x1 += x;  x2 += x;
    y1 += y;  y2 += y;
  }
  else {
    x1 = x;  x2 = x;
    y1 = y;  y2 = y;
  }
}

// ----------------------------------------------------------------
void Wire::paint(QPainter *p)
{
  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,6));
    p->drawLine(x1, y1, x2, y2);
    p->setPen(QPen(QPen::lightGray,2));
    p->drawLine(x1, y1, x2, y2);
  }
  else {
    p->setPen(QPen(QPen::darkBlue,2));
    p->drawLine(x1, y1, x2, y2);
  }

  if(!Name.isEmpty()) {   // draw node name label
    p->setPen(QPen(QPen::darkMagenta,1));
    if(x1 == x2) {
      p->drawLine(x1+4, y1+delta+4, nx, ny);
      p->drawArc(x1-4, y1+delta, 8, 8, 16*140, 16*255);
    }
    else {
      p->drawLine(x1+delta+4, y1-4, nx, ny);
      p->drawArc(x1+delta, y1-4, 8, 8, 16*230, 16*255);
    }
    p->drawLine(nx, ny, nx, ny-10);
    p->drawLine(nx, ny, nx+15, ny);
    p->setPen(QPen(QPen::black,1));
    p->drawText(nx+3, ny-3, Name);
  }
}

// ----------------------------------------------------------------
bool Wire::isHorizontal()
{
  return (y1 == y2);
}

// ----------------------------------------------------------------
// Converts all necessary data of the wire into a string. This can be used to
// save it to an ASCII file or to transport it via the clipboard.
QString Wire::save()
{
  QString s  = "   <"+QString::number(x1)+" "+QString::number(y1);
          s += " "+QString::number(x2)+" "+QString::number(y2);
          s += " \""+Name +"\" ";
          s += QString::number(nx)+" "+QString::number(ny)+" ";
          s += QString::number(delta)+">";
  return s;
}

// ----------------------------------------------------------------
// This is the counterpart to Wire::save.
bool Wire::load(const QString& _s)
{
  bool ok;
  QString s = _s;

  if(s.at(0) != '<') return false;
  if(s.at(s.length()-1) != '>') return false;
  s = s.mid(1, s.length()-2);   // cut off start and end character
  
  QString n;
  n  = s.section(' ',0,0);    // x1
  x1 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',1,1);    // y1
  y1 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',2,2);    // x2
  x2 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',3,3);    // y2
  y2 = n.toInt(&ok);
  if(!ok) return false;

  Name = s.section('"',1,1);  // Name

  n  = s.section(' ',5,5);    // nx
  nx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',6,6);    // ny
  ny = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',7,7);    // delta
  delta = n.toInt(&ok);
  if(!ok) return false;

  return true;
}
