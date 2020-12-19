// A test
// (c) 2020 Felix Salfelder
// GPLv3+

#include "schematic_model.h"
#include "globals.h"

void more()
{
	SchematicModel M;

	auto wp=symbol_dispatcher["Wire"];
//	 Wire(0,0,1,0);
	auto w0 = prechecked_cast<Symbol*>(wp->clone());
	w0->setParameter(std::string("deltax"), "1");

//	 Wire(0,0,10,0);
	auto w1 = prechecked_cast<Symbol*>(wp->clone());
	w1->setParameter(std::string("deltax"), "10");
//	assert(!w0->hasNet());
	M.pushBack(w0);
	M.pushBack(w1);
//	assert(w0->hasNet());

	assert(numWires(M)==2);
	M.clear();
	assert(numWires(M)==0);

	std::cout << "cleared\n";

	w0 = prechecked_cast<Symbol*>(wp->clone());
//	assert(!w0->hasNet());
	M.pushBack(w0);
//	assert(w0->hasNet());
	assert(dynamic_cast<Symbol const*>(w0)->portValue(0));
	assert(dynamic_cast<Symbol const*>(w0)->portValue(1));
//	trace1("w0", w0->net());

	{
		std::cout << "disconnect\n";
		M.disconnect(w0);
		assert(numWires(M)==1);
//		assert(!w0->hasNet());
	}
	{
		std::cout << "reconnect\n";
		M.connect(w0);
//		assert(w0->hasNet());
	}

	std::cout << "...\n";
	// new Wire(1,0,1,1);
	w1 = prechecked_cast<Symbol*>(wp->clone());
	w1->setParameter(std::string("$xposition"), "1");
	w1->setParameter(std::string("deltax"), "1");
	w1->setParameter(std::string("deltay"), "1");

	M.pushBack(w1);
//	trace1("w1", w1->net());

	std::cout << "disconnect\n";
	M.disconnect(w1);
	assert(numWires(M)==2);
//	assert(!w1->hasNet());
	std::cout << "reconnect\n";
	M.connect(w1);
//	assert(w1->hasNet());

//	auto w2 = new Wire(0,1,1,1);
	auto w2 = prechecked_cast<Symbol*>(w0->clone());
	w2->setParameter(std::string("$yposition"), "1");
	w2->setParameter(std::string("deltax"), "1");

	M.pushBack(w2);
	assert(numWires(M)==3);
//	trace1("w2", w2->net());

	unsigned i = M.nodeCount();
	trace1("nodecount", i);
	assert(i==4);

//	assert(w0->net());
//	assert(w1->net() == w0->net());
//	assert(w1->net() == w2->net());

	std::cout << "=== try detach\n";
	delete(M.detach(w1));
	std::cout << "=== detached\n";
	assert(numWires(M)==2);
//	trace2("different", w0->net(), w2->net());
//	assert(w0->net() != w2->net());

	std::cout << "=== U test\n";
//	auto w3 = new Wire(0,0,0,1);
	auto w3 = prechecked_cast<Symbol*>(w0->clone());
	w3->setParameter(std::string("deltay"), "1");

	M.pushBack(w3);
	assert(numWires(M)==3);
//	trace2("same", w0->net(), w2->net());
//	assert(w2->net() == w0->net());
//	assert(w3->net() == w0->net());

	// w1 = new Wire(1,0,1,1);
	w1 = prechecked_cast<Symbol*>(w0->clone());
	w2->setParameter(std::string("$xposition"), "1");
	w2->setParameter(std::string("deltay"), "1");
	// W0010
	// W0111
	// W0001
	// W1011

	std::cout << "=== square test\n";
	M.pushBack(w1);
	SchematicModel const& cM = M;
	for(auto i : cM){
		Symbol* s = prechecked_cast<Symbol*>(i);
		assert(s);
		int cx = atoi(s->paramValue("$xposition").c_str());
		int cy = atoi(s->paramValue("$xposition").c_str());
		std::cout << "W" << cx << cy << "\n";
	}
	
	assert(numWires(M)==4);
//	trace1("w1", w1->net());

	std::cout << "=== try clear\n";
	M.clear();
}

int main()
{
	SchematicModel M;
	//auto w0 = new Wire(0,0,1,0);
	auto wp=symbol_dispatcher["Wire"];
	auto w0 = prechecked_cast<Symbol*>(wp->clone());
	w0->setParameter(std::string("deltax"), "1");

	//auto w1 = new Wire(1,0,0,0);
	auto w1 = prechecked_cast<Symbol*>(wp->clone());
	w1->setParameter(std::string("$xposition"), "1");
	w1->setParameter(std::string("deltax"), "-1");

	{
		M.pushBack(w0);
		assert(M.nodeCount() == 2);
		M.pushBack(w1);
		assert(M.nodeCount() == 2);
	}

#if 0
	auto i0 = w0->connectionsBegin();
	auto e0 = w0->connectionsEnd();
	auto i1 = w1->connectionsBegin();
	auto e1 = w1->connectionsEnd();

	auto n0 = *i0;
	auto n1 = *i1;

	{ untested();
		auto ni0 = n0->connectionsBegin();
		// BUG: could be the other way around
		assert(*ni0 == w0);
		++ni0;
		assert(*ni0 == w1);
	}
	{ untested();
		auto ni1 = n1->connectionsBegin();
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
#endif

	{
		//assert(n0->degree()==2);
		//assert(n1->degree()==2);
	}


	{
		std::cout << "== detach\n";
		M.detach(w0);
		M.detach(w1);
	}

	more();
}
