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
void Symbol::recreate(){ // }SchematicModel const&){ untested();
}

Symbol::Symbol()
    : Element(),
		_subckt(nullptr)
{ untested();
}

Symbol::Symbol(Symbol const& s)
    : Element(s),
		_subckt(nullptr)
{ untested();
  setTypeName(s.typeName());
}

SchematicModel* Symbol::scope()
{ untested();
	if(auto o=dynamic_cast<Symbol*>(owner())){ untested();
		return o->subckt();
	}else{ untested();
		return nullptr;
	}
}

// reuse overrides to give both const and non-const access.
SchematicModel const* Symbol::scope() const
{ untested();
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
{ untested();
	trace2("connectNode", label(), i);
	Port const& pp = port(i);
	Port& mp = port(i);
	Node* n = &nm.at(pp.x_()+cx(), pp.y_()+cy());
	assert(n->hasNet());

// 	if(auto c=dynamic_cast<Conductor*>(this)){ untested();
// 		l.addEdge(n, c);
// 	}else{ untested();
// 	}
//	n->connectionsAppend(this); // why??
	mp.connect(n);
	return n;
}

Node* Symbol::disconnectNode(unsigned i, NodeMap&)
{ untested();
	trace2("disconnectNode", label(), i);
	Port& mp = port(i);
	Node* n = mp.value();
	assert(n); // disconnecting twice is not allowed.
	mp.disconnect();
	//n->connectionsRemove(this);

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
{ untested();
  assert(i<unsigned(numPorts()));
  if(port(i).isConnected()){ untested();
	  assert( port(i).value()->net() );
	  return port(i).value()->net();
  }else{ untested();
	  return nullptr;
  }
}

// "position"?
std::pair<int, int> Symbol::center()const
{ untested();
	return std::make_pair(_cx, _cy);
}

Port const& Symbol::port(unsigned i) const
{ untested();
	Symbol* s=const_cast<Symbol*>(this);
	return s->port(i);
}

std::string Symbol::getParameter(std::string const& n) const
{ untested();
	if(n=="$xposition"){ untested();
		return std::to_string(cx());
	}else if(n=="$yposition"){ untested();
		return std::to_string(cy());
	}else{ untested();
	  throw ExceptionCantFind(n, label().toStdString());
	}
}

#include "geometry.h"
void Symbol::paint(ViewPainter* p) const
{ untested();
	for(unsigned i=0; i<numPorts(); ++i){ itested();
		if(!port(i).isConnected()){ untested();
			p->setPen(QPen(Qt::red,2));
		}else if(port(i)->degree()==0){ untested();
			unreachable();
		}else if(port(i)->degree()==1){ untested();
			auto pp = portPosition(i);
			auto x = getX(pp)-_cx;
			auto y = getY(pp)-_cy;
			// p->setPen(QPen(Qt::red,2));
			p->setPen(QPen(Qt::red,1));
			p->drawEllipse(x-2, y-2, 4, 4);
		}else if(port(i)->degree()==3){ untested();
			auto pp = portPosition(i);
			auto x = getX(pp)-_cx;
			auto y = getY(pp)-_cy;
			p->setPen(QPen(Qt::black,2));
			p->drawEllipse(x-1, y-1, 2, 2);
		}else{ untested();
		}
	}
}

// global position? rename to netPosition??
std::pair<int, int> Symbol::portPosition(unsigned i) const
{ untested();
	assert(port(i).isConnected());
	auto p = port(i)->position();	
	return p;
}

// BUG: not here. legacy stuff...
void Symbol::new_subckt()
{ untested();
	assert(!_subckt);
	_subckt = new SchematicModel(nullptr);
}

Symbol::~Symbol()
{ untested();
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
{ untested();
	std::string n = name.toStdString();
	std::string v = b.toStdString();
	setParameter(n, v);
}

void Symbol::setParameter(unsigned pos, QString const& b)
{ untested();
	auto v = b.toStdString();
	setParameter(pos, v);
}
