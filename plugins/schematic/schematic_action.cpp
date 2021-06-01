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
#include <QFileDialog>
#include <QGraphicsItem>
#include <QGraphicsSceneEvent>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QRegExpValidator>
#include <QUndoCommand>

#include "misc.h"
#include "node.h"
#include "qucs_app.h"
#include "qucsdoc.h"
#include "qucs_globals.h"
#include "schematic_actions.h"
#include "schematic_action.h"
#include "schematic_dialog.h"
#include "schematic_edit.h"
#include "widget.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#if 0 // TODO
void QucsApp::slotSelectMarker()
{
	qucs::Doc *qd = DocumentTab->current();
  assert(qd);

  incomplete();
//  qd->actionSelectMarker();
}
  showNet = new QAction(tr("Show Last Netlist"), this);
  showNet->setShortcut(Qt::Key_F6);
  showNet->setStatusTip(tr("Show last simulation netlist"));
  showNet->setWhatsThis(
	tr("Show Last Netlist\n\nShows the netlist of the last simulation"));
  connect(showNet, &QAction::toggled, this, &QucsApp::slotShowLastNetlist);

  showMsg = new QAction(tr("Show Last Messages"), this);
  showMsg->setShortcut(Qt::Key_F5);
  showMsg->setStatusTip(tr("Show last simulation messages"));
  showMsg->setWhatsThis(
        tr("Show Last Messages\n\nShow the messages of the last simulation"));
  connect(showMsg, SIGNAL(triggered()), SLOT(slotShowLastMsg()));

#endif
/*--------------------------------------------------------------------------*/
#if 0 // not yet.
class ActionSimulate : public QAction{
public:
	explicit ActionSimulate(QObject* parent) : QAction(parent) { untested();
		setIcon(QIcon(":/bitmaps/gear.png"));
		setText(tr("Simulate"));
		setShortcut(Qt::Key_F2);
		setStatusTip(tr("Simulates the current document"));
		setWhatsThis(tr("Simulate\n\nSimulates the current document"));
	}
};
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class ActionSelect : public QAction{
public:
	explicit ActionSelect(QObject* parent) : QAction(parent) { untested();
		setIcon(QIcon(":/bitmaps/pointer.png"));
		setText(tr("Select"));
		setStatusTip(tr("Activate select mode"));
		setWhatsThis(tr("Select\n\nActivates select mode"));
		setCheckable(true);
	}
};
/*--------------------------------------------------------------------------*/
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
inline Symbol* symbol(Element* e)
{ untested();
	return dynamic_cast<Symbol*>(e);
}
/*--------------------------------------------------------------------------*/
inline Symbol* symbol(QGraphicsItem* g)
{
	auto e=dynamic_cast<ElementGraphics*>(g);
	if(!e){ untested();
		return nullptr;
	}else{
		return symbol(&**e);
	}
}
/*--------------------------------------------------------------------------*/
inline Symbol* symbol(ElementGraphics* e)
{ untested();
	return symbol(&**e);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void setParent_(Object* o, QObject* par)
{
	auto oo = dynamic_cast<QObject*>(o); // prechecked?
	assert(oo);
	oo->setParent(par);
}
/*--------------------------------------------------------------------------*/
void SchematicActions::actionSelectElement(QObject*e)
{ untested();
	maInsertElement->activate(e);
	possiblyToggleAction(maInsertElement, nullptr);
}
/*--------------------------------------------------------------------------*/
void SchematicActions::stash_toolbar(QAction* p)
{
	trace1("stash_toolbar", p);
	assert(_toolbar);
	_toolbar->addAction(p);
}
/*--------------------------------------------------------------------------*/
SchematicActions::SchematicActions(qucs::Doc* ctx)
  : MouseActions(ctx)
{itested();
	assert(ctx);
	assert(doc());
	auto p = dynamic_cast<QWidget*>(ctx);
	assert(p);

	_undoStack = new QUndoStack(this);
	_toolbar = new QToolBar(p); // p?
//	assert(!_toolbar->parent());

	Action* cc;

	cc = action_dispatcher.clone("SchematicDeleteSelection");
	assert(cc);
	setParent_(cc, this);
	maDelete = dynamic_cast<MouseAction*>(cc);
	assert(maDelete);

	cc = action_dispatcher.clone("SchematicActionSelect");
	assert(cc);
	setParent_(cc, this);
	maSelect = dynamic_cast<MouseAction*>(cc);
	assert(maSelect);
//	setCurrentMode(maSelect); does not work.

	cc = action_dispatcher.clone("SchematicAddWire");
	assert(cc);
	setParent_(cc, this);
	maWire = dynamic_cast<MouseAction*>(cc);
	assert(maWire);

	cc = action_dispatcher.clone("ZoomIn");
	assert(cc);
	setParent_(cc, this);
	maZoomIn = dynamic_cast<MouseAction*>(cc);
	assert(maZoomIn);

	cc = action_dispatcher.clone("actionPaste");
	assert(cc);
	setParent_(cc, this);
	maEditPaste = dynamic_cast<MouseAction*>(cc);
	assert(maEditPaste);

//	maZoomOut = new MouseActionZoomOut(*this); // not a mouseaction.

	//  maMove = new MouseActionMove(*this);
	Element const* gnd = symbol_dispatcher["GND"];
	assert(gnd);
	// maInsertGround = new MouseActionNewElement(gnd);
	cc = action_dispatcher.clone("actionNew");
	assert(cc);
	cc->set_param_by_name("dev_type", "GND");
	setParent_(cc, this);
	maInsertGround = dynamic_cast<MouseAction*>(cc);
	assert(maInsertGround);

	cc = action_dispatcher.clone("actionNew");
	assert(cc);
	cc->set_param_by_name("dev_type", "Port");
	setParent_(cc, this);
	maInsertPort = dynamic_cast<MouseAction*>(cc);
	assert(maInsertPort);

	cc = action_dispatcher.clone("actionNew");
	assert(cc);
	cc->set_param_by_name("dev_type", "Eqn");
	setParent_(cc, this);
	maInsertEqn = dynamic_cast<MouseAction*>(cc);
	assert(maInsertEqn);

	cc = action_dispatcher.clone("actionNew");
	assert(cc);
	setParent_(cc, this);
	maInsertElement = dynamic_cast<MouseAction*>(cc);
	assert(maInsertElement);

	cc = action_dispatcher.clone("SchematicActivate");
	assert(cc);
	setParent_(cc, this);
	maActivate = dynamic_cast<MouseAction*>(cc);
	assert(maActivate);

	cc = action_dispatcher.clone("SchematicMirrorX");
	assert(cc);
	setParent_(cc, this);
	maMirrorXaxis = dynamic_cast<MouseAction*>(cc);
	assert(maMirrorXaxis);

	cc = action_dispatcher.clone("SchematicMirrorY");
	assert(cc);
	setParent_(cc, this);
	maMirrorYaxis = dynamic_cast<MouseAction*>(cc);
	assert(maMirrorYaxis);

	cc = action_dispatcher.clone("SchematicRotate");
	assert(cc);
	setParent_(cc, this);
	maRotate = dynamic_cast<MouseAction*>(cc);
	assert(maRotate);

	cc = action_dispatcher.clone("actionPaste");
	assert(cc);
	setParent_(cc, this);
	maEditPaste = dynamic_cast<MouseAction*>(cc);
	assert(maEditPaste);

	assert(_toolbar);

	auto sel = maSelect->createAction(p);
	sel->toggle(); // seems to work, but does not appear "activated" (?)

	stash_toolbar(maSelect->createAction(p));
	stash_toolbar(maWire->createAction(p));
	stash_toolbar(maInsertPort->createAction(p));
	stash_toolbar(maRotate->createAction(p));
	stash_toolbar(maMirrorYaxis->createAction(p));
	stash_toolbar(maMirrorXaxis->createAction(p));
	stash_toolbar(maInsertPort->createAction(p));
	stash_toolbar(maInsertGround->createAction(p));
	stash_toolbar(maInsertEqn->createAction(p));

	ctx->addToolBar(_toolbar);

//	setCurrentMode(maSelect); does not work.
} // SchematicActions::SchematicActions
/*--------------------------------------------------------------------------*/
void SchematicActions::slotRedo()
{
	QUndoStack* u = undoStack();
	assert(u);

	u->redo();
}
/*--------------------------------------------------------------------------*/
void SchematicActions::slotUndo()
{
	QUndoStack* u = undoStack();
	assert(u);

	u->undo();
}
/*--------------------------------------------------------------------------*/
void SchematicActions::executeCommand(QUndoCommand /*??*/ * c)
{
	assert(c);
	assert(doc());
	QUndoStack* u = undoStack();

	if(u){itested();
		u->push(c); // also calls redo

#if 0 // train wreck. must be part of push. fix later.
		if(doc()->_app){ untested();
			assert(doc()->_app->undo);
			doc()->_app->undo->setEnabled(true); // base class??
		}else{ untested();
			incomplete();
		}
#endif
	}else{ untested();
		// forget about it.
		delete c;
	}
}
/*--------------------------------------------------------------------------*/
void SchematicActions::setControls(qucs::Doc* ctx)
{
	// ?
	//MouseActions::setControls(ctx);
	//{
	//connect(undoStack , SIGNAL(canRedoChanged(bool)), redoAction, SLOT(setEnabled(bool)));
	//}
	auto p = dynamic_cast<QWidget*>(ctx); // prech?
	assert(p);
}
/*--------------------------------------------------------------------------*/
#if 0
// (how to?) connect directly?
void SchematicActions::slotEditCopy(QObject* sender)
{ untested();
	if(auto s=dynamic_cast<QAction*>(sender)){ untested();
		possiblyToggleAction(maCopy, sender);
	}else{ untested();
		unreachable();
	}
}
/*--------------------------------------------------------------------------*/
// (how to?) connect directly?
void SchematicActions::slotEditPaste(QObject* sender)
{ untested();
	if(auto s=dynamic_cast<QAction*>(sender)){ untested();
		possiblyToggleAction(maPaste, sender);
	}else{ untested();
		unreachable();
	}
}
#endif
/*--------------------------------------------------------------------------*/
// (how to?) connect directly?
void SchematicActions::slotEditDelete(QObject* sender)
{ untested();
	if(auto s=dynamic_cast<QAction*>(sender)){ untested();
		possiblyToggleAction(maDelete, sender);
	}else{ untested();
		unreachable();
	}
}
/*--------------------------------------------------------------------------*/
SchematicActions::~SchematicActions()
{itested();
//	delete maActivate;
//	delete maDelete;
//	delete maInsertGround;
//	delete maInsertPort;
//	delete maMirrorXaxis;
//	delete maMirrorYaxis;
//	delete maRotate;
//	delete maSelect;
//	delete maWire;
//	delete maZoomIn;
}
/*--------------------------------------------------------------------------*/
SchematicScene const* SchematicActions::scene() const
{ untested();
	auto d = dynamic_cast<QGraphicsView const*>(doc());
	assert(d);
	return dynamic_cast<SchematicScene const*>(d->scene());
}
/*--------------------------------------------------------------------------*/
void SchematicActions::updateViewport()
{itested();
	doc()->updateViewport();	
}
/*--------------------------------------------------------------------------*/
QPoint SchematicActions::snapToGrid(QPointF const&p) const
{
	return scene()->snapToGrid(p);
}
/*--------------------------------------------------------------------------*/
QRegExp Expr_CompProp;
QRegExpValidator Val_CompProp(Expr_CompProp, 0);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
