
#include "schematic_model.h"

static void connect_push(SchematicModel& M, Symbol* w0)
{
	M.connect(w0);
	M.push_back(w0);
}

