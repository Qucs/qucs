// (C) 2020 Felix Salfelder
//
#ifndef QUCS_NODEMAP_H
#define QUCS_NODEMAP_H

#include "qt_compat.h"
#include "conductor.h"
#include "node.h"
#include <set>

template<class G>
struct graph_traits{};

#include "dynamic_cc.h"

class Node;
class NodeMap;
class NetList;
// describe the conductor graph.
// Conductors are the vertices.
// a connected component in this graph is a Net
template<>
struct graph_traits<NodeMap>{
	typedef Conductor* vertex_descriptor;
	typedef AdjConductorIterator adjacency_iterator;
	typedef Net* cc_descriptor;

	static std::pair<AdjConductorIterator,AdjConductorIterator>
	adjacent_vertices(vertex_descriptor t, NodeMap const&) {
		auto n = t->neighbours();
		return std::make_pair(n.begin(), n.end());
	}

	// connected components parameters
	static cc_descriptor invalid_cc() { return nullptr; }
	static void set_cc(Conductor* t, Net* n) {
		// assert(n); // no. could as well detach
		trace2("set_cc", t, n);
		t->setNet(n);
	}
	static cc_descriptor get_cc(vertex_descriptor t) {
		return t->getNet();
	}
	static Net* new_cc(NodeMap& s);
	static void del_cc(Net*, NodeMap& s);
	static size_t& cc_size(Net* n, NodeMap const&);
	// needed for searching
	static void visit(Conductor* t, unsigned level) {
		t->visit(level);
	}
	static bool visited(Conductor const* t, unsigned level) {
		return t->visited(level);
	}
};

class NodeMap {
public:
  	typedef std::pair<int, int> key_type;
private:
	class NodeCompare{
	public:
		using is_transparent = void;
		bool operator()(Node const* n1, Node const* n2) const{
			return c(n1->position(), n2->position());
		}
		bool operator()(Node const* n1, key_type const& n2) const{
			return c(n1->position(), n2);
		}
		bool operator()(key_type const& n1, Node const* n2) const{
			return c(n1, n2->position());
		}
//		bool operator()(key_type const& n1, key_type const& n2) const{ untested();
//			return c(n1, n2);
//		}
	private:
		std::less< std::pair<int, int> > c;
	};
public:
	// need *set*, because map splits key from payload.
	// using a set of *pointers* because nodes must be mutable.
	// adds another indirection, but it is not exposed.
  	typedef std::set<Node*, NodeCompare > container_type;
	typedef container_type::iterator iterator;
	typedef container_type::const_iterator const_iterator;
private:
	// NodeList(NodeList const& x) : _model(x._model){unreachable();}
	NodeMap(NodeMap const& x) = delete;
public:
	explicit NodeMap(NetList& n)
		: _nets(n), _cc(*this) {}

public:
// 	iterator begin(){return _nodes.begin();}
// 	iterator end(){return _nodes.end();}
// 	const_iterator begin() const{return _nodes.begin();}
// 	const_iterator end() const{return _nodes.end();}

	void clear(){ return _nodes.clear(); }
	size_t size() const{ return _nodes.size(); }

public: //obsolete interface
// 	void append(Node* n){
// 		return _nodes.push_back(n);
// 	}
#if 1
	int erase(Node* tt);
#endif

public: // friend Symbol?
	void addEdge(Conductor* a, Conductor* b);
	void postRemoveEdge(Conductor* a, Conductor* b);

public:
	Node* find_at(int x, int y);
	Node* find_at(key_type);
	Node& at(int x, int y);
	Node& new_at(int x, int y); // same as at. but don't search

public: // net access
	Net* newNet();
	void delNet(Net*);

	void registerVertex(Conductor*);
	void deregisterVertex(Conductor*);

private:
	NetList& _nets;
	container_type _nodes;
private:
	ConnectedComponents<NodeMap> _cc;
};

inline Net* graph_traits<NodeMap>::new_cc(NodeMap& s)
{
	return s.newNet();
}
inline void graph_traits<NodeMap>::del_cc(Net* n, NodeMap& s)
{
	return s.delNet(n);
}


#endif
