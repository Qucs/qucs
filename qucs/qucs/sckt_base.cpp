
#include "sckt_base.h"
#include "schematic_model.h"
void SubcktBase::new_subckt()
{
	assert(!_subckt);
	_subckt = new SchematicModel(nullptr);
}
