/***************************************************************************
                          element.cpp  -  description
                             -------------------
    begin                : Sat Sep 20 2003
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

#include "graphicitem.h"

GraphicItem::GraphicItem()
{
  ElemType = isDummyElement;
  cx = cy = x1 = y1 = x2 = y2 = 0;
  drawScheme = false;

  setFlags(ItemIsSelectable|ItemIsMovable);
#if QT_VERSION < 0x050000
  setAcceptsHoverEvents(true);
#else
  setAcceptHoverEvents(true);
#endif
}

GraphicItem::~GraphicItem()
{
}

void GraphicItem::paintScheme(QPainter *)
{
}

void GraphicItem::setCenter(int, int, bool)
{
}

void GraphicItem::getCenter(int&, int&)
{
}

QRectF GraphicItem::boundingRect() const
{
  return QRectF(x1, y1, x2-x1, y2-y1);
}

void GraphicItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
}
