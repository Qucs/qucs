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
#include <QGraphicsItem>
#include "element.h"
#include "geometry.h"
/*--------------------------------------------------------------------------*/
class SchematicScene;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// A graphics element on the screen.
// sort of proxy for Element
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
	Element* cloneElement() const;
	ElementGraphics* newUnion(ElementGraphics const*) const;
//	ElementGraphics* newPort(pos_t) const; obsolete?

public:
	SchematicScene* scene() const; // really?

public: // QGraphicsItem
  void update();
  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*) override;

  QRectF absoluteBoundingRect() const;

private: // QGraphicsItem
  bool sceneEvent(QEvent*) override;
  QVariant itemChange(GraphicsItemChange c, const QVariant &v) override;

public: // manipulate (used in UndotaskElements)
  void hide();
  void show();

  template<class P>
  void moveElement(P const& delta);

public: // schematic_action.cpp
  void transform(rotate_after_mirror1_t, std::pair<int, int> pivot=std::make_pair(0,0));
  // void snap();

public:
  Element& operator*(){ itested();
	  assert(_e); return *_e;
  }
  Element* operator->(){ itested();
	  assert(_e); return _e;
  }
  Element const* operator->() const{itested();
	  assert(_e); return _e;
  }
//  QPointF pos() const; // QGraphicsItem
  void setPos(int i, int j, bool relative=false);
  void setPos(QPoint const&);
//  void center(int& i, int& j);

	// BUG
	void prepareGeometryChange(){
		return QGraphicsItem::prepareGeometryChange();
	}

	// BUG: selected is stored in Element.
	void setSelected(bool s);

private:
	Element* _e;
	QGraphicsItem* _elementText;
	bool _select__;
}; // ElementGraphics
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// Component const* const_component(ElementGraphics const);
WireLabel const* const_wireLabel(ElementGraphics const);
Diagram const* const_diagram(ElementGraphics const);
Painting const* const_painting(ElementGraphics const);

Element* element(ElementGraphics*);
Element const* element(ElementGraphics const*);

// obsolete stuff (historic attempt to rescue legacy code)
// Component* component(QGraphicsItem*);
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
