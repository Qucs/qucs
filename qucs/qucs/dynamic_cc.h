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
#ifndef QUCS_DYNAMIC_CC
#define QUCS_DYNAMIC_CC

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

/*--------------------------------------------------------------------------*/
// find a node in a graph
template<class G, class V>
class FindVertex{
public:
	enum status { sDUNNO, sSUCCESS, sFAIL };
private:
	FindVertex(FindVertex const&){ unreachable(); }
public:
	explicit FindVertex(G const&, V const& start, V const& needle)
		: _graph(g), _needle(needle) {

		_stack.push(g, neighbors(next).begin();
	}
public:
	status_t step();

private:
	G const& _graph;
	V const& _needle;
	std::stack<std::pair<V, VI> > _stack;
};
/*--------------------------------------------------------------------------*/
FindVertex::status_t FindVertex::step()
{
	t = _stack.top()

	if _stack.empty(){
		return sFAIL;
	}else if(t.second == _needle){
		return sSUCCESS;
	}else if(t.second=neighbors(t.first).end()){
		_stack.pop();
	}else{
		next = t.second;
		++t.second;
		_stack.push(g, neighbors(next).begin();
	}

	return sDUNNO;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<class T>
class ConnectedComponents {
public:
	explicit ConnectedComponents() : _find_lvl(0) {}
public:
	typedef T vertex;
	typedef size_t ccid_t;
private:

public:
	vertex& registerVertex(vertex&);
	vertex& deregisterVertex(vertex&);
public:
	void preAddEdge(vertex&, vertex&);
	void postRemoveEdge(vertex&, vertex&);

private:
	size_t set_ccid_recursive(vertex&, cc_id i);

private:
  unsigned _find_lvl;

private:
	std::stack<size_t> _garbage; // unused cc_numbers
	std::vector<size_t> _cc_size; // size of connected component
}
/*--------------------------------------------------------------------------*/
// assign unused or new ccid to vertex.
template<class T>
T& ConnectedComponents::registerVertex(T t)
{
	assert(index(t) == INVALID);
	auto cc=new_ccid();

	set_ccid(t, cc);
	_cc_size[cc] += 1;
	return t;
}
/*--------------------------------------------------------------------------*/
template<class T>
T& ConnectedComponents::new_ccid()
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
/*--------------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------*/
template<class T>
void ConnectedComponents::preAddEdge(
		ConnectedComponents::vertex& v, ConnectedComponents::vertex& w)
{
}
/*--------------------------------------------------------------------------*/
void ConnectedComponents::void separate(
		ConnectedComponents::vertex& v, ConnectedComponents::vertex& w)
{
	vertex* smallerv = &v;
	vertex* biggerv = &v;
	if(_cc_size[cc_id(w)] < _cc_size[cc_id(v)]){ untested();
		smallerv = &w;
	}else{ untested();
		biggerv = &w;
	}

	auto nn=new_ccid();
	size_t howmany = set_ccid_recursive(smallerv, nn);

	_cc_size[nn] += howmany;
	_cc_size[cc_id(*biggerv)] -= howmany;
}
/*--------------------------------------------------------------------------*/
template<class T>
void ConnectedComponents:: void postRemoveEdge(
		ConnectedComponents::vertex& v, ConnectedComponents::vertex& w)
{
	assert(cc_id(v) == cc_id(w));
	// now let's see if they are still connected.
	//
	FindVertex fv(*this, v, w);
	FindVertex fw(*this, w, v);
	if(_lvl == -1u){
		// reset.
		incomplete();
	}else{
		++_lvl;
	}

	status_t stat = DUNNO;

	while(stat == sDUNNO){
		std::swap(fv, fw);
		status = fv.step();
	}

	switch(stat){
	case sFAIL:
		separate(v, w);
		break;
	case sSUCCESS;
	   // still connected. done
		break;
	default:
		unreachable();
	}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
