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
#include "wire.h"
#include "schematic_scene.h"
#include <QGraphicsScene>
#include "element_graphics.h"
#include "io.h"
#include "qt_compat.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
ElementGraphics::ElementGraphics() : QGraphicsItem()
{
	unreachable();
}
/*--------------------------------------------------------------------------*/
ElementGraphics::ElementGraphics(ElementGraphics const& e) 
	: QGraphicsItem(), _e(nullptr)
{ untested();
	assert(e._e);
	Element* el = e._e->clone();
	assert(el);
	attachElement(el);
}
/*--------------------------------------------------------------------------*/
ElementGraphics::ElementGraphics(Element* e)
	: QGraphicsItem(), _e(nullptr)
{
	assert(e);
	attachElement(e);
}
/*--------------------------------------------------------------------------*/
ElementGraphics* ElementGraphics::clone() const
{
	return new ElementGraphics(*this);
}
/*--------------------------------------------------------------------------*/
ElementGraphics::~ElementGraphics()
{
	if(isVisible()){itested();
		// element is owned by SchematicModel.
	}else{ untested();
		// check: is this correct?
		delete(_e);
	}
}
/*--------------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------*/
// not here.
inline int dsin(int angle)
{
	int d = angle%2;
	d *= 1-2*(angle/2);
	return d;
}
inline int dcos(int angle)
{
	return dsin(angle-1);
}
inline std::pair<int, int> angle_t::apply(std::pair<int, int> const& p) const
{
	assert(! (_degrees%90) ); //for now
	int a = _degrees/90;
	int s = dsin(a);
	int c = dcos(a);

	int rx =  c*p.first - s*p.second;
	int ry =  s*p.first + c*p.second;
	return std::make_pair(rx, ry);
}
/*--------------------------------------------------------------------------*/
// rotate around center in local coordinates.
void ElementGraphics::rotate(angle_t a, std::pair<int, int> center)
{
	assert(_e);
	if(auto s=dynamic_cast<Wire*>(_e)){ untested();
		trace0("start wire rotate");
		hide();
		s->rotate(); // does not work
		show();
		trace0("done wire rotate");
	}else if(auto* s=dynamic_cast<Symbol*>(_e)){ untested();
		bool sel = isSelected();
		hide();
		std::string rs=s->getParameter("rotated");
		unsigned r = atoi(rs.c_str());
		assert(r<4); // yikes //
		r += a.degrees_int()/90;
		r%=4;
		s->setParameter("rotated", std::to_string(r));

		// is this needed? QGraphicsItem::pos should be the same
		std::string x_ = s->getParameter("$xposition");
		std::string y_ = s->getParameter("$yposition");

		int x = atoi(x_.c_str());
		int y = atoi(y_.c_str());

		// c = (x,y);
		x -= center.first;
		y -= center.second;

		std::pair<int, int> ac = a.apply(center);

		x += ac.first;
		y += ac.second;

		s->setParameter("$xposition", std::to_string(x));
		s->setParameter("$yposition", std::to_string(y));
		show();
		setSelected(sel);
	}else{
	}
}
/*--------------------------------------------------------------------------*/
QRectF ElementGraphics::boundingRect() const
{itested();
	assert(_e);
	return _e->boundingRect();
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::setSelected(bool s)
{
	qDebug() << "setSeletected" << s << this;
	QGraphicsItem::setSelected(s);
	assert(QGraphicsItem::isSelected()==s);
	assert(_e);
//	_e->setSelected(s); // BUG
}
/*--------------------------------------------------------------------------*/
// Reimplement this function to intercept events before they are dispatched to
// the specialized event handlers. should return true if the event e was
// recognized and processed.
bool ElementGraphics::sceneEvent(QEvent* e)
{
	if(e->type() == QEvent::WindowActivate){
	}else if(e->type() == QEvent::WindowDeactivate){
	}else if(!e->isAccepted()){
		// possibly set in filter?
	}else{
		unreachable();
		// strange.
		trace1("ElementGraphics::sceneEvent already accepted", e->type());
	}
	assert(scene());

	auto sc = scene();
	auto s = dynamic_cast<SchematicScene*>(sc);
	assert(s);

	ItemEvent ie(*e, *this);
	if(s->itemEvent(&ie)){ untested();
		bool acc = e->isAccepted();
		trace1("ElementGraphics::sceneEvent itemEvent", acc);
		if(acc){ itested();
		}else{
			// what's the difference between "acc" and the condition above?!
			unreachable();
		}
		return acc;
	}else if(QGraphicsItem::sceneEvent(e)){ itested();
		trace1("ElementGraphics::sceneEvent fwd", e->type());
		return e->isAccepted();
	}else{ untested();
		return false;
		return e->isAccepted();
	}
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::show()
{
	if(_e->scope()){ untested();
		_e->attachToModel();
	}else{ untested();
	}
	QGraphicsItem::show();
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::hide()
{ untested();
	assert(_e);
	QGraphicsItem::hide();

	if(_e->scope()){
		_e->detachFromModel();
	}else{
	}
}
/*--------------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------*/
void ElementGraphics::setPos(int i, int j, bool relative)
{
	assert(!relative); // use move, for now.
	// prepareGeometryChange();
	QGraphicsItem::setPos(i, j);
	_e->setCenter(i, j, false);
}
/*--------------------------------------------------------------------------*/
template
void ElementGraphics::moveElement<QPoint>(QPoint const& delta);
/*--------------------------------------------------------------------------*/
ItemEvent::ItemEvent(QEvent const& a, ElementGraphics& b)
	: QEvent(a), _item(b)
{itested();
}
/*--------------------------------------------------------------------------*/
