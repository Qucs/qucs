/***************************************************************************
                          graphicline.cpp  -  description
                             -------------------
    begin                : Mon Nov 24 2003
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

#include "graphicline.h"
#include "linedialog.h"

#include <math.h>


GraphicLine::GraphicLine()
{
  isSelected = false;
  Pen = QPen(QColor());
  cx = cy = 0;
  x1 = x2 = 0;
  y1 = y2 = 0;
}

GraphicLine::~GraphicLine()
{
}

// --------------------------------------------------------------------------
void GraphicLine::paint(QPainter *p)
{
  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,Pen.width()+5));
    p->drawLine(cx, cy, cx+x2, cy+y2);
    p->setPen(QPen(QPen::white, Pen.width(), Pen.style()));
    p->drawLine(cx, cy, cx+x2, cy+y2);
    return;
  }
  p->setPen(Pen);
  p->drawLine(cx, cy, cx+x2, cy+y2);
}

// --------------------------------------------------------------------------
void GraphicLine::paintScheme(QPainter *p)
{
  p->drawLine(cx, cy, cx+x2, cy+y2);
}

// --------------------------------------------------------------------------
void GraphicLine::getCenter(int& x, int &y)
{
  x = cx+(x2>>1);
  y = cy+(y2>>1);
}

// --------------------------------------------------------------------------
// Sets the center of the painting to x/y.
void GraphicLine::setCenter(int x, int y, bool relative)
{
  if(relative) { cx += x;  cy += y; }
  else { cx = x-(x2>>1);  cy = y-(y2>>1); }
}

// --------------------------------------------------------------------------
GraphicLine* GraphicLine::newOne()
{
  return new GraphicLine();
}

// --------------------------------------------------------------------------
bool GraphicLine::load(const QString& _s)
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
QString GraphicLine::save()
{
  QString s = "   <Line "+QString::number(cx)+" "+QString::number(cy)+" ";
  s += QString::number(x2)+" "+QString::number(y2)+" ";
  s += Pen.color().name()+" "+QString::number(Pen.width())+" "+QString::number(Pen.style())+">";
  return s;
}

// --------------------------------------------------------------------------
// x/y are the precise coordinates, gx/gy are the coordinates due to the grid.
void GraphicLine::MouseMoving(int x, int y, int gx, int gy, QPainter *p, bool drawn)
{
  if(State > 0) {
    if(State > 1) p->drawLine(cx, cy, cx+x2, cy+y2);  // erase old painting
    State++;
    x2 = gx-cx;
    y2 = gy-cy;
    p->drawLine(cx, cy, cx+x2, cy+y2);  // paint new painting
  }
  else { cx = gx; cy = gy; }


  p->setPen(Qt::SolidLine);
  if(drawn) {
    p->drawLine(x1+27, y1, x1+15, y1+12);  // erase old cursor symbol
    p->drawLine(x1+25, y1-2, x1+29, y1+2);
    p->drawLine(x1+13, y1+10, x1+17, y1+14);
  }
  x1 = x;
  y1 = y;
  p->drawLine(x1+27, y1, x1+15, y1+12);  // paint new cursor symbol
  p->drawLine(x1+25, y1-2, x1+29, y1+2);
  p->drawLine(x1+13, y1+10, x1+17, y1+14);
}

// --------------------------------------------------------------------------
bool GraphicLine::MousePressing()
{
  State++;
  if(State > 2) {
    x1 = y1 = 0;
    State = 0;
    return true;    // painting is ready
  }
  return false;
}

// --------------------------------------------------------------------------
// Checks if the coordinates x/y point to the painting.
// 5 is the precision the user must point onto the painting.
bool GraphicLine::getSelected(int x, int y)
{
  x  -= cx;
  if(x < -5) { if(x < x2-5) return false; }    // lies point between x coordinates ?
  else { if(x > x2+5) return false; }

  y  -= cy;
  if(y < -5) { if(y < y2-5) return false; }    // lies point between y coordinates ?
  else { if(y > y2+5) return false; }

  int A  = x2*y - x*y2;     // calculate the rectangle area spanned
  A *= A;                   // avoid the need for square root
  A -= 25*(x2*x2 + y2*y2);  // substract selectable area

  if(A <= 0)  return true;     // lies x/y onto the graph line ?

  return false;
}

// --------------------------------------------------------------------------
void GraphicLine::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  if(x2 < 0) { _x1 = cx+x2; _x2 = cx; }
  else { _x1 = cx; _x2 = cx+x2; }

  if(y2 < 0) { _y1 = cy+y2; _y2 = cy; }
  else { _y1 = cy; _y2 = cy+y2; }
}

// --------------------------------------------------------------------------
// Rotates around the center.
void GraphicLine::rotate()
{
  cx += (x2>>1) - (y2>>1);
  cy += (x2>>1) + (y2>>1);

  int tmp = x2;
  x2  =  y2;
  y2  = -tmp;
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void GraphicLine::mirrorX()
{
  cy +=  y2;
  y2  = -y2;
}

// --------------------------------------------------------------------------
// Mirrors about center line.
void GraphicLine::mirrorY()
{
  cx +=  x2;
  x2  = -x2;
}

// --------------------------------------------------------------------------
// Calls the property dialog for the painting and changes them accordingly.
// If there were changes, it returns 'true'.
bool GraphicLine::Dialog()
{
  bool changed = false;

  LineDialog *d = new LineDialog(QObject::tr("Edit Line Properties"));
  d->ColorButt->setPaletteBackgroundColor(Pen.color());
  d->LineWidth->setText(QString::number(Pen.width()));
  d->SetComboBox(Pen.style());

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

  delete d;
  return changed;
}
