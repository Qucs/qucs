// A test
// (c) 2021 Felix Salfelder
// GPLv3+

#include "element_list.h"
#include "next_idx.h"
#include "qucs_globals.h"
#include "component.h"

using qucs::ElementList;
using qucs::Element;
using qucs::device_dispatcher;

void test0()
{
	ElementList M;

	qucs::Element* c1 = device_dispatcher.clone("subckt_proto");
	assert(c1);
	c1->set_label("prefix0");
	M.push_back(c1);

	c1 = c1->clone();
	assert(c1);
	c1->set_label("prefix1");
	M.push_back(c1);

	c1 = c1->clone();
	assert(c1);
	c1->set_label("prefix2");
// 	M.push_back(c1); // omit

	c1 = c1->clone();
	assert(c1);
	c1->set_label("prefix3");
	M.push_back(c1);

	c1 = c1->clone();
	assert(c1);
	c1->set_label("xx");
	M.push_back(c1);

	c1 = c1->clone();
	assert(c1);
	c1->set_label("aa1");
	M.push_back(c1);

	unsigned i = next_idx(&M, "prefix");
	trace1("idx?", i);
	assert(i==4);
}

int main()
{
	test0();
}
