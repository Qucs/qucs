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
#include "component.h"
#include "symbol.h"
#include "sckt_base.h"
#include "schematic_scene.h"
#include "element_list.h"
#include "element_graphics.h"
#include "qio.h"
#include "platform.h"
#include "qt_compat.h"
#include "painting.h"
#include "place.h"
#include "conductor.h"

#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QWidget>
#include <QApplication> // BUG

//#include "../legacy/obsolete_paintings.h"
/*--------------------------------------------------------------------------*/
namespace qucs {
/*--------------------------------------------------------------------------*/
/* remove. later. */
// scene()->selectedItems gives QGraphicsItems
Element* element(ElementGraphics* g)
{
	if(!g){ untested();
		return nullptr;
	}else{itested();
		return &**g;
	}
}
/*--------------------------------------------------------------------------*/
ElementGraphics::ElementGraphics() : QGraphicsItem()
{ untested();
	assert(false);
	unreachable();
}
/*--------------------------------------------------------------------------*/
ElementGraphics::ElementGraphics(ElementGraphics const& e)
	: QGraphicsItem(), _e(nullptr), _elementText(nullptr), _selected(e._selected)
{itested();
	assert(e._e);
	Element* el = e._e->clone();
	assert(!el->owner());
	assert(el);
	attachElement(el);
}
/*--------------------------------------------------------------------------*/
ElementGraphics::ElementGraphics(Element* e)
	: QGraphicsItem(), _e(nullptr), _elementText(nullptr), _selected(false)
{itested();
	assert(e);
	trace2("new ElementGraphics0", e->label(), childItems().size());
	attachElement(e);
	trace2("new ElementGraphics1", e->label(), childItems().size());
}
/*--------------------------------------------------------------------------*/
ElementGraphics* ElementGraphics::clone() const
{itested();
	return new ElementGraphics(*this);
}
/*--------------------------------------------------------------------------*/
ElementGraphics::~ElementGraphics()
{itested();
	if(isVisible()){itested();
		// assert(_e->owner()); ??
		// element is owned by ElementList.
	}else{itested();
		// assert(!_e->owner()); ??
		// check: is this correct?
		delete(_e);
	}

	delete _elementText;
	_elementText = nullptr;
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

		setFlags(ItemIgnoresTransformations);

		int k = 0;
		if(e->showLabel()){
			_labeltext = new QGraphicsTextItem(this);
			_labeltext->setPlainText(QString::fromStdString(e->label()));
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
					t->setFlags(ItemIgnoresTransformations);
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
	~ElementText(){itested();
		// delete stuff?
		delete _labeltext;
	}
private:
	virtual QRectF boundingRect() const override{
		return QRectF();
	}
	void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *) override{}

private:
	QGraphicsTextItem* _labeltext;
	std::vector<QGraphicsTextItem*> _more;
}; // ElementText
/*--------------------------------------------------------------------------*/
#if 0
class TextGraphics : public QGraphicsTextItem{
public:
	explicit TextGraphics(Text& t, QGraphicsItem* parent)
	  : QGraphicsTextItem(parent), _t(t){
		setPlainText(t.s);
		trace1("TextGraphics", t.s);
	}
public:
private:
	Text& _t;
};
#endif
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
	if(_e){itested();
		return _e->clone();
	}else{ untested();
		return nullptr;
	}
}
/*--------------------------------------------------------------------------*/
static void redo_children(ElementGraphics* g)
{
	if(auto sym = dynamic_cast<SubcktBase const*>(element(g))){
		for(auto c : g->childItems()){
			delete c;
		}

		new ElementText(g);

		if(auto s = sym->subckt()){itested();
			for(auto o : *s){itested();
				if(auto i=dynamic_cast<Element*>(o)){
					QGraphicsItem* cg = new ElementGraphics(i->clone());
					cg->setParentItem(g);
				}else{
				}
			}
		}else{itested();
		}
	}
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::attachElement(Element* e)
{itested();
	assert(e);
	trace1("attach", e->label());
	assert(!_e);
	if(e->owner()){itested();
		// freshly parsed model
		// when loading a schematic
	}else{itested();
		// something else.
		// "ghost", freely moving but not interacting
	}
	assert(QGraphicsItem::isVisible()); // why?
	QGraphicsItem::hide();
	delete _elementText;
	_elementText = nullptr;
	_e = e;

	auto flags = ItemSendsGeometryChanges|ItemSendsGeometryChanges;
	if(dynamic_cast<Place const*>(_e)){
	}else{
		flags |= ItemIsSelectable;
		flags |= ItemIsMovable;
	}
	if(_e->legacyTransformHack()){ untested();
		flags |= ItemIgnoresTransformations;
	}else{itested();
	}
	setFlags(flags);
	// BUG: ask element?
	setAcceptHoverEvents(true);

	auto sp = _e->center();
	prepareGeometryChange();
	trace3("attachElement", e->label(), sp.first, sp.second);
	trace2("attachElement", e->label(), boundingRect());
	QGraphicsItem::setPos(sp.first, sp.second);

	// this is probably not needed. test later.
	if(dynamic_cast<Conductor*>(e)){itested();
		setZValue(-1.);
	}else{ itested();
	}

	if(0){
		auto a = new QGraphicsTextItem(this);
		a->setPlainText(QString::fromStdString(e->label()));
	}else{
	}
	auto sym = dynamic_cast<SubcktBase const*>(_e);

	if (auto w=_e->newWidget()){ untested();
		// BUG: this may break if there are multiple views.
		auto p = new QGraphicsProxyWidget(this);
		trace2("attach proxy", this, w);

		if(e->owner()){ untested();
			p->setWidget(w);
		}else{
			// dont expose if there is no link. this is a bit of a hack,
			// need to disentangle show/show_/hide/attach.
		}
		trace2("attached proxy", this, w);
	}else if(sym){itested();
		assert(sym->subckt());
		trace1("got children", sym->subckt()->size());
		redo_children(this);
	}else{
	}
	trace1("ElementGraphics unpacked", childItems().size());

	if(!_e){ untested();
	}else if(_e->legacyTransformHack()){ untested();
		// throw that in the bin some day..
	}else if(auto s=dynamic_cast<Symbol const*>(_e)){itested();
		// could be made accessible through Symbol interface.
		int hflip = atoi(s->paramValue("$hflip").c_str());
		int vflip = atoi(s->paramValue("$vflip").c_str());
		int angle = atoi(s->paramValue("$angle").c_str());
		assert(hflip==1 || hflip==-1);
		assert(vflip==1 || vflip==-1);

		trace4("attach::transform", _e->label(), angle, hflip, vflip);

		QTransform transform;
		transform.rotate(-angle); // chirality...
		transform.scale(hflip, vflip);
		setTransform(transform);
	}else{itested();
	}
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::paint(QPainter *p, const QStyleOptionGraphicsItem *o,
		QWidget* w)
{itested();
	assert(p);
	ViewPainter v(p); // TODO
	assert(v.Scale==1);

	if(auto pp=dynamic_cast<Painting const*>(_e)){
		pp->paint(&v);

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
	}else{
		unreachable();
	}

	(void) o;
	(void) w;
	// QGraphicsItem::paint(p, o, w); //?
}
/*--------------------------------------------------------------------------*/
Symbol const* symbol(ElementGraphics const* e)
{
	if(!e){
		return nullptr;
	}else{
		return dynamic_cast<Symbol const*>(&**e);
	}
}
/*--------------------------------------------------------------------------*/
Element const* element(ElementGraphics const* e)
{
	if(!e){
		return nullptr;
	}else{
		return &**e;
	}
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
			assert(_e->owner());
			u->set_owner(_e->owner());
		}else{
		}

		if(!ng){ untested();
		}else if(u->subckt()){ untested();
			// multiple Symbols?
			ElementList const* sc=u->subckt();
			for(auto c : sc->wires() /*BUG*/ ){ untested();
				auto cg = new ElementGraphics(c->clone());
				assert(_e->owner());
				assert(!element(cg)->owner());
				element(cg)->set_owner(_e->owner());
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
#if 0 // is this still needed? it does not work...
		// HACK HACK HACK
		if(Symbol* u = o->newUnion(symbol(this)) ){ untested();
//			assert(!dynamic_cast<Place const*>(_e));
			trace1("new union2", u);
			ng = new ElementGraphics(u);
			assert(_e->owner());
//			u->set_owner(_e->owner());
//			ng->setParentItem(scene());
			scene()->addItem(ng);
		}else{itested();
		}
	}else
#endif
	if(!symbol(s)){
		// diagram? no union
	}else if(auto c=dynamic_cast<Conductor const*>(_e)){itested();
		assert(symbol(s));

		if(Element* u = c->newUnion(symbol(s)) ){ untested();
			trace3("new union", u, symbol(s)->typeName(), symbol(s)->label());
			ng = new ElementGraphics(u);
			assert(_e->owner());
//			u->set_owner(_e->owner());
//			ng->setParentItem(scene());
			scene()->addItem(ng);
		}else{itested();
			trace1("no new union", symbol(s)->typeName());
		}
	}else{
	}

	return ng;
}
/*--------------------------------------------------------------------------*/
// transform around pivot (in scope coordinates).
void ElementGraphics::transform(qucsSymbolTransform a, std::pair<int, int> pivot)
{itested();
	incomplete(); // obsolete?
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
		}catch(qucs::ExceptionCantFind const&){ untested();
			incomplete();
		}
		try {itested();
			std::string mys = s->paramValue("$vflip"); // indicates if y axis is mirrored
			my = atoi(mys.c_str());
			my -= 1;
			my /= -2;
		}catch(qucs::ExceptionCantFind const&){ untested();
		}
		try {itested();
			std::string rs = s->paramValue("$angle");
			r = atoi(rs.c_str());
			assert(!(r%90));
			assert(r<360);
			r /= 90;
		}catch(qucs::ExceptionCantFind const&){ untested();
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

		s->set_param_by_name(std::string("$hflip"), std::string("1"));
		s->set_param_by_name(std::string("$vflip"), std::to_string(vflip));
		s->set_param_by_name(std::string("$angle"), std::to_string(new_mr.degrees_int()));

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
} // transform
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
	if(auto pp=dynamic_cast<Painting const*>(_e)){
		auto rr = pp->bounding_rect().toRectF();
//		trace2("br", rr, _e->label());
		return rr;
	}else{itested();
		return QRectF();
	}
}
/*--------------------------------------------------------------------------*/
QRectF ElementGraphics::absoluteBoundingRect() const
{
#ifdef DO_TRACE
	if(auto sym=dynamic_cast<Symbol const*>(_e)){
		for(unsigned i=0; i<sym->numPorts(); ++i){
			trace4("abr", pos(), sym->label(), i, sym->nodePosition(i));
		}
	}
#endif
	auto b = boundingRect();
	return mapRectToScene(b);
}
/*--------------------------------------------------------------------------*/
// QGraphicsItem::isSelected does not survive hide/show and clone. hence this
void ElementGraphics::setSelected(bool s)
{itested();
	QGraphicsItem::setSelected(s);
	_selected = s;
}
/*--------------------------------------------------------------------------*/
// """
// Reimplement this function to intercept events before they are dispatched to
// the specialized event handlers. should return true if the event e was
// recognized and processed.
// """
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
void ElementGraphics::restore_ports()
{
	auto s = dynamic_cast<Symbol*>(_e);
	assert(s);
	for(unsigned i=0; i<s->numPorts(); ++i) {
		assert(i < _port_values.size());
		s->set_port_by_index(i, _port_values[i]);
	}
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::init_ports()
{
	auto s = dynamic_cast<Symbol*>(_e);
	assert(!dynamic_cast<Place*>(_e)); // for now.
	assert(s);
	auto scn = scene();
	assert(scn);
	for(unsigned i=0; i<s->numPorts(); ++i){
		auto pp = s->nodePosition(i);
		ElementGraphics* pg = scn->find_place(pp);
		Place const* place = prechecked_cast<Place const*>(element(pg));
		assert(place);

		std::string pv = place->port_value(0);
		trace4("init_ports", s->label(), i, s->port_value(i), pv);

		s->set_port_by_index(i, pv);
	}
}
/*--------------------------------------------------------------------------*/
// show? restore? init?
void ElementGraphics::show_()
{
	trace1("show", _e->label());
	redo_children(this);
	assert(!isVisible());
	assert(scene());
	assert(!_e->owner());
	scene()->attachToModel(_e);
//	scene()->doc()->addElement(e);
	assert(_e->owner());

	QGraphicsItem::show();
	if (auto w=_e->newWidget()){ untested();
		assert(childItems().size()==1);
		for(auto i: childItems()){
			if(auto p = dynamic_cast<QGraphicsProxyWidget*>(i)){
				trace2("show_ it's a proxy", p, this);
				p->setWidget(w);
				p->show();
				w->show();
				trace2("show_ proxy 2", p, this);
				break; // only one proxy..
			}else{
			}
		}
	}else if(dynamic_cast<Symbol*>(_e)){
		trace1("set_ports restore", _e->label());
		if(!_port_values.size()){
			trace1("set_ports init", _e->label());
			init_ports();
		}else{
			restore_ports();
		}
	}else{
	}

	// if(was_selected) ...
	setSelected(_selected); // BUG: keep track somewhere else.
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::show()
{itested();

	assert(!isVisible());
	assert(scene());
//	assert(!_e->owner());
//	scene()->attachToModel(_e);
//	assert(_e->owner());

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
//	for(auto x : childItems()){
//		x->show();
//	}

#if 0
	assert(!_e->owner());
	_e->set_owner(subckt());
	subckt()->push_back(_e);
#endif
	// TODO
	// if (_selected__){
	// 	setSeleted();
	// }else{
	// }
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::hide()
{itested();
	assert(_e);
	_selected = QGraphicsItem::isSelected();
	assert(isVisible());
	auto l = childItems();
	trace1("ElementGraphics hide", childItems().size());
	for(auto i: childItems()){
		if(auto p = dynamic_cast<QGraphicsProxyWidget*>(i)){
			trace2("hide it's a proxy", p, this);
			p->setWidget(nullptr); // the Element has a reference to it.
		}else{
		}
	}
	QGraphicsItem::hide();
//	for(auto x : childItems()){
//		x->hide();
//	}

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

	if(!_e->owner()){itested();
	}else if(auto sym=dynamic_cast<Symbol*>(_e)){
		// store_ports?
		_port_values.clear();
		for(unsigned i=0; i<sym->numPorts(); ++i){
			trace2("unset port", sym->label(), i);
			_port_values.push_back(sym->port_value(i));
			sym->set_port_by_index(i, "");
		}
	}else{
	}

	if(_e->owner()){itested();
		// detach(_e, model());
		_e = scene()->detachFromModel(_e);
	}else{itested();
	}
	assert(!_e->owner());
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::setPos(QPoint const& p)
{
	setPos(getX(p), getY(p), false);
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::update()
{itested();
	QGraphicsItem::update();
	for(auto x : childItems()){
		x->update();
	}
}
/*--------------------------------------------------------------------------*/
void ElementGraphics::setPos(int i, int j, bool relative)
{itested();
	assert(!relative); // use move, for now.
	// prepareGeometryChange();
	QGraphicsItem::setPos(i, j);
	_e->setPosition(pos_t(i, j));
}
/*--------------------------------------------------------------------------*/
ItemEvent::ItemEvent(QEvent const& a, ElementGraphics& b)
	: QEvent(a), _item(b)
{itested();
}
/*--------------------------------------------------------------------------*/
QVariant ElementGraphics::itemChange(GraphicsItemChange c, const QVariant &v)
{
    if (!scene()){itested();
	 }else if(c == ItemPositionChange){ itested();
        QPointF tmp = v.toPointF();

		  // BUG: why is QApplication needed for this??
        if(QApplication::mouseButtons() != Qt::LeftButton){itested();
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
} // qucs
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
