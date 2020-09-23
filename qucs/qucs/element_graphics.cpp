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
{ untested();
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
{itested();
	assert(e);
	attachElement(e);
}
/*--------------------------------------------------------------------------*/
ElementGraphics* ElementGraphics::clone() const
{ untested();
	return new ElementGraphics(*this);
}
/*--------------------------------------------------------------------------*/
ElementGraphics::~ElementGraphics()
{itested();
	if(isVisible()){itested();
		// element is owned by SchematicModel.
	}else{ untested();
		// check: is this correct?
		delete(_e);
	}
}
/*--------------------------------------------------------------------------*/
// there is also QGraphicsSimpleTextItem, but is does not allow for edits.
class TextGraphics : public QGraphicsTextItem{
public:
	explicit TextGraphics(Text& t, QGraphicsItem* parent) : QGraphicsTextItem(parent), _t(t){
		setPlainText(t.s);
		trace1("TextGraphics", t.s);
		setParentItem(parent);
	}
public:
private:
	Text& _t;
};
/*--------------------------------------------------------------------------*/
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

	if(auto c=dynamic_cast<Component*>(e)){ untested();
		trace2("attachElement", e->label(), c->Texts.size());
		for(auto& i : c->Texts){ untested();
			//auto t=
			new TextGraphics(*i, this);

		}
	}
	auto t=new QGraphicsTextItem(this);
	t->setPlainText(e->label());
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{itested();
	assert(_e);
	assert(painter);
	ViewPainter v(painter);

	auto br = boundingRect();

	if(isSelected()){itested();
		// painter->fillRect(br, QColor("grey"));
		painter->setPen(QPen(Qt::darkGray,3));
		painter->drawRoundRect(br);
	}else{itested();
		// debug.
		// painter->fillRect(br, QColor("white"));
		painter->setPen(QPen(Qt::yellow,3));
		painter->drawRoundRect(br);
	}

	_e->paint(&v);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// rotate around pivot (in global coordinates). obsolete.
void ElementGraphics::rotate(angle_t a, std::pair<int, int> pivot)
{ untested();
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
		std::string rs = s->getParameter("$angle");
		int r = atoi(rs.c_str());
		assert(!(r%90));
		assert(!(a.degrees_int()%90));
		r += a.degrees_int();
		assert(!(r%90));
		r += 360;
		r %= 360;
		s->setParameter("$angle", std::to_string(r));

		auto p = pos();
		int x = getX(p.toPoint());
		int y = getY(p.toPoint());

#ifndef NDEBUG
		std::string x_ = s->getParameter("$xposition");
		std::string y_ = s->getParameter("$yposition");
		assert(x == atoi(x_.c_str()));
		assert(y == atoi(y_.c_str()));
#endif

		// c = (x,y);
		trace4("DBG", x,y,pivot.first,pivot.second);
		x -= pivot.first;
		y -= pivot.second;

		auto new_xy = std::make_pair(x,y);
		trace1("DBG", new_xy);
		new_xy = a.apply(new_xy);
		trace1("DBG post", new_xy);

		x = pivot.first + new_xy.first;
		y = pivot.second + new_xy.second;

		setPos(x, y);
		show();
		setSelected(sel);
	}else{ untested();
	}
}
/*--------------------------------------------------------------------------*/
// transform around pivot (in global coordinates).
void ElementGraphics::transform(qucsSymbolTransform a, std::pair<int, int> pivot)
{itested();
	trace1("..", a.degrees_int());
	assert(!(a.degrees_int()%90));
	assert(_e);
	bool sel = isSelected();
	if(auto s=dynamic_cast<Wire*>(_e)){ untested();
		incomplete();
	}else if(auto* s=dynamic_cast<Symbol*>(_e)){itested();
		hide();
		int mx = 0;
		int my = 0;
		unsigned r = 0;
		try {itested();
			std::string mxs = s->getParameter("$hflip"); // indicates if x axis is mirrored
			mx = atoi(mxs.c_str()); // \pm 1
			trace3("hflip", mx, my, r);
			assert(mx == 1);
			mx -= 1;
			mx /= -2;
		}catch(ExceptionCantFind const&){ untested();
		}
		try {itested();
			std::string mys = s->getParameter("$vflip"); // indicates if y axis is mirrored
			my = atoi(mys.c_str());
			my -= 1;
			my /= -2;
		}catch(ExceptionCantFind const&){ untested();
		}
		try {itested();
			std::string rs = s->getParameter("$angle");
			r = atoi(rs.c_str());
			assert(!(r%90));
			r /= 90;
		}catch(ExceptionCantFind const&){ untested();
		}

		trace3("stuff", mx, my, r);

		assert(mx==0 || mx==1);
		assert(my==0 || my==1);
		assert(r < 4); // yikes //

		assert(!mx); // for now.

		rotate_after_mirror1_t current(int(r*90), bool(my));
		assert(!(current.degrees_int()%90));
		rotate_after_mirror1_t new_mr = a * current;
		assert(!(new_mr.degrees_int()%90));

		auto vflip = -2 * int(new_mr.mirror()) + 1;
		trace2("transform", vflip, new_mr.mirror());

		s->setParameter(std::string("$hflip"), std::string("1"));
		s->setParameter(std::string("$vflip"), std::to_string(vflip));
		s->setParameter(std::string("$angle"), std::to_string(new_mr.degrees_int()));

		auto p = pos();
		int x = getX(p.toPoint());
		int y = getY(p.toPoint());

		x -= pivot.first;
		y -= pivot.second;

		auto new_xy = std::make_pair(x,y);
		new_xy = a.apply(new_xy);

		x = pivot.first + new_xy.first;
		y = pivot.second + new_xy.second;

		setPos(x, y);
		show();
	}else{ untested();
	}
	setSelected(sel);
}
/*--------------------------------------------------------------------------*/
QRectF ElementGraphics::boundingRect() const
{itested();
	assert(_e);
	return _e->boundingRect();
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::setSelected(bool s)
{itested();
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
{itested();
	if(e->type() == QEvent::WindowActivate){itested();
	}else if(e->type() == QEvent::WindowDeactivate){itested();
	}else if(!e->isAccepted()){itested();
		// possibly set in filter?
	}else{itested();
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
		}else{ untested();
			// what's the difference between "acc" and the condition above?!
			unreachable();
		}
		return acc;
	}else if(QGraphicsItem::sceneEvent(e)){itested();
		trace1("ElementGraphics::sceneEvent fwd", e->type());
		return e->isAccepted();
	}else{itested();
		return false;
		return e->isAccepted();
	}
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::show()
{itested();
	if(_e->scope()){itested();
		_e->attachToModel();
	}else{ untested();
	}
	QGraphicsItem::show();
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::hide()
{itested();
	assert(_e);
	QGraphicsItem::hide();

	if(_e->scope()){itested();
		_e->detachFromModel();
	}else{ untested();
	}
}
/*--------------------------------------------------------------------------*/
template<class P>
void ElementGraphics::moveElement(P const& delta)
{ untested();
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
{itested();
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
