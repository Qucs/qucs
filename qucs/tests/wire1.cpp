// A test
// (c) 2020 Felix Salfelder
// GPLv3+

#include <qucs/schematic_model.h>
#include <qucs/components/resistor.cpp>
#include "legacy/component.cpp" // BUG
#include "legacy/gatecomponent.cpp" // BUG
#include "globals.h"

void union0()
{
	
	//     __           __  |
	//   -[__]-   ->  -[__]-o
	//                      |
	SchematicModel M(NULL);
	auto wp=symbol_dispatcher["Wire"];
	auto w0 = prechecked_cast<Symbol*>(wp->clone());
	w0->setParameter(std::string("deltay"), "100");
	w0->setParameter(std::string("$xposition"), "30");
	w0->setParameter(std::string("$yposition"), "-50");

	wp = symbol_dispatcher["R"];
	assert(wp);
	auto g0 = prechecked_cast<Symbol*>(wp->clone());
	trace1("DBG", g0->nodePosition(0));
	trace1("DBG", g0->nodePosition(1));

	assert(g0->nodePosition(0) == pos_t(-30, 0));
	assert(g0->nodePosition(1) == pos_t(30, 0));

	M.pushBack(w0);
	assert(numWires(M) == 1);
	assert(M.nodeCount() == 2);

	auto c = dynamic_cast<Conductor const*>(w0);
	trace0("nu...");
	Symbol* nu = c->newUnion(g0);
	trace0("nu done");

	assert(nu);
	assert(nu->subckt());
	SchematicModel const* s = nu->subckt();
	assert(s);
	trace1("gnd", numWires(*s));
	assert(numWires(*s)==2);

	auto ii = s->begin();
	assert(ii != s->end());
	auto a = prechecked_cast<Symbol const*>(*ii);
	assert(a); // resistor.
	++ii;
	a = prechecked_cast<Symbol const*>(*ii);

	assert(a->nodePosition(0) == pos_t(30, -50));
	assert(a->nodePosition(1) == pos_t(30, 0));
	++ii;
	assert(ii != s->end());
	a = prechecked_cast<Symbol const*>(*ii);
	assert(a);
	assert(a->nodePosition(0) == pos_t(30, 0));
	assert(a->nodePosition(1) == pos_t(30, 50));
	++ii;
	assert(ii == s->end());

//	assert(w->subckt()->numComponents()==3);
}

void union1()
{
	trace0("union1");

	// |         |
	// |   =>    |
	// |      ---o---
	SchematicModel M(NULL);
	auto wp=symbol_dispatcher["Wire"];
	auto w0 = prechecked_cast<Symbol*>(wp->clone());
	w0->setParameter(std::string("deltay"), "100");

	auto w1 = prechecked_cast<Symbol*>(wp->clone());
	w1->setParameter(std::string("deltax"), "100");
	w1->setParameter(std::string("$xposition"), "-50");

	M.pushBack(w0);
	assert(numWires(M) == 1);
	assert(M.nodeCount() == 2);

	auto c = dynamic_cast<Conductor const*>(w0);
	trace0("union...");
	Symbol* w = c->newUnion(w1);
	trace0("union done");

	assert(w);
	assert(w->subckt());
	trace1("gnd", numWires(*w->subckt()));
	assert(numWires(*w->subckt())==3);
//	assert(w->subckt()->numComponents()==3);
}

void union2()
{
	trace0("union1");
	//                |
	//          =>    |
	// ------      ---o---
	SchematicModel M(NULL);
	auto wp=symbol_dispatcher["Wire"];

	auto w0 = prechecked_cast<Symbol*>(wp->clone());
	w0->setParameter(std::string("deltax"), "100");
	w0->setParameter(std::string("$xposition"), "-50");
	M.pushBack(w0);
	assert(numWires(M) == 1);
	assert(M.nodeCount() == 2);

	auto w1 = prechecked_cast<Symbol*>(wp->clone());
#if 1
	w1->setParameter(std::string("deltay"), "100");
#else
	w1->setParameter(std::string("$yposition"), "-100");
	w1->setParameter(std::string("deltay"), "100");
#endif

	auto c = dynamic_cast<Conductor const*>(w0);
	Symbol* w = c->newUnion(w1);

	assert(w);
	assert(w->subckt());
	trace1("gnd", numWires(*w->subckt()));
	assert(numWires(*w->subckt())==3);
//	assert(w->subckt()->numComponents()==3);
}


int main()
{
	union0();
	union1();
	union2();
}
