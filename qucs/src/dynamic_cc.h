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
template<class G>
class FindVertex{
private:
	typedef typename G::adjacency_iterator adjacency_iterator;
	typedef typename G::vertex_descriptor vertex_descriptor;
public:
	enum status_t { sDUNNO = 0, sSUCCESS = 1, sFAIL = 2};
private:
	FindVertex(FindVertex const&){ unreachable(); }
public:
	explicit FindVertex(int id, G const& g) : _id(id), _graph(g) {}
	~FindVertex(){
		clearstack();
	}
public:
	void init(vertex_descriptor const& start, vertex_descriptor& needle);

public:
	vertex_descriptor& needle() { return _needle; }
	status_t dfs_step();

private:
	std::pair<adjacency_iterator, adjacency_iterator> av(vertex_descriptor v){
		return _graph.av(v);
	}
	void skip_to_unvisited_or_end(adjacency_iterator& i, adjacency_iterator const& end);
	void clearstack(){
		while(_stack.size()){
			_stack.pop();
		}
	}

private:
	int _id;
	G const& _graph;
	vertex_descriptor _needle;
	std::stack<std::pair<adjacency_iterator, adjacency_iterator> > _stack;
};
/*--------------------------------------------------------------------------*/
template<class G>
void FindVertex<G>::skip_to_unvisited_or_end(
	typename  FindVertex<G>::adjacency_iterator& i,
	typename  FindVertex<G>::adjacency_iterator const& end)
{
	while (i!=end){
		if(_graph.visited(*i, (_id-1)%2)){
			// the other process has been here.
			break;
		}else if(_graph.visited(*i, _id)){
			++i;
		}else{
			break;
		}
	}
}
/*--------------------------------------------------------------------------*/
template<class G>
typename FindVertex<G>::status_t FindVertex<G>::dfs_step()
{
	status_t st = sDUNNO;

	if (_stack.empty()){
		st = sFAIL;
	}else{
		trace1("step", _stack.size());
		auto& t = _stack.top();

		assert(t.first == t.second || *t.first);

		if(t.first == t.second){
			_stack.pop();
		}else if(_graph.visited(*t.first, (_id+1)%2)){
			st = sSUCCESS;
		}else{
			vertex_descriptor current = *t.first;
			++t.first;
			skip_to_unvisited_or_end(t.first, t.second);
			_graph.visit(current, _id);

			auto ff = av(current).first;
			auto gg = av(current).second;
			skip_to_unvisited_or_end(ff, gg);

			// TODO: tail recursion if t.second==t.first
			assert(ff == gg || *ff);
			_stack.push(std::make_pair(ff, gg));
		}
	}

	return st;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<class G>
class ConnectedComponents {
private:
	typedef FindVertex<ConnectedComponents<G> > vertexfinder;
public:
	typedef typename graph_traits<G>::adjacency_iterator adjacency_iterator;
	typedef typename graph_traits<G>::vertex_descriptor vertex_descriptor;
	typedef typename graph_traits<G>::vertex_descriptor vertex;
	typedef typename graph_traits<G>::cc_descriptor ccid_t;
public:
	explicit ConnectedComponents(G& g)
	  	: _find_lvl(0),
		  _graph(g),
        _fv(0, *this),
        _fw(1, *this)
	{}
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
	void visit(vertex_descriptor const& v, int id) const{
		graph_traits<G>::visit(v, _find_lvl-id);
	}
	bool visited(vertex_descriptor const& v, int id) const{
		return graph_traits<G>::visited(v, _find_lvl-id);
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
	size_t swap_cc_recursive(vertex&, ccid_t from, ccid_t to, bool root=false);
public:
	std::pair<adjacency_iterator, adjacency_iterator> av(vertex_descriptor v) const{
		return graph_traits<G>::adjacent_vertices(v, _graph);
	}

private:
  unsigned _find_lvl; // maybe not a good place
  G& _graph;

private:
	std::stack<size_t> _garbage; // unused cc_numbers

	vertexfinder _fv;
	vertexfinder _fw;
}; // ConnectedComponents
/*--------------------------------------------------------------------------*/
// assign unused or new ccid to vertex.
template<class G>
typename ConnectedComponents<G>::vertex&
ConnectedComponents<G>::registerVertex(
		typename ConnectedComponents<G>::vertex& t)
{
	trace1("dcc::rv", t);
	graph_traits<G>::new_cc(t, _graph);
	return t;
}
/*--------------------------------------------------------------------------*/
#if 0
template<class T>
typename ConnectedComponents<T>::ccid_t ConnectedComponents<T>::new_ccid()
{
	return graph_traits<T>::new_cc(_graph);
}
#endif
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

	graph_traits<T>::unset_cc(t, cc);

	if(!graph_traits<T>::cc_size(cc, _graph)) {
		graph_traits<T>::del_cc(cc, _graph);
	}else{ untested();
	}

	return t;
}
/*--------------------------------------------------------------------------*/
// set ccid for connected component containing v
template<class T>
size_t ConnectedComponents<T>::swap_cc_recursive(
		typename ConnectedComponents<T>::vertex& v,
		typename ConnectedComponents<T>::ccid_t from,
		typename ConnectedComponents<T>::ccid_t to,
		bool cont)
{
	unsigned cnt=0;
	if(graph_traits<T>::get_cc(v) != to){
		graph_traits<T>::unset_cc(v, from);
		graph_traits<T>::set_cc(v, to);
		cont = true;
	}else{
	}

	if(cont){
		auto a = av(v);
		for(auto n=a.first; n!=a.second; ++n){
			auto nn = *n;
			assert(nn);
			cnt += swap_cc_recursive(nn, from, to);
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
		trace0("addEdge merge");
		auto from = graph_traits<T>::get_cc(v);
		vertex* smallerv = &v;
		auto ccv = graph_traits<T>::get_cc(v);
		auto ccw = graph_traits<T>::get_cc(w);
		auto to = ccw;


		if(graph_traits<T>::cc_size(ccw, _graph) < graph_traits<T>::cc_size(ccv, _graph)){
			smallerv = &w;
			to = ccv;
			from = graph_traits<T>::get_cc(w);
		}else{
		}
		trace1("addedge", graph_traits<T>::cc_size(from, _graph) );
		trace1("addedge", graph_traits<T>::cc_size(to, _graph) );

		size_t howmany = swap_cc_recursive(*smallerv, from, to);
		trace1("addedge", howmany);
		assert(graph_traits<T>::cc_size(from, _graph) == 0);

		if(graph_traits<T>::cc_size(from, _graph)==0){
			graph_traits<T>::del_cc(from, _graph);
		}else{
		}
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
	auto from = graph_traits<T>::get_cc(v);
	auto wcc = graph_traits<T>::get_cc(w);

	trace1("reassign", graph_traits<T>::cc_size(from, _graph));
	trace1("reassign", graph_traits<T>::cc_size(wcc, _graph));

	graph_traits<T>::unset_cc(v, from);
	ccid_t to = graph_traits<T>::new_cc(v, _graph);

	size_t howmany = swap_cc_recursive(v, from, to, true);

	trace1("reassigned", howmany);
	assert(graph_traits<T>::cc_size(wcc, _graph));
	assert(graph_traits<T>::cc_size(from, _graph));
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

	next_level();
	next_level();

	_fv.init(v,w);
	_fw.init(w,v);
	vertexfinder* fvp=&_fv;
	vertexfinder* fwp=&_fw;

	typename vertexfinder::status_t stat = vertexfinder::sDUNNO;

	while(stat == vertexfinder::sDUNNO){
		if(1){
			std::swap(fvp, fwp);
		}else{
		}
		stat = fvp->dfs_step();
	}

	auto vbig = fvp->needle(); // starting fom v failed, hence the
	                           // needle is in the bigger subgraph
	auto vsmall = fwp->needle();
	assert(graph_traits<T>::get_cc(vsmall) == graph_traits<T>::get_cc(vbig));

	switch(stat){
	case vertexfinder::sFAIL:
		// no longer connected. reassign the smaller one.
		trace3("find fail", stat, cc_size(vsmall), cc_size(vbig));
		assign_new_cc(vsmall, vbig); // ??!
		break;
	case vertexfinder::sSUCCESS:
		trace3("find success", stat, cc_size(vsmall), cc_size(vbig));
	   // still connected. done
		break;
	default:
		assert(false);
	}
}
/*--------------------------------------------------------------------------*/
template<class G>
void FindVertex<G>::init(vertex_descriptor const& start, vertex_descriptor& needle)
{
	clearstack();
	_needle = needle; // bug

	assert(!_graph.visited(start, _id));
	assert(start != needle);

	auto f = av(start).first;
	auto fe = av(start).second;
	if(f==fe){
	}else{
		assert(!_graph.visited(*f, _id));
	}
	_graph.visit(start, _id);
	skip_to_unvisited_or_end(f, fe);
	assert(f==fe || *f);
	_stack.push(std::make_pair(f, fe));
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
