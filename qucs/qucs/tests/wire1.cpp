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
	assert(M.numWires() == 1);
	assert(M.nodeCount() == 2);

	auto c = dynamic_cast<Conductor const*>(w0);
	trace0("nu...");
	Symbol* nu = c->newUnion(g0);
	trace0("nu done");

	assert(nu);
	assert(nu->subckt());
	trace1("gnd", nu->subckt()->numWires());
	SchematicModel const* s = nu->subckt();
	assert(s->numWires()==2);

	auto a = s->wires().begin();

	assert((*a)->nodePosition(0) == pos_t(30, -50));
	assert((*a)->nodePosition(1) == pos_t(30, 0));
	++a;
	assert((*a)->nodePosition(0) == pos_t(30, 0));
	assert((*a)->nodePosition(1) == pos_t(30, 50));

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
	assert(M.numWires() == 1);
	assert(M.nodeCount() == 2);

	auto c = dynamic_cast<Conductor const*>(w0);
	trace0("union...");
	Symbol* w = c->newUnion(w1);
	trace0("union done");

	assert(w);
	assert(w->subckt());
	trace1("gnd", w->subckt()->numWires());
	assert(w->subckt()->numWires()==3);
//	assert(w->subckt()->numComponents()==3);
}


int main()
{
	union0();
	union1();
}
