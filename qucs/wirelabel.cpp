/***************************************************************************
                          wirelabel.cpp  -  description
                             -------------------
    begin                : Sun February 29 2004
    copyright            : (C) 2004 by Michael Margraf
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

#include "wirelabel.h"
#include "wire.h"
#include "main.h"


WireLabel::WireLabel(const QString& _Name, int _cx, int _cy, int _x1, int _y1, ElementType _Type)
{
  cx = _cx;
  cy = _cy;
  x1 = _x1;
  y1 = _y1;
  setName(_Name);

  Type = _Type;
  isSelected = false;
}

WireLabel::~WireLabel()
{
}

// ----------------------------------------------------------------
void WireLabel::paintScheme(QPainter *p)
{
  p->drawRect(x1, y1, x2, -y2);

  // which corner of rectangle should be connected to line ?
  if(cx < x1+(x2>>1)) {
    if(cy < y1-(y2>>1))
      p->drawLine(cx, cy, x1, y1-y2);
    else
      p->drawLine(cx, cy, x1, y1);
  }
  else {
    if(cy < y1-(y2>>1))
      p->drawLine(cx, cy, x1+x2, y1-y2);
    else
      p->drawLine(cx, cy, x1+x2, y1);
  }
}

// ----------------------------------------------------------------
void WireLabel::setCenter(int x_, int y_, bool relative)
{
  if(Type == isMovingLabel) {
    if(relative) {
      x1 += x_;  cx += x_;
      y1 += y_;  cy += y_;
    }
    else {
      x1 = x_;  cx = x_;
      y1 = y_;  cy = y_;
    }
  }
  else {
    if(relative) {
      x1 += x_;  y1 += y_; // moving cx/cy must be done by owner (wire, node)
    }
    else {
      x1 = x_;
      y1 = y_;
    }
  }
}

// ----------------------------------------------------------------
bool WireLabel::getSelected(int x, int y)
{
  if(x1 <= x)
    if(y1 >= y)
      if((x1+x2) >= x)
        if((y1-y2) <= y)
          return true;

  return false;
}

// ----------------------------------------------------------------
void WireLabel::paint(QPainter *p)
{
  p->setPen(QPen(QPen::darkMagenta,0));

  int xpaint=0, ypaint=4;
  switch(Type) {
    case isVWireLabel:  ypaint=0;  xpaint=4;
                        p->drawArc(cx-4, cy-4, 9, 9, 16*140, 16*255);
                        break;
    case isHWireLabel:  p->drawArc(cx-4, cy-4, 9, 9, 16*230, 16*255);
                        break;
    case isNodeLabel:   ypaint = 0;
    default:            ;
  }

  if(cx < x1+(x2>>1)) {    // where should frame be painted ?
    if(cy < y1-(y2>>1)) {
      p->drawLine(x1-2, y1-y2, x1-2, y1);
      p->drawLine(x1-2, y1-y2, x1+13, y1-y2);
      p->drawLine(cx+xpaint, cy+ypaint, x1-2, y1-y2);
    }
    else {
      p->drawLine(x1-2, y1+2, x1-2, y1-y2+2);
      p->drawLine(x1-2, y1+2, x1+17, y1+2);
      p->drawLine(cx+xpaint, cy-ypaint, x1-2, y1+2);
    }
  }
  else {
    if(cy < y1-(y2>>1)) {
      p->drawLine(x1+x2+2, y1-y2, x1+x2+2, y1);
      p->drawLine(x1+x2+2, y1-y2, x1+x2-13, y1-y2);
      p->drawLine(cx-xpaint, cy+ypaint, x1+x2+2, y1-y2);
    }
    else {
      p->drawLine(x1+x2+2, y1+2, x1+x2+2, y1-y2+2);
      p->drawLine(x1+x2+2, y1+2, x1+x2-13, y1+2);
      p->drawLine(cx-xpaint, cy-ypaint, x1+x2+2, y1+2);
    }
  }

  p->setPen(QPen(QPen::black,1));
  p->drawText(x1, y1, Name);

  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,3));
    p->drawRoundRect(x1-2, y1+2, x2+4, -y2-2);
  }
}

// ----------------------------------------------------------------
void WireLabel::setName(const QString& Name_)
{
  Name = Name_;

  QFontMetrics  metrics(QucsSettings.font);    // get size of text
  QRect r = metrics.boundingRect(0,0,0,0, Qt::AlignAuto, Name);
  x2 = r.width();
  y2 = r.height()-2;    // remember size of text
}

// ----------------------------------------------------------------
// Converts all necessary data of the wire into a string. This can be used to
// save it to an ASCII file or to transport it via the clipboard.
// Wire labels use the same format like wires, but with length zero.
QString WireLabel::save()
{
  QString s  = "   <";
          s += QString::number(cx)+" "+QString::number(cy)+" ";
          s += QString::number(cx)+" "+QString::number(cy);
          s += " \""+Name +"\" ";
          s += QString::number(x1)+" "+QString::number(y1)+" 0>";
  return s;
}
