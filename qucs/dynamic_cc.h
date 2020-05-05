/***************************************************************************
                                 dynamic_cc.h
                             --------------------
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
// keep track of "nets", i.e. connected components in the circuit graph.
// the Qucs schematic Graph has vertices (aka Conductors) and edges (one
// conductor connnecting to another). Other than that, this file has nothing to
// do with Qucs.
//
// each node x has a number n(x), two nodes have the same number, iff they
// are connected by a sequence of edges. the number identifies a net (aka
// "node" in spice).
//
// The edges are accessed in terms of a neighbourhood iterator.
//
// need two algorithms: one for adding edges, and one for removing edges.
//
// add an edge between x and y. if n(x)!=n(y), set n(z)=n(x) for all nodes z
// connected to y.
//
// remove an edge between x and y. n(x) == n(y).  the stupid algorithm just
// does a bfs on both sides, we are done as soon as one of them
// - meets the other end -> done, or else
// - terminates without meeting the other end -> set new numbers.
// (there are faster algorithms, perhaps not needed, but could be done here)

#ifndef QUCS_DYNAMIC_CC
#define QUCS_DYNAMIC_CC

#include <stack>

const unsigned INVALID = -1u;

/*--------------------------------------------------------------------------*/
// find a node in a graph
template<class G, class V>
class FindVertex{
private:
	typedef typename G::adjacency_iterator adjacency_iterator;
	typedef typename G::vertex_descriptor vertex_descriptor;
public:
	enum status_t { sDUNNO, sSUCCESS, sFAIL };
private:
	FindVertex(FindVertex const&){ unreachable(); }
public:
	explicit FindVertex(G const& g,
			vertex_descriptor const& start, vertex_descriptor& needle)
		: _graph(g), _needle(needle) {

		assert(!_graph.visited(start));
		assert(start!=needle);

		auto f = av(start).first;
		auto fe = av(start).second;
		if(f==fe){
		}else{
			assert(!_graph.visited(*f));
		}
//		skip_to_unvisited_or_end(f, g);
		_graph.visit(start);
		_stack.push(std::make_pair(start, f));
	}
public:
	vertex_descriptor& needle() { return _needle; }
	status_t step();

private:
	void skip_to_unvisited_or_end(adjacency_iterator& i, adjacency_iterator const& end);
	std::pair<adjacency_iterator, adjacency_iterator> av(vertex_descriptor v){
		return _graph.av(v);
	}

private:
	G const& _graph;
	vertex_descriptor& _needle;
	std::stack<std::pair<vertex_descriptor, adjacency_iterator> > _stack;
};
/*--------------------------------------------------------------------------*/
template<class G, class V>
void FindVertex<G, V>::skip_to_unvisited_or_end(
	typename  FindVertex<G, V>::adjacency_iterator& i,
	typename  FindVertex<G, V>::adjacency_iterator const& end)
{
	while (i!=end){
		if(_graph.visited(*i)){
			++i;
		}else{
			break;
		}
	}
}
/*--------------------------------------------------------------------------*/
template<class G, class V>
typename FindVertex<G, V>::status_t FindVertex<G, V>::step()
{
	status_t st = sDUNNO;

	if (_stack.empty()){
		st = sFAIL;
	}else{
		auto& t = _stack.top();

		if(t.second == av(t.first).second){
			_stack.pop();
		}else if(*t.second == _needle){ untested();
			st = sSUCCESS;
		}else{
			auto current = *t.second;
			++t.second;
			skip_to_unvisited_or_end(t.second, av(t.first).second);
			_graph.visit(current);

			auto ff = av(current).first;
			auto gg = av(current).second;
			skip_to_unvisited_or_end(ff, gg);

			// TODO: tail recursion if t.second==t.first
			_stack.push(std::make_pair(current, ff));
		}
	}

	return st;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<class G>
class ConnectedComponents {
public:
	typedef typename graph_traits<G>::adjacency_iterator adjacency_iterator;
	typedef typename graph_traits<G>::vertex_descriptor vertex_descriptor;
	typedef typename graph_traits<G>::vertex_descriptor vertex;
	typedef typename graph_traits<G>::cc_descriptor ccid_t;
public:
	explicit ConnectedComponents(G& g) : _find_lvl(0), _graph(g) {}
public:

	unsigned next_level(){
		if(_find_lvl == -1u){ untested();
			incomplete(); // reset all nodes
			_find_lvl = 0;
		}else{
		}
		trace1("next_level", _find_lvl);
		return ++_find_lvl;
	}
	unsigned level() const{ return _find_lvl; }

public:
	void visit(vertex_descriptor const& v) const{
		trace2("visit", v, _find_lvl);
		graph_traits<G>::visit(v, _find_lvl);
	}
	bool visited(vertex_descriptor const& v) const{
		bool vis = graph_traits<G>::visited(v, _find_lvl);
		trace3("visited?", v, _find_lvl, vis);
		return vis;
	}

public:
	vertex& registerVertex(vertex&);
	vertex& deregisterVertex(vertex&);
public:
	void addEdge(vertex&, vertex&);
	void postRemoveEdge(vertex&, vertex&);
private:
	size_t cc_size(vertex const&) const;
	void assign_new_cc(vertex&, vertex&);
	ccid_t new_ccid();
	size_t set_ccid_recursive(vertex&, ccid_t i);
public:
	std::pair<adjacency_iterator, adjacency_iterator> av(vertex_descriptor v) const{
		return graph_traits<G>::adjacent_vertices(v, _graph);
	}

private:
  unsigned _find_lvl; // maybe not a good place
  G& _graph;

private:
	std::stack<size_t> _garbage; // unused cc_numbers
};
/*--------------------------------------------------------------------------*/
// assign unused or new ccid to vertex.
template<class G>
typename ConnectedComponents<G>::vertex&
ConnectedComponents<G>::registerVertex(
		typename ConnectedComponents<G>::vertex& t)
{
//	assert(index(t) == INVALID);
	auto cc = new_ccid();
	assert(cc); // BUG

	graph_traits<G>::set_cc(t, cc);
	graph_traits<G>::cc_size(cc, _graph) += 1;
	return t;
}
/*--------------------------------------------------------------------------*/
template<class T>
typename ConnectedComponents<T>::ccid_t ConnectedComponents<T>::new_ccid()
{
	return graph_traits<T>::new_cc(_graph);
}
/*--------------------------------------------------------------------------*/
// take out vertex.
// this may lead to an unused cc id. garbage collect it.
template<class T>
typename ConnectedComponents<T>::vertex&
ConnectedComponents<T>::deregisterVertex(
		typename ConnectedComponents<T>::vertex& t)
{
	auto cc = graph_traits<T>::get_cc(t);
	assert(cc != graph_traits<T>::invalid_cc());

	assert(graph_traits<T>::cc_size(cc, _graph));
	size_t n = --graph_traits<T>::cc_size(cc, _graph);

	graph_traits<T>::set_cc(t, graph_traits<T>::invalid_cc());

	trace1("more vertices?", n);
	if (n==0){
		graph_traits<T>::del_cc(cc, _graph);
	}else{
		trace1("strange", n);
	}
	return t;
}
/*--------------------------------------------------------------------------*/
// set ccid for connected component containing v
template<class T>
size_t ConnectedComponents<T>::set_ccid_recursive(
		typename ConnectedComponents<T>::vertex& v,
		typename ConnectedComponents<T>::ccid_t i)
{
	unsigned cnt=0;
	if(graph_traits<T>::get_cc(v) != i){
		graph_traits<T>::set_cc(v, i);
		auto a = av(v);
		for(auto n=a.first; n!=a.second; ++n){
			auto nn = *n;
			assert(nn);
			cnt += set_ccid_recursive(nn, i);
		}
		cnt += 1;
	}else{
	}
	return cnt;
}
/*--------------------------------------------------------------------------*/
template<class T>
void ConnectedComponents<T>::addEdge(
		typename ConnectedComponents<T>::vertex& v,
		typename ConnectedComponents<T>::vertex& w)
{
	assert(graph_traits<T>::get_cc(v) != graph_traits<T>::invalid_cc());
	assert(graph_traits<T>::get_cc(w) != graph_traits<T>::invalid_cc());

	trace2("addEdge", graph_traits<T>::get_cc(v), graph_traits<T>::get_cc(w));
	if(graph_traits<T>::get_cc(v) == graph_traits<T>::get_cc(w)){
		// we are done.
	}else{
		auto freed_cc = graph_traits<T>::get_cc(v);
		vertex* smallerv = &v;
		auto ccv = graph_traits<T>::get_cc(v);
		auto ccw = graph_traits<T>::get_cc(w);
		auto nn = ccw;


	  	if(graph_traits<T>::cc_size(ccw, _graph) < graph_traits<T>::cc_size(ccv, _graph)){
			smallerv = &w;
			nn = ccv;
			freed_cc = graph_traits<T>::get_cc(w);
		}else{
		}

		size_t howmany = set_ccid_recursive(*smallerv, nn);
		graph_traits<T>::cc_size(nn, _graph) += howmany;
		assert(graph_traits<T>::cc_size(freed_cc, _graph) == howmany);
		graph_traits<T>::cc_size(freed_cc, _graph) = 0;
		graph_traits<T>::del_cc(freed_cc, _graph);
	}
}
/*--------------------------------------------------------------------------*/
template<class T>
size_t ConnectedComponents<T>::cc_size(
		typename ConnectedComponents<T>::vertex const& w) const
{
	auto vcc = graph_traits<T>::get_cc(w);
	return graph_traits<T>::cc_size(vcc, _graph);
}
/*--------------------------------------------------------------------------*/
template<class T>
void ConnectedComponents<T>::assign_new_cc(
		typename ConnectedComponents<T>::vertex& v,
		typename ConnectedComponents<T>::vertex& w)
{
	auto nn = ConnectedComponents<T>::new_ccid();
	size_t howmany = set_ccid_recursive(v, nn);

	auto vcc = graph_traits<T>::get_cc(v);
	auto wcc = graph_traits<T>::get_cc(w);

	graph_traits<T>::cc_size(vcc, _graph) += howmany;
	graph_traits<T>::cc_size(wcc, _graph) -= howmany;
	trace1("reassigned", howmany);
}
/*--------------------------------------------------------------------------*/
template<class T>
void ConnectedComponents<T>::postRemoveEdge(
		ConnectedComponents::vertex& v, ConnectedComponents::vertex& w)
{
	assert(graph_traits<T>::get_cc(v)); // BUG
	assert(graph_traits<T>::get_cc(w)); // BUG

	assert(graph_traits<T>::get_cc(v) == graph_traits<T>::get_cc(w));
	// now let's see if they are still connected.
	typedef FindVertex<ConnectedComponents<T>, size_t> vertexfinder;
	//_g.
	next_level();

	// TODO: reuse memory
	// _fv->init(v,w);
	// _fw->init(w,v);
	vertexfinder fv(*this, v, w);
	vertexfinder fw(*this, w, v);
	vertexfinder* fvp=&fv;
	vertexfinder* fwp=&fw;

	typename vertexfinder::status_t stat = vertexfinder::sDUNNO;

	while(stat == vertexfinder::sDUNNO){
		std::swap(fvp, fwp);
		stat = fvp->step();
	}

	auto vbig = fvp->needle(); // starting fom v failed, hence the
	                           // needle is in the bigger subgraph
	auto vsmall = fwp->needle();
	assert(graph_traits<T>::get_cc(vsmall) == graph_traits<T>::get_cc(vbig));

	trace2("find", stat, cc_size(vsmall));

	switch(stat){
	case vertexfinder::sFAIL:
		// no longer connected. reassign the smaller one.
		assign_new_cc(vsmall, vbig); // ??!
		break;
	case vertexfinder::sSUCCESS:
	   // still connected. done
		break;
	default:
		unreachable();
	}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
