/***************************************************************************
                               node_adj.h
                                --------
    begin                : Sat Sep 20 2003
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
#ifdef QUCS_NODE_ADJ
#define QUCS_NODE_ADJ

class AdjacentNodesIter{
	friend class AdjacentNodes;
private:
	AdjacentNodesIter(Node const& n, Q3PtrList<Element>::iterator i)
		: _n(n),
		  _i(i){
		skip();
	}
	AdjacentNodesIter(AdjacentNodesIter const& c): _n(c._n){unreachable();}
public:
	void operator++(){ ++_i; skip(); }
	void operator->();
	void operator*();
private:
	void skip();
private:
	Node const& _n;
	Q3PtrList<Element>::iterator _i;
};

class AdjacentNodes{
public:
	explicit AdjacentNodes(Node const& n);
private:
	AdjacentNodes(AdjacentNodes const&c): _n(c._n){unreachable();}
public:
	AdjacentNodesIter begin();
	AdjacentNodesIter end();
private:
	Node const& _n;
};

#endif
