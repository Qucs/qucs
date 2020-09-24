// (c) 2020 Felix Salfelder
// GPLv3+

#include "globals.h"
#include "symbol.h"
#include "viewpainter.h"
#include "schematic_model.h"

namespace{

// BUG/FEATURE. Wires can't go around the corner
// constructing a ghostwire, which is then converted to a (pair of) wires
class GhostWire : public Symbol /*subckt*/ {
public:
	enum angle_mode_t{ am_H=0, am_V=1 };
private:
	GhostWire(GhostWire const&) = default;
public:
	GhostWire() : _mode(am_H) { }
	~GhostWire() { }

private:
	Element* clone() const override{ untested();
		return new GhostWire(*this);
	}

public:
	void paint(ViewPainter *p) const;

private:
	QPoint pmid() const{ untested();
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

private: // symbol
	unsigned numPorts() const override{ return 2; }
	void setParameter(std::string const& n, std::string const& v) override;
	QRectF boundingRect() const override;
	void expand() override;

private:
	angle_mode_t _mode;
	QPoint _p0;
	QPoint _p1;
} w;

void GhostWire::paint(ViewPainter *p) const
{ itested();
	assert(p);

	QPoint pm = pmid();
	trace3("paint", _p0, _p1, pm);
	p->setPen(QPen(Qt::darkBlue, 1));
	p->drawLine(_p0, pm);
	p->drawLine(pm, _p1);
}

void GhostWire::expand()
{ untested();
	assert(!subckt());
	new_subckt();
	assert(subckt());

	Symbol* proto = symbol_dispatcher["Wire"];
	assert(proto);
	auto pm = pmid();

	Element* wc = proto->clone();
	Symbol* w = prechecked_cast<Symbol*>(wc);
	assert(w);

	w->setParameter("$xposition", std::to_string(getX(_p0)));
	w->setParameter("$yposition", std::to_string(getY(_p0)));
	w->setParameter("deltax", std::to_string(getX(pm)-getX(_p0)));
	w->setParameter("deltay", std::to_string(getY(pm)-getY(_p0)));

	trace2("expand", _p0, _p1);
	subckt()->pushBack(w);

	wc = proto->clone();
	w = prechecked_cast<Symbol*>(wc);
	assert(w);

	w->setParameter("$xposition", std::to_string(getX(_p1)));
	w->setParameter("$yposition", std::to_string(getY(_p1)));
	w->setParameter("deltax", std::to_string(getX(pm)-getX(_p1)));
	w->setParameter("deltay", std::to_string(getY(pm)-getY(_p1)));

	trace2("expand", _p0, _p1);
	subckt()->pushBack(w);

	SchematicModel const* sc = subckt();
	assert(sc->wires().size()); // BUG?
}

QRectF GhostWire::boundingRect() const { itested();
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

void GhostWire::setParameter(std::string const& n, std::string const& v)
{ untested();
	trace2("gwsp", n, v);
	if(n=="x0"){ untested();
		_p0.setX(atoi(v.c_str()));
	}else if(n=="y0"){ untested();
		_p0.setY(atoi(v.c_str()));
	}else if(n=="x1"){ untested();
		_p1.setX(atoi(v.c_str()));
	}else if(n=="y1"){ untested();
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

Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "__ma_ghostwire", &w);

} // namespace

// namespace {
// // reuse newElementCommand?
class MakeWire : public QUndoCommand {
public:
	template<class IT>
	MakeWire(SchematicDoc& ctx, IT wires)
	: _ctx(ctx){ untested();
		trace1("newwire", wires.size());
		size_t k = 0;

		// optimise and find tees.
		// TODO //
		// TODO //
		// TODO //
		// TODO //

		for(auto i : wires){ untested();
			++k;
			if(auto e=dynamic_cast<Element*>(i)){ untested();
				auto eg = new ElementGraphics(e);
				ctx.sceneAddItem(eg);
				ctx.takeOwnership(e); // BUG?
				_gfx.push_back(eg);
			}else{ untested();
				unreachable(); // really? use prechecked_cast then.
			}
		}
		setText("create" + QString::number(k) + " wires");
	}
private:
	void undo() override { untested();
		for(auto& d : _gfx){ untested();
			d->hide();
		}
	}
	void redo() override { untested();
		trace1("newwire redo", _gfx.size());
		for(auto& d : _gfx){ untested();
			d->show();
		}
	}
private:
    SchematicDoc& _ctx;
    std::vector<ElementGraphics*> _gfx;
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
{ untested();
	assert(!_gfx.size());
	new_gfx();
	_phase = 1;
	_oldcursor = doc().cursor();
	setCursor(crosshair()); // Qt::CrossCursor);
	return MouseAction::activate(sender);
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionWire::deactivate()
{ untested();
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
{ untested();
	Element* elt = _proto->clone();
	assert(elt);

	auto gfx = new ElementGraphics(elt);
	_gfx.push_back(gfx);
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionWire::release(QMouseEvent* m)
{ untested();
	if(!m){ untested();
		unreachable();
	}else if(m->button() == Qt::RightButton){ untested();
		// toggleMode();
	}
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionWire::press(QEvent* e)
{ untested();
	auto m = dynamic_cast<QMouseEvent*>(e);
	auto se = dynamic_cast<QGraphicsSceneMouseEvent*>(e);

	if(!se){ untested();
		// unreachable(); no.
		// QEvent::GraphicsSceneMousePress == 156
		trace3("wirepress", m, se, e->type());
	}else if(se->button() == Qt::RightButton){ untested();
		toggleMode();
		e->accept();
	}else if(_phase == 1){ untested();
		return press1(se);
		e->accept();
	}else if(_phase == 2){ untested();
		return press2(se);
		e->accept();
	}else{ untested();
		unreachable();
	}

	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionWire::move(QEvent* e)
{ untested();
	if(!_gfx.size()){ untested();
		// no ghost yet.
	}else if(auto se=dynamic_cast<QGraphicsSceneMouseEvent*>(e)){ untested();
		QPointF pos = se->scenePos(); // mapToScene(ev->pos());
		float fX = pos.x();
		float fY = pos.y();

		QPoint xx = doc().setOnGrid(fX, fY);
		fX = getX(xx);
		fY = getY(xx);
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
{ untested();
	assert(ev);
	QPointF pos = ev->scenePos(); // mapToScene(ev->pos());
	float fX = pos.x();
	float fY = pos.y();
	QPoint xx = doc().setOnGrid(fX, fY);
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
	//   Doc->setOnGrid(MAx3, MAy3);
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
{ untested();
	cmd* c=nullptr;

	assert(ev);
	QPointF pos = ev->scenePos(); // mapToScene(ev->pos());

	QPoint xx = doc().snapToGrid(pos);
	auto fX = getX(xx);
	auto fY = getY(xx);

	if(isNode(fX, fY)){ untested();
		std::list<Element*> new_wires;
		for(auto& i: _gfx){ untested();
			auto s = prechecked_cast<Symbol*>(element(i));
			assert(s);
			s->expand();
			SchematicModel const* cs = s->subckt();
			assert(cs);
			trace1("prepare NewWire", cs->wires().size());
			for(Element const* j : cs->wires()){ untested();
				new_wires.push_back(j->clone());
			}
			delete(i);
		}
		c = new MakeWire(doc(), new_wires);
		_gfx.clear();

//	}else if(is_on_wire){ untested();
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
#if 0
	switch(e->button()) {
	case Qt::LeftButton :
		if(MAx1 == 0) { // which wire direction first ?
			if(MAy2 != MAy3)
				set1 = Doc->insertWire(new Wire(MAx3, MAy3, MAx3, MAy2));

			if(MAx2 != MAx3) { untested();
				set2 = set1;
				set1 = Doc->insertWire(new Wire(MAx3, MAy2, MAx2, MAy2));
			}
		}else{ untested();
			if(MAx2 != MAx3)
				set1 = Doc->insertWire(new Wire(MAx3, MAy3, MAx2, MAy3));

			if(MAy2 != MAy3) { untested();
				set2 = set1;
				set1 = Doc->insertWire(new Wire(MAx2, MAy3, MAx2, MAy2));
			}
		}

		if(set1 & 2) { untested();
			// if last port is connected, then...
			if(formerAction) { untested();
				// ...restore old action
				QucsMain->select->setChecked(true);
			}
			else { untested();
				// ...start a new wire
				QucsMain->MouseMoveAction = &MouseActions::MMoveWire1;
				QucsMain->MousePressAction = &MouseActions::MPressWire1;
				QucsMain->MouseDoubleClickAction = 0;
			}
		}

		//ALYS: excessive update. end of function does it.
		//Doc->viewport()->update();

		setDrawn(false);
		if(set1 | set2) Doc->setChanged(true, true);
		MAx3 = MAx2;
		MAy3 = MAy2;
		break;

		/// \todo document right mouse button changes the wire corner
	case Qt::RightButton :
		TODO("Sort out paintAim and GhostLine")

#if 0
			//ALYS - old code preserved because isn't clear - what it was???
			//looks like deletion via painting.
			//i'll delete it after possible clarification from team
			if(MAx1 == 0) { untested();
				Doc->PostPaintEvent (_Line, MAx3, MAy3, MAx3, MAy2); // erase old
				Doc->PostPaintEvent (_Line, MAx3, MAy2, MAx2, MAy2); // erase old
			}
			else { untested();
				Doc->PostPaintEvent (_Line, MAx3, MAy3, MAx2, MAy3); // erase old
				Doc->PostPaintEvent (_Line, MAx2, MAy3, MAx2, MAy2); // erase old
			}
#endif

		MAx2 = int(fX);
		MAy2 = int(fY);
		//     Doc->setOnGrid(MAx2, MAy2);

		MAx1 ^= 1;    // change the painting direction of wire corner
		if(MAx1 == 0) { untested();
			/// \todo paintGhostLineV(Doc,MAx3,MAy3,MAy2);
			///paintGhostLineH(Doc,MAx3,MAy2,MAx2);
		}
		else { untested();
			/// \todo paintGhostLineH(Doc,MAx3,MAy3,MAx2);
			//paintGhostLineV(Doc,MAx2,MAy3,MAy2);
		}
		break;

	default: ;    // avoids compiler warnings
	}
#endif

	/// \todo paintAim(Doc,MAx2,MAy2); //ALYS - added missed aiming cross
	// Doc->viewport()->update();
	return c;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
//} namespace
