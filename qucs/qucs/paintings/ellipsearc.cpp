/***************************************************************************
                         ellipsearc.cpp  -  description
                             -------------------
    begin                : Thu Sep 9 2004
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

#include "ellipsearc.h"
#include "filldialog.h"

#include <math.h>


EllipseArc::EllipseArc()
{
  isSelected = false;
  Pen = QPen(QColor());
  cx = cy = x1 = x2 = y1 = y2 = Angle = ArcLen = 0;
}

EllipseArc::~EllipseArc()
{
}

// --------------------------------------------------------------------------
void EllipseArc::paint(QPainter *p)
{
  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,Pen.width()+5));
    p->drawArc(cx, cy, x2, y2, Angle, ArcLen);
    p->setPen(QPen(QPen::white, Pen.width(), Pen.style()));
    p->drawArc(cx, cy, x2, y2, Angle, ArcLen);

    p->setPen(QPen(QPen::darkRed,2));
    p->drawRect(cx, cy+y2-10, 10, 10);  // markers for changing the size
    p->drawRect(cx, cy, 10, 10);
    p->drawRect(cx+x2-10, cy+y2-10, 10, 10);
    p->drawRect(cx+x2-10, cy, 10, 10);
    return;
  }
  p->setPen(Pen);
  p->drawArc(cx, cy, x2, y2, Angle, ArcLen);
}

// --------------------------------------------------------------------------
void EllipseArc::paintScheme(QPainter *p)
{
  p->drawArc(cx, cy, x2, y2, Angle, ArcLen);
}

// --------------------------------------------------------------------------
void EllipseArc::getCenter(int& x, int &y)
{
  x = cx+(x2>>1);
  y = cy+(y2>>1);
}

// --------------------------------------------------------------------------
// Sets the center of the painting to x/y.
void EllipseArc::setCenter(int x, int y, bool relative)
{
  if(relative) { cx += x;  cy += y; }
  else { cx = x-(x2>>1);  cy = y-(y2>>1); }
}

// --------------------------------------------------------------------------
EllipseArc* EllipseArc::newOne()
{
  return new EllipseArc();
}

// --------------------------------------------------------------------------
bool EllipseArc::load(const QString& s)
{
  bool ok;
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

  n  = s.section(' ',5,5);    // start angle
  Angle = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',6,6);    // arc length
  ArcLen = n.toInt(&ok);
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

  return true;
}

// --------------------------------------------------------------------------
QString EllipseArc::save()
{
  QString s = "EArc " +
	QString::number(cx) + " " + QString::number(cy) + " " +
	QString::number(x2) + " " + QString::number(y2) + " " +
	QString::number(Angle) + " " + QString::number(ArcLen) + " " +
	Pen.color().name()  + " " + QString::number(Pen.width()) + " " +
	QString::number(Pen.style());
  return s;
}

// --------------------------------------------------------------------------
// Checks if the resize area was clicked.
bool EllipseArc::ResizeTouched(int x, int y)
{
  State = -1;
  if(x < cx) return false;
  if(y < cy) return false;
  if(x > cx+x2) return false;
  if(y > cy+y2) return false;

  State = 0;
  if(x < cx+10) State = 1;
  else if(x <= cx+x2-10) { State = -1; return false; }
  if(y < cy+10)  State |= 2;
  else if(y <= cy+y2-10) { State = -1; return false; }

  return true;
}

// --------------------------------------------------------------------------
// Mouse move action during resize.
void EllipseArc::MouseResizeMoving(int x, int y, QPainter *p)
{
  paintScheme(p);  // erase old painting
  switch(State) {
    case 0: x2 = x-cx; y2 = y-cy; // lower right corner
	    break;
    case 1: x2 -= x-cx; cx = x; y2 = y-cy; // lower left corner
	    break;
    case 2: x2 = x-cx; y2 -= y-cy; cy = y; // upper right corner
	    break;
    case 3: x2 -= x-cx; cx = x; y2 -= y-cy; cy = y; // upper left corner
	    break;
  }
  if(x2 < 0) { State ^= 1; x2 *= -1; cx -= x2; }
  if(y2 < 0) { State ^= 2; y2 *= -1; cy -= y2; }

  paintScheme(p);  // paint new painting
}

// --------------------------------------------------------------------------
// x/y are the precise coordinates, gx/gy are the coordinates due to the grid.
void EllipseArc::MouseMoving(int x, int y, int gx, int gy, QPainter *p, bool drawn)
{
  switch(State) {
    case 0 :
       x2 = gx;
       y2 = gy;
       break;
    case 1 :
      State++;
      gx -= x1;
      gy -= y1;
      x2 = int(sqrt(double(gx*gx + gy*gy)));
      Angle  = x1 + (gx-x2) >> 2;
      ArcLen = y1 + (gy-x2) >> 2;
      p->drawArc(Angle, ArcLen, x2, x2, 0, 16*360);  // paint new painting
      break;
    case 2 :
      p->drawArc(Angle, ArcLen, x2, x2, 0, 16*360);  // erase old painting
      gx -= x1;
      gy -= y1;
      x2 = int(sqrt(double(gx*gx + gy*gy)));
      Angle  = x1 + (gx-x2) >> 2;
      ArcLen = y1 + (gy-x2) >> 2;
      p->drawArc(Angle, ArcLen, x2, x2, 0, 16*360);  // paint new painting
      break;
    case 3 :
      break;
  }


  p->setPen(Qt::SolidLine);
  if(drawn)
    p->drawArc(cx+13, cy, 18, 12, 16*45, 16*200); // erase old cursor symbol

  cx = x;
  cy = y;
  p->drawArc(cx+13, cy, 18, 12, 16*45, 16*200);  // paint new cursor symbol
}

// --------------------------------------------------------------------------
bool EllipseArc::MousePressing()
{
  State++;
  switch(State) {
    case 1 :
//	Angle  = 0;
//	ArcLen = 16*360;
	x1 = x2;
	y1 = y2;    // first corner is determined
	break;
    case 3 :
	State = 0;
	return true;    // painting is ready
  }
  return false;
}

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
bool EllipseArc::getSelected(int x, int y)
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
void EllipseArc::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  _x1 = cx;     _y1 = cy;
  _x2 = cx+x2;  _y2 = cy+y2;
}

// --------------------------------------------------------------------------
// Rotates around the center.
void EllipseArc::rotate()
{
  cy += (y2-x2) >> 1;
  cx += (x2-y2) >> 1;
  int tmp = x2;
  x2 = y2;
  y2 = tmp;
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void EllipseArc::mirrorX()
{
  // nothing to do
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void EllipseArc::mirrorY()
{
  // nothing to do
}

// --------------------------------------------------------------------------
// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool EllipseArc::Dialog()
{
  bool changed = false;

  FillDialog *d = new FillDialog(QObject::tr("Edit Arc Properties"), false);
  d->ColorButt->setPaletteBackgroundColor(Pen.color());
  d->LineWidth->setText(QString::number(Pen.width()));
  d->StyleBox->setCurrentItem(Pen.style()-1);

  if(d->exec() == QDialog::Rejected) {
    delete d;
    return false;
  }

  if(Pen.color() != d->ColorButt->paletteBackgroundColor()) {
    Pen.setColor(d->ColorButt->paletteBackgroundColor());
    changed = true;
  }
  if(Pen.width()  != d->LineWidth->text().toUInt()) {
    Pen.setWidth(d->LineWidth->text().toUInt());
    changed = true;
  }
  if(Pen.style()  != (Qt::PenStyle)(d->StyleBox->currentItem()+1)) {
    Pen.setStyle((Qt::PenStyle)(d->StyleBox->currentItem()+1));
    changed = true;
  }

  delete d;
  return changed;
}
