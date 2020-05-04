// (C) 2020 Felix Salfelder
//
#ifndef NODELIST_H
#define NODELIST_H

#include "qt_compat.h"

class Node;
class SchematicModel;

class NodeList : /*BUG*/ public Q3PtrList<Node> {
private:
	NodeList(NodeList const& x) : _model(x._model){unreachable();}
public:
	explicit NodeList(SchematicModel& m) : Q3PtrList<Node>(), _model(m) {}

public:
	Node& at(int x, int y);
	SchematicModel& _model;
};

#endif
