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
{ untested();
	trace1("ElementGraphics::sceneEvent", e->type());
	assert(scene());

	auto sc = scene();
	auto s = dynamic_cast<SchematicScene*>(sc);
	assert(s);

	//186
	//156 <= item press
	//187
	
	ItemEvent ie(*e, *this);

	return s->itemEvent(&ie)
		|| QGraphicsItem::sceneEvent(e);
}

void ElementGraphics::setPos(int i, int j, bool relative)
{ untested();

	assert(false); // obsolete
#if 0
  assert(_e);
  auto p = _e->center();
  trace1("setPos pre", p);
  _e->setPos(i, j, relative);

  p = _e->getCenter();
  trace1("setPos post", p);

  QGraphicsItem::setPos(p.first, p.second);
#endif
}
//	assert(_e);
//	qDebug() << "EG::setPos" << a << _e->cx_();
//	QGraphicsItem::setPos(QPointF(a, b));
//	qDebug() << "EG::setPos" << boundingRect();
//}

void ElementGraphics::show()
{
	_e->attachToModel();
	QGraphicsItem::show();
}
void ElementGraphics::hide()
{ untested();
	assert(_e);
	_e->detachFromModel();
	untested();
	QGraphicsItem::hide();
}

template<class P>
void ElementGraphics::moveElement(P const& delta)
{
	// TODO: delta might be not needed.
	auto gp = QGraphicsItem::pos();
	assert(_e);
	int dx = getX(delta);
	int dy = getY(delta);
	trace3("moveElement", gp, dx, dy);
	
	QGraphicsItem::setPos(0, 0); // avoid drawing?!
	_e->setCenter(dx, dy, true);
}

template
void ElementGraphics::moveElement<QPoint>(QPoint const& delta);

ItemEvent::ItemEvent(QEvent const& a, ElementGraphics& b)
	: QEvent(a), _item(b)
{ untested();
}

// inline?
ElementGraphics& ItemEvent::item()
{
	return _item;
}
