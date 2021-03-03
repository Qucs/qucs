// A test
// (c) 2020 Felix Salfelder
// GPLv3+

#include "element_list.h"
#include "qucs_globals.h"
#include "wiretest.h"

void test0()
{
	Symbol* root = symbol_dispatcher.clone("subckt_proto");
	assert(root);
	assert(root->scope());
	ElementList& M = *root->scope();

	Symbol* wp = symbol_dispatcher.clone("place");
	assert(wp);
	wp->set_owner(root);

	auto pl = prechecked_cast<Place*>(wp);

	pl->set_port_by_index(0, "a");
	assert(pl->node_degree()==0);

	M.push_back(wp);

	wp = symbol_dispatcher.clone("Wire");
	assert(wp);
	wp->set_owner(root);
	wp->set_port_by_index(0, "a");
	assert(pl->node_degree()==1);
	wp->set_port_by_index(0, "");
	assert(pl->node_degree()==0);

	M.push_back(wp);

	delete root;
}

int main()
{
	//CMD::command("attach legacy/components", nullptr);
	test0();
}
