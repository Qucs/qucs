// (C) 2020 Felix Salfelder
//
#ifndef NODELIST_H
#define NODELIST_H

#include "qt_compat.h"

class Node;
class SchematicModel;

class NodeList {
public:
  	typedef std::list<Node*> container_type;
	typedef container_type::iterator iterator;
	typedef container_type::const_iterator const_iterator;
private:
	// NodeList(NodeList const& x) : _model(x._model){unreachable();}
	NodeList(NodeList const& x) = delete;
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
		return _nodes.push_back(n);
	}
#if 0
	bool removeRef(Node* tt) {
		if(n==_nodes.end()){ untested();
		}else{ untested();
			return _nodes.erase(n);
		}
	}
#endif

public:
	Node* find_at(int x, int y);
	Node& at(int x, int y);
	Node& new_at(int x, int y); // same as at. but don't search
	SchematicModel& _model;

private:
	container_type _nodes;
};

#endif
