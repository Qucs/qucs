/***************************************************************************
                        portsymbol.cpp  -  description
                             -------------------
    begin                : Sun Sep 5 2004
    copyright            : (C) 2004 by Michael Margraf
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

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "main.h"
#include "portsymbol.h"


PortSymbol::PortSymbol(int cx_, int cy_, int number_)
{
  Name = "PortSym ";
  isSelected = false;
  cx = cx_;
  cy = cy_;
  x2 = 16;
  y1 = 8;

  numberStr = QString::number(number_);
  QFontMetrics  metrics(QucsSettings.font);
  QSize r = metrics.size(0, numberStr);
  x1 = (r.width() >> 1) + 5;
  if(x1 < 8)  x1 = 8;
  y2 = r.height() + 20;
}

PortSymbol::~PortSymbol()
{
}

// --------------------------------------------------------------------------
void PortSymbol::paint(QPainter *p)
{
  p->setPen(QPen(QPen::red,1));  // like open node
  p->drawEllipse(cx-4, cy-4, 8, 8);

  p->setPen(QPen(QPen::lightGray,1));
  p->drawRect(cx-x1, cy-y1, x2, y2);

  p->drawText(cx-x1+5, cy-y1+y2-5, numberStr);

  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,3));
    p->drawRoundRect(cx-x1-4, cy-y1-4, x2+8, y2+8);
  }
}

// --------------------------------------------------------------------------
void PortSymbol::paintScheme(QPainter *p)
{
  p->drawEllipse(cx-4, cy-4, 8, 8);
  p->drawRect(cx-x1, cy-y1, x2, y2);
}

// --------------------------------------------------------------------------
void PortSymbol::getCenter(int& x, int &y)
{
  x = cx;
  y = cy;
}

// --------------------------------------------------------------------------
// Sets the center of the painting to x/y.
void PortSymbol::setCenter(int x, int y, bool relative)
{
  if(relative) { cx += x;  cy += y; }
  else { cx = x;  cy = y; }
}

// --------------------------------------------------------------------------
bool PortSymbol::load(const QString& s)
{
  bool ok;

  QString n;
  n  = s.section(' ',1,1);    // cx
  cx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',2,2);    // cy
  cy = n.toInt(&ok);
  if(!ok) return false;

  numberStr  = s.section(' ',3,3);    // number
  if(!ok) return false;

  return true;
}

// --------------------------------------------------------------------------
QString PortSymbol::save()
{
  QString s = Name+QString::number(cx)+" "+QString::number(cy)+" ";
  s += numberStr;
  return s;
}

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
bool PortSymbol::getSelected(int x, int y)
{
  if(x < cx-x1-4)  return false;
  if(y < cy-y1-4)  return false;
  if(x > cx+x2+8)  return false;
  if(y > cy+y2+8)  return false;

  return true;
}

// --------------------------------------------------------------------------
void PortSymbol::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  if(x2 < 0) { _x1 = cx+x2; _x2 = cx; }
  else { _x1 = cx; _x2 = cx+x2; }

  if(y2 < 0) { _y1 = cy+y2; _y2 = cy; }
  else { _y1 = cy; _y2 = cy+y2; }
}

// --------------------------------------------------------------------------
// Rotates around the center.
void PortSymbol::rotate()
{
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void PortSymbol::mirrorX()
{
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void PortSymbol::mirrorY()
{
}
