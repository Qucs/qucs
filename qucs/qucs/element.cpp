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

#include "element.h"
#include "schematic.h"

Element::Element() :
	Selected(false)
{
  Type = isDummyElement;
  cx = cy = x1 = y1 = x2 = y2 = 0;
}

Element::~Element()
{
}

void Element::paintScheme(Schematic *)
{
}

void Element::paintScheme(QPainter *)
{
}

void Element::setCenter(int, int, bool)
{
}

void Element::getCenter(int&, int&)
{
}

void Element::snapToGrid(Schematic& s){
    s.setOnGrid(cx, cy);
}

// pure? maybe not. there could be non-paintable elements...
void Element::paint(ViewPainter*){
	unreachable();
}

QRectF Element::boundingRect() const
{
	return QRectF(x1, y1, x2-x1, y2-y1);

//node:
 // return QRect(cx-4,cy-4,8,8);

}


// legacy stuff. pretend that Element points to an Element
#include "components/component.h"
#include "diagrams/diagram.h"
#include "wire.h"
#include "wirelabel.h"

Component* component(Element* e){ return dynamic_cast<Component*>(e); }
Wire* wire(Element* e){ return dynamic_cast<Wire*>(e); }
WireLabel* wireLabel(Element* e){ return dynamic_cast<WireLabel*>(e); }
Diagram* diagram(Element* e){ return dynamic_cast<Diagram*>(e); }
Painting* painting(Element* e){ return dynamic_cast<Painting*>(e); }
Marker* marker(Element* e){ return dynamic_cast<Marker*>(e); }
Graph* graph(Element* e){ return dynamic_cast<Graph*>(e); }
Node* node(Element* e){ return dynamic_cast<Node*>(e); }

Component const* component(Element const* e){ return dynamic_cast<Component const*>(e); }
Wire const* wire(Element const* e){ return dynamic_cast<Wire const*>(e); }
WireLabel const* wireLabel(Element const* e){ return dynamic_cast<WireLabel const*>(e); }
Diagram const* diagram(Element const* e){ return dynamic_cast<Diagram const*>(e); }
Painting const* painting(Element const* e){ return dynamic_cast<Painting const*>(e); }

