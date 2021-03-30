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
#include "element_list.h"
/*--------------------------------------------------------------------------*/
namespace qucs {
/*--------------------------------------------------------------------------*/
void SubcktBase::new_subckt()
{
	assert(!_subckt);
	_subckt = new ElementList();
}
/*--------------------------------------------------------------------------*/
std::string SubcktBase::dev_type()const
{
	if(common()){ untested();
		return common()->modelname();
	}else{ untested();
		unreachable();
		return "bogus_sckt_type";
	}
}
/*--------------------------------------------------------------------------*/
#if 0
pos_t SubcktBase::portPosition(index_t i) const
{
	std::string n = portName(i);
	trace3("SubcktBase::portPosition", i, label(), n);
	//ElementList const* s = subckt(); // scope?
	assert(makes_own_scope());
	ElementList const* s = scope(); // scope?

	if(s){
//		return subckt()->portValue(i)->position();
		auto ii = s->find_(n);
		if(ii!=s->end()){
			trace1("sckt_base port hit ", n);
			return (*ii)->position();
		}else{
			trace2("sckt_base port miss", i, n);
		}
	}else{ untested();
		trace2("sckt_base port miss", i, n);
	}
	unreachable();
	return pos_t(0,0);
}
#endif
/*--------------------------------------------------------------------------*/
Port& SubcktBase::port(index_t i)
{
	assert(scope());
	if(_ports.size() > i){
	}else{
		_ports.resize(i+1);
	}

	if(_ports[i]){
	}else{
		_ports[i] = new Port();
	}

	return *_ports[i];
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
} // qucs
// a bug. need to disentangle from nodemap.
#include "nodemap.h"
/*--------------------------------------------------------------------------*/
namespace qucs {
/*--------------------------------------------------------------------------*/
std::string net_name(SubcktBase const* s, std::string const& n)
{
	assert(s);
	assert(s->scope());
	assert(s->scope()->nodes());
	return s->scope()->nodes()->netName(n);
}
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
