/***************************************************************************
    copyright            : (C) 2016-2020 Felix Salfelder / QUCS team
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
void Symbol::recreate(){ // }SchematicModel const&){ untested();
}
/*--------------------------------------------------------------------------*/
Symbol::Symbol()
    : Element(),
		_subckt(nullptr)
{
}
/*--------------------------------------------------------------------------*/
Symbol::Symbol(Symbol const& s)
    : Element(s),
		_subckt(nullptr)
{
  setTypeName(s.typeName());
}
/*--------------------------------------------------------------------------*/
// dup
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
	Node* n = &nm.at(pp.x_()+cx(), pp.y_()+cy());
	assert(n->hasNet());

// 	if(auto c=dynamic_cast<Conductor*>(this)){ untested();
// 		l.addEdge(n, c);
// 	}else{ untested();
// 	}
	mp.connect(n /*,this*/);
	return n;
}

Node* Symbol::disconnectNode(unsigned i, NodeMap&)
{
	trace2("disconnectNode", label(), i);
	Port& mp = port(i);
	Node* n = mp.value();
	mp.disconnect(n /*, this*/);

	return n;
}

Node const* Symbol::portNode(unsigned i) const
{ untested();
  assert(i<unsigned(numPorts()));
  if(port(i).isConnected()){ untested();
	  assert( port(i).value() );
	  return port(i).value();
  }else{ untested();
	  return nullptr;
  }
}
Net const* Symbol::portValue(unsigned i) const
{
  assert(i<unsigned(numPorts()));
  if(port(i).isConnected()){
	  assert( port(i).value()->net() );
	  return port(i).value()->net();
  }else{
	  return nullptr;
  }
}

// "position"?
std::pair<int, int> Symbol::center()const
{itested();
	return std::make_pair(_cx, _cy);
}

Port const& Symbol::port(unsigned i) const
{
	Symbol* s=const_cast<Symbol*>(this);
	return s->port(i);
}

std::string Symbol::getParameter(std::string const& n) const
{
	if(n=="$xposition"){
		return std::to_string(cx());
	}else if(n=="$yposition"){
		return std::to_string(cy());
	}else if(n=="$mfactor"){
		return "1"; // isActive in Component.
	}else{ untested();
	  throw ExceptionCantFind(n, label().toStdString());
	}
}
/*--------------------------------------------------------------------------*/
std::string Symbol::getParameter(unsigned i) const
{
	throw ExceptionCantFind(std::to_string(i), label().toStdString());
}
/*--------------------------------------------------------------------------*/
#include "geometry.h"
void Symbol::paint(ViewPainter* p) const
{itested();

	 // does not make a lot of sense right now
	for(unsigned i=0; i<numPorts(); ++i){ itested();
//		auto pp = common()->portPosition(i);
		auto pp = portPosition(i);
		int x = getX(pp);
		int y = getY(pp);

		if(!port(i).isConnected()){ untested();
			p->setPen(QPen(Qt::red,2));
			p->drawEllipse(x-1, y-1, 2, 2);
		}else if(port(i)->numPorts()==0){ untested();
			unreachable();
		}else if(port(i)->numPorts()==1){itested();
			// p->setPen(QPen(Qt::red,2));
			p->setPen(QPen(Qt::red,1));
			p->drawEllipse(x-2, y-2, 4, 4);
		}else if(port(i)->numPorts()==3){itested();
			p->setPen(QPen(Qt::black,2));
			p->drawEllipse(x-1, y-1, 2, 2);
		}else{itested();
		}
	}
}

// global position? rename to netPosition??
std::pair<int, int> Symbol::nodePosition(unsigned i) const
{
	assert(port(i).isConnected());
	auto p = port(i)->position();	
	return p;
}
// global position? rename to netPosition??
std::pair<int, int> Symbol::portPosition(unsigned i) const
{
//	assert(port(i).isConnected());
	auto p = port(i).position();	
	return p;
}

// BUG: not here. legacy stuff...
void Symbol::new_subckt()
{
	assert(!_subckt);
	_subckt = new SchematicModel(nullptr);
}
/*--------------------------------------------------------------------------*/
Symbol::~Symbol()
{
	delete _subckt;
	_subckt = nullptr;
}
/*--------------------------------------------------------------------------*/
std::string Symbol::paramValue(unsigned) const
{ untested();
	unreachable();
	return "NA";
}
/*--------------------------------------------------------------------------*/
std::string Symbol::paramName(unsigned) const
{ untested();
	unreachable();
	return "NA";
}
/*--------------------------------------------------------------------------*/
void Symbol::setParameter(QString const& name, QString const& b)
{
	std::string n = name.toStdString();
	std::string v = b.toStdString();
	setParameter(n, v);
}
/*--------------------------------------------------------------------------*/
void Symbol::setParameter(unsigned pos, QString const& b)
{
	auto v = b.toStdString();
	setParameter(pos, v);
}
