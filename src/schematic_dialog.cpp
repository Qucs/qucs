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
#include "schematic_dialog.h"
#include "qucsdoc.h"
/*--------------------------------------------------------------------------*/
SchematicDialog::SchematicDialog()
    : Widget()
{
}
/*--------------------------------------------------------------------------*/
SchematicDialog::SchematicDialog(SchematicDialog const& d)
    : Widget(d)
{
}
/*--------------------------------------------------------------------------*/
void SchematicDialog::execute(QUndoCommand* c)
{
	assert(doc());
	doc()->executeCommand(c);
}
/*--------------------------------------------------------------------------*/
QucsDoc* SchematicDialog::doc()
{ untested();
	auto t = prechecked_cast<QWidget*>(this);
	assert(t);
	auto p = prechecked_cast<QucsDoc*>(t->parentWidget());
	assert(p);
	return p;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
