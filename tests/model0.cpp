// A test
// (c) 2020 Felix Salfelder
// GPLv3+

#include "schematic_model.h"
#include "qucs_globals.h"
#include "nodemap.h"
#include "wiretest.h"

void test1()
{
	Symbol* root = symbol_dispatcher.clone("subckt_proto");
	assert(root);
	assert(root->scope());
	SchematicModel& M = *root->scope();

	auto wp=symbol_dispatcher["Wire"];
//	 Wire(0,0,1,0);
	auto w0 = prechecked_cast<Symbol*>(wp->clone());
	w0->set_param_by_name("deltax", "1");

//	 Wire(0,0,10,0);
	auto w1 = prechecked_cast<Symbol*>(wp->clone());
	w1->set_param_by_name("deltax", "10");
//	assert(!w0->hasNet());
	connect_push(root, w0);
	connect_push(root, w1);
//	assert(w0->hasNet());

	assert(numWires(M)==2);
	M.clear();
	assert(numWires(M)==0);

	std::cout << "cleared\n";

	w0 = prechecked_cast<Symbol*>(wp->clone());
//	assert(!w0->hasNet());
	connect_push(root, w0);
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
	w1->set_param_by_name("$xposition", "1");
	w1->set_param_by_name("deltax", "1");
	w1->set_param_by_name("deltay", "1");

	connect_push(root, w1);
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
	w2->set_param_by_name("$yposition", "1");
	w2->set_param_by_name("deltax", "1");

	connect_push(root, w2);
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
	w3->set_param_by_name("deltay", "1");

	connect_push(root, w3);
	assert(numWires(M)==3);
//	trace2("same", w0->net(), w2->net());
//	assert(w2->net() == w0->net());
//	assert(w3->net() == w0->net());

	// w1 = new Wire(1,0,1,1);
	w1 = prechecked_cast<Symbol*>(w0->clone());
	w2->set_param_by_name("$xposition", "1");
	w2->set_param_by_name("deltay", "1");
	// W0010
	// W0111
	// W0001
	// W1011

	std::cout << "=== square test\n";
	connect_push(root, w1);
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

void test0()
{
	Symbol* root = symbol_dispatcher.clone("subckt_proto");
	assert(root);
	assert(root->scope());
	SchematicModel& M = *root->scope();

	//auto w0 = new Wire(0,0,1,0);
	auto wp=symbol_dispatcher["Wire"];
	auto w0 = prechecked_cast<Symbol*>(wp->clone());
	w0->set_param_by_name("deltax", "1");

	//auto w1 = new Wire(1,0,0,0);
	auto w1 = prechecked_cast<Symbol*>(wp->clone());
	w1->set_param_by_name("$xposition", "1");
	w1->set_param_by_name("deltax", "-1");

	{
		connect_push(root, w0);
		assert(M.nodeCount() == 2);
		trace1("model0", w0->port_value(0));
		trace1("model0", M.nodes()->netName(":0:0"));
		assert(":0:0" == w0->port_value(0));
		assert(":1:0" == w0->port_value(1));
		assert("_net0" == M.nodes()->netName(":0:0"));
		assert("_net0" == M.nodes()->netName(":1:0"));
	}

	{
		connect_push(root, w1);
		assert(M.nodeCount() == 2);
		assert(":1:0" == w1->port_value(0));
		assert(":0:0" == w1->port_value(1));
		assert("_net0" == M.nodes()->netName(":0:0"));
		assert("_net0" == M.nodes()->netName(":1:0"));
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
}

int main()
{
	test0();
	test1();
}
