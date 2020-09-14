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
#include "element.h"
#include "symbol.h"
#include "schematic_scene.h"
#include <QGraphicsScene>
#include "element_graphics.h"
#include "io.h"
#include "qt_compat.h"

ElementGraphics::ElementGraphics() : QGraphicsItem()
{
	unreachable();
}

ElementGraphics::ElementGraphics(ElementGraphics const& e) 
	: QGraphicsItem(), _e(nullptr)
{ untested();
	assert(e._e);
	Element* el = e._e->clone();
	assert(el);
	attachElement(el);
}

ElementGraphics::ElementGraphics(Element* e)
	: QGraphicsItem(), _e(nullptr)
{
	assert(e);
	attachElement(e);
}

ElementGraphics* ElementGraphics::clone() const
{
	return new ElementGraphics(*this);
}

ElementGraphics::~ElementGraphics()
{
	if(isVisible()){itested();
		// element is owned by SchematicModel.
	}else{ untested();
		// check: is this correct?
		delete(_e);
	}
}

void ElementGraphics::attachElement(Element* e)
{itested();
	assert(e);
	_e = e;
	// BUG: ask element
	setFlags(ItemIsSelectable|ItemIsMovable);
	setAcceptHoverEvents(true);

	auto sp = _e->center();
	prepareGeometryChange();
	QGraphicsItem::setPos(sp.first, sp.second);
}

void ElementGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	assert(_e);
	assert(painter);
	ViewPainter v(painter);

	auto br = boundingRect();

	if(isSelected()){
		// painter->fillRect(br, QColor("grey"));
		painter->setPen(QPen(Qt::darkGray,3));
		painter->drawRoundRect(br);
	}else{
		// debug.
		// painter->fillRect(br, QColor("white"));
		painter->setPen(QPen(Qt::yellow,3));
		painter->drawRoundRect(br);
	}

	_e->paint(&v);
}

void ElementGraphics::rotate(angle_t a)
{
	assert(_e);
	if(Symbol* s=dynamic_cast<Symbol*>(_e)){ untested();
		std::string rs=s->getParameter("rotated");
		unsigned r = atoi(rs.c_str());
		assert(r<4); // yikes //
		r += a.degrees_int()/90;
		r%=4;
		s->setParameter("rotated", std::to_string(r));
	}else{
	}
//	update();
}

QRectF ElementGraphics::boundingRect() const
{itested();
	assert(_e);
	return _e->boundingRect();
}

void ElementGraphics::setSelected(bool s)
{
	qDebug() << "setSeletected" << s << this;
	QGraphicsItem::setSelected(s);
	assert(QGraphicsItem::isSelected()==s);
	assert(_e);
	_e->setSelected(s); // BUG
}

// Reimplement this function to intercept events before they are dispatched to the specialized event handlers
// should return true if the event e was recognized and processed.
bool ElementGraphics::sceneEvent(QEvent* e)
{
	trace1("ElementGraphics::sceneEvent", e->type());
	assert(scene());

	auto sc = scene();
	auto s = dynamic_cast<SchematicScene*>(sc);
	assert(s);

	ItemEvent ie(*e, *this);
	if(s->itemEvent(&ie)){ untested();
		return e->isAccepted();
	}else if(QGraphicsItem::sceneEvent(e)){itested();
		return e->isAccepted();
	}else{ untested();
		return false;
		return e->isAccepted();
	}
}

void ElementGraphics::show()
{ untested();
	if(_e->scope()){
		_e->attachToModel();
	}else{
	}
	QGraphicsItem::show();
}

void ElementGraphics::hide()
{ untested();
	assert(_e);
	QGraphicsItem::hide();

	if(_e->scope()){
		_e->detachFromModel();
	}else{
	}
}

template<class P>
void ElementGraphics::moveElement(P const& delta)
{
	assert(_e);
	int dx = getX(delta);
	int dy = getY(delta);
	trace3("moveElement", _e->label(), dx, dy);
	
	prepareGeometryChange(); // needed??
	_e->setCenter(dx, dy, true);
	auto p = _e->center();
	QGraphicsItem::setPos(p.first, p.second);
}
	
void ElementGraphics::setPos(int i, int j, bool relative)
{
	assert(!relative); // use move, for now.
	// prepareGeometryChange();
	QGraphicsItem::setPos(i, j);
	_e->setCenter(i, j, false);
}
	

template
void ElementGraphics::moveElement<QPoint>(QPoint const& delta);

ItemEvent::ItemEvent(QEvent const& a, ElementGraphics& b)
	: QEvent(a), _item(b)
{itested();
}
