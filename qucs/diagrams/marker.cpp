/***************************************************************************
                          marker.cpp  -  description
                             -------------------
    begin                : Sat Apr 10 2004
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

#include "marker.h"
#include "diagram.h"
#include "graph.h"

#include <qstring.h>
#include <qwidget.h>
#include <qpainter.h>

#include <math.h>


Marker::Marker(Diagram *Diag_, Graph *pg_, int _cx, int _cy, double _xpos,
               double _yrpos, double _yipos)
{
  Type = isMarker;
  isSelected = false;

  cx = _cx;
  cy = _cy;
  x1 = cx + 100;
  y1 = cy + 100;

  Diag = Diag_;
  pGraph = pg_;
  Precision = 2;
  xpos = _xpos;
  yr = _yrpos;  yi = _yipos;
  createText();
  lookNfeel = 1;
}

Marker::~Marker()
{
}

// ---------------------------------------------------------------------
void Marker::createText()
{
  if(fabs(yi) < 1e-250) Text = QString::number(yr);
  else {
    Text = QString::number(yi,'g',Precision);
    if(Text.at(0) == '-') { Text.at(0) = 'j'; Text = '-'+Text; }
    else { Text = "+j"+Text; }
    Text = QString::number(yr,'g',Precision) + Text;
  }
  Text = "x: "+QString::number(xpos,'g',Precision)+"\ny: " + Text;

  QWidget w;
  QPainter p(&w);
  p.setFont(QFont("Helvetica",12, QFont::Light));
  QRect r = p.boundingRect(0,0,0,0,Qt::AlignAuto,Text);  // width of text
  x2 = r.width()+4;
  y2 = r.height()+4;
}

// ---------------------------------------------------------------------
void Marker::makeInvalid()
{
//  xpos = 0.0;
  yr = 0.0;
  yi = 0.0;
  cx = 0;
  cy = 0;
  Text = QObject::tr("invalid");

  QWidget w;
  QPainter p(&w);
  p.setFont(QFont("Helvetica",12, QFont::Light));
  QRect r = p.boundingRect(0,0,0,0,Qt::AlignAuto,Text);  // width of text
  x2 = r.width()+5;
  y2 = r.height()+5;
}

// ---------------------------------------------------------------------
void Marker::paint(QPainter *p, int x0, int y0)
{
  p->setPen(QPen(QPen::darkMagenta,0));
  p->drawRect(x0+x1, y0-y1, x2, -y2);

  // which corner of rectangle should be connected to line ?
  if(cx < x1+(x2>>1)) {
    if(cy < y1+(y2>>1))
      p->drawLine(x0+cx, y0-cy, x0+x1, y0-y1);
    else
      p->drawLine(x0+cx, y0-cy, x0+x1, y0-y1-y2);
  }
  else {
    if(cy < y1+(y2>>1))
      p->drawLine(x0+cx, y0-cy, x0+x1+x2, y0-y1);
    else
      p->drawLine(x0+cx, y0-cy, x0+x1+x2, y0-y1-y2);
  }

  p->setPen(QPen(QPen::black,1));
  p->drawText(x0+x1+3, y0-y1-y2+3, x2, y2, Qt::AlignAuto, Text);

  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,3));
    p->drawRoundRect(x0+x1-2, y0-y1+2, x2+4, -y2-4);
  }
}

// ---------------------------------------------------------------------
void Marker::paintScheme(QPainter *p)
{
  int x0 = Diag->cx;
  int y0 = Diag->cy;
  p->drawRect(x0+x1, y0-y1, x2, -y2);

  // which corner of rectangle should be connected to line ?
  if(cx < x1+(x2>>1)) {
    if(cy < y1+(y2>>1))
      p->drawLine(x0+cx, y0-cy, x0+x1, y0-y1);
    else
      p->drawLine(x0+cx, y0-cy, x0+x1, y0-y1-y2);
  }
  else {
    if(cy < y1+(y2>>1))
      p->drawLine(x0+cx, y0-cy, x0+x1+x2, y0-y1);
    else
      p->drawLine(x0+cx, y0-cy, x0+x1+x2, y0-y1-y2);
  }
}

// ------------------------------------------------------------
void Marker::setCenter(int x, int y, bool relative)
{
  if(relative) {
    x1 += x;  y1 -= y;
  }
  else {
    x1 = x;  y1 = y;
  }
}

// -------------------------------------------------------
// Provides the coordinates relative to the diagram !!!
void Marker::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  _x1 = x1;
  _y1 = y1;
  _x2 = x1+x2;
  _y2 = y1-y2;
}

// ---------------------------------------------------------------------
QString Marker::save()
{
  int GraphNum = Diag->Graphs.findRef(pGraph);
  QString s  = "      <Mkr "+QString::number(GraphNum)+" ";
          s += QString::number(xpos)+" "+QString::number(x1)
	       +" "+QString::number(y1)+" "
	       +QString::number(Precision)+">";
  return s;
}

// ---------------------------------------------------------------------
bool Marker::load(const QString& _s)
{
  bool ok;
  QString s = _s;

  if(s.at(0) != '<') return false;
  if(s.at(s.length()-1) != '>') return false;
  s = s.mid(1, s.length()-2);   // cut off start and end character

  if(s.section(' ',0,0) != "Mkr") return false;

  QString n;
  n  = s.section(' ',1,1);    // pGraph
  int Num = n.toInt(&ok);
  if(!ok) return false;
  pGraph = Diag->Graphs.at(Num);

  n  = s.section(' ',2,2);    // xpos
  xpos = n.toDouble(&ok);
  if(!ok) return false;

  n  = s.section(' ',3,3);    // x1
  x1 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',4,4);    // y1
  y1 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',5,5);      // Precision
  if(n.isEmpty()) return true;  //  is optional
  Precision = n.toInt(&ok);

  return true;
}

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the marker text. x/y are relative to diagram cx/cy.
int Marker::getSelected(int x_, int y_)
{
  if(x_ >= x1) if(x_ <= x1+x2) if(y_ >= y1) if(y_ <= y1+y2)
    return 1;

  return -1;
}
