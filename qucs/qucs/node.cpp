/***************************************************************************
                          node.cpp  -  description
                             -------------------
    begin                : Sat Sep 20 2003
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

#include "node.h"

Node::Node(int _x, int _y)
{
  Label = 0;
  Type  = isNode;

  cx = _x;
  cy = _y;
}

Node::~Node()
{
}

// -------------------------------------------------------------
void Node::paint(QPainter *p)
{
  switch(Connections.count()) {
    case 1 :  p->setPen(QPen(QPen::red,1));  // node is open
              p->drawEllipse(cx-4, cy-4, 8, 8);
              break;
    case 2 :  if(Connections.getFirst()->Type == isWire)
                if(Connections.getLast()->Type == isWire) break;
              p->fillRect(cx-2, cy-2, 4, 4, QBrush(Qt::darkBlue));
              break;
    default : p->setPen(QPen(QPen::darkBlue,4));  // more than 2 connections
              p->drawEllipse(cx-2, cy-2, 4, 4);
              break;
  }

  if(Label) Label->paint(p);
}

// ----------------------------------------------------------------
bool Node::getSelected(int x_, int y_)
{
  if(cx-5 <= x_) if(cx+5 >= x_) if(cy-5 <= y_) if(cy+5 >= y_)
    return true;

  return false;
}

// ----------------------------------------------------------------
void Node::setName(const QString& Name_, int x_, int y_)
{
  if(Name_.isEmpty()) {
    if(Label) delete Label;
    Label = 0;
    return;
  }

  if(!Label) Label = new WireLabel(Name_, cx, cy, x_, y_, isNodeLabel);
  else Label->setName(Name_);
  Label->pNode = this;
  Label->pWire = 0;
}
