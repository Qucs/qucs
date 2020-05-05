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

Net* NetList::newNet()
{
	unsigned idx;
	if(_g.empty()){ untested();
		idx = _l.size();
		_l.push_back(new Net());
		trace3("newNet", idx, _l.size(), this);
	}else{ untested();
		idx = _g.top();
		_g.pop();
		assert(_l[idx]->getPos() == idx);
	}

	assert(_l[idx]);
	_l[idx]->setPos(idx);
	_l[idx]->setLabel("_net" + std::to_string(idx));
	return _l[idx];
}

void NetList::delNet(Net* n)
{
	assert(n);
	assert(n->size() == 0);

	size_t idx = n->getPos();
	trace3("delNet", idx, _l.size(), this);
	assert(n==_l[idx]);
	_g.push(idx);
}
