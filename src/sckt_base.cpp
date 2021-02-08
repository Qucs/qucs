/***************************************************************************
    copyright            : (C) 2019, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sckt_base.h"
#include "schematic_model.h"
/*--------------------------------------------------------------------------*/
void SubcktBase::new_subckt()
{
	assert(!_subckt);
	_subckt = new SchematicModel();
}
/*--------------------------------------------------------------------------*/
std::string SubcktBase::dev_type()const
{
	assert(common());
	return common()->modelname();
}
/*--------------------------------------------------------------------------*/
pos_t SubcktBase::portPosition(unsigned i) const
{
	std::string n = portName(i);
	SchematicModel const* s = subckt(); // scope?

	if(s){
//		return subckt()->portValue(i)->position();
		auto i = s->find_(n);
		if(i!=s->end()){
			return (*i)->position();
		}else{
		}

	}else{ untested();
	}
	unreachable();
	return pos_t(0,0);
}
/*--------------------------------------------------------------------------*/
Port& SubcktBase::port(index_t i)
{ untested();
	assert(scope());
	if(_ports.size() > i){ untested();
	}else{ untested();
		_ports.resize(i+1);
	}

	if(_ports[i]){
	}else{
		_ports[i] = new Port();
	}

	return *_ports[i];
}
/*--------------------------------------------------------------------------*/
// a bug. need to disentangle from nodemap.
#include "nodemap.h"
std::string net_name(SubcktBase const* s, std::string const& n)
{
	assert(s);
	assert(s->scope());
	assert(s->scope()->nodes());
	return s->scope()->nodes()->netName(n);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
