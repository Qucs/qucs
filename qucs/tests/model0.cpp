
#include <qucs/schematic_model.h>
#include <qucs/wire.h>


int main()
{
	SchematicModel M(NULL);

	auto w0 = new Wire(0,0,1,0);
	assert(!w0->hasNetNumber());
	M.pushBack(w0);
	trace1("w0", w0->netNumber());

	auto w1 = new Wire(1,0,1,1);
	M.pushBack(w1);
	trace1("w1", w1->netNumber());

	auto w2 = new Wire(0,1,1,1);
	M.pushBack(w2);
	trace1("w2", w2->netNumber());

	unsigned i=0;
	SchematicModel const& cM = M;
	for(auto x : cM.nodes()){
		(void) x;
		++i;
	}
	assert(i==4);

	assert(w0->hasNetNumber());
	assert(w1->netNumber() == w0->netNumber());
	assert(w1->netNumber() == w2->netNumber());

	M.erase(w1);
	trace2("different", w0->netNumber(), w2->netNumber());
	assert(w0->netNumber() != w2->netNumber());
	auto w3 = new Wire(0,0,0,1);
	M.pushBack(w3);
	trace2("same", w0->netNumber(), w2->netNumber());
	assert(w1->netNumber() == w0->netNumber());

	w1 = new Wire(1,0,1,1);
	M.pushBack(w1);
	trace1("w1", w1->netNumber());

	trace0("clearing");
	M.clear();
}
