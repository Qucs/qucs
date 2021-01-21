/***************************************************************************
    copyright            : (C) 2020 Felix Salfelder

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// user interaction, undo commands
// derived from various Qucs "MouseAction" etc.

#include <QAction>
#include <QMessageBox> // BUG
// #include <QLineEdit> // BUG?
// #include <QRegExp> // BUG?
#include <QRegExpValidator>
#include <QFileDialog>
#include <QUndoCommand>
#include <QGraphicsSceneEvent>
#include <QGraphicsItem>

#include "schematic_doc.h"
#include "qucs.h"
#include "misc.h"
#include "globals.h"
#include "schematic_action.h"
#include "schematic_dialog.h"
#include "schematic_edit.h"
#include "globals.h"

#include "changedialog.h"
//#include "components/component.h" // BUG

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#include "action/zoom.cpp" // for now.
/*--------------------------------------------------------------------------*/
#include "action/wire.cpp" // for now.
/*--------------------------------------------------------------------------*/
class MouseActionSelect : public MouseAction{
public:
	explicit MouseActionSelect(MouseActions& ctx)
		: MouseAction(ctx) {}

private: // override
//	cmd* activate(QAction* sender) override;
	cmd* move(QEvent*) override;
	cmd* press(QEvent*) override;
	cmd* release(QEvent*) override;
	//	cmd* release2(QMouseEvent*); // what is this?
	// cmd* enter(QEvent*) override;
	cmd* dblclk(QEvent*) override;

#if 0
private: // rectangles?  // this was in MouseActions. BUG. remove
	int MAx1;
	int MAy1;
	int MAx2;
	int MAy2;
#endif
private:
	void showSchematicWidget(QWidget*, ElementGraphics*);
	cmd* release_left(QEvent*);

protected:
	void setPos1(QPointF pos){ untested();
		_pos1 = pos;
	}
	QPointF const& pos1() const{ untested();
		return _pos1;
	}

private: // more decoupling
	//ElementMouseAction focusElement;
	bool isMoveEqual; //?
	QPointF _pos1;
}; // MouseActionSelect
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionSelect::dblclk(QEvent* evt)
{ untested();
	incomplete();
	//  Doc->releaseKeyboard();  // allow keyboard inputs again
	//  QucsMain->editText->setHidden(true);
	//  editElement(Doc, Event);
	Element* elt = nullptr;
	ElementGraphics* gfx = nullptr;
	//
	if(auto i = dynamic_cast<ItemEvent*>(evt)){ untested();
		// QList<ElementGraphics*> l;
		gfx = &i->item();
		elt = element(gfx);
		// l.push_back(&i->item());
	}else{ untested();
	}

	if(!elt){ untested();
	}else if(auto ew = elt->schematicWidget(&doc())){ untested();
		trace0("got editElement");
		assert(gfx);
		showSchematicWidget(ew, gfx);
	}else{ untested();
		trace0("no editElement");
		incomplete(); // memory leak
	}

	return nullptr;
}
/*--------------------------------------------------------------------------*/
// not sure I like this.
void MouseActionSelect::showSchematicWidget(QWidget* ew, ElementGraphics* gfx)
{
	if(auto eew=dynamic_cast<SchematicDialog*>(ew)){ untested();
		assert(gfx);
		eew->attach(gfx);
		if(eew->exec() != 1){ untested();
			// done=true;   // dialog is WDestructiveClose
		}else{ untested();
			incomplete();
		}
		//delete eew; // crash. why?
	}else{
	}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// stange glue that should perhaps go into a class derived from QAction and
// replace the button in the toolbar.
template<class CMD>
class MouseActionSelCmd : public MouseAction{
public:
	explicit MouseActionSelCmd(MouseActions& ctx)
		: MouseAction(ctx){}

private:
	cmd* activate(QObject* sender) override;
	cmd* deactivate() override;
//	cmd* move(QEvent*) override;
	cmd* press(QEvent*) override;
	cmd* release(QEvent*) override;
//	cmd* generic(QEvent*) override;
private:
	QCursor _oldcursor;
};
/*--------------------------------------------------------------------------*/
template<class CMD>
QUndoCommand* MouseActionSelCmd<CMD>::deactivate()
{itested();
	setCursor(_oldcursor);
	return MouseAction::deactivate();
}
/*--------------------------------------------------------------------------*/
template<class CMD>
QUndoCommand* MouseActionSelCmd<CMD>::activate(QObject* sender)
{itested();
	MouseAction::activate(sender); // ...

	_oldcursor = doc().cursor();
	setCursor(Qt::CrossCursor);

	auto s = doc().selectedItems();
	bool selected = !s.empty();

	if(selected){itested();
		auto cmd = new CMD(doc(), s);
		return cmd;
	}else{ untested();
		return nullptr;
	}
}
/*--------------------------------------------------------------------------*/
template<class CMD>
QUndoCommand* MouseActionSelCmd<CMD>::press(QEvent* e)
{ untested();
	if(!e){ untested();
		return nullptr;
	}else if(auto i = dynamic_cast<ItemEvent*>(e)){ untested();
		QList<ElementGraphics*> l;
		l.push_back(&i->item());
		return new CMD(doc(), l);
	}else{ untested();
		trace1("delete::scene unknown sender", e->type());
		return nullptr;
	}
} // select::press
/*--------------------------------------------------------------------------*/
template<class CMD>
QUndoCommand* MouseActionSelCmd<CMD>::release(QEvent*)
{ untested();
	incomplete(); // why?
	return nullptr;
} // select::release
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// swapSelection?
class DeleteSelection : public SchematicEdit {
public:
	template<class IT>
	DeleteSelection(SchematicDoc& ctx, IT deletelist)
	  : SchematicEdit(*ctx.sceneHACK()) { untested();
		size_t k = 0;

		qDelete(deletelist);

		setText("delete " + QString::number(k) + " items");
	}
}; // DeleteSelection
typedef MouseActionSelCmd<DeleteSelection> MouseActionDelete;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void transformElement(Element* e, qucsSymbolTransform a, pos_t pivot)
{itested();
	trace1("..", a.degrees_int());
	assert(!(a.degrees_int()%90));
	assert(e);
	if(auto* s=dynamic_cast<Symbol*>(e)){ untested();
		int angle = 0;
		int vflip = 0;
		int hflip = 0;
		int mx = 0;
		int my = 0;
		unsigned r = 0;
		try {itested();
			std::string mxs = s->paramValue("$hflip");
			hflip = atoi(mxs.c_str()); // \pm 1
			//assert(hflip == 1); // for now.
			// 1  |-> 0
			// -1 |-> 1
			mx = (1 - hflip) / 2;
		}catch(ExceptionCantFind const&){ untested();
			incomplete();
		}
		try {itested();
			// this is what legacy used ("mirrorX", vertical flip);
			std::string mys = s->paramValue("$vflip");
			vflip = atoi(mys.c_str());
			// 1  |-> 0
			// -1 |-> 1
			my = (1 - vflip) / 2;
		}catch(ExceptionCantFind const&){ untested();
			unreachable();
		}
		try {itested();
			std::string rs = s->paramValue("$angle");
			angle = atoi(rs.c_str());
			assert(!(angle%90));
			assert(angle<360);
			r = angle/90;
		}catch(ExceptionCantFind const&){ untested();
			unreachable();
		}

		trace3("pretransform", hflip, vflip, angle);

		assert(mx==0 || mx==1);
		assert(my==0 || my==1);
		assert(r < 4); // yikes //

		//assert(hflip==1);

		rotate_after_mirror current(int(r*90), bool(mx), bool(my));
		assert(!(current.degrees_int()%90));
		rotate_after_mirror1_t new_mr = a * current;
		assert(!(new_mr.degrees_int()%90));

		vflip = -2 * int(new_mr.mirror()) + 1;

		s->setParameter(std::string("$hflip"), std::string("1"));
		s->setParameter(std::string("$vflip"), std::to_string(vflip));
		s->setParameter(std::string("$angle"), std::to_string(new_mr.degrees_int()));

		trace3("posttransform", hflip, vflip, new_mr.degrees_int());

		auto p = e->center();
		trace1("posttransform setpos0", p);
		int x = getX(p);
		int y = getY(p);

		x -= pivot.first;
		y -= pivot.second;

		pos_t new_xy(x,y);
		new_xy = a.apply(new_xy);

		x = pivot.first + new_xy.first;
		y = pivot.second + new_xy.second;

		// todo: rounding errors.

		p = pos_t(x, y);
		trace1("posttransform setpos1", p);
		e->setPosition(p);
	// prepareGeometryChange(); // needed??
	}else{ untested();
	}
}
/*--------------------------------------------------------------------------*/
template<class T>
class TransformSelection : public SchematicEdit {
public:
	template<class IT>
	TransformSelection(SchematicDoc& ctx, IT selection, T const& t)
	: SchematicEdit(*ctx.sceneHACK()){itested();
		size_t k = 0;
		// TODO: bounding box center?
		QRectF bb;
		std::vector<std::pair<ElementGraphics*,Element*>> buf;
		for(auto i : selection){itested();
			++k;
			QGraphicsItem const* g = i;//huh?
			if(auto eg=dynamic_cast<ElementGraphics*>(i)){itested();
				auto br = g->boundingRect();
				bb |= i->mapToScene(br).boundingRect(); // translate(eg->pos());
				Element* e = eg->cloneElement();
				buf.push_back(std::make_pair(eg, e));
				// qSwap(eg, e);
			}else{ untested();
				unreachable(); // really? use prechecked_cast then.
			}
		}

		auto center = bb.center();
		pos_t pivot(getX(center), getY(center));
		trace1("transform", pivot);

		for(auto i : buf){
			transformElement(i.second, t, pivot);
			qSwap(i.first, i.second);
		}

		setText("transform " + QString::number(k) + " items");
	}
}; // TransformSelection
/*--------------------------------------------------------------------------*/
static const rotate_after_mirror1_t ninety_degree_transform(90, false);
class RotateSelectionTransform : public TransformSelection<qucsSymbolTransform>{
	typedef TransformSelection<qucsSymbolTransform> base;
public:
	template<class IT>
	RotateSelectionTransform(SchematicDoc& ctx, IT selection)
	  : base(ctx, selection, ninety_degree_transform) {}
};
/*--------------------------------------------------------------------------*/
class MirrorXaxisSelection : public TransformSelection<qucsSymbolTransform>{
	typedef TransformSelection<qucsSymbolTransform> base;
public:
	template<class IT>
	MirrorXaxisSelection(SchematicDoc& ctx, IT selection)
	  : base(ctx, selection, transformFlipHorizontally) {}
};
/*--------------------------------------------------------------------------*/
class MirrorYaxisSelection : public TransformSelection<qucsSymbolTransform>{
	typedef TransformSelection<qucsSymbolTransform> base;
public:
	template<class IT>
	MirrorYaxisSelection(SchematicDoc& ctx, IT selection)
	  : base(ctx, selection, transformFlipVertically) {}
};
/*--------------------------------------------------------------------------*/
typedef MouseActionSelCmd<DeleteSelection> MouseActionDelete;
typedef MouseActionSelCmd<DeleteSelection> MouseActionActivate; // TODO
typedef MouseActionSelCmd<RotateSelectionTransform> MouseActionRotate;
typedef MouseActionSelCmd<MirrorXaxisSelection> MouseActionMirrorXaxis;
typedef MouseActionSelCmd<MirrorYaxisSelection> MouseActionMirrorYaxis;
/*--------------------------------------------------------------------------*/
class MouseActionNewElement : public MouseAction{
public:
	explicit MouseActionNewElement(MouseActions& ctx, Element const* proto=nullptr)
		: MouseAction(ctx), _gfx(nullptr), _proto(proto)
  	{}
private:
	cmd* activate(QObject* sender) override;
	cmd* deactivate() override;
	cmd* move(QEvent*) override;
	cmd* press(QEvent*) override;
	cmd* enter(QEvent*) override;
	cmd* leave(QEvent*) override;
	cmd* release(QEvent*) override;

private:
	cmd* makeNew(QEvent*);
	cmd* rotate(QEvent*);

private:
	ElementGraphics* _gfx;
	Element const* _proto;
};
/*--------------------------------------------------------------------------*/
class NewElementCommand : public SchematicEdit {
public:
	NewElementCommand(SchematicDoc& ctx, ElementGraphics* gfx)
	: SchematicEdit(*ctx.sceneHACK()) { untested();
		assert(gfx->scene());
		assert(!element(gfx)->mutable_owner());
		gfx->hide();
//		ctx.takeOwnership(element(gfx)); // BUG?
		// elment->setOwner(ctx)...?
		setText("NewElement" /*element(gfx)->label()*/); // tr?
		trace0("NewElementCommand::NewElementCommand");
		qInsert(gfx);
	}
	~NewElementCommand(){ untested();
		// _gfx owns element(_gfx)
		// ctx owns _gfx
	}
}; // NewElementCommand
/*--------------------------------------------------------------------------*/
#include <component_widget.h> // not here.
QUndoCommand* MouseActionNewElement::activate(QObject* sender)
{
	if(auto s=dynamic_cast<ComponentListWidgetItem*>(sender)){
		_proto = s->proto(); // better clone?
	}else{
	}
	return MouseAction::activate(sender);
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::release(QEvent* ev)
{ untested();
	QUndoCommand* cmd = nullptr;
	auto m = dynamic_cast<QGraphicsSceneMouseEvent*>(ev);
	if(!m){ untested();
	}else if(m->button() == Qt::LeftButton){ untested();
		cmd = makeNew(ev);
	}else if(m->button() == Qt::RightButton){ untested();
	}
	return cmd;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::makeNew(QEvent* ev)
{ untested();
	// assert(ev->widget=doc->scene()) // or so.
	trace1("RELEASE", ev->type());
	if(ev->type() == QEvent::MouseButtonRelease){ itested();
	}else{ untested();
		unreachable();
	}

	assert(element(_gfx));
	auto elt = element(_gfx);
	assert(elt);

	if(auto se=dynamic_cast<QGraphicsSceneMouseEvent*>(ev)){ untested();
		QPointF pos = se->scenePos();
		QPoint xx = doc().snapToGrid(pos);
		_gfx->setPos(xx);
	}else{
	}

	cmd* c = new NewElementCommand(doc(), _gfx);
	// _gfx = nullptr;

	{ untested();
		_gfx = _gfx->clone(); // new ElementGraphics(elt);
		doc().sceneAddItem(_gfx); // does not attach.
		assert(!element(_gfx)->scope());
	}

	ev->accept();
	return c;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::deactivate()
{ untested();
	// assert(!attached);
	doc().sceneRemoveItem(_gfx);
	delete _gfx; // CHECK: who owns _elt?
	_gfx = nullptr;
	incomplete();
	return MouseAction::deactivate();
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::move(QEvent* ev)
{ untested();
	QPointF sp;
	trace1("move", ev->type());
	if(auto ee=dynamic_cast<QMouseEvent*>(ev)){ untested();
		unreachable();
		QPointF wp;
		wp = ee->localPos(); // use oldPos?
		sp = mapToScene(wp.toPoint());
	}else if(auto ee=dynamic_cast<QGraphicsSceneMouseEvent*>(ev)){ untested();
		sp = ee->scenePos();

		QPoint xx = doc().snapToGrid(sp);
		sp = xx;
	}else{ untested();
		unreachable();
	}

	if(_gfx){ untested();
		_gfx->setPos(sp.x(), sp.y());
	}else{ untested();
		unreachable();
	}

	ev->accept();
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::enter(QEvent* ev)
{ untested();
	trace1("new enter", ev->type());
	auto ee = prechecked_cast<QEnterEvent*>(ev);
	assert(ee);
	
	auto wp = ee->localPos();

	SchematicDoc* d = &doc();
	auto sp = d->mapToScene(wp.toPoint());

	Element* elt;
	if(!_gfx){ untested();
		assert(_proto);
		elt = _proto->clone_instance();
		if(auto sym=dynamic_cast<Symbol const*>(elt)){
			elt->setLabel(sym->typeName());
		}else{
		}
		elt->setPosition(pos_t(sp.x(), sp.y()));
		_gfx = new ElementGraphics(elt); // BUG
	}else{ untested();
		_gfx->setPos(sp.x(), sp.y());
	}
	
//	scene().addItem(_gfx);
	doc().sceneAddItem(_gfx);

	ev->accept();
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::leave(QEvent* ev)
{ untested();
	sceneRemoveItem(_gfx);
	ev->accept();
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::rotate(QEvent*)
{ untested();
	if(!_gfx){ untested();
		unreachable();
	}else if(dynamic_cast<Symbol*>(element(_gfx))){ untested();
		// always do this?
		_gfx->transform(ninety_degree_transform);
	}else{ untested();
		unreachable();
	}
	return nullptr;
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionNewElement::press(QEvent* ev)
{ untested();
	auto a = dynamic_cast<QMouseEvent*>(ev);
	auto m = dynamic_cast<QGraphicsSceneMouseEvent*>(ev);
	QUndoCommand* cmd = nullptr;
	if(a){ untested();
		unreachable();
		// somehow it is a scene event??
	}else if(!m){ untested();
		trace1("MouseActionNewElement::press", ev->type());
		unreachable();
	}else if(m->button() == Qt::LeftButton){ untested();
	}else if(m->button() == Qt::RightButton){ untested();
		cmd = MouseActionNewElement::rotate(ev);
		ev->accept(); // really?
	}else{ untested();
		unreachable();
	}
	return cmd;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#include "action/move.cpp" // for now.
#include "action/paste.cpp" // for now.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// was: MouseActions::MMoveSelect
QUndoCommand* MouseActionSelect::move(QEvent *)
{itested();
	// obsolete?
	if(isMoveEqual) {itested();
		// square?
	}else{itested();
	}

	return nullptr;
}
/*--------------------------------------------------------------------------*/
// was: MouseActions::MPressSelect
QUndoCommand* MouseActionSelect::press(QEvent*)
{itested();

	incomplete();
	return nullptr;
#if 0
	QMouseEvent* e;
	SchematicDoc* Doc = &doc();
	assert(Doc);
	QPointF pos = Doc->mapToScene(e->pos());
	trace2("sel::press", pos, e->pos());
	setPos1(pos);
	float fX=pos.x();
	float fY=pos.y();

	bool Ctrl = e->modifiers().testFlag(Qt::ControlModifier);

	int No=0;

	// memorise first click position
	//MAx1 = int(fX);
	//MAy1 = int(fY);

	focusElement = ctx().selectElement(e->pos(), Ctrl, &No);
	isMoveEqual = false;   // moving not neccessarily square

	incomplete(); //this does not add up.
#if 0
	if(!focusElement){ untested();
	}else if(focusElement->Type == isDiagramHScroll){ untested();
		// BUG: move to selectElement? what is MAy1?!
		MAy1 = MAx1;
	}else{ untested();
	}
#endif

	if(!focusElement){ untested();
		qDebug() << "MPressSelect miss" << e->pos() << pos;
	}else if(focusElement->Type == isPaintingResize){ untested();
		incomplete(); // delegate. how?
#if 0
		focusElement->Type = isPainting;
		QucsMain->MouseReleaseAction = &MouseActions::MReleaseResizePainting;
		QucsMain->MouseMoveAction = &MouseActions::MMoveResizePainting;
		QucsMain->MousePressAction = 0;
		QucsMain->MouseDoubleClickAction = 0;
		Doc->grabKeyboard();  // no keyboard inputs during move actions
		// Update matching wire label highlighting
		Doc->highlightWireLabels ();
		return nullptr;
	}else if(focusElement->Type == isDiagramResize){ untested();
		incomplete();

		if(((Diagram*)focusElement)->name().left(4) != "Rect")
			if(((Diagram*)focusElement)->name().at(0) != 'T')
				if(((Diagram*)focusElement)->name() != "Curve")
					/* if(((Diagram*)focusElement)->name() != "Waveac")
						if(((Diagram*)focusElement)->name() != "Phasor")*/
					isMoveEqual = true;  // diagram must be square

		focusElement->Type = isDiagram;
		MAx1 = focusElement->cx_();
		MAx2 = focusElement->x2_();

		focusElement->someDiagramStateCallback()

			// old:
			// if(diagram(focusElement)->State & 1) { untested();
			//   MAx1 += MAx2;
			//   MAx2 *= -1;
			// }

			MAy1 =  focusElement->cy_();
		MAy2 = -focusElement->y2_();
		if(((Diagram*)focusElement)->State & 2) { untested();
			MAy1 += MAy2;
			MAy2 *= -1;
		}

		 // diagram_action?
		QucsMain->MouseReleaseAction = &MouseActions::MReleaseResizeDiagram;
		QucsMain->MouseMoveAction = &MouseActions::MMoveSelect;
		QucsMain->MousePressAction = 0;
		QucsMain->MouseDoubleClickAction = 0;
		Doc->grabKeyboard(); // no keyboard inputs during move actions
		// Update matching wire label highlighting
		Doc->highlightWireLabels ();
		return nullptr;

	}else if(focusElement->Type == isDiagramHScroll
			|| focusElement->Type == isDiagramVScroll){ untested();
		incomplete();

		focusElement->Type = isDiagram; // reset happens here. FIXME.

		auto d=diagram(focusElement); // is this necessary?!
		assert(d);
		No = d->scroll(MAy1);

		switch(No) {
		case 1:
			Doc->setChanged(true, true, 'm'); // 'm' = only the first time
			break;
		case 2:  // move scroll bar with mouse cursor
			QucsMain->MouseMoveAction = &MouseActions::MMoveScrollBar;
			QucsMain->MousePressAction = 0;
			QucsMain->MouseDoubleClickAction = 0;
			Doc->grabKeyboard();  // no keyboard inputs during move actions

			// Remember inital scroll bar position.
			MAx2 = int(d->xAxis_limit_min());
			// Update matching wire label highlighting
			Doc->highlightWireLabels ();
			return;
		}
		// Update matching wire label highlighting
		Doc->highlightWireLabels ();
		Doc->viewport()->update();
		drawn = false;
		return;

	}else if(focusElement->Type == isComponentText){ untested();
		incomplete();

		focusElement->Type &= (~isComponentText) | isComponent;

		MAx3 = No;
		QucsMain->slotApplyCompText();
		// Update matching wire label highlighting
		Doc->highlightWireLabels ();
		return;

	}else if(auto n=node(focusElement)){ untested();
		(void)n;
		if (QucsSettings.NodeWiring) { untested();
			incomplete();

			MAx1 = 0;   // paint wire corner first up, then left/right
			MAx3 = focusElement->cx_();  // works even if node is not on grid
			MAy3 = focusElement->cy_();
			QucsMain->MouseMoveAction = &MouseActions::MMoveWire2;
			QucsMain->MousePressAction = &MouseActions::MPressWire2;
			QucsMain->MouseReleaseAction = 0; // if function is called from elsewhere
			QucsMain->MouseDoubleClickAction = 0;

			formerAction = QucsMain->select; // to restore action afterwards
			QucsMain->activeAction = QucsMain->insWire;

			QucsMain->select->blockSignals(true);
			QucsMain->select->setChecked(false);
			QucsMain->select->blockSignals(false);

			QucsMain->insWire->blockSignals(true);
			QucsMain->insWire->setChecked(true);
			QucsMain->insWire->blockSignals(false);
			// Update matching wire label highlighting
			Doc->highlightWireLabels ();
			return;
		}else{ untested();
		}
#endif
	}else{ untested();
		// default case
		// unreachable?
	}

	QucsMain->MousePressAction = 0;
	QucsMain->MouseDoubleClickAction = 0;
	Doc->grabKeyboard();  // no keyboard inputs during move actions
	Doc->viewport()->update();
	//setDrawn(false);

	// Update matching wire label highlighting
	assert(Doc);
	// Doc->highlightWireLabels ();
//	e->ignore(); // handle in QGraphicsView?
	return nullptr;
#endif
} // select::press
/*--------------------------------------------------------------------------*/
// was MouseActions::MReleaseSelect(SchematicDoc *Doc, QMouseEvent *Event)
QUndoCommand* MouseActionSelect::release(QEvent *ev)
{itested();
	QUndoCommand* cmd = nullptr;
	auto m = dynamic_cast<QMouseEvent*>(ev);
	if(!m){ untested();
	}else if(m->button() == Qt::LeftButton){itested();
		cmd = release_left(ev);
	}else if(m->button() == Qt::RightButton){ untested();
	}
	return cmd;
}
/*--------------------------------------------------------------------------*/
static QPoint getDelta(ElementGraphics* e)
{itested();
	auto p = e->pos().toPoint();
	assert(element(e));
	auto p1_ = element(e)->center();
	auto p1 = QPoint(getX(p1_), getY(p1_));
	return p - p1;
}
/*--------------------------------------------------------------------------*/
// it's a "wire" if it has two ports that connect to the same net.
static bool isWire(Symbol const* e)
{
	assert(e);
	if(e->numPorts()!=2){
		return false;
	}else{
	}

	assert(e->portValue(0));
	assert(e->portValue(1));

	return e->portValue(0)->net() == e->portValue(1)->net();
}
/*--------------------------------------------------------------------------*/
inline Symbol* symbol(Element* e)
{
	return dynamic_cast<Symbol*>(e);
}
inline Symbol* symbol(QGraphicsItem* g)
{
	auto e=dynamic_cast<ElementGraphics*>(g);
	if(!e) return nullptr;
	return symbol(e->operator->());
}
inline Symbol* symbol(ElementGraphics* e)
{
	if(!e) return nullptr;
	return symbol(e->operator->());
}

/*--------------------------------------------------------------------------*/
static void selectWireLine(ElementGraphics *g)
{
	Symbol* s = symbol(g);
	assert(isWire(s));
	auto scn = g->scene();
	assert(scn);
	
	for(unsigned i=0; i<s->numPorts(); ++i){
		auto pos = makeQPointF(s->nodePosition(i));
		auto items = scn->items(pos);
		if(items.size()==2){

			for(auto ii : items){
				Symbol* si = symbol(ii);
				if(!si){
				}else if(g->isSelected()==ii->isSelected()){
				}else if(si == s){
				}else if(isWire(si)){
					ii->setSelected(g->isSelected());
					selectWireLine(ii);
				}else{
				}
			}
		}else{ untested();
		}
	}
}
/*--------------------------------------------------------------------------*/
QUndoCommand* MouseActionSelect::release_left(QEvent *e)
{itested();
    auto m = prechecked_cast<QMouseEvent*>(e);
	 if(!m){
		 unreachable();
		 return nullptr;
	 }else{
	 }
    bool ctrl = m->modifiers().testFlag(Qt::ControlModifier);

	if(!ctrl) {itested();
		incomplete();
	}else{itested();
	}

	cmd* c = nullptr;

	auto s = doc().selectedItems();
	if(s.isEmpty()){ untested();
	}else{itested();

		auto delta = getDelta(s.first());
#ifndef NDEBUG
		for(auto i : s){
			trace2("check delta", delta, getDelta(i));
			assert(delta == getDelta(i));
		}
#endif
		int fX = int(delta.x());
		int fY = int(delta.y());

		if(fX || fY){itested();
			trace1("possible move", delta);
			c = new MoveSelection(delta, doc(), s);
		}else{itested();
		}
	}

	if(c){
	}else if(s.size()!=1){
	}else if(!symbol(s.front())){
	}else if(m->button() == Qt::LeftButton){itested();
			// if it's a wire, select the whole thing?
			// (what is a wire?)
		if(isWire(symbol(s.front()))) { untested();
			incomplete();
#if 1
			selectWireLine(s.front());
#endif
		}else{itested();
		}
	}else{itested();
	}

	doc().releaseKeyboard();
//	Doc->highlightWireLabels ();
	updateViewport();
	// drawn = false;
	return c;
} // select::release
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SchematicActions::SchematicActions(SchematicDoc& ctx)
  : MouseActions(ctx)
{itested();

	// not entirely clear how to refactor this
	// maybe dispatch mouse actions.
	// or merge into QAction buttons (connect as needed?)

	maDelete = new MouseActionDelete(*this);
	maSelect = new MouseActionSelect(*this);
	maWire = new MouseActionWire(*this);
	maZoomIn = new MouseActionZoomIn(*this);
//	maZoomOut = new MouseActionZoomOut(*this); // not a mouseaction.

	//  maMove = new MouseActionMove(*this);
	Element const* gnd = symbol_dispatcher["GND"];
	assert(gnd);
	maInsertGround = new MouseActionNewElement(*this, gnd);

	Element const* port = symbol_dispatcher["Port"];
	assert(port);
	maInsertPort = new MouseActionNewElement(*this, port);

	Element const* eqn = symbol_dispatcher["Eqn"];
	assert(eqn);
	maInsertEqn = new MouseActionNewElement(*this, eqn);

	maInsertElement = new MouseActionNewElement(*this);

	maActivate = new MouseActionActivate(*this);
	maMirrorXaxis = new MouseActionMirrorXaxis(*this);
	maMirrorYaxis = new MouseActionMirrorYaxis(*this);
	maRotate = new MouseActionRotate(*this);
	maEditPaste = new MouseActionPaste(*this);

	// this was in App previously, and scattered across a couple of pointer hacks.
	// possibly initialised to "select". recheck.
	_maCurrent = maSelect;
}
/*--------------------------------------------------------------------------*/
SchematicActions::~SchematicActions()
{itested();
	delete maActivate;
	delete maDelete;
	delete maInsertGround;
	delete maInsertPort;
	delete maMirrorXaxis;
	delete maMirrorYaxis;
	delete maRotate;
	delete maSelect;
	delete maWire;
	delete maZoomIn;
}
/*--------------------------------------------------------------------------*/
SchematicScene const* SchematicActions::scene() const
{ untested();
	auto d=dynamic_cast<SchematicDoc const*>(&_doc);
	assert(d);
	return d->scene();
}
/*--------------------------------------------------------------------------*/
SchematicDoc* SchematicActions::doc()
{ untested();
	auto d=dynamic_cast<SchematicDoc*>(&_doc);
	assert(d);
	return d;
}
/*--------------------------------------------------------------------------*/
void SchematicActions::updateViewport()
{ untested();
	doc()->updateViewport();	
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

QRegExp Expr_CompProp;
QRegExpValidator Val_CompProp(Expr_CompProp, 0);

// forward to mouseActions... TODO rearrange.
void SchematicDoc::actionInsertGround(QAction* sender)
{ untested();
	possiblyToggleAction(schematicActions().maInsertGround, sender);
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::actionInsertEquation(QAction* sender)
{
	possiblyToggleAction(schematicActions().maInsertEqn, sender);
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::actionInsertPort(QAction* sender)
{ untested();
	possiblyToggleAction(schematicActions().maInsertPort, sender);
}
/*--------------------------------------------------------------------------*/
void SchematicDoc::actionSelect(QAction* sender)
{itested();
	// sender is a button. maSelect is an action. connect the two.
	// this looks a bit redundant (but later...)
	possiblyToggleAction(schematicActions().maSelect, sender);

} // SchematicDoc::actionSelect
/*--------------------------------------------------------------------------*/
void SchematicDoc::actionOnGrid(QAction* sender)
{ untested();
	possiblyToggleAction(schematicActions().maOnGrid, sender);
//  performToggleAction(on, App->onGrid, &SchematicDoc::elementsOnGrid,
//		&MouseActions::MMoveOnGrid, &MouseActions::MPressOnGrid);
}

void SchematicDoc::actionEditRotate(QAction* sender)
{ untested();
	possiblyToggleAction(schematicActions().maRotate, sender);
//  performToggleAction(on, App->editRotate, &SchematicDoc::rotateElements,
//		&MouseActions::MMoveRotate, &MouseActions::MPressRotate);
}

void SchematicDoc::actionEditMirrorX(QAction* sender)
{ untested();
	possiblyToggleAction(schematicActions().maMirrorYaxis, sender);
//  performToggleAction(on, App->editMirror, &SchematicDoc::mirrorXComponents,
//		&MouseActions::MMoveMirrorX, &MouseActions::MPressMirrorX);
}

void SchematicDoc::actionEditMirrorY(QAction* sender)
{ untested();
	possiblyToggleAction(schematicActions().maMirrorXaxis, sender);
//  performToggleAction(on, App->editMirrorY, &SchematicDoc::mirrorYComponents,
//		&MouseActions::MMoveMirrorY, &MouseActions::MPressMirrorY);
}

void SchematicDoc::actionEditActivate(QAction* sender)
{ untested();
	possiblyToggleAction(schematicActions().maActivate, sender);
}

void SchematicDoc::actionEditDelete(QAction* sender)
{ untested();
	possiblyToggleAction(schematicActions().maDelete, sender);

	updateViewport();
//  assert(mouseActions());
//  mouseActions()->setDrawn(false);
}

void SchematicDoc::actionSetWire(QAction* sender)
{itested();
	possiblyToggleAction(schematicActions().maWire, sender);
}

void SchematicDoc::actionInsertLabel(QAction*)
{ untested();
//  possiblyToggleAction(schematicActions().maInsertLabel, sender);
  incomplete();
//  performToggleAction(on, App->insLabel, 0,
//		&MouseActions::MMoveLabel, &MouseActions::MPressLabel);
  // mouseAction = mouseActions().maInsLabel;
}

void SchematicDoc::actionSetMarker(QAction*)
{ untested();
  incomplete();
//  performToggleAction(on, App->setMarker, 0,
//		&MouseActions::MMoveMarker, &MouseActions::MPressMarker);
  // mouseAction = mouseActions().maSetMarker;
}

void SchematicDoc::actionMoveText(QAction*)
{ untested();
  incomplete();
//  performToggleAction(on, App->moveText, 0,
//		&MouseActions::MMoveMoveTextB, &MouseActions::MPressMoveText);
  // mouseAction = mouseActions().maMoveText;
}

void SchematicDoc::actionZoomIn(QAction* sender)
{ untested();
	possiblyToggleAction(schematicActions().maZoomIn, sender);
}

#if 0 // obsolete.
void SchematicDoc::actionInsertEquation(QAction*)
{ untested();
  App->hideEdit(); // disable text edit of component property
  App->MouseReleaseAction = 0;
  App->MouseDoubleClickAction = 0;

  if(!on) { untested();
    App->MouseMoveAction = 0;
    App->MousePressAction = 0;
    App->activeAction = 0;   // no action active
    return;
  }
  if(App->activeAction) { untested();
    App->activeAction->blockSignals(true); // do not call toggle slot
    App->activeAction->setChecked(false);       // set last toolbar button off
    App->activeAction->blockSignals(false);
  }
  App->activeAction = App->insEquation;

  if(mouseActions().hasElem()){ untested();
    incomplete(); // undo??
    delete mouseActions().getElem();  // delete previously selected component
  }else{ untested();
  }

  Symbol* sym=symbol_dispatcher.clone("Eqn");
  assert(sym);
  mouseActions().setElem(prechecked_cast<Component*>(sym));
  assert(mouseActions().hasElem());

  if(mouseActions().wasDrawn()){ untested();
    updateViewport();
  }else{ untested();
  }
  mouseActions().setDrawn(false);
  App->MouseMoveAction = &MouseActions::MMoveElement;
  App->MousePressAction = &MouseActions::MPressElement;
}
#endif

void SchematicDoc::actionEditPaste(QAction* sender)
{ untested();
	possiblyToggleAction(schematicActions().maEditPaste, sender);
#if 0
	// if it's not a text doc, prevent the user from editing
	// while we perform the paste operation
	App->hideEdit();

	if(!on) { untested();
		App->MouseMoveAction = 0;
		App->MousePressAction = 0;
		App->MouseReleaseAction = 0;
		App->MouseDoubleClickAction = 0;
		App->activeAction = 0;   // no action active
		if(mouseActions().wasDrawn()) { untested();
			viewport()->update();
		}
		return;
	}else{ untested();
        }

	if(!mouseActions()->pasteElements(this)) { untested();
		App->editPaste->blockSignals(true); // do not call toggle slot
		App->editPaste->setChecked(false);       // set toolbar button off
		App->editPaste->blockSignals(false);
		return;   // if clipboard empty
	}else{ untested();
        }

	uncheckActive();
	App->activeAction = App->editPaste;

        mouseActions().setDrawn(false)
	App->MouseMoveAction = &MouseActions::MMovePaste;
	mouseActions()->movingRotated = 0;
	App->MousePressAction = 0;
	App->MouseReleaseAction = 0;
	App->MouseDoubleClickAction = 0;

#endif
}

void SchematicDoc::actionSelectElement(QObject*e)
{ untested();
  schematicActions().maInsertElement->activate(e);
  possiblyToggleAction(schematicActions().maInsertElement, nullptr);
}

// is this still in use?
void SchematicDoc::actionEditUndo(QAction*)
{itested();
  // really?
	assert(_app);
  _app->hideEdit(); // disable text edit of component property

  undo();
  updateViewport();
  assert(mouseActions());
  setDrawn(false);
}

// is this still in use?
void SchematicDoc::actionEditRedo(QAction*)
{ untested();
	assert(_app);
  _app->hideEdit(); // disable text edit of component property

  redo();
  updateViewport();
  setDrawn(false);
}

void SchematicDoc::actionAlign(int what)
{ untested();
	assert(_app);
  _app->hideEdit(); // disable text edit of component property

  if(!aligning(what)){ untested();
    QMessageBox::information(this, tr("Info"),
		      tr("At least two elements must be selected !"));
  }
  updateViewport();
  assert(mouseActions());
  setDrawn(false);
}

void SchematicDoc::actionDistrib(int d)
{ untested();
  assert(_app);
  _app->hideEdit(); // disable text edit of component property

  if (d==0){ untested();
	  distributeHorizontal();
  }else if(d==1){ untested();
	  distributeVertical();
  }else{ untested();
	  unreachable();
  }
  updateViewport();
  setDrawn(false);
}

void SchematicDoc::actionSelectAll(QAction*)
{ untested();
	incomplete();
    // selectElements(INT_MIN, INT_MIN, INT_MAX, INT_MAX, true);
    updateViewport();
}

void SchematicDoc::actionSelectMarker()
{ untested();
	assert(_app);
  _app->hideEdit(); // disable text edit of component property

  selectMarkers();
  updateViewport();
  setDrawn(false);
}

void SchematicDoc::actionChangeProps(QAction*)
{ untested();
	ChangeDialog *d = new ChangeDialog(this);
	if(d->exec() == QDialog::Accepted) { untested();
		setChanged(true, true);
		updateViewport();
	}
}

void SchematicDoc::actionCursor(arrow_dir_t)
{ untested();
	incomplete();
#ifdef USE_SCROLLVIEW
	int sign = 1;

	if(dir==arr_left){ untested();
		sign = -1;
	}

	if(App->editText->isHidden()) { untested();
		// for edit of component property ?
		auto movingElements = cropSelectedElements();
		int markerCount=0;
		for(auto const& i : movingElements){ untested();
			if(marker(i)){ untested();
				++markerCount;
			}
		}

		if((movingElements.count() - markerCount) < 1) { // all selections are markers
			incomplete();
#if 0
			if(markerCount > 0) {  // only move marker if nothing else selected
				markerMove(dir, &movingElements);
			} else if(dir==arr_up) { untested();
				// nothing selected at all
				if(scrollUp(verticalScrollBar()->singleStep()))
					scrollBy(0, -verticalScrollBar()->singleStep());
			} else if(dir==arr_down) { untested();
				if(scrollDown(-verticalScrollBar()->singleStep()))
					scrollBy(0, verticalScrollBar()->singleStep());
			} else if(dir==arr_left) { untested();
				if(scrollLeft(horizontalScrollBar()->singleStep()))
					scrollBy(-horizontalScrollBar()->singleStep(), 0);
			} else if(dir==arr_right) { untested();
				if(scrollRight(-horizontalScrollBar()->singleStep()))
					scrollBy(horizontalScrollBar()->singleStep(), 0);
			}else{ untested();
				// unreachable. TODO: switch
			}

			updateViewport();
			mouseActions()->drawn = false;
#endif
		}else if(dir==arr_up || dir==arr_down){ untested();
			// some random selection, put it back
			mouseActions()->moveElements(movingElements, 0, ((dir==arr_up)?-1:1) * GridY);
			mouseActions()->MAx3 = 1;  // sign for moved elements
			mouseActions()->endElementMoving(this, &movingElements);
		}else if(dir==arr_left || dir==arr_right){ untested();
			mouseActions()->moveElements(movingElements, sign*GridX, 0);
			mouseActions()->MAx3 = 1;  // sign for moved elements
			mouseActions()->endElementMoving(this, &movingElements);
		}else{ untested();
			//unreachable(); //TODO: switch.
		}

	}else if(dir==arr_up){ // BUG: redirect.
		if(mouseActions()->MAx3 == 0) return;  // edit component namen ?
		Component *pc = component(mouseActions()->focusElement);
		Property *pp = pc->Props.at(mouseActions()->MAx3-1);  // current property
		int Begin = pp->Description.indexOf('[');
		if(Begin < 0) return;  // no selection list ?
		int End = pp->Description.indexOf(App->editText->text(), Begin); // current
		if(End < 0) return;  // should never happen
		End = pp->Description.lastIndexOf(',', End);
		if(End < Begin) return;  // was first item ?
		End--;
		int Pos = pp->Description.lastIndexOf(',', End);
		if(Pos < Begin) Pos = Begin;   // is first item ?
		Pos++;
		if(pp->Description.at(Pos) == ' ') Pos++; // remove leading space
		App->editText->setText(pp->Description.mid(Pos, End-Pos+1));
		App->editText->selectAll();
	}else if(dir==arr_down) { // BUG: redirect.
		if(mouseActions()->MAx3 == 0) return;  // edit component namen ?
		Component *pc = component(mouseActions()->focusElement);
		Property *pp = pc->Props.at(mouseActions()->MAx3-1);  // current property
		int Pos = pp->Description.indexOf('[');
		if(Pos < 0) return;  // no selection list ?
		Pos = pp->Description.indexOf(App->editText->text(), Pos); // current list item
		if(Pos < 0) return;  // should never happen
		Pos = pp->Description.indexOf(',', Pos);
		if(Pos < 0) return;  // was last item ?
		Pos++;
		if(pp->Description.at(Pos) == ' ') Pos++; // remove leading space
		int End = pp->Description.indexOf(',', Pos);
		if(End < 0) {  // is last item ?
			End = pp->Description.indexOf(']', Pos);
			if(End < 0) return;  // should never happen
		}
		App->editText->setText(pp->Description.mid(Pos, End-Pos));
		App->editText->selectAll();
	}else{ untested();

	}
#endif
} // actionCursor

void SchematicDoc::actionApplyCompText()
{ untested();
#if 0 // what is focusElement??
	auto Doc=this;
	auto editText=App->editText;

	QString s;
	QFont f = QucsSettings.font;
	f.setPointSizeF(Scale * float(f.pointSize()) );
	editText->setFont(f);

	Property  *pp = 0;
	Component *pc = component(mouseActions()->focusElement);
	if(!pc) return;  // should never happen
	mouseActions()->MAx1 = pc->cx_() + pc->tx;
	mouseActions()->MAy1 = pc->cy_() + pc->ty;

	int z, n=0;  // "n" is number of property on screen
	pp = pc->Props.first();
	for(z=mouseActions()->MAx3; z>0; z--) {  // calculate "n"
		if(!pp) {  // should never happen
			App->hideEdit();
			return;
		}
		if(pp->display) n++;   // is visible ?
		pp = pc->Props.next();
	}

	pp = 0;
	if(mouseActions()->MAx3 > 0)  pp = pc->Props.at(mouseActions()->MAx3-1); // current property
	else s = pc->name();

	if(!editText->isHidden()) {   // is called the first time ?
		// no -> apply value to current property
		if(mouseActions()->MAx3 == 0) {   // component name ?
			if(!editText->text().isEmpty())
				if(pc->name() != editText->text()) { untested();
					auto pc2=Doc->find_component( editText->text());
					if(!pc2) { untested();
						pc->obsolete_name_override_hack( editText->text() );
						setChanged(true, true);  // only one undo state
					}
				}
		}
		else if(pp) {  // property was applied
			if(pp->Value != editText->text()) { untested();
				pp->Value = editText->text();
				recreateSymbol(pc);  // because of "Num" and schematic symbol
				setChanged(true, true); // only one undo state
			}
		}

		n++;     // next row on screen
		(mouseActions()->MAx3)++;  // next property
		pp = pc->Props.at(mouseActions()->MAx3-1);  // search for next property

		viewport()->update();
                mouseActions().setDrawn(false);

		if(!pp) {     // was already last property ?
			App->hideEdit();
			return;
		}


		while(!pp->display) {  // search for next visible property
			(mouseActions()->MAx3)++;  // next property
			pp = pc->Props.next();
			if(!pp) {     // was already last property ?
				App->hideEdit();
				return;
			}
		}
	}

	// avoid seeing the property text behind the line edit
	if(pp)  // Is it first property or component name ?
		s = pp->Value;
	editText->setMinimumWidth(editText->fontMetrics().width(s)+4);

  incomplete();
  /// Doc->contentsToViewport(int(Doc->Scale * float(view->MAx1 - Doc->ViewX1)),
  ///			 int(Doc->Scale * float(view->MAy1 - Doc->ViewY1)),
  ///	 view->MAx2, view->MAy2);
	editText->setReadOnly(false);
	if(pp) {  // is it a property ?
		s = pp->Value;
		mouseActions()->MAx2 += editText->fontMetrics().width(pp->Name+"=");
		if(pp->Description.indexOf('[') >= 0)  // is selection list ?
			editText->setReadOnly(true);
		Expr_CompProp.setPattern("[^\"]*");
		if(!pc->showName) n--;
	}
	else   // it is the component name
		Expr_CompProp.setPattern("[\\w_]+");
	Val_CompProp.setRegExp(Expr_CompProp);
	editText->setValidator(&Val_CompProp);

	z = editText->fontMetrics().lineSpacing();
	mouseActions()->MAy2 += n*z;
	editText->setText(s);
	misc::setWidgetBackgroundColor(editText, QucsSettings.BGColor);
	editText->setFocus();
	editText->selectAll();
	// make QLineEdit editable on mouse click
	QPoint p = QPoint(mouseActions()->MAx2, mouseActions()->MAy2);
	editText->setParent(Doc->viewport());
	App->editText->setGeometry(p.x(), p.y(), App->editText->width(), App->editText->height());
	App->editText->show();
#endif
}

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
