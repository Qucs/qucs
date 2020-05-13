
#include <qucs/schematic_model.h>
#include <qucs/wire.h>


void more()
{
	SchematicModel M(NULL);

	auto w0 = new Wire(0,0,1,0);
	auto w1 = new Wire(0,0,10,0);
	assert(!w0->hasNet());
	M.pushBack(w0);
	M.pushBack(w1);
	assert(w0->hasNet());

	assert(M.wireCount()==2);
	M.clear();
	assert(M.wireCount()==0);

	std::cout << "cleared\n";

	w0 = new Wire(0,0,1,0);
	assert(!w0->hasNet());
	M.pushBack(w0);
	assert(w0->hasNet());
	assert(dynamic_cast<Symbol const*>(w0)->port(0).connected());
	assert(dynamic_cast<Symbol const*>(w0)->port(1).connected());
	trace1("w0", w0->getNet());

	std::cout << "disconnect\n";
	M.disconnect(w0);
	assert(M.wireCount()==1);
	assert(!w0->hasNet());
	std::cout << "reconnect\n";
	M.connect(w0);
	assert(w0->hasNet());

	std::cout << "...\n";
	w1 = new Wire(1,0,1,1);
	M.pushBack(w1);
	trace1("w1", w1->getNet());

	std::cout << "disconnect\n";
	M.disconnect(w1);
	assert(M.wireCount()==2);
	assert(!w1->hasNet());
	std::cout << "reconnect\n";
	M.connect(w1);
	assert(w1->hasNet());

	auto w2 = new Wire(0,1,1,1);
	M.pushBack(w2);
	assert(M.wireCount()==3);
	trace1("w2", w2->getNet());

	unsigned i = M.nodeCount();
	trace1("nodecount", i);
	assert(i==4);

	assert(w0->getNet());
	assert(w1->getNet() == w0->getNet());
	assert(w1->getNet() == w2->getNet());

	std::cout << "=== try detach\n";
	delete(M.detach(w1));
	std::cout << "=== detached\n";
	assert(M.wireCount()==2);
	trace2("different", w0->getNet(), w2->getNet());
	assert(w0->getNet() != w2->getNet());

	std::cout << "=== U test\n";
	auto w3 = new Wire(0,0,0,1);
	M.pushBack(w3);
	assert(M.wireCount()==3);
	trace2("same", w0->getNet(), w2->getNet());
	assert(w2->getNet() == w0->getNet());
	assert(w3->getNet() == w0->getNet());

	w1 = new Wire(1,0,1,1);
	// W0010
	// W0111
	// W0001
	// W1011

	std::cout << "=== square test\n";
	M.pushBack(w1);
	SchematicModel const& cM = M;
	for(auto i : cM.wires()){
		std::cout << "W" << i->x1__() << i->y1__() << i->x2__() << i->y2__() << "\n";
	}
	
	assert(M.wireCount()==4);
	trace1("w1", w1->getNet());

	std::cout << "=== try clear\n";
	M.clear();
}

int main()
{
	SchematicModel M(NULL);
	auto w0 = new Wire(0,0,1,0);
	auto w1 = new Wire(1,0,0,0);

	M.pushBack(w0);
	assert(M.nodeCount() == 2);
	M.pushBack(w1);
	assert(M.nodeCount() == 2);

	auto i0 = w0->connectionsBegin();
	auto e0 = w0->connectionsEnd();
	auto i1 = w1->connectionsBegin();
	auto e1 = w1->connectionsEnd();

	Node* n0 = *i0;
	Node* n1 = *i1;

	auto ni0 = n0->connectionsBegin();
	{
		// BUG: could be the other way around
		assert(*ni0 == w0);
		++ni0;
		assert(*ni0 == w1);
	}
	auto ni1 = n1->connectionsBegin();
	{
		// BUG: could be the other way around
		assert(*ni1 == w0);
		++ni1;
		assert(*ni1 == w1);
	}

	assert(*i0 != *i1);
	++i0;
	assert(*i0 == *i1);
	++i1;
	assert(*i0 != *i1);

	{
		auto N = n0->neighbours();
		unsigned k=0;
		for(auto Ni=N.begin(); Ni!=N.end(); ++Ni){
			++k;
		}
		// wire induces self loops, hence 4.
		assert(k==4);
	}
	{
		auto N = n1->neighbours();
		unsigned k=0;
		for(auto Ni=N.begin(); Ni!=N.end(); ++Ni){
			++k;
		}
		assert(k==4);
	}


	std::cout << "== detach\n";
	M.detach(w0);
	M.detach(w1);

	more();
}
