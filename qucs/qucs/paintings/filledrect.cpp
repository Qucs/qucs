/***************************************************************************
                          filledrect.cpp  -  description
                             -------------------
    begin                : Thu May 20 2004
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

#include "filledrect.h"

#include "filldialog.h"



// **********************************************************************
// In contrast to "rectangle", the "filledrect" is also selected when
// clicking within the rectangle.

FilledRect::FilledRect()
{
  isSelected = false;
  Pen   = QPen(QColor());
  Brush = QBrush(QPen::lightGray);
  cx = cy = 0;
  x1 = x2 = 0;
  y1 = y2 = 0;
}

FilledRect::~FilledRect()
{
}

// --------------------------------------------------------------------------
void FilledRect::paint(QPainter *p)
{
  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,Pen.width()+5));
    p->setBrush(Brush);
    p->drawRect(cx, cy, x2, y2);
    p->setPen(QPen(QPen::white, Pen.width(), Pen.style()));
    p->setBrush(QBrush::NoBrush);
    p->drawRect(cx, cy, x2, y2);
    return;
  }
  p->setPen(Pen);
  p->setBrush(Brush);
  p->drawRect(cx, cy, x2, y2);
  p->setBrush(QBrush::NoBrush);  // no filling for the next paintings
}

// --------------------------------------------------------------------------
void FilledRect::paintScheme(QPainter *p)
{
  p->drawRect(cx, cy, x2, y2);
}

// --------------------------------------------------------------------------
void FilledRect::getCenter(int& x, int &y)
{
  x = cx+(x2>>1);
  y = cy+(y2>>1);
}

// --------------------------------------------------------------------------
// Sets the center of the painting to x/y.
void FilledRect::setCenter(int x, int y, bool relative)
{
  if(relative) { cx += x;  cy += y; }
  else { cx = x-(x2>>1);  cy = y-(y2>>1); }
}

// --------------------------------------------------------------------------
FilledRect* FilledRect::newOne()
{
  return new FilledRect();
}

// --------------------------------------------------------------------------
bool FilledRect::load(const QString& _s)
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

  n  = s.section(' ',8,8);    // fill color
  co.setNamedColor(n);
  Brush.setColor(co);
  if(!Brush.color().isValid()) return false;

  n  = s.section(' ',9,9);    // fill style
  Brush.setStyle((Qt::BrushStyle)n.toInt(&ok));
  if(!ok) return false;

  return true;
}

// --------------------------------------------------------------------------
QString FilledRect::save()
{
  QString s = "   <FilledRect " +
	QString::number(cx) + " " + QString::number(cy) + " " +
	QString::number(x2) + " " + QString::number(y2) + " " +
	Pen.color().name() + " " + QString::number(Pen.width()) + " " +
	QString::number(Pen.style()) + " " +
	Brush.color().name() + " " + QString::number(Brush.style()) + ">";
  return s;
}

// --------------------------------------------------------------------------
// x/y are the precise coordinates, gx/gy are the coordinates due to the grid.
void FilledRect::MouseMoving(int x, int y, int gx, int gy, QPainter *p, bool drawn)
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
  if(drawn) {
    p->drawRect(cx+13, cy, 18, 12);  // erase old cursor symbol
    p->drawLine(cx+14, cy+6, cx+19, cy+1);
    p->drawLine(cx+26, cy+1, cx+17, cy+10);
    p->drawLine(cx+29, cy+5, cx+24, cy+10);
  }
  cx = x;
  cy = y;
  p->drawRect(cx+13, cy, 18, 12);  // paint new cursor symbol
  p->drawLine(cx+14, cy+6, cx+19, cy+1);
  p->drawLine(cx+26, cy+1, cx+17, cy+10);
  p->drawLine(cx+29, cy+5, cx+24, cy+10);
}

// --------------------------------------------------------------------------
bool FilledRect::MousePressing()
{
  State++;
  if(State == 1) {
    x1 = x2;
    y1 = y2;    // first corner is determined
  }
  else {
    if(x1 < x2) { cx = x1; x2 = x2-x1; } // cx/cy always be upper left corner
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
bool FilledRect::getSelected(int x, int y)
{
  if(x > (cx+x2)) return false;   // coordinates outside the rectangle ?
  if(y > (cy+y2)) return false;
  if(x < cx) return false;
  if(y < cy) return false;

  return true;
}

// --------------------------------------------------------------------------
void FilledRect::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  _x1 = cx;     _y1 = cy;
  _x2 = cx+x2;  _y2 = cy+y2;
}

// --------------------------------------------------------------------------
// Rotates around the center.
void FilledRect::rotate()
{
  cy += (y2-x2) >> 1;
  cx += (x2-y2) >> 1;
  int tmp = x2;
  x2 = y2;
  y2 = tmp;
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void FilledRect::mirrorX()
{  // nothing to do
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void FilledRect::mirrorY()
{  // nothing to do
}

// --------------------------------------------------------------------------
// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool FilledRect::Dialog()
{
  bool changed = false;

  FillDialog *d = new FillDialog(QObject::tr("Edit Rectangle Properties"));
  d->ColorButt->setPaletteBackgroundColor(Pen.color());
  d->LineWidth->setText(QString::number(Pen.width()));
  d->SetComboBox(Pen.style());
  d->FillColorButt->setPaletteBackgroundColor(Brush.color());
  d->SetFillComboBox(Brush.style());

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
  if(Pen.style()  != d->LineStyle) {
    Pen.setStyle(d->LineStyle);
    changed = true;
  }
  if(Brush.color() != d->FillColorButt->paletteBackgroundColor()) {
    Brush.setColor(d->FillColorButt->paletteBackgroundColor());
    changed = true;
  }
  if(Brush.style()  != d->FillStyle) {
    Brush.setStyle(d->FillStyle);
    changed = true;
  }

  delete d;
  return changed;
}
