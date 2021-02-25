// A test
// (c) 2020 Felix Salfelder
// GPLv3+

#include "schematic_model.h"
#include "qucs_globals.h"
#include "conductor.h"
#include "wiretest.h"

void union0()
{
	Symbol* root = symbol_dispatcher.clone("subckt_proto");
	assert(root);
	assert(root->scope());
	SchematicModel& M = *root->scope();

	auto wp = symbol_dispatcher["Wire"];
	auto w0 = prechecked_cast<Symbol*>(wp->clone());
	w0->set_param_by_name(std::string("deltax"), "10");

	assert( w0->portPosition(0) == pos_t(0,0));
	assert( w0->portPosition(1) == pos_t(10,0));

	auto w1 = prechecked_cast<Symbol*>(wp->clone());
	w1->set_param_by_name(std::string("$xposition"), "1");
	w1->set_param_by_name(std::string("deltax"), "1");

	assert( w1->portPosition(0) == pos_t(0,0));
	assert( w1->portPosition(1) == pos_t(1,0));
	assert( w1->nodePosition(0) == pos_t(1,0));
	assert( w1->nodePosition(1) == pos_t(2,0));

	connect_push(root, w0);
	assert(M.nodeCount() == 2);

	auto c = dynamic_cast<Conductor const*>(w0);
	Symbol* w = c->newUnion(w1);

	assert(w);
	assert(!w->subckt());

	trace2("DBG", w->paramValue("$xposition"), w->paramValue("$yposition"));

	assert(w->nodePosition(0) == pos_t(0,0));
	assert(w->nodePosition(1) == pos_t(10,0));
	delete root;
}

void union1()
{
	Symbol* root = symbol_dispatcher.clone("subckt_proto");
	assert(root);
	assert(root->scope());
	SchematicModel& M = *root->scope();

	auto wp=symbol_dispatcher["Wire"];
	auto w0 = prechecked_cast<Symbol*>(wp->clone());
	w0->set_param_by_name(std::string("deltax"), "10");

	auto w1 = prechecked_cast<Symbol*>(wp->clone());
	w1->set_param_by_name(std::string("deltay"), "1");

	connect_push(root, w0);
	assert(M.nodeCount() == 2);

	auto c = dynamic_cast<Conductor const*>(w0);
	Symbol* w = c->newUnion(w1);

	assert(!w);
	delete root;
}

void union2()
{
	// o----o.. + ..o----o
	Symbol* root = symbol_dispatcher.clone("subckt_proto");
	assert(root);
	assert(root->scope());
	SchematicModel& M = *root->scope();

	auto wp=symbol_dispatcher["Wire"];
	auto w0 = prechecked_cast<Symbol*>(wp->clone());
	w0->set_param_by_name(std::string("deltax"), "10");

	auto w1 = prechecked_cast<Symbol*>(wp->clone());
	w1->set_param_by_name(std::string("$xposition"), "5");
	w1->set_param_by_name(std::string("deltax"), "10");

	connect_push(root, w0);
	assert(M.nodeCount() == 2);

	auto c = dynamic_cast<Conductor const*>(w0);
	Symbol* w = c->newUnion(w1);
	assert(w);

	trace2("DBG", w->paramValue("$xposition"), w->paramValue("$yposition"));
	trace2("DBG", w->nodePosition(0), w->nodePosition(1));
	delete root;
}

void union3()
{
	Symbol* root = symbol_dispatcher.clone("subckt_proto");
	assert(root);
	assert(root->scope());
	SchematicModel& M = *root->scope();

	auto wp=symbol_dispatcher["Wire"];
	auto w0 = prechecked_cast<Symbol*>(wp->clone());
	w0->set_param_by_name(std::string("deltax"), "10");

	auto w1 = prechecked_cast<Symbol*>(wp->clone());
	w1->set_param_by_name(std::string("deltax"), "-10");

	auto w2 = prechecked_cast<Symbol*>(wp->clone());
	w2->set_param_by_name(std::string("deltay"), "-10");

	connect_push(root, w0);
	connect_push(root, w1);
	assert(M.nodeCount() == 3);

	auto c = dynamic_cast<Conductor const*>(w0);
	Symbol* w = c->newUnion(w2);
	assert(!w);
	delete root;
}

void union4()
{
	Symbol* root = symbol_dispatcher.clone("subckt_proto");
	assert(root);
	assert(root->scope());
	SchematicModel& M = *root->scope();

	auto wp=symbol_dispatcher["Wire"];
	auto w0 = prechecked_cast<Symbol*>(wp->clone());
	w0->set_param_by_name(std::string("deltax"), "10");
	w0->set_param_by_name(std::string("$xposition"), "-5");

	assert( w0->nodePosition(0) == pos_t(-5,0));
	assert( w0->nodePosition(1) == pos_t(5,0));

	auto w2 = prechecked_cast<Symbol*>(wp->clone());
	w2->set_param_by_name(std::string("deltay"), "-10");

	trace1("dbg", w2->portPosition(1));
	assert( w2->nodePosition(1) == pos_t(0,-10));

	connect_push(root, w0);
	assert(M.nodeCount() == 2);

	auto c = dynamic_cast<Conductor const*>(w0);
	Symbol* w = c->newUnion(w2);
	assert(w);
	assert(w->subckt());
	assert(numWires(*w->subckt()));
	delete root;
}

void union5()
{
	Symbol* root = symbol_dispatcher.clone("subckt_proto");
	assert(root);
	assert(root->scope());
	SchematicModel& M = *root->scope();

	auto wp=symbol_dispatcher["Wire"];
	auto w0 = prechecked_cast<Symbol*>(wp->clone());
	w0->set_param_by_name(std::string("deltax"), "10");
	w0->set_param_by_name(std::string("$xposition"), "-5");

	wp = symbol_dispatcher["GND"];
	assert(wp);
	auto g0 = prechecked_cast<Symbol*>(wp->clone());
	assert(g0->nodePosition(0) == pos_t(0, 0));

	connect_push(root, w0);
	assert(numWires(M) == 1);
	assert(M.nodeCount() == 2);

	auto c = dynamic_cast<Conductor const*>(w0);
	trace0("union...");
	Symbol* w = c->newUnion(g0);
	trace0("union done");

	assert(w);
	assert(w->subckt());
	trace1("gnd", numWires(*w->subckt()));
	assert(numWires(*w->subckt())==2);
	delete root;
}

void union6()
{
	// |
	// o---
	//+|
	Symbol* root = symbol_dispatcher.clone("subckt_proto");
	assert(root);
	assert(root->scope());
	SchematicModel& M = *root->scope();

	auto wp=symbol_dispatcher["Wire"];
	auto w0 = prechecked_cast<Symbol*>(wp->clone());
	w0->set_param_by_name(std::string("deltax"), "10");

	auto w1 = prechecked_cast<Symbol*>(wp->clone());
	w1->set_param_by_name(std::string("deltay"), "10");

	auto w2 = prechecked_cast<Symbol*>(wp->clone());
	w2->set_param_by_name(std::string("deltay"), "-10");

	connect_push(root, w0);
	connect_push(root, w1);
	assert(M.nodeCount() == 3);

	{
		auto c = dynamic_cast<Conductor const*>(w0);
		Symbol* w = c->newUnion(w2);
		assert(!w);
	}

	{
		auto c = dynamic_cast<Conductor const*>(w1);
		Symbol* w = c->newUnion(w2);
		assert(!w);
	}
	delete root;
}

int main()
{
	Command::command("attach legacy/components", nullptr);
	union4();
	union0();
	union1();
	union2();
	union3();
	union5();
	union6();
}
