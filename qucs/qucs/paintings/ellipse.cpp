/***************************************************************************
                          ellipse.cpp  -  description
                             -------------------
    begin                : Sun Nov 23 2003
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

#include "ellipse.h"
#include "linedialog.h"


Ellipse::Ellipse()
{
  isSelected = false;
  Pen = QPen(QColor());
  cx = cy = 0;
  x1 = x2 = 0;
  y1 = y2 = 0;
}

Ellipse::~Ellipse()
{
}

// --------------------------------------------------------------------------
void Ellipse::paint(QPainter *p)
{
  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,Pen.width()+5));
    p->drawArc(cx, cy, x2, y2, 0, 16*360);
    p->setPen(QPen(QPen::white, Pen.width(), Pen.style()));
    p->drawArc(cx, cy, x2, y2, 0, 16*360);
    return;
  }
  p->setPen(Pen);
  p->drawArc(cx, cy, x2, y2, 0, 16*360);
}

// --------------------------------------------------------------------------
void Ellipse::paintScheme(QPainter *p)
{
  p->drawArc(cx, cy, x2, y2, 0, 16*360);
}

// --------------------------------------------------------------------------
void Ellipse::getCenter(int& x, int &y)
{
  x = cx+(x2>>1);
  y = cy+(y2>>1);
}

// --------------------------------------------------------------------------
// Sets the center of the painting to x/y.
void Ellipse::setCenter(int x, int y, bool relative)
{
  if(relative) { cx += x;  cy += y; }
  else { cx = x-(x2>>1);  cy = y-(y2>>1); }
}

// --------------------------------------------------------------------------
Ellipse* Ellipse::newOne()
{
  return new Ellipse();
}

// --------------------------------------------------------------------------
bool Ellipse::load(const QString& _s)
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
QString Ellipse::save()
{
  QString s = "   <Ellipse "+QString::number(cx)+" "+QString::number(cy)+" ";
  s += QString::number(x2)+" "+QString::number(y2)+" ";
  s += Pen.color().name()+" "+QString::number(Pen.width())+" "+QString::number(Pen.style())+">";
  return s;
}

// --------------------------------------------------------------------------
// x/y are the precise coordinates, gx/gy are the coordinates due to the grid.
void Ellipse::MouseMoving(int x, int y, int gx, int gy, QPainter *p, bool drawn)
{
  if(State > 0) {
    if(State > 1) p->drawArc(x1, y1, x2-x1, y2-y1, 0, 16*360);  // erase old painting
    State++;
    x2 = gx;
    y2 = gy;
    p->drawArc(x1, y1, x2-x1, y2-y1, 0, 16*360);  // paint new painting
  }
  else { x2 = gx; y2 = gy; }


  p->setPen(Qt::SolidLine);
  if(drawn) p->drawArc(cx+13, cy, 18, 12, 0, 16*360);  // erase old cursor symbol
  cx = x;
  cy = y;
  p->drawArc(cx+13, cy, 18, 12, 0, 16*360);  // paint new cursor symbol
}

// --------------------------------------------------------------------------
bool Ellipse::MousePressing()
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
    return true;    // painting is ready
  }
  return false;
}

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
bool Ellipse::getSelected(int x, int y)
{
  x  = (x-cx-(x2>>1));  x *= x;
  y  = (y-cy-(y2>>1));  y *= y;

  int a1 = (x2-5)>>1;  a1 *= a1;
  int a2 = (x2+5)>>1;  a2 *= a2;
  int b1 = (y2-5)>>1;  b1 *= b1;
  int b2 = (y2+5)>>1;  b2 *= b2;

  double x_double = double(x);
  double y_double = double(y);

  if((x_double/double(a1) + y_double/double(b1)) < 1.0) return false;
  if((x_double/double(a2) + y_double/double(b2)) > 1.0) return false;
  return true;
}

// --------------------------------------------------------------------------
void Ellipse::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  _x1 = cx;     _y1 = cy;
  _x2 = cx+x2;  _y2 = cy+y2;
}

// --------------------------------------------------------------------------
// Rotates around the center.
void Ellipse::rotate()
{
  cy += (y2-x2) >> 1;
  cx += (x2-y2) >> 1;
  int tmp = x2;
  x2 = y2;
  y2 = tmp;
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void Ellipse::mirrorX()
{
  // nothing to do
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void Ellipse::mirrorY()
{
  // nothing to do
}

// --------------------------------------------------------------------------
// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool Ellipse::Dialog()
{
  bool changed = false;

  LineDialog *d = new LineDialog(QObject::tr("Edit Ellipse Properties"));
  d->ColorButt->setPaletteBackgroundColor(Pen.color());
  d->LineWidth->setText(QString::number(Pen.width()));
  d->SetComboBox(Pen.style());

  if(d->exec() == QDialog::Rejected) return false;

  if(Pen.color() != d->ColorButt->paletteBackgroundColor()) {
    Pen.setColor(d->ColorButt->paletteBackgroundColor());
    changed = true;
  }
  if(Pen.width()  != d->LineWidth->text().toUInt()) {
    Pen.setWidth(d->LineWidth->text().toUInt());
    changed = true;
  }
  if(Pen.style()  != d->LineStyle) {
    Pen.setStyle(d->LineStyle);
    changed = true;
  }

  return changed;
}
