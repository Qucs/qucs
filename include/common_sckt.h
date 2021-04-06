/***************************************************************************
    copyright            : (C) 2019, 2020, 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QUCS_COMMON_SUBCKT
#define QUCS_COMMON_SUBCKT

#include <memory>
#include "paramlist.h"

namespace qucs {

using qucs::CommonParamlist;

// TODO? use commonComponent instead and stick a symbol to it.
class CommonSubckt : public CommonParamlist{
public:
	explicit CommonSubckt(int x)
		: CommonParamlist(x){
		_subckt = std::make_shared<ElementList>();
	}
	explicit CommonSubckt(CommonSubckt const& c)
		: CommonParamlist(c), _subckt(c._subckt)
	{
		// use real ports?
		for(auto& p : c._ports){
			if(p){
				_ports.push_back(new Port(p)); // clone?
			}else{
				_ports.push_back(nullptr);
			}
		}
	}

	~CommonSubckt(){
		for(auto& p : _ports){
			delete p;
			p = nullptr;
		}
	}
public:
	ElementList* subckt(){ return _subckt.get(); }
	ElementList const* subckt() const{ return _subckt.get(); }
	CommonSubckt* clone()const override{
		incomplete();
		return new CommonSubckt(*this);
	}
   bool operator==(const CommonComponent& x)const override{
		auto p = dynamic_cast<const CommonSubckt*>(&x);
		bool rv = p
			&& _subckt == p->_subckt
			&& CommonParamlist::operator==(x);

		return rv;
	}
	pos_t portPosition(index_t i) const;
	index_t numPorts() const {
		return _ports.size();
	}
	std::string port_value(index_t i) const;

private:
public: // hmm
	Port& port(index_t i){
		trace2("cport", i, _ports.size());
		if(i<_ports.size()){
		}else{
			_ports.resize(i+1);
		}
		if(_ports[i]){
			trace2("oldport", i, _ports.size());
		}else{
			trace2("newport", i, _ports.size());
			_ports[i] = new Port();
		}
		return *_ports[i];
	}
	Port const& port(index_t i)const{
		auto s=const_cast<CommonSubckt*>(this);
		return s->port(i);
	}
private:
	std::shared_ptr<ElementList> _subckt;
	std::vector<Port*> _ports;
};

} // qucs

#endif
