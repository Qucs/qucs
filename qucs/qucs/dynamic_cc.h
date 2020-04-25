/***************************************************************************
                            connected_components.h
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
#ifndef QUCS_DYNAMIC_CC
#define QUCS_DYNAMIC_CC

// keep track of "nets", i.e. connected components in the circuit graph.
// the Graph has vertices (aka Nodes) and edges (aka shorts).
//
// each node x has a number n(x), two nodes have the same number, iff they are
// connected. the number identifies a net (aka "node" in spice).
//
// storing the connectivity graph here, and only here.
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


template<class T>
class ConnectedComponents {
public:
	typedef T vertex;

public:
	vertex& registerVertex(vertex&);
	vertex& deregisterVertex(vertex&);
public:
	void preAddEdge(vertex&, vertex&);
	void postRemoveEdge(vertex&, vertex&);

private:
	size_t set_ccid_recursive(vertex&);

private:
	std::stack<size_t> _garbage; // unused cc_numbers
	std::stack<size_t> _cc_size; // size of connected component
}

// assign unused or new ccid to vertex.
template<class T>
T& ConnectedComponents::registerVertex(T t)
{
	assert(index(t) == INVALID);
	size_t idx;
	if(_garbage.size()){
		idx = _garbage.top();
		_garbage.pop();
		_vertices[idx] = t;
	}else{
		idx = _vertices.size();
		_vertices.push_back(t);
	}
	set_ccid(t, idx);
	return t;
}

// take out vertex.
// this may lead to an unused cc id. garbage collect it.
template<class T>
T& ConnectedComponents::deregisterVertex(vertex& t)
{
	size_t idx=index(t);
	assert(_cc_size[idx]);
	size_t n = --_cc_size[idx];
	if (n==0){ untested();
		_garbage.push_back(idx);
	}else{ untested();
	}
	set_index(t, INVALID);
}

// set ccid for connected component containing v
template<class T>
size_t ConnectedComponents::set_ccid_recursive(
		T& v, ConnectedComponents<T>::cc_id i)
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

#endif
