
#include <qucs/schematic_model.h>
#include <qucs/wire.h>


int main()
{
	SchematicModel M(NULL);

	auto w0 = new Wire(0,0,1,0);
	auto w1 = new Wire(1,0,2,0);
	auto w2 = new Wire(2,0,3,0);

	assert(!w0->hasNumber());

	M.pushBack(w0);
	M.pushBack(w1);
	M.pushBack(w2);

	unsigned i=0;
	SchematicModel const& cM = M;
	for(auto x : cM.nodes()){
		(void) x;
		++i;
	}
	assert(i==4);

	assert(w0->hasNumber());
	assert(w1->netNumber() == w0->netNumber());


	M.clear();
}
