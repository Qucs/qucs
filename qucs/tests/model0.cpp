
#include <qucs/schematic_model.h>
#include <qucs/wire.h>


int main()
{
	SchematicModel M(NULL);

	auto w0 = new Wire(0,0,1,0);
	auto w1 = new Wire(1,0,2,0);
	auto w2 = new Wire(2,0,3,0);

	M.pushBack(w0);
	M.pushBack(w1);
	M.pushBack(w2);

	M.clear()
}
