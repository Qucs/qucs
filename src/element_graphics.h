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
namespace qucs {
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

private:
	void attachElement(Element* e);
	Element* detachElement();

public:
	Element* cloneElement() const;
	ElementGraphics* newUnion(ElementGraphics const*) const;

private:
	bool has_port_values() const{ return _port_values.size(); }
//	ElementGraphics* newPort(pos_t) const; obsolete?

public:
	SchematicScene* scene() const; // really?

private:
	qucs::ElementList* model();

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
	void show_();

private:
	void restore_ports();
	void init_ports();

public: // schematic_action.cpp
	void transform(rotate_after_mirror1_t, std::pair<int, int> pivot=std::make_pair(0,0));
	// void snap();

public:
	Element const& operator*() const{ itested();
		assert(_e); return *_e;
	}
	Element& operator*(){itested();
		assert(_e); return *_e;
	}
	//  QPointF pos() const; // QGraphicsItem
	void setPos(int i, int j, bool relative=false);
	void setPos(QPoint const&);
	//  void center(int& i, int& j);

	// BUG
	void prepareGeometryChange(){
		return QGraphicsItem::prepareGeometryChange();
	}

	void setSelected(bool s);

private:
	Element* _e;
	QGraphicsItem* _elementText;
	bool _selected;
	std::vector<std::string> _port_values;
}; // ElementGraphics
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
Element* element(ElementGraphics*);
Element const* element(ElementGraphics const*);
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
#include <QEvent> // TODO: check if ItemEvent is needed at all, if so,
// move to a different place
namespace qucs {
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
} // qucs
/*--------------------------------------------------------------------------*/
#endif
