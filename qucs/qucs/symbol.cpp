/***************************************************************************
                                symbol.cpp
                                -----------
    copyright            : (C) 2016-2019 Felix Salfelder / QUCS team
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "symbol.h"
#include "schematic_model.h"
#include <assert.h>
#include "net.h"

// recreate schematic symbol. not sure why, maybe after parameter changes
// (why not just call "Symbol::create??!")
void Symbol::recreate(){
}

Symbol::~Symbol(){
	// disconnect();
}

// obsolete? portValue?
QString const& Symbol::netLabel(unsigned i) const
{ untested();
	auto const* s=getScope();
	assert(s);
//	assert(hasPort(i));
	return portValue(i);
}

// could as well be NodeMap::connect(Symbol). but why?
Node* Symbol::connectNode(unsigned i, NodeMap&l)
{
	Port const& pp = port(i);
	Port& mp = port(i);
	Node* n = &l.at(pp.x_()+cx_(), pp.y_()+cy_());
	assert(n->hasNet());

	if(auto c=dynamic_cast<Conductor*>(this)){
		l.addEdge(n, c);
	}else{
	}
	n->connectionsAppend(this);
	mp.connect(n);
	return n;
}

Node* Symbol::disconnectNode(unsigned i, NodeMap&nm)
{
	Port& mp = port(i);
	Node* n = mp.value();
	assert(n);
	mp.disconnect();
	n->connectionsRemove(this);
	if(auto c=dynamic_cast<Conductor*>(this)){
		// it's a wire.
		nm.postRemoveEdge(n, c);
	}else{ untested();
	}
	if(n->connectionsCount() == 0){
		nm.erase(n);
	}
	return n;
}

static const QString OPEN_PORT("open");
QString const& Symbol::portValue(unsigned i) const
{
  assert(i<unsigned(portCount()));
  if(port(i).connected()){
	  return port(i).netLabel();
  }else{
	  return OPEN_PORT;
  }
}

Port const& Symbol::port(unsigned i) const
{
	Symbol* s=const_cast<Symbol*>(this);
	return s->port(i);
}
