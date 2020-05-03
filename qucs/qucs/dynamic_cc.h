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
// the Graph has vertices (aka Nodes) and edges (aka Wires).
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


// ** this could be part of Conductor. but it' better kept separate ** //

#ifndef QUCS_DYNAMIC_CC
#define QUCS_DYNAMIC_CC

#include <stack>

const size_t INVALID = -1;

/*--------------------------------------------------------------------------*/
// find a node in a graph
template<class G, class V>
class FindVertex{
private:
	typedef typename G::vertex_iterator vertex_iterator;
public:
	enum status_t { sDUNNO, sSUCCESS, sFAIL };
private:
	FindVertex(FindVertex const&){ unreachable(); }
public:
	explicit FindVertex(G const& g, V const& start, V const& needle)
		: _graph(g), _needle(needle) {

		_stack.push(start, neighbors(start).begin());
	}
public:
	status_t step();

private:
	G const& _graph;
	V const& _needle;
	std::stack<std::pair<V, vertex_iterator> > _stack;
};
/*--------------------------------------------------------------------------*/
template<class G, class V>
typename FindVertex<G, V>::status_t FindVertex<G, V>::step()
{
	auto t = _stack.top();

	if (_stack.empty()){ untested();
		return sFAIL;
	}else if(t.second == _needle){ untested();
		return sSUCCESS;
	}else if(t.second=neighbors(t.first).end()){ untested();
		_stack.pop();
	}else{ untested();
		auto next = t.second;
		++t.second;
		_stack.push(std::make_pair(next, neighbors(next).begin()));
	}

	return sDUNNO;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<class G>
class ConnectedComponents {
public:
	explicit ConnectedComponents() : _find_lvl(0) {}
public:
	typedef typename graph_traits<G>::vertex_descriptor vertex;
	typedef size_t ccid_t;

	unsigned next_level(){
		if(_find_lvl == -1u){
			incomplete(); // reset all nodes
			_find_lvl = 0;
		}else{
		}
		return ++_find_lvl;
	}

public:
	vertex& registerVertex(vertex&);
	vertex& deregisterVertex(vertex&);
public:
	void preAddEdge(vertex&, vertex&);
	void postRemoveEdge(vertex&, vertex&);
private:
	void separate(vertex&, vertex&);
	ccid_t new_ccid();
	size_t set_ccid_recursive(vertex&, ccid_t i);

private:
  unsigned _find_lvl;

private:
	std::stack<size_t> _garbage; // unused cc_numbers
	std::vector<size_t> _cc_size; // size of connected component
};
/*--------------------------------------------------------------------------*/
// assign unused or new ccid to vertex.
template<class T>
typename ConnectedComponents<T>::vertex&
ConnectedComponents<T>::registerVertex(
		typename ConnectedComponents<T>::vertex& t)
{
	assert(index(t) == INVALID);
	auto cc = ConnectedComponents<T>::new_ccid();

	set_ccid(t, cc);
	_cc_size[cc] += 1;
	return t;
}
/*--------------------------------------------------------------------------*/
template<class T>
typename ConnectedComponents<T>::ccid_t ConnectedComponents<T>::new_ccid()
{
	ccid_t cc;
	if(_garbage.size()){
		cc = _garbage.top();
		_garbage.pop();

		assert(!_cc_size[cc]);
	}else{
		cc = _cc_size.size();
		_cc_size.push_back(0);
	}
	return cc;
}
/*--------------------------------------------------------------------------*/
// take out vertex.
// this may lead to an unused cc id. garbage collect it.
template<class T>
typename ConnectedComponents<T>::vertex&
ConnectedComponents<T>::deregisterVertex(
		typename ConnectedComponents<T>::vertex& t)
{
	size_t idx=index(t);
	assert(_cc_size[idx]);
	size_t n = --_cc_size[idx];
	if (n==0){ untested();
		_garbage.push(idx);
	}else{ untested();
	}
	set_index(t, INVALID);
}
/*--------------------------------------------------------------------------*/
// set ccid for connected component containing v
template<class T>
size_t ConnectedComponents<T>::set_ccid_recursive(
		typename ConnectedComponents<T>::vertex& v,
		typename ConnectedComponents<T>::ccid_t i)
{
	unsigned cnt=0;
	if(ccid(v) != i){ untested();
		for(auto n : neighbors(v)){
			cnt += set_ccid_recursive(n, i);
		}
		cnt += 1;
		set_ccid(v, i);
	}else{ untested();
	}
	return cnt;
}
/*--------------------------------------------------------------------------*/
template<class T>
void ConnectedComponents<T>::preAddEdge(
		typename ConnectedComponents<T>::vertex& v,
		typename ConnectedComponents<T>::vertex& w)
{
}
/*--------------------------------------------------------------------------*/
template<class T>
void ConnectedComponents<T>::separate(
		typename ConnectedComponents<T>::vertex& v,
		typename ConnectedComponents<T>::vertex& w)
{
	vertex* smallerv = &v;
	vertex* biggerv = &v;
	if(_cc_size[cc_id(w)] < _cc_size[cc_id(v)]){ untested();
		smallerv = &w;
	}else{ untested();
		biggerv = &w;
	}

	auto nn = ConnectedComponents<T>::new_ccid();
	size_t howmany = set_ccid_recursive(smallerv, nn);

	_cc_size[nn] += howmany;
	_cc_size[cc_id(*biggerv)] -= howmany;
}
/*--------------------------------------------------------------------------*/
template<class T>
void ConnectedComponents<T>::postRemoveEdge(
		ConnectedComponents::vertex& v, ConnectedComponents::vertex& w)
{
	assert(cc_id(v) == cc_id(w));
	// now let's see if they are still connected.
	//
	typedef FindVertex<ConnectedComponents<T>, size_t> vertexfinder;

	vertexfinder fv(*this, v, w);
	vertexfinder fw(*this, w, v);

	//_g.
	next_level();

	typename vertexfinder::status_t stat = vertexfinder::sDUNNO;

	while(stat == vertexfinder::sDUNNO){
		std::swap(fv, fw);
		stat = fv.step();
	}

	switch(stat){
	case vertexfinder::sFAIL:
		separate(v, w);
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
