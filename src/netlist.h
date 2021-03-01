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

// not a "netlist", but just a list of Nets.
// a Net is a connected component in the conductor graph
//
// nets come and go. keep track and recycle.
/*--------------------------------------------------------------------------*/
#ifndef QUCS_NETLIST_H
#define QUCS_NETLIST_H
/*--------------------------------------------------------------------------*/
#include <stack>
#include <vector>
#include "io_trace.h"
/*--------------------------------------------------------------------------*/
namespace qucs {
/*--------------------------------------------------------------------------*/
class Net;
/*--------------------------------------------------------------------------*/
class NetList{
public:
	typedef std::vector<Net*> container_type;
private:
	typedef std::stack<unsigned> garbage_type;

private:
	NetList( const NetList&){ unreachable(); }

public:
	explicit NetList(){}
	~NetList(){}

public:
	Net* newNet();
	void delNet(Net*);

	size_t size() const{ return _l.size() - _g.size(); }

private:
	container_type _l;
	garbage_type _g;
};
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif // guard
