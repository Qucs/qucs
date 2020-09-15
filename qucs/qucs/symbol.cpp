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
void Symbol::recreate(){ // }SchematicModel const&){
}

Symbol::Symbol()
    : Element(),
		_subckt(nullptr)
{
}

Symbol::Symbol(Symbol const& s)
    : Element(s),
		_subckt(nullptr)
{
}

SchematicModel* Symbol::scope()
{
	if(auto o=dynamic_cast<Symbol*>(owner())){
		return o->subckt();
	}else{ untested();
		return nullptr;
	}
}

// reuse overrides to give both const and non-const access.
SchematicModel const* Symbol::scope() const
{
	auto s=const_cast<Symbol*>(this);
	return s->scope();
}

#if 0
// obsolete? portValue?
QString const& Symbol::netLabel(unsigned i) const
{ untested();
//	assert(hasPort(i));
	return portValue(i);
}
#endif

// could as well be NodeMap::connect(Symbol). but why?
Node* Symbol::connectNode(unsigned i, NodeMap&nm)
{
	trace2("connectNode", label(), i);
	Port const& pp = port(i);
	Port& mp = port(i);
	Node* n = &nm.at(pp.x_()+cx_(), pp.y_()+cy_());
	assert(n->hasNet());

// 	if(auto c=dynamic_cast<Conductor*>(this)){
// 		l.addEdge(n, c);
// 	}else{
// 	}
	n->connectionsAppend(this);
	mp.connect(n);
	return n;
}

Node* Symbol::disconnectNode(unsigned i, NodeMap&)
{
	trace2("disconnectNode", label(), i);
	Port& mp = port(i);
	Node* n = mp.value();
	assert(n); // disconnecting twice is not allowed.
	mp.disconnect();
	n->connectionsRemove(this);

	return n;
}

static const QString OPEN_PORT("open");
Net const* Symbol::portValue(unsigned i) const
{
  assert(i<unsigned(numPorts()));
  if(port(i).connected()){
	  assert( port(i).value()->net() );
	  return port(i).value()->net();
  }else{
	  return nullptr;
  }
}

Port const& Symbol::port(unsigned i) const
{
	Symbol* s=const_cast<Symbol*>(this);
	return s->port(i);
}

std::pair<int, int> const& Symbol::portPosition(unsigned i) const
{
	trace3("portPosition", this, i, &port(i));
	trace1("portPosition", port(i).connected());
	trace2("portPosition", port(i).position().first, port(i).position().second);
	assert(port(i).connected());
	return port(i)->position();	
}

// BUG: not here. legacy stuff...
void Symbol::new_subckt()
{
	assert(!_subckt);
	_subckt = new SchematicModel(nullptr);
}

Symbol::~Symbol()
{
	delete _subckt;
	_subckt = nullptr;
}

std::string Symbol::paramValue(unsigned) const
{ untested();
	unreachable();
	return "NA";
}

std::string Symbol::paramName(unsigned) const
{ untested();
	unreachable();
	return "NA";
}

void Symbol::setParameter(QString const& name, QString const& b)
{
	std::string n = name.toStdString();
	std::string v = b.toStdString();
	setParameter(n, v);
}

void Symbol::setParameter(unsigned pos, QString const& b)
{
	auto v = b.toStdString();
	setParameter(pos, v);
}
