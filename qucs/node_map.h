

#ifndef QUCS_NODEMAP
#define QUCS_NODEMAP

#include "sharedObjectList.h" // BUG. not needed

class Node;
class Element;
class Wire; // BUG

class NodeMap {
	// std::map<position, Node*> _pos; // TODO
	// std::map<std::string, Node*> _names; // TODO
	//
	// std::list<Node*> _list;
	typedef SharedObjectList<Node> list_t; // BUG. not sharing any nodes
	typedef list_t::const_iterator const_iterator;
	
	list_t _list;

	typedef std::shared_ptr<Node> node_ptr;
public:
	typedef list_t::iterator iterator;
//	typedef Node* node_ptr; // TODO
//
public:
	explicit NodeMap() {}
	~NodeMap() {}
	NodeMap(NodeMap const&) = delete;
	
	void erase(Node* n){
		_list.erase(n);
	}
	void erase(std::shared_ptr<Node> n){
		_list.erase(n);
	}

	iterator begin() {return _list.begin();}
	iterator end()   {return _list.end();}
	const_iterator begin()const {return _list.begin();}
	const_iterator end()const   {return _list.end();}
   iterator find(Node*);
   void clear();
	void erase(iterator n){
		_list.erase(n);
	}



	node_ptr new_node(int x, int y);
	node_ptr new_node(int x, int y, std::shared_ptr<Element> e);
	node_ptr new_node(int x, int y, std::shared_ptr<Wire> e);

};

#endif
