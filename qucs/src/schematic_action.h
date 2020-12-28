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
#include "mouseactions.h"
#include "schematic_edit.h"
/*--------------------------------------------------------------------------*/
class SchematicDoc;
/*--------------------------------------------------------------------------*/
// merge into scene altogether?
class SchematicActions : public MouseActions{
public:
	explicit SchematicActions(SchematicDoc& d);
	~SchematicActions();

private: // MouseActions
	// void handle(QEvent*) override;

protected: // Doc stuff
	SchematicDoc* doc(); // TODO: scene!!
	SchematicScene const* scene()const;
	void updateViewport();
	QPoint snapToGrid(QPointF const&p) const{
		return scene()->snapToGrid(p);
	}

public: // actions... private?
	     // not yet. still toggled from outside
	MouseAction* maDelete{nullptr};
	MouseAction* maSelect{nullptr};
	MouseAction* maWire{nullptr};
	MouseAction* maMirror{nullptr};
	MouseAction* maActivate{nullptr};
	MouseAction* maRotate{nullptr};
	MouseAction* maEditPaste{nullptr};
	MouseAction* maOnGrid{nullptr};
	MouseAction* maMirrorXaxis{nullptr};
	MouseAction* maMirrorYaxis{nullptr};
	MouseAction* maInsertGround{nullptr};
	MouseAction* maInsertElement{nullptr};
	MouseAction* maInsertPort{nullptr};
	MouseAction* maInsertEqn{nullptr};
	MouseAction* maZoomIn{nullptr};
	// MouseAction* maZoomOut{nullptr}; // not a mouseaction
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
