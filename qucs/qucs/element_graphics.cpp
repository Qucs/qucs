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
#include "schematic_model.h"
#include <QGraphicsScene>
#include "element_graphics.h"
#include "io.h"
#include "platform.h"
#include "place.h"
#include "qt_compat.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
ElementGraphics::ElementGraphics() : QGraphicsItem()
{ untested();
	assert(false);
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
class ElementText : public QGraphicsItem{
private:
	ElementText(ElementText const&) = delete;
public:
	explicit ElementText(ElementGraphics* parent)
	  : QGraphicsItem(parent), _labeltext(nullptr) {
		Element const* e = element(parent);
		setParentItem(parent);

		int k=0;
		if(e->showLabel()){
			_labeltext = new QGraphicsTextItem(this);
			_labeltext->setPlainText(e->label());
			k += _labeltext->boundingRect().height();
		}else{
		}
		if(auto s=dynamic_cast<Symbol const*>(e)){
			int show = atoi(s->paramValue("$param_display").c_str());
			for(unsigned i=0; i<s->paramCount(); ++i){
				auto n = QString::fromStdString(s->paramName(i));
				if(n.at(0)=='$'){
				}else if(show % 2){
					auto t=new QGraphicsTextItem(this);
					auto v = QString::fromStdString(s->paramValue(i));
					t->setPlainText(n+"="+v);
					t->setPos(0, k/2);
					k += t->boundingRect().height();
				}else{
				}
				show/=2;
			}
		}else{
		}
	}
	~ElementText(){
		// delete stuff?
	}
private:
	virtual QRectF boundingRect() const override{
		return QRectF();
	}
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override{}
public:
private:
	QGraphicsTextItem* _labeltext;
	std::vector<QGraphicsTextItem*> _more;
};
/*--------------------------------------------------------------------------*/
class TextGraphics : public QGraphicsTextItem{
public:
	explicit TextGraphics(Text& t, QGraphicsItem* parent)
	  : QGraphicsTextItem(parent), _t(t){
		setPlainText(t.s);
		trace1("TextGraphics", t.s);
		setParentItem(parent);
	}
public:
private:
	Text& _t;
};
/*--------------------------------------------------------------------------*/
inline std::ostream& operator<<(std::ostream& o, QRectF const& r)
{
	o << r.topLeft() << ":" << r.bottomRight();
	return o;
}
/*--------------------------------------------------------------------------*/
// really?
Element* ElementGraphics::detachElement()
{
	Element* tmp = _e;
	_e = nullptr;
	return tmp;
}
/*--------------------------------------------------------------------------*/
Element* ElementGraphics::cloneElement() const
{
	if(_e){ untested();
		return _e->clone();
	}else{ untested();
		return nullptr;
	}
}
/*--------------------------------------------------------------------------*/
#include "components/component.h" // BUG
#include <QGraphicsProxyWidget>
void ElementGraphics::attachElement(Element* e)
{itested();
	QGraphicsItem::hide();
	assert(e);
	_e = e;
	// BUG: ask element?
	setFlags(ItemIsSelectable|ItemIsMovable|ItemSendsGeometryChanges);
	setAcceptHoverEvents(true);

	auto sp = _e->center();
	prepareGeometryChange();
	trace3("attachElement", e->label(), sp.first, sp.second);
	trace2("attachElement", e->label(), boundingRect());
	QGraphicsItem::setPos(sp.first, sp.second);

	if(dynamic_cast<Conductor*>(e)){ untested();
		setZValue(-1.);
	}else{ itested();
	}

	if(auto c=dynamic_cast<Component*>(e)){itested();
		trace2("attachElement", e->label(), c->Texts.size());
		for(auto& i : c->Texts){itested();
			//auto t=
			new TextGraphics(*i, this);

		}
	}else{ untested();
	}

	// who owns this?
	auto t = new ElementText(this);

	auto sym = dynamic_cast<Symbol const*>(_e);

	if (auto w=_e->newWidget()){
		auto p = new QGraphicsProxyWidget(this);
		p->setWidget(w);
	}else if(!sym){
	}else if(auto s = sym->subckt()){
		trace0("child gfx");
		for(auto i : s->components()){itested();
			trace0("child comp clone");
			QGraphicsItem* cg = new ElementGraphics(i->clone());
			cg->setParentItem(this);
		}

		// BUG
		for(auto i : s->wires()){itested();
			trace0("child gfx clone");
			QGraphicsItem* cg = new ElementGraphics(i->clone());
			cg->setParentItem(this);
		}
	}else{
	}
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::paint(QPainter *p, const QStyleOptionGraphicsItem *o,
		QWidget* w)
{itested();
	assert(_e);
	ViewPainter v(p); // TODO
	_e->paint(&v);
	assert(p);

	auto br = boundingRect();

	if(isSelected()){itested();
		// p->fillRect(br, QColor("grey"));
		p->setPen(QPen(Qt::darkGray,3));
		p->drawRoundedRect(br, 3, 3);
	}else{itested();
#ifdef DO_TRACE
		p->setPen(QPen(Qt::green,3));
		p->drawRoundedRect(br, 3, 3);
	  p->drawEllipse(-3, -3, 6, 6);
#endif
	}

	(void) p;
	(void) o;
	(void) w;
	// QGraphicsItem::paint(p, o, w); //?
}
/*--------------------------------------------------------------------------*/
Symbol const* symbol(ElementGraphics const* e)
{
	return dynamic_cast<Symbol const*>(e->operator->());
}
/*--------------------------------------------------------------------------*/
Element const* element(ElementGraphics const* e)
{
	return e->operator->();
}
/*--------------------------------------------------------------------------*/
// add new port into ElementGraphics, build new
#if 0
ElementGraphics* ElementGraphics::newPort(pos_t where) const
{
	ElementGraphics* ng = nullptr;
	if(auto* c=dynamic_cast<Conductor const*>(_e)){ untested();
		auto place = new Place(where);
		Symbol* u = c->newUnion(place);

		if(u){
			ng = new ElementGraphics(u);
			assert(scene());
			scene()->addItem(ng);
			((QGraphicsItem*)ng)->hide(); // yikes.
			assert(_e->mutable_owner());
			u->setOwner(_e->mutable_owner());
		}else{
		}

		if(!ng){ untested();
		}else if(u->subckt()){ untested();
			// multiple Symbols?
			SchematicModel const* sc=u->subckt();
			for(auto c : sc->wires() /*BUG*/ ){ untested();
				auto cg = new ElementGraphics(c->clone());
				assert(_e->mutable_owner());
				assert(!element(cg)->mutable_owner());
				element(cg)->setOwner(_e->mutable_owner());
				assert(!cg->scene());
				cg->setParentItem(ng);
				assert(cg->scene());
			}
		}else{
			incomplete();
		}
	}
	return ng;
}
#endif
/*--------------------------------------------------------------------------*/
// built the union of two ElementGraphics
// BUG: only Conductors, for now
ElementGraphics* ElementGraphics::newUnion(ElementGraphics const* s) const
{
	ElementGraphics* ng = nullptr;
	if(auto c=dynamic_cast<Conductor const*>(_e)){ untested();
		assert(symbol(s));

		if(Symbol* u = c->newUnion(symbol(s)) ){ untested();
			trace1("new union", u);
			ng = new ElementGraphics(u);
			assert(_e->mutable_owner());
			u->setOwner(_e->mutable_owner());
//			ng->setParentItem(scene());
			scene()->addItem(ng);
			return ng;
		}else{ untested();
		}
	}else if(auto o=dynamic_cast<Conductor const*>(element(s))){ untested();
		// HACK HACK HACK
		if(Symbol* u = o->newUnion(symbol(this)) ){ untested();
			trace1("new union2", u);
			ng = new ElementGraphics(u);
			assert(_e->mutable_owner());
			u->setOwner(_e->mutable_owner());
//			ng->setParentItem(scene());
			scene()->addItem(ng);
			return ng;
		}else{ untested();
		}
	}else{
	}

	return ng;
}
/*--------------------------------------------------------------------------*/
// transform around pivot (in scope coordinates).
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
			std::string mxs = s->paramValue("$hflip"); // indicates if x axis is mirrored
			mx = atoi(mxs.c_str()); // \pm 1
			trace3("hflip", mx, my, r);
			assert(mx == 1);
			mx -= 1;
			mx /= -2;
		}catch(ExceptionCantFind const&){ untested();
			incomplete();
		}
		try {itested();
			std::string mys = s->paramValue("$vflip"); // indicates if y axis is mirrored
			my = atoi(mys.c_str());
			my -= 1;
			my /= -2;
		}catch(ExceptionCantFind const&){ untested();
		}
		try {itested();
			std::string rs = s->paramValue("$angle");
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

		s->setParameter(std::string("$hflip"), std::string("1"));
		s->setParameter(std::string("$vflip"), std::to_string(vflip));
		s->setParameter(std::string("$angle"), std::to_string(new_mr.degrees_int()));

		auto p = pos();
		int x = getX(p.toPoint());
		int y = getY(p.toPoint());

		x -= pivot.first;
		y -= pivot.second;

		pos_t new_xy(x,y);
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
SchematicScene* ElementGraphics::scene() const
{
	auto s = prechecked_cast<SchematicScene*>(QGraphicsItem::scene());
	if(s){itested();
	}else{itested();
	}
	return s;
}
/*--------------------------------------------------------------------------*/
QRectF ElementGraphics::boundingRect() const
{itested();
	assert(_e);
	return _e->boundingRect();
}
/*--------------------------------------------------------------------------*/
QRectF ElementGraphics::absoluteBoundingRect() const
{
#ifdef DO_TRACE
	if(auto sym=dynamic_cast<Symbol const*>(_e)){
		for(unsigned i=0; i<sym->numPorts(); ++i){
			trace3("abr", sym->label(), i, sym->nodePosition(i));
		}
	}
#endif
	auto b=boundingRect();
	auto p=pos();

	b.moveTopLeft(p + b.topLeft());
	return b;
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::setSelected(bool s)
{itested();
	qDebug() << "setSeletected" << s << this;
	QGraphicsItem::setSelected(s);
	assert(QGraphicsItem::isSelected()==s);
	assert(_e);
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
//		trace1("ElementGraphics::sceneEvent fwd", e->type());
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
		trace0("no attachToModel");
	}
#ifdef DO_TRACE
	if(auto sym=dynamic_cast<Symbol const*>(_e)){
		if(sym->numPorts() == 1){
			trace2("show", sym->label(), sym->nodePosition(0));
		}else if(sym->numPorts() == 2){
			trace3("show", sym->label(), sym->nodePosition(0), sym->nodePosition(1));
		}else{
		}
	}
#endif

	QGraphicsItem::show();
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::hide()
{itested();
	assert(_e);
	QGraphicsItem::hide();

#ifdef DO_TRACE
	if(auto sym=dynamic_cast<Symbol const*>(_e)){
		if(sym->numPorts() == 1){
			trace2("hide", sym->label(), sym->nodePosition(0));
		}else if(sym->numPorts() == 2){
			trace3("hide", sym->label(), sym->nodePosition(0), sym->nodePosition(1));
		}else{
		}
	}
#endif

	if(_e->scope()){itested();
		_e->detachFromModel();
	}else{ untested();
	}
}
/*--------------------------------------------------------------------------*/
	// does not make sense.
template<class P>
void ElementGraphics::moveElement(P const& delta)
{ untested();
	hide();
	assert(_e);
	int dx = getX(delta);
	int dy = getY(delta);
	trace3("moveElement", _e->label(), dx, dy);
	
	prepareGeometryChange(); // needed??
	auto x = _e->center();
	x = x + pos_t(dx, dy);
	_e->setCenter(x);

	// not redundant.
	auto p = _e->center();
	QGraphicsItem::setPos(p.first, p.second);

#ifndef NDEBUG
	auto pp = pos();
	assert(getX(p) == getX(pp));
	assert(getY(p) == getY(pp));
#endif

	show();
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::setPos(QPoint const& p)
{
	setPos(getX(p), getY(p), false);
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::setPos(int i, int j, bool relative)
{itested();
	assert(!relative); // use move, for now.
	// prepareGeometryChange();
	QGraphicsItem::setPos(i, j);
	_e->setCenter(pos_t(i, j));
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
    if (!scene()){itested();
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
