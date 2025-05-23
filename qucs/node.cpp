/***************************************************************************
                          node.cpp  -  description
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
#include "node.h"

#include "viewpainter.h"
#include "wirelabel.h"

#include <QPainter>

Node::Node(int _x, int _y)
{
  Label = 0;
  Type  = isNode;
  State = 0;
  DType = "";

  set_cx(_x);
  set_cy(_y);
}

Node::~Node()
{
	if(refcount()){
	}else{
		// historical bug?
		// missing destructors?
		// start debugging here.

		// incomplete();
	}
}

// -------------------------------------------------------------
void Node::paint(ViewPainter *p)
{
  switch(refcount()) {
    case 1:  if(Label)
               p->fillRect(cx()-2, cy()-2, 4, 4, Qt::darkBlue); // open but labeled
             else {
               p->Painter->setPen(QPen(Qt::red,1));  // node is open
               p->drawEllipse(cx()-4, cy()-4, 8, 8);
             }
             return;
    case 2:  if(is_wire(connections().front())){
               if(is_wire(connections().back())) {
						return;
					}else{
					}
				 }else{
				 }
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
bool Node::getSelected(int x_, int y_)
{
  if(cx()-5 <= x_) if(cx()+5 >= x_) if(cy()-5 <= y_) if(cy()+5 >= y_)
    return true;

  return false;
}

// ----------------------------------------------------------------
void Node::setName(const QString& Name_, const QString& Value_, int x_, int y_)
{
  if(Name_.isEmpty() && Value_.isEmpty()) {
    Label = 0;
    return;
  }

  if(!Label) Label.reset(new WireLabel(Name_, cx(), cy(), x_, y_, isNodeLabel));
  else Label->setName(Name_);
  Label->pOwner = this;
  Label->initValue = Value_;
}

// ----------------------------------------------------------------
void Node::removeConnection(const std::shared_ptr<Element> &e)
{
  for (auto i = connections().begin(); i != connections().end(); ++i) {
    if (i->lock() == e) {
      mutable_conn().erase (i);
      break;
    }
  }
}

// ----------------------------------------------------------------
void
Node::appendConnection(const std::shared_ptr<Element> &e)
{
  mutable_conn().push_back(e);
}

// ----------------------------------------------------------------
std::string Node::label()const
{
  return "n_"
    +std::string((cx()<0)?"m"+std::to_string(-cx()):std::to_string(cx()))
    +"_"
    +std::string((cy()<0)?"m"+std::to_string(-cy()):std::to_string(cy()));
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
