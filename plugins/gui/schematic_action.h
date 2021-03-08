/***************************************************************************
    copyright            : (C) 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_SCHEMATIC_ACTION
#define QUCS_SCHEMATIC_ACTION
/*--------------------------------------------------------------------------*/
#include "doc_actions.h"
#include "schematic_edit.h"
/*--------------------------------------------------------------------------*/
using namespace qucs;
// merge into scene altogether?
class SchematicActions : public MouseActions{
public:
	explicit SchematicActions(qucs::Doc*);
	~SchematicActions();

private: // MouseActions
	// void handle(QEvent*) override;
	void setControls(qucs::Doc* ctx);
	// void setParent(QWidget* ctx);
	void executeCommand(QUndoCommand* c) override;
	void stash_toolbar(QAction* p);

protected: // Doc stuff
//	qucs::Doc* doc();
	SchematicScene const* scene()const;
	void updateViewport();
	QPoint snapToGrid(QPointF const&p) const;
	QUndoStack* undoStack(){ return _undoStack; }

private:
	void slotRedo() override;
	void slotUndo() override;

private:
	QUndoStack* _undoStack{nullptr};

public: // actions... private?
	     // not yet. still toggled from outside
	MouseAction* maDelete;
	MouseAction* maSelect;
	MouseAction* maWire{nullptr};
	MouseAction* maMirror;
	MouseAction* maActivate;
	MouseAction* maRotate;
	MouseAction* maEditPaste;
	MouseAction* maOnGrid;
	MouseAction* maMirrorXaxis;
	MouseAction* maMirrorYaxis;
	MouseAction* maInsertGround;
	MouseAction* maInsertElement;
	MouseAction* maInsertPort;
	MouseAction* maInsertEqn;
	MouseAction* maZoomIn;
	// MouseAction* maZoomOut; // not a mouseaction
	
	void actionSelectElement(QObject*e);
private:
	QToolBar* _toolbar{nullptr};
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
