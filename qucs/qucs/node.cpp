/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "node.h"
#include "wire.h"
#include "viewpainter.h" //?
#include "wirelabel.h"
#include "net.h"
#include "netlist.h"

#include <QPainter>

Node::Node(std::pair<int, int> pos)
  : Object(),
   _position(pos),
   _ports(0)
{
  trace1("Node::Node", this);
  // State = 0;
  // DType = ""; //?
}
/*--------------------------------------------------------------------------*/
Node::~Node()
{ untested();
  trace1("~Node", this);
  assert(!degree());
}

// -------------------------------------------------------------
#if 0
TODO: untangle Label <-> Node
void Node::paint(ViewPainter *)
{ untested();
  switch(Connections.count()) {
    case 1:  if(hasLabel())
               p->fillRect(cx()-2, cy()-2, 4, 4, Qt::darkBlue); // open but labeled
             else { untested();
               p->Painter->setPen(QPen(Qt::red,1));  // node is open
               p->drawEllipse(cx()-4, cy()-4, 8, 8);
             }
             return;
    case 2:  if(Connections.getFirst()->Type == isWire)
               if(Connections.getLast()->Type == isWire) return;
             p->fillRect(cx()-2, cy()-2, 4, 4, Qt::darkBlue);
             break;
    default: p->Painter->setBrush(Qt::darkBlue);  // more than 2 connections
	     p->Painter->setPen(QPen(Qt::darkBlue,1));
	     p->drawEllipse(cx()-3, cy()-3, 6, 6);
	     p->Painter->setBrush(Qt::NoBrush);
             break;
  }
}
// ----------------------------------------------------------------
#endif
// ----------------------------------------------------------------

// BUG: does not set Name
// what is a "Name"??
void Node::setName(const QString& , const QString& , int, int )
{ untested();
  incomplete();
#if 0
  if(Name_.isEmpty() && Value_.isEmpty()) { untested();
    if(Label) delete Label;
    Label = 0;
    return;
  }

  if(!Label) Label = new WireLabel(Name_, cx(), cy(), x_, y_, isNodeLabel);
  else Label->setName(Name_);
  Label->pOwner = this;
  Label->initValue = Value_;
#endif
}

// ----------------------------------------------------------------
#if 0
QRectF Node::boundingRect() const
{ untested();
  QRectF b(cx()-4,cy()-4,8,8);
  return b;
}
#endif
// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------
// vim:ts=8:sw=2:et
