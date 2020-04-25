// (C) 2020 Felix Salfelder
//
#ifndef NODELIST_H
#define NODELIST_H

#include "qt_compat.h"

class Node;

class NodeList : /*BUG*/ public Q3PtrList<Node> {
private:
	NodeList(NodeList const&){unreachable();}
public:
	explicit NodeList() : Q3PtrList<Node>() {}

public:
	Node& at(int x, int y);
};

#endif
