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
#include "components/component.h" // BUG
void ElementGraphics::attachElement(Element* e)
{itested();
	assert(e);
	_e = e;
	// BUG: ask element?
	setFlags(ItemIsSelectable|ItemIsMovable|ItemSendsGeometryChanges);
	setAcceptHoverEvents(true);

	auto sp = _e->center();
	prepareGeometryChange();
	trace3("attachElement", e->label(), sp.first, sp.second);
	QGraphicsItem::setPos(sp.first, sp.second);

	if(auto c=dynamic_cast<Component*>(e)){ untested();
		trace2("attachElement", e->label(), c->Texts.size());
		for(auto& i : c->Texts){ untested();
			//auto t=
			new TextGraphics(*i, this);

		}
	}

	if(e->showLabel()){
		auto t=new QGraphicsTextItem(this);
		t->setPlainText(e->label());
	}else{
	}
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
#ifdef DO_TRACE
		painter->setPen(QPen(Qt::yellow,3));
		painter->drawRoundRect(br);
#endif
	}

	_e->paint(&v);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// transform around pivot (in global coordinates).
void ElementGraphics::transform(qucsSymbolTransform a, std::pair<int, int> pivot)
{itested();
	trace1("..", a.degrees_int());
	assert(!(a.degrees_int()%90));
	assert(_e);
	bool sel = isSelected();
	if(auto* s=dynamic_cast<Symbol*>(_e)){ untested();
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
			incomplete();
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
			assert(r<360);
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
		trace2("transform", vflip, new_mr.degrees_int());

		// BUG. vflip last.
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

		trace2("posttransform setpos", x ,y);
		setPos(x, y);
	// prepareGeometryChange(); // needed??
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
		// unreachable();
		// strange. getting here when shutting down?
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
	hide();
	assert(_e);
	int dx = getX(delta);
	int dy = getY(delta);
	trace3("moveElement", _e->label(), dx, dy);
	
	prepareGeometryChange(); // needed??
	_e->setCenter(dx, dy, true);
	auto p = _e->center();

	// possibly redundant
	QGraphicsItem::setPos(p.first, p.second);
	show();
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
#include <QApplication> // BUG
QVariant ElementGraphics::itemChange(GraphicsItemChange c, const QVariant &v)
{
    if (!scene()){ untested();
	 }else if(c == ItemPositionChange){ itested();
        QPointF tmp = v.toPointF();
        if(QApplication::mouseButtons() != Qt::LeftButton){ untested();
            return tmp;
		  }else if(auto scn = dynamic_cast<SchematicScene*> (scene())){ itested();
            return scn->snapToGrid(tmp.toPoint()); // does toPoint round as intended?
        }else{ untested();
            return tmp;
		  }
    }else{ itested();
	 }
	 return QGraphicsItem::itemChange(c, v);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
