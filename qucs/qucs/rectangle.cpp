/***************************************************************************
                          rectangle.cpp  -  description
                             -------------------
    begin                : Sat Nov 22 2003
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

#include "rectangle.h"
#include "linedialog.h"


Rectangle::Rectangle()
{
  isSelected = false;
  Pen = QPen(QColor());
  cx = cy = 0;
  x1 = x2 = 0;
  y1 = y2 = 0;
}

Rectangle::~Rectangle()
{
}

// --------------------------------------------------------------------------
void Rectangle::paint(QPainter *p)
{
  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,Pen.width()+5));
    p->drawRect(cx, cy, x2, y2);
    p->setPen(QPen(QPen::white, Pen.width(), Pen.style()));
    p->drawRect(cx, cy, x2, y2);
    return;
  }
  p->setPen(Pen);
  p->drawRect(cx, cy, x2, y2);
}

// --------------------------------------------------------------------------
void Rectangle::paintScheme(QPainter *p)
{
  p->drawRect(cx, cy, x2, y2);
}

// --------------------------------------------------------------------------
void Rectangle::getCenter(int& x, int &y)
{
  x = cx+(x2>>1);
  y = cy+(y2>>1);
}

// --------------------------------------------------------------------------
// Sets the center of the painting to x/y.
void Rectangle::setCenter(int x, int y, bool relative)
{
  if(relative) { cx += x;  cy += y; }
  else { cx = x-(x2>>1);  cy = y-(y2>>1); }
}

// --------------------------------------------------------------------------
Rectangle* Rectangle::newOne()
{
  return new Rectangle();
}

// --------------------------------------------------------------------------
bool Rectangle::load(const QString& _s)
{
  bool ok;
  QString s = _s;

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

  n  = s.section(' ',5,5);    // color
  QColor co;
  co.setNamedColor(n);
  Pen.setColor(co);
  if(!Pen.color().isValid()) return false;

  n  = s.section(' ',6,6);    // thickness
  Pen.setWidth(n.toInt(&ok));
  if(!ok) return false;

  n  = s.section(' ',7,7);    // line style
  Pen.setStyle((Qt::PenStyle)n.toInt(&ok));
  if(!ok) return false;

  return true;
}

// --------------------------------------------------------------------------
QString Rectangle::save()
{
  QString s = "   <Rectangle "+QString::number(cx)+" "+QString::number(cy)+" ";
  s += QString::number(x2)+" "+QString::number(y2)+" ";
  s += Pen.color().name()+" "+QString::number(Pen.width())+" "+QString::number(Pen.style())+">";
  return s;
}

// --------------------------------------------------------------------------
// x/y are the precise coordinates, gx/gy are the coordinates due to the grid.
void Rectangle::MouseMoving(int x, int y, int gx, int gy, QPainter *p, bool drawn)
{
  if(State > 0) {
    if(State > 1) p->drawRect(x1, y1, x2-x1, y2-y1);  // erase old painting
    State++;
    x2 = gx;
    y2 = gy;
    p->drawRect(x1, y1, x2-x1, y2-y1);  // paint new rectangle
  }
  else { x2 = gx; y2 = gy; }


  p->setPen(Qt::SolidLine);
  if(drawn) p->drawRect(cx+13, cy, 18, 12);  // erase old cursor symbol
  cx = x;
  cy = y;
  p->drawRect(cx+13, cy, 18, 12);  // paint new cursor symbol
}

// --------------------------------------------------------------------------
bool Rectangle::MousePressing()
{
  State++;
  if(State == 1) {
    x1 = x2;
    y1 = y2;    // first corner is determined
  }
  else {
    if(x1 < x2) { cx = x1; x2 = x2-x1; }    // cx/cy should always be the upper left corner
    else { cx = x2; x2 = x1-x2; }
    if(y1 < y2) { cy = y1; y2 = y2-y1; }
    else { cy = y2; y2 = y1-y2; }
    x1 = y1 = 0;
    State = 0;
    return true;    // rectangle is ready
  }
  return false;
}

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
bool Rectangle::getSelected(int x, int y)
{
  // 5 is the precision the user must point onto the rectangle
  if(x > (cx+x2+5)) return false;   // coordinates outside the rectangle ?
  if(y > (cy+y2+5)) return false;
  if(x < (cx-5)) return false;
  if(y < (cy-5)) return false;

  // coordinates inside the rectangle ?
  if(x < (cx+x2-5)) if(x > (cx+5)) if(y < (cy+y2-5)) if(y > (cy+5)) return false;
  return true;
}

// --------------------------------------------------------------------------
void Rectangle::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  _x1 = cx;     _y1 = cy;
  _x2 = cx+x2;  _y2 = cy+y2;
}

// --------------------------------------------------------------------------
// Rotates around the center.
void Rectangle::rotate()
{
  cy += (y2-x2) >> 1;
  cx += (x2-y2) >> 1;
  int tmp = x2;
  x2 = y2;
  y2 = tmp;
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void Rectangle::mirrorX()
{
  // nothing to do
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void Rectangle::mirrorY()
{
  // nothing to do
}

// --------------------------------------------------------------------------
// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool Rectangle::Dialog()
{
  bool changed = false;

  LineDialog *d = new LineDialog("Edit Rectangle Properties");
  d->ColorButt->setPaletteBackgroundColor(Pen.color());
  d->LineWidth->setText(QString::number(Pen.width()));

  if(d->exec() == QDialog::Rejected) return false;

  if(Pen.color() != d->ColorButt->paletteBackgroundColor()) {
    Pen.setColor(d->ColorButt->paletteBackgroundColor());
    changed = true;
  }
  if(Pen.width()  != d->LineWidth->text().toUInt()) {
    Pen.setWidth(d->LineWidth->text().toUInt());
    changed = true;
  }

  return changed;
}
