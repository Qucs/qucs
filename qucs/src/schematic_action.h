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
	MouseAction* maDelete;
	MouseAction* maSelect;
	MouseAction* maWire;
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
	MouseAction* maZoomIn;
	// MouseAction* maZoomOut; // not a mouseaction
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
