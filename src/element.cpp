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
#include "schematic_model.h"
//#include "schematic_doc.h"

Element::Element() : _position(0, 0), _owner(nullptr)
{
  // Type = isDummyElement; // BUG
  x1 = y1 = 0; // x2 = y2 = 0; // really?
//  setLabel(name());
}

Element::Element(Element const& e)
 : Object(e),
   _position(e._position),
   x1(e.x1), y1(e.y1), // x2(e.x2), y2(e.y2), // BUG diagram & whatever.
   _owner(nullptr) // sic.
	//Name(e.Name) // yikes.
{
  setLabel(e.label());

  // BUG
  // Selected = false;
}

Element::~Element()
{
}

void Element::getCenter(int&x, int&y) const
{
	unreachable();
	x = _position.first;
	y = _position.second;
}

#if 0 // fixed?
// pure? maybe not. there could be non-paintable elements...
void Element::paint(ViewPainter* p) const
{
#ifndef NDEBUG
	p->setPen(QPen(Qt::yellow,1));
	p->drawRoundedRect(bounding_rect().toRectF());
#endif
}

// does not work for nodes and diagrams
rect_t Element::bounding_rect() const
{
	trace1("bogus BR call", label());
	unreachable(); // but from diagrams or so
	// QRectF b(cx+x1, cy+y1, x2-x1, y2-y1);
//	QRectF b(x1, y1, x2-x1, y2-y1);
//	return b;
//
	return rect_t(0,0,0,0);
}

void Element::attachToModel()
{itested();
	assert(false); // obsolete, hopefully
	trace1("attachToModel", label());
	assert(scope());
	scope()->push_back(this);
}
#endif

void Element::detachFromModel()
{
	assert(scope());
	scope()->detach(this);
}
/*--------------------------------------------------------------------------*/
SchematicModel* Element::scope()
{
	if(auto o=dynamic_cast<Symbol*>(owner())){
		if(o->subckt()){
			return o->subckt();
//		}else if(o->makes_own_scope()){ untested();
		}else{
			return o->scope();
			return nullptr;
		}
	}else{
		return nullptr;
	}
}
/*--------------------------------------------------------------------------*/
void Element::set_param_by_index(index_t i, std::string const&)
{
	throw qucs::ExceptionCantFind(label(), "parameter " + std::to_string(i), label());
}
/*--------------------------------------------------------------------------*/
void Element::set_param_by_name(std::string const& name, std::string const&)
{
	throw qucs::ExceptionCantFind("parameter " + name, label());
}
/*--------------------------------------------------------------------------*/
std::string Element::get_param_by_name(std::string const& name) const{
	throw qucs::ExceptionCantFind("parameter " + name, label());
}
/*--------------------------------------------------------------------------*/
pos_t /* const & */ Element::center()const
{
	return pos_t(cx(), cy());
}

// borrowed/modified from e_card.h
const Element* Element::find_looking_out(const std::string& name)const
{
	trace2("find_looking_out1", name, label());
	try {
		return find_in_parent_scope(name);
	}catch (qucs::ExceptionCantFind&) {
		if (auto o=dynamic_cast<Element const*>(owner())) {
			trace3("find_looking_out2", name, label(), o->label());
			return o->find_looking_out(name);
		// }else if (makes_own_scope()) {
		// 	// probably a subckt or "module"
		// 	CARD_LIST::const_iterator i = CARD_LIST::card_list.find_(name);
		// 	if (i != CARD_LIST::card_list.end()) {
		// 		return *i;
		// 	}else{
		// 		throw;
		// 	}
		}else{
			throw;
		}
	}
}
/*--------------------------------------------------------------------------*/
// borrowed/modified from e_card.h
const Element* Element::find_in_parent_scope(const std::string& name)const
{
  assert(name != "");
  auto const* p_scope = (scope()->parent()) ? scope()->parent() : scope();

  if(!p_scope){
	  // unreachable(); happens in some lib files
	  trace2("no scope", name, label());
	  throw qucs::ExceptionCantFind(label(), name);
  }else{
  }

  auto i = p_scope->find_(name);
  if (i == p_scope->end()) {
    throw qucs::ExceptionCantFind(label(), name);
  }else{
  }
  return *i;

}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

// legacy stuff. pretend that Element points to an Element
//#include "components/component.h"
#include "diagram.h"
#include "wirelabel.h"
#include "task_element.h"
#include "node.h"
#include "painting.h"

// Component* component(Element* e){ return dynamic_cast<Component*>(e); }
TaskElement* command(Element* e){ return dynamic_cast<TaskElement*>(e); }
// Wire* wire(Element* e){ return dynamic_cast<Wire*>(e); }
WireLabel* wireLabel(Element* e){ return dynamic_cast<WireLabel*>(e); }
Diagram* diagram(Element* e){ return dynamic_cast<Diagram*>(e); }
Painting* painting(Element* e){ return dynamic_cast<Painting*>(e); }
Marker* marker(Element* e){ return dynamic_cast<Marker*>(e); }
//Graph* graph(Element* e){ return dynamic_cast<Graph*>(e); }
Node* node(Element* e){ return dynamic_cast<Node*>(e); }
//Label* label(Element* e){ return dynamic_cast<Label*>(e); }

// Component const* component(Element const* e){ return dynamic_cast<Component const*>(e); }
// Wire const* wire(Element const* e){ return dynamic_cast<Wire const*>(e); }
WireLabel const* wireLabel(Element const* e){ return dynamic_cast<WireLabel const*>(e); }
Diagram const* diagram(Element const* e){ return dynamic_cast<Diagram const*>(e); }
Painting const* painting(Element const* e){ return dynamic_cast<Painting const*>(e); }
