/*
 * frame.cpp - Implement the schematic grid
 *
 * Copyright (C) 2006, Michael Margraf
 * Copyright (C) 2008, Stefan Jahn
 * Copyright (C) 2017, Guilherme Brondani Torri, guitorri@gmail.com
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QPainter>
#include <QObject>

#include "frame.h"

Frame::Frame()
{
  Title    = QObject::tr("Title");
  Author   = QObject::tr("Drawn By:");
  Date     = QObject::tr("Date:");;
  Revision = QObject::tr("Revision:");

  // default don't show, emulate deprecated showFrame variable
  PageType = 0;
}

QRectF Frame::boundingRect() const
{
  /// \todo return size based on item boundaries
  return QRectF(0, 0, Width, Height);
}

void Frame::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
  Q_UNUSED(item);
  Q_UNUSED(widget);

  // dimensions:  X cm / 2.54 * 144
  int xall, yall;
  if(!sizeOfFrame(xall, yall))
    return;
  Height = yall;
  Width = xall;
  painter->setPen(QPen(Qt::darkGray,1));
  int d = painter->fontMetrics().lineSpacing() + int(4.0);
  int x1_ = 0, y1_ =0;
  int x2_, y2_;
  x2_ = int(xall);
  y2_ = int(yall);
  // outer and inner box
  painter->drawRect(x1_, y1_, x2_, y2_);
  painter->drawRect(x1_ + d, y1_ + d, x2_ - 2*d, y2_ - 2*d);

  // top an bottom margins
  int z;
  int step = xall / ((xall+127) / 255);
  for(z=step; z<=xall-step; z+=step) {
    painter->drawLine(z, 0, z, d);
    painter->drawLine(z, yall-d, z, yall);
  }
  char Letter[2] = "1";
  for(z=step/2+5; z<xall; z+=step) {
    painter->drawText(z, d-3,Letter);
    painter->drawText(z, yall-3, Letter);
    Letter[0]++;
  }

  // left and right margin
  step = yall / ((yall+127) / 255);
  for(z=step; z<=yall-step; z+=step) {
    painter->drawLine(0, z, d, z);
    painter->drawLine(xall-d, z, xall, z);
  }
  Letter[0] = 'A';
  for(z=step/2+5; z<yall; z+=step) {
    painter->drawText(5, z, Letter);
    painter->drawText(xall-d+5, z, Letter);
    Letter[0]++;
  }

  // draw bottom right box with text
  x1_ = xall-340-d;
  y1_ = yall-3-d;
  x2_ = xall-3-d;
  y2_ = yall-3-d;
  d = int(6.0);
  z = int(200.0);
  int lineSpace = painter->fontMetrics().lineSpacing();
  y1_ -= lineSpace + d;
  painter->drawLine(x1_, y1_, x2_, y1_);
  painter->drawText(x1_+d, y1_+(d>>1), 0, 0, Qt::TextDontClip, Date);
  painter->drawLine(x1_+z, y1_, x1_+z, y1_ + lineSpace+d);
  painter->drawText(x1_+d+z, y1_+(d>>1), 0, 0, Qt::TextDontClip, Revision);
  y1_ -= lineSpace + d;
  painter->drawLine(x1_, y1_, x2_, y1_);
  painter->drawText(x1_+d, y1_+(d>>1), 0, 0, Qt::TextDontClip, Author);
  y1_ -= (Title.count('\n')+1) * lineSpace + d;
  painter->drawRect(x2_, y2_, x1_-x2_-1, y1_-y2_-1);
  painter->drawText(x1_+d, y1_+(d>>1), 0, 0, Qt::TextDontClip, Title);
}

/*!
 * \brief sizeOfFrame return size of frame by reference
 * \param xall horizontal size by reference
 * \param yall vertical size by reference
 * \return true if type of frame exists (false otherwise)
 */
bool Frame::sizeOfFrame(int& xall, int& yall)
{
  // Values exclude border of 1.5cm at each side.
  switch( PageType ) {
    case 1:  xall = 1020; yall =  765; break;  // DIN A5 landscape
    case 2:  xall =  765; yall = 1020; break;  // DIN A5 portrait
    case 3:  xall = 1530; yall = 1020; break;  // DIN A4 landscape
    case 4:  xall = 1020; yall = 1530; break;  // DIN A4 portrait
    case 5:  xall = 2295; yall = 1530; break;  // DIN A3 landscape
    case 6:  xall = 1530; yall = 2295; break;  // DIN A3 portrait
    case 7:  xall = 1414; yall = 1054; break;  // letter landscape
    case 8:  xall = 1054; yall = 1414; break;  // letter portrait
    default:  return false;
  }

  return true;
}
