/***************************************************************************
    copyright            : (C) 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*--------------------------------------------------------------------------*/
#include "nodemap.h"
#include "node.h"
#include "io_trace.h"
#include "netlist.h"
#include "net.h"
#include "dynamic_cc.h"
#include "qio.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<>
struct graph_traits<NodeMap>{
	typedef Conductor* vertex_descriptor;
	typedef std::list<Conductor*>::iterator adjacency_iterator;
	typedef Net* cc_descriptor;

	static std::pair<adjacency_iterator, adjacency_iterator>
	adjacent_vertices(vertex_descriptor t, NodeMap const&) {
		assert(t);
		return std::make_pair(t->connectionsBegin(), t->connectionsEnd());
	}

	// connected components parameters
	static cc_descriptor invalid_cc() { return nullptr; }
	static void unset_cc(Conductor* t, Net* n) {
		assert(t);
		t->detachNet(n);
	}
	static void set_cc(Conductor* t, Net* n) {
		assert(t);
		t->attachNet(n);
	}
	static cc_descriptor get_cc(vertex_descriptor t) {
		return t->net();
	}
	static Net* new_cc(Conductor*, NodeMap& s);
	static void del_cc(Net*, NodeMap& s);
	static size_t cc_size(Net* n, NodeMap const&);
	// needed for searching
	static void visit(Conductor* t, unsigned level) {
		assert(t);
		t->visit(level);
	}
	static bool visited(Conductor const* t, unsigned level) {
		assert(t);
		return t->visited(level);
	}
};
/*--------------------------------------------------------------------------*/
NodeMap::NodeMap(NetList& n)
  : _nets(n),
    _cc(new_ccs())
{
}
/*--------------------------------------------------------------------------*/
NodeMap::~NodeMap()
{
	delete _cc;
}
/*--------------------------------------------------------------------------*/
ConnectedComponents<NodeMap>* NodeMap::new_ccs()
{
	return new ConnectedComponents<NodeMap>(*this);
}
/*--------------------------------------------------------------------------*/
// Qucs schematic node.
// at most one node in one place,
// may have strange side effects.
int NodeMap::erase(Node* tt)
{
	assert(tt);
	trace2("NodeMap::erase", tt->degree(), tt->label());
	assert(tt->hasNet());
	Conductor* c = tt;
	_cc->deregisterVertex(c);
	assert(!tt->hasNet());

	_nodes.erase(tt);
	return 1;
}
/*--------------------------------------------------------------------------*/
Node* NodeMap::new_node(std::string const& s)
{
	//  if (OPT::case_insensitive) {
	//    notstd::to_lower(&s);
	//  }else{
	//  }

	if(s==""){
		// HACK
		return nullptr;
	}else{
	}
	Node* node;

	auto f = _nodes.find(s);
	if(f != _nodes.end()){
		node = *f;
	}else{
		node = nullptr;
	}

	// increments how_many() when lookup fails (new s)  
	if (!node) {
		node = new Node(s, size());
		//                 ^^^^ is really the map number of the new node
		assert(!node->hasNet());
		// hmm
		_nodes.insert(node);
		Conductor* c = node;
		_cc->registerVertex(c);
		assert(node->hasNet());
	}
	assert(node->hasNet());
	assert(node);
	return node;
}
/*--------------------------------------------------------------------------*/
std::string NodeMap::netName(std::string const& l) const
{
	auto i = _nodes.find(l);
	if(i == _nodes.end()){
		unreachable();
		return "unknown_net";
	}else{
	}

	if(!(*i)->hasNet()){
		unreachable();
		return "no_net";
	}else if((*i)->net()->hasLabel()){
		trace3("netName", (*i), (*i)->net(), (*i)->netLabel());
		return (*i)->netLabel();
	}else{
		return "_net" + std::to_string((*i)->net()->pos());
	}
}
/*--------------------------------------------------------------------------*/
Net* NodeMap::newNet()
{ untested();
	return _nets.newNet();
}
/*--------------------------------------------------------------------------*/
void NodeMap::delNet(Net* n)
{
	assert(n);
	return _nets.delNet(n);
}
/*--------------------------------------------------------------------------*/
void NodeMap::removeEdge(Conductor* a, Conductor* b)
{
  a->rmAdj(b);
  b->rmAdj(a);
  _cc->postRemoveEdge(a, b);
}
/*--------------------------------------------------------------------------*/
void NodeMap::addEdge(Conductor* a, Conductor* b)
{
  _cc->addEdge(a, b);
  a->addAdj(b);
  b->addAdj(a);
}
/*--------------------------------------------------------------------------*/
void NodeMap::registerVertex(Conductor*c)
{
  _cc->registerVertex(c);
}
/*--------------------------------------------------------------------------*/
void NodeMap::deregisterVertex(Conductor*c)
{
  _cc->deregisterVertex(c);
}
/*--------------------------------------------------------------------------*/
size_t graph_traits<NodeMap>::cc_size(Net* n, NodeMap const&)
{
	assert(n);
	return n->size();
}
/*--------------------------------------------------------------------------*/
Net* graph_traits<NodeMap>::new_cc(Conductor*n, NodeMap& s)
{
	assert(n);
	assert(!n->hasNet());
	return n->newNet(s.netList());
}
/*--------------------------------------------------------------------------*/
void graph_traits<NodeMap>::del_cc(Net* n, NodeMap& s)
{
	s.delNet(n);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
