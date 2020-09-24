
#include <qucs/schematic_model.h>
#include <qucs/wire.h>


void more()
{ untested();
	SchematicModel M(NULL);

	untested();
	auto w0 = new Wire(0,0,1,0);
	untested();
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
	assert(dynamic_cast<Symbol const*>(w0)->portValue(0));
	assert(dynamic_cast<Symbol const*>(w0)->portValue(1));
	trace1("w0", w0->net());

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
	trace1("w1", w1->net());

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
	trace1("w2", w2->net());

	unsigned i = M.nodeCount();
	trace1("nodecount", i);
	assert(i==4);

	assert(w0->net());
	assert(w1->net() == w0->net());
	assert(w1->net() == w2->net());

	std::cout << "=== try detach\n";
	delete(M.detach(w1));
	std::cout << "=== detached\n";
	assert(M.wireCount()==2);
	trace2("different", w0->net(), w2->net());
	assert(w0->net() != w2->net());

	std::cout << "=== U test\n";
	auto w3 = new Wire(0,0,0,1);
	M.pushBack(w3);
	assert(M.wireCount()==3);
	trace2("same", w0->net(), w2->net());
	assert(w2->net() == w0->net());
	assert(w3->net() == w0->net());

	w1 = new Wire(1,0,1,1);
	// W0010
	// W0111
	// W0001
	// W1011

	std::cout << "=== square test\n";
	M.pushBack(w1);
	SchematicModel const& cM = M;
	for(auto i : cM.wires()){ untested();
		Symbol* s = i;
		int cx = atoi(s->getParameter("$xposition").c_str());
		int cy = atoi(s->getParameter("$xposition").c_str());
		std::cout << "W" << cx << cy << "\n";
	}
	
	assert(M.wireCount()==4);
	trace1("w1", w1->net());

	std::cout << "=== try clear\n";
	M.clear();
}

int main()
{ untested();
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
	{ untested();
		// BUG: could be the other way around
		assert(*ni0 == w0);
		++ni0;
		assert(*ni0 == w1);
	}
	auto ni1 = n1->connectionsBegin();
	{ untested();
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

	{ untested();
		auto N = n0->neighbours();
		unsigned k=0;
		for(auto Ni=N.begin(); Ni!=N.end(); ++Ni){ untested();
			++k;
		}
		// wire induces self loops, hence 4.
		assert(k==4);
	}
	{ untested();
		auto N = n1->neighbours();
		unsigned k=0;
		for(auto Ni=N.begin(); Ni!=N.end(); ++Ni){ untested();
			++k;
		}
		assert(k==4);
	}


	std::cout << "== detach\n";
	M.detach(w0);
	M.detach(w1);

	more();
}
