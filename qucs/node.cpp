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

Node::Node(int _x, int _y) : x(_x), y(_y)
{
  isNamed = false;
}

Node::~Node()
{
}

// -------------------------------------------------------------
void Node::paint(QPainter *p)
{
  switch(Connections.count()) {
    case 1 :  p->setPen(QPen(QPen::red,1));  // node is open
              p->drawEllipse(x-4, y-4, 8, 8);
              break;
    case 2 :  if(Connections.first()->Type == isWire) if(Connections.last()->Type == isWire) break;
              p->setPen(QPen(QPen::darkBlue,3));
              p->drawRect(x-1, y-1, 2, 2);
              break;
    default : p->setPen(QPen(QPen::darkBlue,4));  // more than 2 connections
              p->drawEllipse(x-2, y-2, 4, 4);
              break;
  }
}
