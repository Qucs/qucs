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

#include <qstring.h>
#include <qwidget.h>
#include <qpainter.h>

#include <math.h>


Marker::Marker(int _cx, int _cy, double _xpos, double _yrpos, double _yipos, int _gNum)
{
  Type = isMarker;
  isSelected = false;

  cx = _cx;
  cy = _cy;
  x1 = cx + 100;
  y1 = cy + 100;

  xpos = _xpos;
  if(fabs(_yipos) < 1e-250) Text = QString::number(_yrpos);
  else {
    Text = QString::number(_yipos);
    if(Text.at(0) == '-') { Text.at(0) = 'j'; Text = '-'+Text; }
    else { Text = "+j"+Text; }
    Text = QString::number(_yrpos) + Text;
  }
  Text = "x: "+QString::number(_xpos)+"\ny: " + Text;

  QWidget w;
  QPainter p(&w);
  p.setFont(QFont("Helvetica",12, QFont::Light));
  QRect r = p.boundingRect(0,0,0,0,Qt::AlignAuto,Text);    // get width of text
  x2 = r.width()+4;
  y2 = r.height()+4;

  GraphNum = _gNum;

  lookNfeel = 1;
}

Marker::~Marker()
{
}

// ---------------------------------------------------------------------
void Marker::paint(QPainter *p, int x0, int y0)
{
  p->setPen(QPen(QPen::darkMagenta,0));
  p->drawRect(x0+x1, y0-y1, x2, -y2);
  p->drawLine(x0+cx, y0-cy, x0+x1, y0-y1);

  p->setPen(QPen(QPen::black,1));
  p->drawText(x0+x1+2, y0-y1-y2+2, x2, y2, Qt::AlignAuto, Text);

  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,3));
    p->drawRoundRect(x0+x1-3, y0-y1+3, x2+6, -y2-6);
  }
}

// ---------------------------------------------------------------------
QString Marker::save()
{
  QString s  = "      <Mkr "+QString::number(GraphNum)+" ";
          s += QString::number(xpos)+" "+QString::number(x1)+" "+QString::number(y1)+">";
  return s;
}

// ---------------------------------------------------------------------
bool Marker::load(const QString& _s)
{
//  bool ok;
  QString s = _s;

  if(s.at(0) != '<') return false;
/*  if(s.at(s.length()-1) != '>') return false;
  s = s.mid(1, s.length()-2);   // cut off start and end character

  Line  = s.section(' ',0,0);    // Line

  QString n;
  n  = s.section(' ',1,1);    // Color
  Color.setNamedColor(n);
  if(!Color.isValid()) return false;

  n  = s.section(' ',2,2);    // Thick
  Thick = n.toInt(&ok);
  if(!ok) return false;
*/
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
