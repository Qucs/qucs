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

class angle_t{
public:
	angle_t(int d):_degrees(d){ }

public:
	angle_t operator-() const{
		return angle_t(-_degrees);
	}
public:
	int degrees_int() const{return _degrees;}

private:
	int _degrees;
};
static const angle_t ninety_degree(90);

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
private:
	void attachElement(Element* e);

public: //?
  void paintScheme(SchematicDoc *p);
private: // later: Qgraphics virtual overrides
//  void paint() { assert(_e); _e->paint(); }
//  void paintScheme(SchematicDoc *s) { assert(_e); _e->paintScheme(s); }
  QRectF boundingRect() const;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*); // const...

public:
  bool sceneEvent(QEvent*) override;

public: // manipulate (used in UndoCommands)
  void hide();
  void show();
  void rotate(angle_t);
  void mirrorX();
  void mirrorY();
  void snap();

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
  template<class P>
  void moveElement(P const& delta);
  void center(int& i, int& j);
	void toggleSelected(){
		assert(_e);
		setSelected(!isSelected());
	}

	// BUG
	void prepareGeometryChange(){
		return QGraphicsItem::prepareGeometryChange();
	}

	// BUG: selected is stored in Element.
	void setSelected(bool s);

	int const& cx_() const { assert(_e); return _e->cx_(); }
	int const& cy_() const { assert(_e); return _e->cy_(); }
	int const& x1_() const { assert(_e); return _e->x1_(); }
	int const& y1_() const { assert(_e); return _e->y1_(); }
	int const& x2_() const { assert(_e); return _e->x2_(); }
	int const& y2_() const { assert(_e); return _e->y2_(); }
private:
  Element* _e;
}; // ElementGraphics

Component const* const_component(ElementGraphics const);
Wire const* const_wire(ElementGraphics const);
WireLabel const* const_wireLabel(ElementGraphics const);
Diagram const* const_diagram(ElementGraphics const);
Painting const* const_painting(ElementGraphics const);

Element* element(QGraphicsItem*);
Component* component(QGraphicsItem*);
Wire* wire(QGraphicsItem*);
WireLabel* wireLabel(QGraphicsItem*);
Diagram* diagram(QGraphicsItem*);
Painting* painting(QGraphicsItem*);
Graph* graph(QGraphicsItem*);
Marker* marker(QGraphicsItem*);
Node* node(QGraphicsItem*);
// Label* label(QGraphicsItem*);

#include <QEvent> // TODO: check if ItemEvent is needed at all, if so,
                  // move to a different place

class ItemEvent: public QEvent{
public:
	explicit ItemEvent(QEvent const& e, ElementGraphics& item);
public:
	ElementGraphics& item() { return _item; }
private:
	ItemEvent(ItemEvent const& e) = delete;
	ElementGraphics& _item;
};

#endif
