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
#include "schematic_doc.h"
/*--------------------------------------------------------------------------*/
void SchematicDialog::execute(QUndoCommand* c)
{
	Doc->executeCommand(c);
}
/*--------------------------------------------------------------------------*/
SchematicDialog::SchematicDialog(QucsDoc* d)
  : QDialog(prechecked_cast<SchematicDoc*>(d))
{
  assert(d);
  Doc=dynamic_cast<SchematicDoc*>(d);
  assert(Doc);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
