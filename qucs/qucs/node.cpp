/*
 * node.cpp - Implementation of an electrical node
 *
 * Copyright (C) 2003, Michael Margraf, michael.margraf@alumni.tu-berlin.de
 * Copyright (C) 2015, Guilherme Brondani Torri, guitorri@gmail.com
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

#include "node.h"
#include "wirelabel.h"

#include <QPainter>

Node::Node(int _x, int _y)
{
  Label = 0;
  ElemType  = isNode;
  State = 0;
  DType = "";

  cx = _x;
  cy = _y;

  setFlags(ItemIsSelectable|ItemIsMovable);
#if QT_VERSION < 0x050000
  setAcceptsHoverEvents(true);
#else
  setAcceptHoverEvents(true);
#endif
}

Node::~Node()
{
}

QRectF Node::boundingRect() const
{
  return *(new QRect(cx-4,cy-4,8,8) );
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
  Q_UNUSED(item);
  Q_UNUSED(widget);

  switch (Connections.count()) {
    case 1: {
      if (Label) {
        // open but labeled
        painter->fillRect(cx-2, cy-2, 4, 4, Qt::darkBlue);
      }
      else {
        // node is open
        painter->setPen(QPen(Qt::red,1));
        painter->drawEllipse(cx-4, cy-4, 8, 8);
      }
      return;
    }
    case 2: {
      if(Connections.getFirst()->ElemType == isWire)
        if(Connections.getLast()->ElemType == isWire)
          return;
      painter->fillRect(cx-2, cy-2, 4, 4, Qt::darkBlue);
      break;
    }
    default: {
      // more than 2 connections
      painter->setBrush(Qt::darkBlue);
      painter->setPen(QPen(Qt::darkBlue,1));
      painter->drawEllipse(cx-3, cy-3, 6, 6);
      painter->setBrush(Qt::NoBrush);
      break;
    }
  }

  // debug
  if (isSelected()) {
    painter->setPen(QPen(Qt::green,2));
    painter->drawRect(boundingRect());
    ElemSelected = true;
  }
  ElemSelected = false;
}

// ----------------------------------------------------------------
bool Node::getSelected(int x_, int y_)
{
  if(cx-5 <= x_) if(cx+5 >= x_) if(cy-5 <= y_) if(cy+5 >= y_)
    return true;

  return false;
}

/*!
 * \brief Node::setName
 * \param Name_
 * \param Value_
 * \param x_ Label x-distance from node center
 * \param y_ Label y-distance from node center
 *
 * Create a WireLabel and attach it the Node.
 * Called from mouse press handling.
 */
void Node::setName(const QString& Name_, const QString& Value_, int x_, int y_)
{
  if(Name_.isEmpty() && Value_.isEmpty()) {
    if(Label) delete Label;
    Label = 0;
    return;
  }

  if(!Label) Label = new WireLabel(Name_, cx, cy, x_, y_, isNodeLabel);
  else Label->setName(Name_);
  Label->pOwner = this;
  Label->initValue = Value_;
}
