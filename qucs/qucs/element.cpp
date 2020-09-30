/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2018, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "element.h"
#include "schematic_doc.h"

Element::Element()
  : _cx(0), _cy(0), _owner(nullptr)
{
  Type = isDummyElement; // BUG
  x1 = y1 = x2 = y2 = 0; // really?
  setLabel(name());

  // BUG
  // Selected = false;
}

Element::Element(Element const& e)
 : Object(e),
   _cx(e._cx),
   _cy(e._cy),
   x1(e.x1), y1(e.y1), x2(e.x2), y2(e.y2), // BUG diagram & whatever.
   _owner(nullptr) // sic.
{
  setLabel(e.label());

  // BUG
  // Selected = false;
}

Element::~Element()
{
}

// pure?
void Element::setCenter(int, int, bool)
{
	assert(false);
	unreachable();
}

void Element::getCenter(int&x, int&y) const
{
	incomplete();
	x=_cx;
	y=_cy;
}

void /*really?*/ Element::editElement(QucsDoc*)
{
	incomplete();
}

// pure? maybe not. there could be non-paintable elements...
void Element::paint(ViewPainter* p) const
{
	// draw bounding box for debugging.
	//if(isSelected()){
	//	p->Painter->setPen(QPen(Qt::red,2));
	//}else
	{
		p->Painter->setPen(QPen(Qt::yellow,1));
	}
	p->Painter->drawRoundRect(boundingRect());
}

// does not work for nodes and diagrams
QRectF Element::boundingRect() const
{
	// QRectF b(cx+x1, cy+y1, x2-x1, y2-y1);
	QRectF b(x1, y1, x2-x1, y2-y1);
	return b;
}

void Element::attachToModel()
{
	assert(scope());
	scope()->attach(this);
}

void Element::detachFromModel()
{
	assert(scope());
	scope()->detach(this);
}

std::pair<int, int> Element::center()const
{
	incomplete();
	// incomplete. bug. wrong.
	std::pair<int, int> ret;
	getCenter( ret.first, ret.second);
	return ret;
}

// legacy stuff. pretend that Element points to an Element
#include "components/component.h"
#include "diagram.h"
#include "wire.h"
#include "wirelabel.h"
#include "command.h"

Component* component(Element* e){ return dynamic_cast<Component*>(e); }
CmdElement* command(Element* e){ return dynamic_cast<CmdElement*>(e); }
Wire* wire(Element* e){ return dynamic_cast<Wire*>(e); }
WireLabel* wireLabel(Element* e){ return dynamic_cast<WireLabel*>(e); }
Diagram* diagram(Element* e){ return dynamic_cast<Diagram*>(e); }
Painting* painting(Element* e){ return dynamic_cast<Painting*>(e); }
Marker* marker(Element* e){ return dynamic_cast<Marker*>(e); }
Graph* graph(Element* e){ return dynamic_cast<Graph*>(e); }
Node* node(Element* e){ return dynamic_cast<Node*>(e); }
//Label* label(Element* e){ return dynamic_cast<Label*>(e); }

Component const* component(Element const* e){ return dynamic_cast<Component const*>(e); }
Wire const* wire(Element const* e){ return dynamic_cast<Wire const*>(e); }
WireLabel const* wireLabel(Element const* e){ return dynamic_cast<WireLabel const*>(e); }
Diagram const* diagram(Element const* e){ return dynamic_cast<Diagram const*>(e); }
Painting const* painting(Element const* e){ return dynamic_cast<Painting const*>(e); }
