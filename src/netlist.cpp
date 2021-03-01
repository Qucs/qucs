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

#include "netlist.h"
#include "net.h"

namespace qucs {

Net* NetList::newNet()
{
	unsigned idx;
	if(_g.empty()){
		idx = _l.size();
		_l.push_back(new Net());
		trace4("newNet", _l.back(), idx, _l.size(), this);
		_l[idx]->setPos(idx);
	}else{
		idx = _g.top();
		_g.pop();
		assert(_l[idx]->size() == size_t(-1));
		_l[idx]->inc_nodes();
		assert(_l[idx]->pos() == idx);
		_l[idx]->set_label("");
	}

	assert(_l[idx]->size() == 0);
	assert(_l[idx]);

	// incomplete. need to negotiate more.
	assert(!_l[idx]->hasLabel());
	return _l[idx];
}

void NetList::delNet(Net* n)
{
	trace1("delNet", n);
	assert(n);
	trace1("delNet", n->size());
	assert(n->size() == 0);
	n->dec_nodes();

	size_t idx = n->pos();
	trace3("delNet", idx, _l.size(), this);
	assert(n==_l[idx]);
	_g.push(idx);
}

} // qucs
