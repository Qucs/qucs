/***************************************************************************
    copyright            : 2018, 2020, 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SCHEMATIC_DIALOG_H
#define SCHEMATIC_DIALOG_H

#include <QDialog>
#include <assert.h>
#include "widget.h"
#include "platform.h"

class QucsDoc;
class QUndoCommand;
class ElementGraphics;
class SchematicDoc;
/*--------------------------------------------------------------------------*/
class SchematicDialog : public Widget{
protected:
	explicit SchematicDialog();
	SchematicDialog(SchematicDialog const& d);
	virtual ~SchematicDialog(){}

public:
	virtual void attach(ElementGraphics*){ assert(false); unreachable(); }

public: // needed?
	void enableButtons(){}
	void disableButtons(){}

protected slots: // needed?
	virtual void reject(){}
	virtual bool eventFilter(QObject * /*obj*/, QEvent * /*event*/){return false;}

protected:
	QucsDoc* doc();
	void execute(QUndoCommand* c);
};
/*--------------------------------------------------------------------------*/
Q_DECLARE_INTERFACE(SchematicDialog, "qucs.SchematicDialog")
/*--------------------------------------------------------------------------*/
#endif
