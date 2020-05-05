
#include <qucs/schematic_model.h>
#include <qucs/wire.h>


int main()
{
	SchematicModel M(NULL);

	auto w0 = new Wire(0,0,1,0);
	assert(!w0->hasNet());
	M.pushBack(w0);
	assert(w0->hasNet());
	assert(dynamic_cast<Symbol const*>(w0)->port(0).connected());
	assert(dynamic_cast<Symbol const*>(w0)->port(1).connected());
	trace1("w0", w0->getNet());

	std::cout << "disconnect\n";
	M.disconnect(w0);
	assert(!w0->hasNet());
	std::cout << "reconnect\n";
	M.connect(w0);
	assert(w0->hasNet());

	std::cout << "...\n";
	auto w1 = new Wire(1,0,1,1);
	M.pushBack(w1);
	trace1("w1", w1->getNet());

	std::cout << "disconnect\n";
	M.disconnect(w1);
	assert(!w1->hasNet());
	std::cout << "reconnect\n";
	M.connect(w1);
	assert(w1->hasNet());

	auto w2 = new Wire(0,1,1,1);
	M.pushBack(w2);
	trace1("w2", w2->getNet());

	unsigned i = M.nodeCount();
	trace1("nodecount", i);
	assert(i==4);

	assert(w0->getNet());
	assert(w1->getNet() == w0->getNet());
	assert(w1->getNet() == w2->getNet());

	std::cout << "=== try erase\n";
	M.erase(w1);
	trace2("different", w0->getNet(), w2->getNet());
	assert(w0->getNet() != w2->getNet());
	auto w3 = new Wire(0,0,0,1);
	M.pushBack(w3);
	trace2("same", w0->getNet(), w2->getNet());
	assert(w1->getNet() == w0->getNet());

	w1 = new Wire(1,0,1,1);
	M.pushBack(w1);
	trace1("w1", w1->getNet());

	std::cout << "clearing\n";
	M.clear();
}
