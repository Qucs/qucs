/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2020, 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MOUSE_ACTION_H
#define MOUSE_ACTION_H

#include "element.h"
#include "qt_compat.h"
#include "schematic_scene.h"
#include "element_graphics.h"
#include <QEvent>
#include <QAction>

class Label;
class MouseActions;
class QAction;
class QMenu;
class QMouseEvent;
class QPainter;
class QUndoCommand;
class Schematic;

// something happens to the mouse on a schematic
// BUG: wrong file. schematic_mouse.h maybe?
// FIXME: this is rather a DocActionHandler it has modes...
//
// (it is a mode. not an action.)
class MouseAction {
public:
	typedef QUndoCommand cmd;
protected:
public:
	explicit MouseAction(MouseActions& ctx)
		:_ctx(ctx), _sender(nullptr){}
	MouseAction(MouseAction const&) = delete;

public:
	virtual ~MouseAction(){}

public:
	cmd* handle(QEvent*);

	virtual cmd* activate(QObject* sender);
	virtual cmd* deactivate();
// private: TODO
	// TODO: only use POS in those
	virtual cmd* move(QEvent*) { return nullptr; }
	virtual cmd* press(QEvent*) { return nullptr; }
	// virtual cmd* grab(QGraphicsSceneEvent*) { return nullptr; }
	virtual cmd* release(QEvent*) { return nullptr; }
	virtual cmd* dblclk(QEvent*) { return nullptr; }

	virtual cmd* generic(QEvent*) { return nullptr; } // remove
	virtual cmd* enter(QEvent*) {itested(); return nullptr; }
	virtual cmd* leave(QEvent*) {itested(); return nullptr; }

	void uncheck();

protected:
	MouseActions& ctx(){return _ctx;}
	QGraphicsView* view(); // what?
	SchematicScene* scene(); // passed to UndoCommands (check: why?)

protected:
	QucsDoc const* doc() const;
	SchematicScene const* scene() const;

protected:
	void sceneAddItem(ElementGraphics*);
	void sceneRemoveItem(ElementGraphics*);

protected:
	QucsDoc* doc(); // BUG _ctx.
	QList<ElementGraphics*> selectedItems(); // BUG. copies.
	QPointF mapToScene(QPoint const& p) const;
	void updateViewport(); // why?
	void setCursor(QCursor const& c);
   bool isNode(pos_t const&) const; // needed??
   bool isConductor(pos_t const&) const;

protected: // UC
	QList<ElementGraphics*> items(const QPointF &pos,
                                 Qt::ItemSelectionMode mode=Qt::IntersectsItemShape,
                                 Qt::SortOrder order = Qt::DescendingOrder) const;

	Node const* nodeAt(pos_t) const;

private:
	MouseActions& _ctx;
	QAction* _sender;
}; // MouseAction

extern QAction *formerAction;


class Label;

#if 0
ElementGraphics* element(ElementMouseAction);
Component* component(ElementMouseAction);
WireLabel* wireLabel(ElementMouseAction);
Diagram* diagram(ElementMouseAction);
Painting* painting(ElementMouseAction);
Graph* graph(ElementMouseAction);
Marker* marker(ElementMouseAction);
Node* node(ElementMouseAction);
Label* label(ElementMouseAction);
WireLabel* wireLabel(ElementMouseAction);
#endif
#endif
