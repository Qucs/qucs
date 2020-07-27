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
#include "schematic_scene.h"
#include <QGraphicsScene>
#include "element_graphics.h"
#include "io.h"
#include "qt_compat.h"

ElementGraphics::ElementGraphics() : QGraphicsItem()
{
	unreachable();
}

ElementGraphics::ElementGraphics(Element* e)
	: QGraphicsItem(), _e(e)
{
	setFlags(ItemIsSelectable|ItemIsMovable);
	setAcceptHoverEvents(true);
	assert(_e);
	auto p = _e->center();
	QGraphicsItem::setPos(p.first, p.second);
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
		painter->fillRect(br, QColor("white"));
	}

	_e->paint(&v);
}

QRectF ElementGraphics::boundingRect() const
{ itested();
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
		return true;
	}else if(QGraphicsItem::sceneEvent(e)){ untested();
		return true;
	}else{ untested();
		return false;
	}
}

void ElementGraphics::show()
{ untested();
	_e->attachToModel();
	QGraphicsItem::show();
}

void ElementGraphics::hide()
{ untested();
	assert(_e);
	QGraphicsItem::hide();
	_e->detachFromModel();
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
	
// void ElementGraphics::setPos(int i, int j, bool relative=false)
// {
// 	_e->setCenter(dx, dy, true);
// }
	

template
void ElementGraphics::moveElement<QPoint>(QPoint const& delta);

ItemEvent::ItemEvent(QEvent const& a, ElementGraphics& b)
	: QEvent(a), _item(b)
{ untested();
}
