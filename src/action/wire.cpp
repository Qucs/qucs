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
	WireUC()
	  : _mode(am_H),
	    _p0(0,0),
	    _p1(0,0),
	    _proto(nullptr) { }
	~WireUC() { }

private: // Element
	Element* clone() const override{itested();
		return new WireUC(*this);
	}
	void paint(ViewPainter *p) const override;

private:
	pos_t pmid() const;
	void pushWire(pos_t, pos_t);

private: // Symbol
	unsigned numPorts() const override{ return 0; }
	Port& port(unsigned){ unreachable(); return *new Port();}
	pos_t portPosition(unsigned) const override { unreachable(); return pos_t(0,0); }
	void setParameter(std::string const& n, std::string const& v) override;
	rect_t bounding_rect() const override;
	void expand() override;

private:
	angle_mode_t _mode;
	pos_t _p0;
	pos_t _p1;
	Element const* _proto;
} w;
/*--------------------------------------------------------------------------*/
void WireUC::paint(ViewPainter *p) const
{ itested();
	assert(p);

	pos_t m = pmid();
	QPoint pm = QPoint(getX(m), getY(m));
	QPoint p0 = QPoint(getX(_p0), getY(_p0));
	QPoint p1 = QPoint(getX(_p1), getY(_p1));
	trace3("paint", _p0, _p1, pm);
	p->setPen(QPen(Qt::darkBlue, 1));
	p->drawLine(p0, pm);
	p->drawLine(pm, p1);
}
/*--------------------------------------------------------------------------*/
void WireUC::pushWire(pos_t p0, pos_t delta)
{
	int x = getX(p0);
	int y = getY(p0);
	int mx = getX(delta);
	int my = getY(delta);
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
pos_t WireUC::pmid() const
{itested();
	pos_t pm = _p0;
	switch(_mode){
	case am_H: itested();
				  pm.setX(_p1.x());
				  break;
	case am_V: untested();
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
	pos_t delta = pm - _p0;
	if(delta){
		pushWire(_p0, delta);
	}else{
	}

	delta = pm - _p1;
	if(delta){
		pushWire(_p1, delta);
	}else{
	}

	SchematicModel const* sc = subckt();
	if(sc->size()){ itested();
	}else{ itested();
		// possible when double clicking on empty wire
	}
}
/*--------------------------------------------------------------------------*/
rect_t WireUC::bounding_rect() const
{ itested();
  int xlo = std::min(_p0.x(), _p1.x());
  int xhi = std::max(_p0.x(), _p1.x());
  int ylo = std::min(_p0.y(), _p1.y());
  int yhi = std::max(_p0.y(), _p1.y());
  pos_t tl(xlo-5, ylo-5);
  pos_t br(xhi+5, yhi+5);
  auto r = rect_t(tl, br);
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

		for(auto i : wires){itested();
			++k;
			if(auto e = dynamic_cast<Element*>(i)){itested();
				auto eg = new ElementGraphics(e);

				{ // BUG: not here/one call or do later?
					ctx.sceneAddItem(eg);
					eg->setVisible(false);
//					ctx.takeOwnership(e);
				}

				qInsert(eg);
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
	cmd* release(QEvent*) override;
	cmd* activate(QObject* sender) override;
	cmd* deactivate() override;
	cmd* dblclk(QEvent* e) override;

private: // legacy code
	cmd* press1(QGraphicsSceneMouseEvent*);
	cmd* press2(QGraphicsSceneMouseEvent*);

private:
	cmd* finish();
	void toggleMode(){ untested();
		if(!_gfx.size()){
			incomplete(); // always keep a wireUC
			return;
		}
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
QUndoCommand* MouseActionWire::release(QEvent* e)
{itested();
	auto m =prechecked_cast<QGraphicsSceneMouseEvent*>(e);
	if(!m){ untested();
		unreachable();
	}else if(m->button() == Qt::RightButton){ untested();
		// toggleMode();
	}
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionWire::dblclk(QEvent* e)
{ untested();
	trace1("wire::dblclk1", _gfx.size());

	if(_gfx.size()){
		delete _gfx.back();
		_gfx.pop_back();
		trace1("wire::dblclk2", _gfx.size());
	}else{
	}


	if(_gfx.size()){
		return finish();
	}else{
		unreachable();
		return nullptr;
	}
	e->accept();
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

	assert(_gfx.size());
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
QUndoCommand* MouseActionWire::finish()
{
	trace1("finish", _gfx.size());
	std::list<Element*> new_wires;
	for(auto& i: _gfx){itested();
		auto s = prechecked_cast<Symbol*>(element(i));
		assert(s);
		s->expand();
		SchematicModel const* cs = s->subckt();
		assert(cs);
		trace1("prepare NewWire", cs->size());
		for(Element const* j : *cs){itested();
			new_wires.push_back(j->clone());
		}
		delete(i);
	}
	QUndoCommand* c = new MakeWire(doc(), new_wires);
	_gfx.clear();

// 	_phase=1; ??

	return c;
}
/*--------------------------------------------------------------------------*/
//* MouseActions::MPressWire2 Is called if ending point of wire is pressed
QUndoCommand* MouseActionWire::press2(QGraphicsSceneMouseEvent* ev)
{untested();
	cmd* c=nullptr;

	assert(ev);
	QPointF pos = ev->scenePos(); // mapToScene(ev->pos());

	QPoint xx = doc().snapToGrid(pos);
	auto fX = getX(xx);
	auto fY = getY(xx);
	pos_t p(fX, fY);

	if(isNode(p) || isConductor(p)){itested();
		c = finish();
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
