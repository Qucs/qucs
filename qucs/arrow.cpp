/***************************************************************************
                          arrow.cpp  -  description
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

#include "arrow.h"
#include "linedialog.h"

#include <math.h>


Arrow::Arrow()
{
  isSelected = false;
  Pen = QPen(QColor());
  cx = cy = 0;
  x1 = x2 = 0;
  y1 = y2 = 0;

  Height = 20.0;
  Width  =  8.0;
  beta   = atan2(double(Width), double(Height));
  Length = sqrt(Width*Width + Height*Height);
}

Arrow::~Arrow()
{
}

// --------------------------------------------------------------------------
void Arrow::paint(QPainter *p)
{
  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,Pen.width()+5));
    p->drawLine(cx, cy, cx+x2, cy+y2);
    p->drawLine(cx+x2, cy+y2, cx+xp1, cy+yp1);
    p->drawLine(cx+x2, cy+y2, cx+xp2, cy+yp2);
    p->setPen(QPen(QPen::white, Pen.width(), Pen.style()));
    p->drawLine(cx, cy, cx+x2, cy+y2);
    p->drawLine(cx+x2, cy+y2, cx+xp1, cy+yp1);
    p->drawLine(cx+x2, cy+y2, cx+xp2, cy+yp2);
    return;
  }
  p->setPen(Pen);
  p->drawLine(cx, cy, cx+x2, cy+y2);
  p->drawLine(cx+x2, cy+y2, cx+xp1, cy+yp1);
  p->drawLine(cx+x2, cy+y2, cx+xp2, cy+yp2);
}

// --------------------------------------------------------------------------
void Arrow::paintScheme(QPainter *p)
{
  p->drawLine(cx, cy, cx+x2, cy+y2);
  p->drawLine(cx+x2, cy+y2, cx+xp1, cy+yp1);
  p->drawLine(cx+x2, cy+y2, cx+xp2, cy+yp2);
}

// --------------------------------------------------------------------------
void Arrow::getCenter(int& x, int &y)
{
  x = cx+(x2>>1);
  y = cy+(y2>>1);
}

// --------------------------------------------------------------------------
// Sets the center of the painting to x/y.
void Arrow::setCenter(int x, int y, bool relative)
{
  if(relative) { cx += x;  cy += y; }
  else { cx = x-(x2>>1);  cy = y-(y2>>1); }
}

// --------------------------------------------------------------------------
Arrow* Arrow::newOne()
{
  return new Arrow();
}

// --------------------------------------------------------------------------
bool Arrow::load(const QString& _s)
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

  n  = s.section(' ',5,5);    // height
  Height = n.toDouble(&ok);
  if(!ok) return false;

  n  = s.section(' ',6,6);    // width
  Width = n.toDouble(&ok);
  if(!ok) return false;

  n  = s.section(' ',7,7);    // color
  QColor co;
  co.setNamedColor(n);
  Pen.setColor(co);
  if(!Pen.color().isValid()) return false;

  n  = s.section(' ',8,8);    // thickness
  Pen.setWidth(n.toInt(&ok));
  if(!ok) return false;

  n  = s.section(' ',9,9);    // line style
  Pen.setStyle((Qt::PenStyle)n.toInt(&ok));
  if(!ok) return false;

  beta   = atan2(double(Width), double(Height));
  Length = sqrt(Width*Width + Height*Height);
  calcArrowHead();
  return true;
}

// --------------------------------------------------------------------------
QString Arrow::save()
{
  QString s = "   <Arrow "+QString::number(cx)+" "+QString::number(cy)+" ";
  s += QString::number(x2)+" "+QString::number(y2)+" ";
  s += QString::number(Height)+" "+QString::number(Width)+" ";
  s += Pen.color().name()+" "+QString::number(Pen.width())+" "+QString::number(Pen.style())+">";
  return s;
}

// --------------------------------------------------------------------------
void Arrow::calcArrowHead()
{
  double phi  = atan2(double(y2), double(x2));

  double w = beta+phi-M_PI;
  xp1 = x2+int(Length*cos(w));
  yp1 = y2+int(Length*sin(w));

  w = M_PI-beta+phi;
  xp2 = x2+int(Length*cos(w));
  yp2 = y2+int(Length*sin(w));
}

// --------------------------------------------------------------------------
// x/y are the precise coordinates, gx/gy are the coordinates due to the grid.
void Arrow::MouseMoving(int x, int y, int gx, int gy, QPainter *p, bool drawn)
{
  if(State > 0) {
    if(State > 1) {
      calcArrowHead();
      paintScheme(p);  // erase old painting
    }
    State++;
    x2 = gx-cx;
    y2 = gy-cy;
    calcArrowHead();
    paintScheme(p);  // paint new painting
  }
  else { cx = gx; cy = gy; }


  p->setPen(Qt::SolidLine);
  if(drawn) {
    p->drawLine(x1+25, y1, x1+13, y1+12);  // erase old cursor symbol
    p->drawLine(x1+18, y1+2, x1+25, y1);
    p->drawLine(x1+23, y1+7, x1+25, y1);
  }
  x1 = x;
  y1 = y;
  p->drawLine(x1+25, y1, x1+13, y1+12);  // paint new cursor symbol
  p->drawLine(x1+18, y1+2, x1+25, y1);
  p->drawLine(x1+23, y1+7, x1+25, y1);
}

// --------------------------------------------------------------------------
bool Arrow::MousePressing()
{
  State++;
  if(State > 2) {
    x1 = y1 = 0;
    State = 0;

    calcArrowHead();
    return true;    // painting is ready
  }
  return false;
}

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
// 5 is the precision the user must point onto the painting.
bool Arrow::getSelected(int x, int y)
{
  double x_double = double(x-cx);
  double y_double = double(y-cy);

  double phi = -atan2(double(y2), double(x2));
  double sin_phi = sin(phi);
  double cos_phi = cos(phi);
  int len = int( sqrt(double( x2*x2 + y2*y2 )) )+5;
  
  int xn = int(x_double*cos_phi - y_double*sin_phi);
  int yn = int(x_double*sin_phi + y_double*cos_phi);

  // lies x/y onto the arrow body ?
  if(xn > -5) if(xn < len) if(yn > -5) if(yn < 5) return true;
  return false;
}

// --------------------------------------------------------------------------
void Arrow::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  if(x2 < 0) { _x1 = cx+x2; _x2 = cx; }
  else { _x1 = cx; _x2 = cx+x2; }

  if(y2 < 0) { _y1 = cy+y2; _y2 = cy; }
  else { _y1 = cy; _y2 = cy+y2; }
}

// --------------------------------------------------------------------------
// Rotates around the center.
void Arrow::rotate()
{
  cx += (x2>>1) - (y2>>1);
  cy += (x2>>1) + (y2>>1);

  int tmp = x2;
  x2  =  y2;
  y2  = -tmp;

  tmp =  xp1;
  xp1 =  yp1;
  yp1 = -tmp;

  tmp =  xp2;
  xp2 =  yp2;
  yp2 = -tmp;
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void Arrow::mirrorX()
{
  yp1 = -yp1;
  yp2 = -yp2;
  cy +=  y2;   // change cy after the other changes !
  y2  = -y2;
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void Arrow::mirrorY()
{
  xp1 = -xp1;
  xp2 = -xp2;
  cx +=  x2;   // change cx after the other changes !
  x2  = -x2;
}

// --------------------------------------------------------------------------
// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool Arrow::Dialog()
{
  bool changed = false;

  LineDialog *d = new LineDialog("Edit Arrow Properties");
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
