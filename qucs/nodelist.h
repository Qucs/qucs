// (C) 2020 Felix Salfelder
//
#ifndef NODELIST_H
#define NODELIST_H

#include "qt_compat.h"

class Node;
class SchematicModel;

class NodeList {
public:
  	typedef Q3PtrList<Node> container_type; // TODO..
	typedef container_type::iterator iterator;
	typedef container_type::const_iterator const_iterator;
private:
	NodeList(NodeList const& x) : _model(x._model){unreachable();}
public:
	explicit NodeList(SchematicModel& m) : _model(m) {}

public:
	iterator begin(){return _nodes.begin();}
	iterator end(){return _nodes.end();}
	const_iterator begin() const{return _nodes.begin();}
	const_iterator end() const{return _nodes.end();}

	void clear(){ return _nodes.clear(); }
	size_t size() const{ return _nodes.size(); }

public: //obsolete interface
	void append(Node* n){
		return _nodes.append(n);
	}
	bool removeRef(Node* tt) {
		return _nodes.removeRef(tt);
	}

public:
	Node* find_at(int x, int y);
	Node& at(int x, int y);
	SchematicModel& _model;

private:
	container_type _nodes;
};

#endif
