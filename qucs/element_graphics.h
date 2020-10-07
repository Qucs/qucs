/* Copyright (C) 2018, 2020 Felix Salfelder
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
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

#ifndef QUCS_ELEMENT_GFX_H
#define QUCS_ELEMENT_GFX_H

#include <QEvent>
#include "geometry.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// A graphics element on the screen.
// kind of smart-pointer/proxy.
class ElementGraphics : public QGraphicsItem {
private:
	ElementGraphics();
	ElementGraphics(ElementGraphics const&);

public:
	explicit ElementGraphics(Element* e);
	~ElementGraphics();

public:
	ElementGraphics* clone() const;
	bool operator!=(Element const* e) const{
		return _e!=e;
	}
	void attachElement(Element* e);
	Element* detachElement();
	ElementGraphics* newUnion(ElementGraphics const*) const;
public:
	SchematicScene* scene(); // really?



public: //?
//  void paintScheme(SchematicDoc *p);
private:
  void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override; // const...
  QRectF boundingRect() const override;
  bool sceneEvent(QEvent*) override;
  QVariant itemChange(GraphicsItemChange c, const QVariant &v) override;

public: // manipulate (used in UndoCommands)
  void hide();
  void show();

  template<class P>
  void moveElement(P const& delta);
  void transform(rotate_after_mirror1_t, std::pair<int, int> pivot=std::make_pair(0,0));

  // void snap();

public:
  Element& operator*(){ itested();
	  assert(_e); return *_e;
  }
  Element* operator->(){ itested();
	  assert(_e); return _e;
  }
  Element const* operator->() const{ untested();
	  assert(_e); return _e;
  }
//  QPointF pos() const; // QGraphicsItem
  void setPos(int i, int j, bool relative=false);
  void setPos(QPoint const&);
  void center(int& i, int& j);

	// BUG
	void prepareGeometryChange(){
		return QGraphicsItem::prepareGeometryChange();
	}

	// BUG: selected is stored in Element.
	void setSelected(bool s);

	int const& cx_() const { assert(_e); return _e->cx(); }
	int const& cy_() const { assert(_e); return _e->cy(); }
	int const& x1_() const { assert(_e); return _e->x1_(); }
	int const& y1_() const { assert(_e); return _e->y1_(); }
	int const& x2_() const { assert(_e); return _e->x2_(); }
	int const& y2_() const { assert(_e); return _e->y2_(); }
private:
  Element* _e;
}; // ElementGraphics
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
Component const* const_component(ElementGraphics const);
WireLabel const* const_wireLabel(ElementGraphics const);
Diagram const* const_diagram(ElementGraphics const);
Painting const* const_painting(ElementGraphics const);

Element* element(ElementGraphics*);
Element const* element(ElementGraphics const*);

// obsolete stuff (historic attempt to rescue legacy code)
Component* component(QGraphicsItem*);
WireLabel* wireLabel(QGraphicsItem*);
Diagram* diagram(QGraphicsItem*);
Painting* painting(QGraphicsItem*);
Graph* graph(QGraphicsItem*);
Marker* marker(QGraphicsItem*);
Node* node(QGraphicsItem*);
// Label* label(QGraphicsItem*);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#include <QEvent> // TODO: check if ItemEvent is needed at all, if so,
                  // move to a different place
/*--------------------------------------------------------------------------*/
class ItemEvent: public QEvent{
public:
	explicit ItemEvent(QEvent const& e, ElementGraphics& item);
public:
	ElementGraphics& item() { return _item; }
private:
	ItemEvent(ItemEvent const& e) = delete;
	ElementGraphics& _item;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
