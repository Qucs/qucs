#include "schematic_dialog.h"
#include "schematic_doc.h"
#include <QUndoCommand>

void SchematicDialog::execute(QUndoCommand* c)
{
	Doc->executetaskElement(c);
}
