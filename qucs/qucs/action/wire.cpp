// (c) 2020 Felix Salfelder
// GPLv3+

#include "globals.h"
#include "symbol.h"
#include "viewpainter.h"
#include "schematic_model.h"
/*--------------------------------------------------------------------------*/
namespace{

// BUG/FEATURE. Wires can't go around the corner
// wire under construction does. convert to pair of Wires eventually
class WireUC : public Symbol /*subckt*/ {
public:
	enum angle_mode_t{ am_H=0, am_V=1 };
private:
	WireUC(WireUC const&) = default;

public:
	WireUC() : _mode(am_H), _proto(nullptr) {
	}
	~WireUC() { }

private:
	Element* clone() const override{itested();
		return new WireUC(*this);
	}
	void paint(ViewPainter *p) const override;

private:
	QPoint pmid() const;
	void pushWire(int x, int y, int mx, int my);

private: // symbol
	unsigned numPorts() const override{ return 2; }
	void setParameter(std::string const& n, std::string const& v) override;
	QRectF boundingRect() const override;
	void expand() override;

private:
	angle_mode_t _mode;
	QPoint _p0;
	QPoint _p1;
	Element const* _proto;
} w;
/*--------------------------------------------------------------------------*/
void WireUC::paint(ViewPainter *p) const
{ itested();
	assert(p);

	QPoint pm = pmid();
	trace3("paint", _p0, _p1, pm);
	p->setPen(QPen(Qt::darkBlue, 1));
	p->drawLine(_p0, pm);
	p->drawLine(pm, _p1);
}
/*--------------------------------------------------------------------------*/
void WireUC::pushWire(int x, int y, int mx, int my)
{
	trace4("pw", x, y, mx, my);
	Element* wc = _proto->clone();
	Symbol* w = prechecked_cast<Symbol*>(wc);
	assert(w);

	w->setParameter("$xposition", std::to_string(x));
	w->setParameter("$yposition", std::to_string(y));
	w->setParameter("deltax", std::to_string(mx));
	w->setParameter("deltay", std::to_string(my));

	trace2("expand", _p0, _p1);
	subckt()->pushBack(w);
}
/*--------------------------------------------------------------------------*/
QPoint WireUC::pmid() const{itested();
	QPoint pm;
	switch(_mode){
	case am_H: itested();
				  pm = _p0;
				  pm.setX(_p1.x());
				  break;
	case am_V: untested();
				  pm = _p0;
				  pm.setY(_p1.y());
				  break;
	}
	return pm;
}
/*--------------------------------------------------------------------------*/
void WireUC::expand()
{itested();
	assert(!subckt());
	new_subckt();
	assert(subckt());

	if(!_proto){
		_proto	= symbol_dispatcher["Wire"];
	}else{
		unreachable();
	}
	assert(_proto);

	auto pm = pmid();
	int x = getX(_p0);
	int y = getY(_p0);
	int dx = getX(pm) - getX(_p0);
	int dy = getY(pm) - getY(_p0);

	if(dx || dy){
		pushWire(x, y, dx, dy);
	}else{
	}

	x = getX(_p1);
	y = getY(_p1);
	dx = getX(pm) - getX(_p1);
	dy = getY(pm) - getY(_p1);

	if(dx || dy){
		pushWire(x, y, dx, dy);
	}else{
	}

	SchematicModel const* sc = subckt();
	trace1("expanded", sc->wires().size());
	assert(sc->wires().size()); // BUG?
}
/*--------------------------------------------------------------------------*/
QRectF WireUC::boundingRect() const { itested();
  int xlo = std::min(_p0.x(), _p1.x());
  int xhi = std::max(_p0.x(), _p1.x());
  int ylo = std::min(_p0.y(), _p1.y());
  int yhi = std::max(_p0.y(), _p1.y());
  QPointF tl(xlo-5, ylo-5);
  QPointF br(xhi+5, yhi+5);
  auto r = QRectF(tl, br);
  assert(!r.isEmpty());
  return r;
}
/*--------------------------------------------------------------------------*/
void WireUC::setParameter(std::string const& n, std::string const& v)
{itested();
	trace2("gwsp", n, v);
	if(n=="x0"){itested();
		_p0.setX(atoi(v.c_str()));
	}else if(n=="y0"){itested();
		_p0.setY(atoi(v.c_str()));
	}else if(n=="x1"){itested();
		_p1.setX(atoi(v.c_str()));
	}else if(n=="y1"){itested();
		_p1.setY(atoi(v.c_str()));
	}else if(n=="mode"){ untested();
		switch(atoi(v.c_str())){
		case 0: untested();
			_mode = am_H;
			break;
		case 1: untested();
			_mode = am_V;
			break;
		}
	}else{ untested();
		Symbol::setParameter(n, v);
	}
}
/*--------------------------------------------------------------------------*/
Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "__ma_ghostwire", &w);

} // namespace

// namespace {
class MakeWire : public SchematicEdit {
public:
	template<class IT>
	MakeWire(SchematicDoc& ctx, IT wires)
	: SchematicEdit(*ctx.sceneHACK()){ itested();
		trace1("newwire", wires.size());
		size_t k = 0;

		// optimise and find tees.
		// TODO //
		// TODO //
		// TODO //
		// TODO //

		for(auto i : wires){itested();
			++k;
			if(auto e=dynamic_cast<Element*>(i)){itested();
				auto eg = new ElementGraphics(e);

				{ // BUG: not here/one call or do later?
					ctx.sceneAddItem(eg);
					ctx.takeOwnership(e);
				}

				qAdd_(eg);
			}else{ untested();
				unreachable(); // really? use prechecked_cast then.
			}
		}
		setText("create" + QString::number(k) + " wires");
	}
private:
#if 0
	void undo() override { untested();
		for(auto& d : _gfx){ untested();
			d->hide();
		}
	}
	void redo() override {itested();
		trace1("newwire redo", _gfx.size());
		for(auto& d : _gfx){itested();
			d->show();
		}
	}
#endif
private:
    // std::vector<ElementGraphics*> _gfx;
}; // MakeWire
/*--------------------------------------------------------------------------*/
class MouseActionWire : public MouseAction{
public:
	explicit MouseActionWire(MouseActions& ctx)
		: MouseAction(ctx), _mode(0), _phase(0) {itested();
		_proto = symbol_dispatcher.clone("__ma_ghostwire");
		assert(_proto);
	}

private: // override
	cmd* press(QEvent*) override;
	cmd* move(QEvent*) override;
	cmd* release(QMouseEvent*) override;
	cmd* activate(QObject* sender) override;
	cmd* deactivate() override;

private: // legacy code
	cmd* press1(QGraphicsSceneMouseEvent*);
	cmd* press2(QGraphicsSceneMouseEvent*);

	void toggleMode(){ untested();
		assert(_gfx.size());
		auto w = _gfx.back();
		Element* ee = element(w);
		auto e = prechecked_cast<Symbol*>(ee);
		assert(e);
		_mode = 1 - _mode;
		auto s = std::to_string(_mode);
		e->setParameter("mode", s);
		w->update();
	}
	void new_gfx();

private:
	int _mode; // V/H
	int _phase;

	int _MAx1;
	int _MAx3;
	int _MAy3;
	QCursor _oldcursor;
	std::vector<ElementGraphics*> _gfx;
	Element* _proto;
};
/*--------------------------------------------------------------------------*/
extern QCursor& crosshair();
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionWire::activate(QObject* sender)
{itested();
	assert(!_gfx.size());
	new_gfx();
	_phase = 1;
	_oldcursor = doc().cursor();
	setCursor(crosshair()); // Qt::CrossCursor);
	return MouseAction::activate(sender);
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionWire::deactivate()
{itested();
	doc().setCursor(_oldcursor);
	for(auto i: _gfx){ untested();
		delete i;
	}
	_gfx.resize(0);
	return MouseAction::deactivate();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void MouseActionWire::new_gfx()
{itested();
	Element* elt = _proto->clone();
	assert(elt);

	auto gfx = new ElementGraphics(elt);
	_gfx.push_back(gfx);
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionWire::release(QMouseEvent* m)
{itested();
	if(!m){ untested();
		unreachable();
	}else if(m->button() == Qt::RightButton){ untested();
		// toggleMode();
	}
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionWire::press(QEvent* e)
{itested();
	auto m = dynamic_cast<QMouseEvent*>(e);
	auto se = dynamic_cast<QGraphicsSceneMouseEvent*>(e);

	if(!se){itested();
		// unreachable(); no.
		// QEvent::GraphicsSceneMousePress == 156
		trace3("wirepress", m, se, e->type());
	}else if(se->button() == Qt::RightButton){ untested();
		toggleMode();
		e->accept();
	}else if(_phase == 1){itested();
		return press1(se);
		e->accept();
	}else if(_phase == 2){itested();
		return press2(se);
		e->accept();
	}else{ untested();
		unreachable();
	}

	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionWire::move(QEvent* e)
{ itested();
	if(!_gfx.size()){ itested();
		// no ghost yet.
	}else if(auto se=dynamic_cast<QGraphicsSceneMouseEvent*>(e)){itested();
		QPointF pos = se->scenePos(); // mapToScene(ev->pos());

		QPoint xx = doc().snapToGrid(pos);
		float fX = getX(xx);
		float fY = getY(xx);
		trace2("build wire", fX, fY);

		ElementGraphics* cur = _gfx.back();
		assert(cur);
		Element* ee = element(cur);
		auto elt = prechecked_cast<Symbol*>(ee);

		elt->setParameter("x1", std::to_string(fX));
		elt->setParameter("y1", std::to_string(fY));
		cur->prepareGeometryChange();

		e->accept();
	}else if(dynamic_cast<QMouseEvent*>(e)){ untested();
		incomplete();
		e->accept();
	}else{ untested();
		incomplete();
	}
	return nullptr;
}
/*--------------------------------------------------------------------------*/
 // MouseActions::MPressWire1 Is called if starting point of wire is pressed
QUndoCommand* MouseActionWire::press1(QGraphicsSceneMouseEvent* ev)
{itested();
	assert(ev);
	QPointF pos = ev->scenePos(); // mapToScene(ev->pos());
	float fX = pos.x();
	float fY = pos.y();
	QPoint xx = doc().snapToGrid(pos);
	fX = getX(xx);
	fY = getY(xx);

	ElementGraphics* cur = _gfx.back();
	Element* ee = element(cur);
	auto e = prechecked_cast<Symbol*>(ee);

	e->setParameter("x0", std::to_string(fX));
	e->setParameter("y0", std::to_string(fY));
	e->setParameter("x1", std::to_string(fX));
	e->setParameter("y1", std::to_string(fY));

	cur->prepareGeometryChange();
	doc().sceneAddItem(cur); // show, does not attach.

	//Doc->PostPaintEvent (_DotLine);
	//Doc->PostPaintEvent (_NotRop);
	//if(drawn) { untested();
#if 0  //ALYS - it draws some garbage, not deleted because of possible questions
	Doc->PostPaintEvent (_Line, 0, MAy3, MAx2, MAy3); // erase old mouse cross
	Doc->PostPaintEvent (_Line, MAx3, 0, MAx3, MAy2);
#endif
	//}
	// setDrawn(false);

	_MAx1 = 0;   // paint wire corner first up, then left/right
	_MAx3 = int(fX);
	_MAy3 = int(fY);
	//   Doc->snapToGrid(MAx3, MAy3);
	//
	//ALYS - draw aiming cross
	/// \todo paintAim(Doc,MAx3, MAy3);
	//#######################

	//  formerAction = 0; // keep wire action active after first wire finished
	_phase = 2;
	//  QucsMain->MouseMoveAction = &MouseActions::MMoveWire2;
	//  QucsMain->MousePressAction = &MouseActions::MPressWire2;
	// Double-click action is set in "MMoveWire2" to not initiate it
	// during "Wire1" actions.
	//  Doc->viewport()->update();
	return nullptr;
}
/*--------------------------------------------------------------------------*/
//* MouseActions::MPressWire2 Is called if ending point of wire is pressed
QUndoCommand* MouseActionWire::press2(QGraphicsSceneMouseEvent* ev)
{itested();
	cmd* c=nullptr;

	assert(ev);
	QPointF pos = ev->scenePos(); // mapToScene(ev->pos());

	QPoint xx = doc().snapToGrid(pos);
	auto fX = getX(xx);
	auto fY = getY(xx);
	pos_t p(fX, fY);

	if(isConductor(p)){itested();
		std::list<Element*> new_wires;
		for(auto& i: _gfx){itested();
			auto s = prechecked_cast<Symbol*>(element(i));
			assert(s);
			s->expand();
			SchematicModel const* cs = s->subckt();
			assert(cs);
			trace1("prepare NewWire", cs->wires().size());
			for(Element const* j : cs->wires()){itested();
				new_wires.push_back(j->clone());
			}
			delete(i);
		}
		c = new MakeWire(doc(), new_wires);
		_gfx.clear();
	}else{  // same as press1?
		new_gfx();

		ElementGraphics* cur = _gfx.back();
		Element* ee = element(cur);
		auto e = prechecked_cast<Symbol*>(ee);

		e->setParameter("x0", std::to_string(fX));
		e->setParameter("y0", std::to_string(fY));
		e->setParameter("x1", std::to_string(fX));
		e->setParameter("y1", std::to_string(fY));

		cur->prepareGeometryChange();
		doc().sceneAddItem(cur); // show, does not attach.
	}

	/// \todo paintAim(Doc,MAx2,MAy2); //ALYS - added missed aiming cross
		/// \todo document right mouse button changes the wire corner
	// Doc->viewport()->update();
	return c;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//} namespace


// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
// a command inserting a wire must check for overlap, then keep a list of wires
// to remove AND a list of wires to add. (or maybe, a list of modifications
// modify, and a list of additional wires).
//
/////// //
/////// // Inserts a port into the schematic and connects it to another node if the
/////// // coordinates are identical. If 0 is returned, no new wire is inserted.
/////// // If 2 is returned, the wire line ended.
/////// int SchematicModel::insertWireNode1(Wire *w)
/////// {
///////     (void)w;
/////// #if 0 // is this really needed?
///////       // maybe in SchematicDoc.
///////     Node *pn = nodes().find_at(w->x1_(), w->y1_());
/////// 
///////     if(pn != 0) {
///////         pn->connectionsAppend(w);
///////         w->setPortByIndex(1, pn);
///////         return 2;   // node is not new
///////     }else{
///////     }
/////// 
///////     // check if the new node lies upon an existing wire
///////     for(auto ptr2 : wires()){
///////         if(ptr2->x1_() == w->x1_()) {
///////             if(ptr2->y1_() > w->y1_()) continue;
///////             if(ptr2->y2_() < w->y1_()) continue;
/////// 
///////             if(ptr2->isHorizontal() == w->isHorizontal()){
///////                 // ptr2-wire is vertical
///////                 if(ptr2->y2_() >= w->y2_()) {
///////                     delete w;    // new wire lies within an existing wire
///////                     return 0;
///////                 } else {
///////                     // one part of the wire lies within an existing wire
///////                     // the other part not
///////                     if(ptr2->portValue(1)->degree() == 1)
///////                     {
///////                         w->y1__() = ptr2->y1_();
///////                         w->setPortByIndex(0, ptr2->portValue(0));
/////// #if 0
///////                         if(ptr2->Label) {
///////                             w->Label = ptr2->Label;
///////                             w->Label->pOwner = w;
///////                         }
/////// #endif
///////                         ptr2->portValue(0)->connectionsRemove(ptr2);  // two -> one wire
///////                         ptr2->portValue(0)->connectionsAppend(w);
/////// 
/////// 			// incomplete.
///////                         // nodes().removeRef(ptr2->portValue(1));
///////                         wires().removeRef(ptr2);
///////                         return 2;
///////                     }
///////                     else
///////                     {
///////                         w->y1__() = ptr2->y2_();
///////                         w->setPortByIndex(0, ptr2->portValue(1));
///////                         ptr2->portValue(1)->connectionsAppend(w);   // shorten new wire
///////                         return 2;
///////                     }
///////                 }
///////             }
///////         }
///////         else if(ptr2->y1_() == w->y1_())
///////         {
///////             if(ptr2->x1_() > w->x1_()) continue;
///////             if(ptr2->x2_() < w->x1_()) continue;
/////// 
///////             if(ptr2->isHorizontal() == w->isHorizontal())   // ptr2-wire is horizontal
///////             {
///////                 if(ptr2->x2_() >= w->x2_())
///////                 {
///////                     delete w;   // new wire lies within an existing wire
///////                     return 0;
///////                 }
///////                 else
///////                 {
///////                     // one part of the wire lies within an existing wire
///////                     // the other part not
///////                     if(ptr2->portValue(1)->degree() == 1)
///////                     {
///////                         w->x1__() = ptr2->x1_();
///////                         w->setPortByIndex(0, ptr2->portValue(0));
/////// 
/////// #if 0
///////                         if(ptr2->Label) {
///////                             w->Label = ptr2->Label;
///////                             w->Label->pOwner = w;
///////                         }
/////// #endif
///////                         ptr2->portValue(0)->connectionsRemove(ptr2); // two -> one wire
///////                         ptr2->portValue(0)->connectionsAppend(w);
/////// 
/////// 			incomplete();
///////                         // nodes().removeRef(ptr2->portValue(1));
///////                         wires().removeRef(ptr2);
///////                         return 2;
///////                     }
///////                     else
///////                     {
///////                         w->x1__() = ptr2->x2_();
///////                         w->setPortByIndex(0, ptr2->portValue(1));
///////                         ptr2->portValue(1)->connectionsAppend(w);   // shorten new wire
///////                         return 2;
///////                     }
///////                 }
///////             }
///////         }
///////         else continue;
/////// 
///////         pn = &nodes().new_at(w->x1_(), w->y1_() );   // create new node
///////         pn->connectionsAppend(w);  // connect schematic node to the new wire
///////         w->setPortByIndex(0, pn);
/////// 
///////         // split the wire into two wires
///////         splitWire(ptr2, pn);
///////         return 2;
///////     }
/////// 
///////     pn = &nodes().new_at(w->x1_(), w->y1_());   // create new node
///////     pn->connectionsAppend(w);  // connect schematic node to the new wire
///////     w->setPortByIndex(0, pn);
/////// #endif
///////     return 1;
/////// }
/////// 
/////// // ---------------------------------------------------
/////// // if possible, connect two horizontal wires to one
/////// // // move to SchematicDoc
/////// bool SchematicModel::connectHWires1(Wire *w)
/////// {
///////     Wire *pw;
///////     // only does port1 (why?)
/////// 
///////     incomplete();
/////// 
///////     (void) pw;
///////     (void) w;
/////// #if 0
///////     Node *n = w->portValue(0);
///////     pw = (Wire*)n->Connections.last();  // last connection is the new wire itself
///////     for(pw = (Wire*)n->Connections.prev(); pw!=0; pw = (Wire*)n->Connections.prev())
///////     {
///////         if(pw->Type != isWire) continue;
///////         if(!pw->isHorizontal()) continue;
///////         if(pw->x1_() < w->x1_())
///////         {
///////             if(n->degree() != 2) continue;
///////             if(pw->Label)
///////             {
///////                 w->Label = pw->Label;
///////                 w->Label->pOwner = w;
///////             }
///////             else if(n->Label)
///////             {
///////                 w->Label = n->Label;
///////                 w->Label->pOwner = w;
///////                 w->Label->Type = isHWireLabel;
///////             }
///////             w->x1__() = pw->x1_();
///////             w->setPortByIndex(0, pw->portValue(0);      // new wire lengthens an existing one
///////             nodes().removeRef(n);
///////             w->portValue(0)->connectionsRemove(pw);
///////             w->portValue(0)->connectionsAppend(w);
///////             wires().removeRef(pw);
///////             return true;
///////         }
///////         if(pw->x2_() >= w->x2_())    // new wire lies within an existing one ?
///////         {
///////             w->portValue(0)->connectionsRemove(w); // second node not yet made
///////             delete w;
///////             return false;
///////         }
///////         if(pw->portValue(1)->degree() < 2)
///////         {
///////             // existing wire lies within the new one
///////             if(pw->Label)
///////             {
///////                 w->Label = pw->Label;
///////                 w->Label->pOwner = w;
///////             }
///////             pw->portValue(0)->connectionsRemove(pw);
///////             nodes().removeRef(pw->portValue(1));
///////             wires().removeRef(pw); // deleted if autodelete.
///////             return true;
///////         }
///////         w->x1__() = pw->x2_();    // shorten new wire according to an existing one
///////         w->portValue(0)->connectionsRemove(w);
///////         w->setPortByIndex(0, pw->portValue(1);
///////         w->portValue(0)->connectionsAppend(w);
///////         return true;
///////     }
/////// #endif
/////// 
///////     return true;
/////// }
/////// 
/////// // ---------------------------------------------------
/////// // if possible, connect two vertical wires to one
/////// bool SchematicModel::connectVWires1(Wire *w)
/////// {
///////     Wire *pw;
///////     (void)pw;
///////     (void)w;
/////// 
/////// #if 0 // see above.
///////     Node *n = w->portValue(0);
///////     pw = (Wire*)n->Connections.last();  // last connection is the new wire itself
///////     for(pw = (Wire*)n->Connections.prev(); pw!=0; pw = (Wire*)n->Connections.prev())
///////     {
///////         if(pw->Type != isWire) continue;
///////         if(pw->isHorizontal()) continue;
///////         if(pw->y1_() < w->y1_())
///////         {
///////             if(n->degree() != 2) continue;
///////             if(pw->Label)
///////             {
///////                 w->Label = pw->Label;
///////                 w->Label->pOwner = w;
///////             }
///////             else if(n->Label)
///////             {
///////                 w->Label = n->Label;
///////                 w->Label->pOwner = w;
///////                 w->Label->Type = isVWireLabel;
///////             }
///////             w->y1__() = pw->y1_();
///////             w->setPortByIndex(0, pw->portValue(0);         // new wire lengthens an existing one
/////// 	    incomplete();
///////             // removeNode(n);
///////             w->portValue(0)->connectionsRemove(pw);
///////             w->portValue(0)->connectionsAppend(w);
///////             w->portValue(0)->Connections.removeRef(pw);
///////             w->portValue(0)->Connections.append(w);
/////// 	    incomplete();
///////             // removeWire(pw);
///////             return true;
///////         }
///////         if(pw->y2_() >= w->y2_())    // new wire lies complete within an existing one ?
///////         {
///////             w->portValue(0)->connectionsRemove(w); // second node not yet made
///////             delete w;
///////             return false;
///////         }
///////         if(pw->portValue(1)->degree() < 2)
///////         {
///////             // existing wire lies within the new one
///////             if(pw->Label)
///////             {
///////                 w->Label = pw->Label;
///////                 w->Label->pOwner = w;
///////             }
///////             pw->portValue(0)->connectionsRemove(pw);
/////// 	    incomplete();
///////             // removeNode(pw->portValue(1));
///////             // removeWire(pw);
///////             return true;
///////         }
///////         w->y1__() = pw->y2_();    // shorten new wire according to an existing one
///////         w->portValue(0)->connectionsRemove(w);
///////         w->setPortByIndex(0, pw->portValue(1);
///////         w->portValue(0)->connectionsAppend(w);
///////         return true;
///////     }
/////// #endif
/////// 
///////     return true;
/////// }
/////// 
/////// // ---------------------------------------------------
/////// // Inserts a port into the schematic and connects it to another node if the
/////// // coordinates are identical. If 0 is returned, no new wire is inserted.
/////// // If 2 is returned, the wire line ended.
/////// //  bug. this is the same as insertWireNode1, but with ports swapped.
/////// int SchematicModel::insertWireNode2(Wire *w)
/////// {
/////// (void)w;
/////// #if 0
///////     Node *pn = nodes().find_at( w->x2_(),  w->x2_());
/////// 
///////     if(pn != 0) {
///////         pn->connectionsAppend(w);
///////         w->setPortByIndex(1, pn);
///////         return 2;   // node is not new
///////     }else{
///////     }
/////// 
///////     // check if the new node lies upon an existing wire
///////     for(auto ptr2 : wires()){
///////         if(ptr2->x1_() == w->x2_())
///////         {
///////             if(ptr2->y1_() > w->y2_()) continue;
///////             if(ptr2->y2_() < w->y2_()) continue;
/////// 
///////             // (if new wire lies within an existing wire, was already check before)
///////             if(ptr2->isHorizontal() == w->isHorizontal())   // ptr2-wire is vertical
///////             {
///////                 // one part of the wire lies within an existing wire
///////                 // the other part not
///////                 if(ptr2->portValue(0)->degree() == 1) {
/////// #if 0
///////                     if(ptr2->Label) {
///////                         w->Label = ptr2->Label;
///////                         w->Label->pOwner = w;
///////                     }
/////// #endif
///////                     w->y2__() = ptr2->y2_();
///////                     w->setPortByIndex(1, ptr2->portValue(1));
///////                     ptr2->portValue(1)->connectionsRemove(ptr2);  // two -> one wire
///////                     ptr2->portValue(1)->connectionsAppend(w);
///////                     removeNode(ptr2->portValue(0));
/////// 		    erase(ptr2);
///////                     return 2;
///////                 }
///////                 else
///////                 {
///////                     w->y2__() = ptr2->y1_();
///////                     w->setPortByIndex(1, ptr2->portValue(0));
///////                     ptr2->portValue(0)->connectionsAppend(w);   // shorten new wire
///////                     return 2;
///////                 }
///////             }
///////         }
///////         else if(ptr2->y1_() == w->y2_())
///////         {
///////             if(ptr2->x1_() > w->x2_()) continue;
///////             if(ptr2->x2_() < w->x2_()) continue;
/////// 
///////             // (if new wire lies within an existing wire, was already check before)
///////             if(ptr2->isHorizontal() == w->isHorizontal())   // ptr2-wire is horizontal
///////             {
///////                 // one part of the wire lies within an existing wire
///////                 // the other part not
///////                 if(ptr2->portValue(0)->degree() == 1) {
/////// 
/////// #if 0
///////                     if(ptr2->Label) {
///////                         w->Label = ptr2->Label;
///////                         w->Label->pOwner = w;
///////                     }
/////// #endif
///////                     w->x2__() = ptr2->x2_();
///////                     w->setPortByIndex(1, ptr2->portValue(1));
///////                     ptr2->portValue(1)->connectionsRemove(ptr2);  // two -> one wire
///////                     ptr2->portValue(1)->connectionsAppend(w);
///////                     removeNode(ptr2->portValue(0));
/////// 		    erase(ptr2);
///////                     return 2;
///////                 }else{
///////                     w->x2__() = ptr2->x1_();
///////                     w->setPortByIndex(1, ptr2->portValue(0));
///////                     ptr2->portValue(0)->connectionsAppend(w);   // shorten new wire
///////                     return 2;
///////                 }
///////             }
///////         }
///////         else continue;
/////// 
///////         pn = &nodes().new_at(w->x2_(), w->y2_());   // create new node
///////         nodes().append(pn);
///////         pn->connectionsAppend(w);  // connect schematic node to the new wire
///////         w->setPortByIndex(1, pn);
/////// 
///////         // split the wire into two wires
///////         splitWire(ptr2, pn);
///////         return 2;
///////     }
/////// 
///////     pn = &nodes().new_at(w->x2_(), w->y2_());   // create new node
///////     nodes().append(pn);
///////     pn->connectionsAppend(w);  // connect schematic node to the new wire
///////     w->setPortByIndex(1, pn);
/////// #endif
///////     return 1;
/////// }
/////// 
/////// // ---------------------------------------------------
/////// // if possible, connect two horizontal wires to one
/////// bool SchematicModel::connectHWires2(Wire *w)
/////// {
///////     Wire *pw;
/////// (void)w;
/////// (void)pw;
/////// 
/////// #if 0 // see above.
///////     Node *n = w->portValue(1);
///////     pw = (Wire*)n->Connections.last(); // last connection is the new wire itself
///////     for(pw = (Wire*)n->Connections.prev(); pw!=0; pw = (Wire*)n->Connections.prev())
///////     {
///////         if(pw->Type != isWire) continue;
///////         if(!pw->isHorizontal()) continue;
///////         if(pw->x2_() > w->x2_())
///////         {
///////             if(n->degree() != 2) continue;
///////             if(pw->Label)
///////             {
///////                 w->Label = pw->Label;
///////                 w->Label->pOwner = w;
///////             }
///////             w->x2__() = pw->x2_();
///////             w->setPortByIndex(1, pw->portValue(1);      // new wire lengthens an existing one
///////             removeNode(n);
///////             w->portValue(1)->connectionsRemove(pw);
///////             w->portValue(1)->connectionsAppend(w);
///////             erase(pw);
///////             return true;
///////         }else if(pw->portValue(0)->degree() < 2) {
/////// 	    // (if new wire lies complete within an existing one, was already
/////// 	    // checked before). existing wire lies within the new one
///////             if(pw->Label)
///////             {
///////                 w->Label = pw->Label;
///////                 w->Label->pOwner = w;
///////             }
///////             pw->portValue(1)->connectionsRemove(pw);
///////             removeNode(pw->portValue(0));
///////             erase(pw);
///////             return true;
///////         }else{
/////// 	}
///////         w->x2__() = pw->x1_();    // shorten new wire according to an existing one
///////         w->portValue(1)->connectionsRemove(w);
///////         w->setPortByIndex(1, pw->portValue(0);
///////         w->portValue(1)->connectionsAppend(w);
///////         return true;
///////     }
/////// #endif
/////// 
///////     return true;
/////// }
/////// 
/////// // ---------------------------------------------------
/////// // if possible, connect two vertical wires to one
/////// // BUG: dup
/////// bool SchematicModel::connectVWires2(Wire *w)
/////// {
///////     Wire *pw;
/////// (void)w;
/////// (void)pw;
/////// 
/////// #if 0 // see above
///////     Node *n = w->portValue(1);
///////     pw = (Wire*)n->Connections.last(); // last connection is the new wire itself
///////     for(pw = (Wire*)n->Connections.prev(); pw!=0; pw = (Wire*)n->Connections.prev())
///////     {
///////         if(pw->Type != isWire) continue;
///////         if(pw->isHorizontal()) continue;
///////         if(pw->y2_() > w->y2_())
///////         {
///////             if(n->degree() != 2) continue;
///////             if(pw->Label)
///////             {
///////                 w->Label = pw->Label;
///////                 w->Label->pOwner = w;
///////             }
///////             w->y2__() = pw->y2_();
///////             w->setPortByIndex(1, pw->portValue(1);     // new wire lengthens an existing one
///////             removeNode(n);
///////             w->portValue(1)->connectionsRemove(pw);
///////             w->portValue(1)->connectionsAppend(w);
///////             erase(pw);
///////             return true;
///////         }
///////         // (if new wire lies complete within an existing one, was already
///////         // checked before)
/////// 
///////         if(pw->portValue(0)->degree() < 2)
///////         {
///////             // existing wire lies within the new one
///////             if(pw->Label)
///////             {
///////                 w->Label = pw->Label;
///////                 w->Label->pOwner = w;
///////             }
///////             pw->portValue(1)->connectionsRemove(pw);
///////             removeNode(pw->portValue(0));
///////             erase(pw);
///////             return true;
///////         }
///////         w->y2__() = pw->y1_();    // shorten new wire according to an existing one
///////         w->portValue(1)->connectionsRemove(w);
///////         w->setPortByIndex(1, pw->portValue(0);
///////         w->portValue(1)->connectionsAppend(w);
///////         return true;
///////     }
/////// #endif
/////// 
///////     return true;
/////// }
//// // ---------------------------------------------------
//// // Inserts a vertical or horizontal wire into the schematic and connects
//// // the ports that hit together. Returns whether the beginning and ending
//// // (the ports of the wire) are connected or not.
//// int SchematicModel::insertWire(Wire *w)
//// {
//// (void)w;
//// 
//// #if 0 // it looks obsolete
////     int  tmp, con = 0;
////     bool ok;
//// 
////     // change coordinates if necessary (port 1 coordinates must be less
////     // port 2 coordinates)
////     if(w->x1_() > w->x2_()) {
////         tmp = w->x1_();
////         w->x1__() = w->x2_();
////         w->x2__() = tmp;
////     }else if(w->y1_() > w->y2_()) {
////         tmp = w->y1_();
////         w->y1__() = w->y2_();
////         w->y2__() = tmp;
////     }else{
////        	con = 0x100;
////     }
//// 
//// 
//// 
////     tmp = insertWireNode1(w);
////     if(tmp == 0) return 3;  // no new wire and no open connection
////     if(tmp > 1) con |= 2;   // insert node and remember if it remains open
//// 
////     if(w->isHorizontal()) ok = connectHWires1(w);
////     else ok = connectVWires1(w);
////     if(!ok) return 3;
//// 
//// 
//// 
//// 
////     tmp = insertWireNode2(w);
////     if(tmp == 0) return 3;  // no new wire and no open connection
////     if(tmp > 1) con |= 1;   // insert node and remember if it remains open
//// 
////     if(w->isHorizontal()) ok = connectHWires2(w);
////     else ok = connectVWires2(w);
////     if(!ok) return 3;
//// 
//// 
//// 
////     // change node 1 and 2
////     if(con > 255) con = ((con >> 1) & 1) | ((con << 1) & 2);
//// 
////     auto wi=wires().end();
////     --wi;
////     pushBack(w);    // add wire to the schematic
////     ++wi;
//// 
////     int  n1, n2;
////     Wire *pw, *nw;
////     Node *pn, *pn2;
////     Element *pe;
////     // ................................................................
////     // Check if the new line covers existing nodes.
////     // In order to also check new appearing wires -> use "for"-loop
////     // this looks like it is not a loop at all.
////     // check: what are the findRef calls really doing, and why?
////     for(; wi!=wires().end(); ++wi){
//// 	pw=*wi;
////         for(auto ni=nodes().begin(); ni != nodes().end(); ){
//// 	    pn = *ni;
//// 	    // check every node
////             if(pn->cx_() == pw->x1_()) {
////                 if(pn->cy_() <= pw->y1_()) {
////                     pn = nodes().next();
////                     continue;
////                 }else if(pn->cy_() >= pw->y2_()) {
////                     pn = nodes().next();
////                     continue;
////                 }
////             } else if(pn->cy_() == pw->y1_()){
////                 if(pn->cx_() <= pw->x1_()) {
////                     pn = nodes().next();
////                     continue;
////                 }else if(pn->cx_() >= pw->x2_())
////                 {
////                     pn = nodes().next();
////                     continue;
////                 }else{
//// 		  // do more stuff below.
//// 		}
////             }else{
////                 pn = nodes().next();
////                 continue;
////             }
//// 
////             n1 = 2;
////             n2 = 3;
////             pn2 = pn;
////             // check all connections of the current node
////             for(pe=pn->Connections.first(); pe!=0; pe=pn->Connections.next())
////             {
////                 if(pe->Type != isWire) continue;
////                 nw = (Wire*)pe;
////                 // wire lies within the new ?
////                 if(pw->isHorizontal() != nw->isHorizontal()) continue;
//// 
////                 pn  = nw->portValue(0);
////                 pn2 = nw->portValue(1);
////                 n1  = pn->degree();
////                 n2  = pn2->degree();
////                 if(n1 == 1) {
////                     removeNode(pn);
////                     pn2->connectionsRemove(nw);   // remove connection
////                     pn = pn2;
////                 }else{
//// 		}
//// 
////                 if(n2 == 1) {
////                     pn->connectionsRemove(nw);   // remove connection
//// 		    // node 2 is open
////                     removeNode(pn2);
////                     pn2 = pn;
////                 }else{
//// 		}
//// 
////                 if(pn == pn2)
////                 {
////                     if(nw->Label)
////                     {
////                         pw->Label = nw->Label;
////                         pw->Label->pOwner = pw;
////                     }
////                     wires().removeRef(nw);    // delete wire
////                     wi=wires().find(pw);      // is this a hidden loop condition?!
//// 		    assert(wi!=wires().end());
////                 }
////                 break;
////             }
////             if(n1 == 1) if(n2 == 1) continue;
//// 
////             // split wire into two wires
////             if((pw->x1_() != pn->cx_()) || (pw->y1_() != pn->cy_())) {
////                 nw = new Wire(pw->x1_(), pw->y1_(), pn->cx_(), pn->cy_(), pw->portValue(0), pn);
////                 pn->connectionsAppend(nw);
////                 wires().append(nw);
////                 wires().findRef(pw); // hidden loop conditional?
////                 pw->portValue(0)->connectionsAppend(nw);
////             }
////             pw->portValue(0)->connectionsRemove(pw);
////             pw->x1__() = pn2->cx_();
////             pw->y1__() = pn2->cy_();
////             pw->setPortByIndex(0, pn2;
////             pn2->connectionsAppend(pw);
//// 
////             ++pn; // = nodes().next();??
////         }
////     }
//// 
//// //    if (wires().containsRef (w))  // if two wire lines with different labels ...
//// //        oneLabel(w->portValue(0));       // ... are connected, delete one label
////     return con | 0x0200;   // sent also end flag
//// #endif
////     return 0;
//// }
