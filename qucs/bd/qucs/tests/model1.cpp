
#include <qucs/schematic_model.h>
#include <qucs/wire.h>

static const unsigned N=9999;

int main()
{
	SchematicModel M(NULL);

	Wire* w0;
	Wire* w1;

	std::cout << "building nets.\n";
	for(unsigned i=0; i<N; ++i){
		w0 = new Wire(i,0,i+1,0);
		M.pushBack(w0);

		w1 = new Wire(0,i+1,0,i+2);
		M.pushBack(w1);
		trace2("new wires", w0, w1);
	}
	assert(w1->getNet() != w0->getNet());

	std::cout << "joining nets.\n";
	auto w2 = new Wire(0,0,0,1);
	M.pushBack(w2);
	assert(w1->getNet() == w0->getNet());

	std::cout << "disjoining nets.\n";
	M.erase(w2);
	assert(w1->getNet() != w0->getNet());

	// this takes ages in comparison
	std::cout << "clearing\n";
	M.clear();
}
