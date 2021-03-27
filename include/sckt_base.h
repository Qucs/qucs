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
#ifndef SCKT_BASE_H
#define SCKT_BASE_H
/*--------------------------------------------------------------------------*/
#include "component.h"
#include <vector>
/*--------------------------------------------------------------------------*/
namespace qucs {
/*--------------------------------------------------------------------------*/
class SubcktBase : public Component {
public:
	explicit SubcktBase() {} // HACK
	~SubcktBase(){
		for(auto pp : _ports){
			delete pp;
		}
	}
protected:
	SubcktBase(SubcktBase const& p) : Component(p) {}
private:
	virtual Element* clone()const {unreachable(); return nullptr;}
//	Port& port(unsigned) override {unreachable(); return *new Port();}
public:
//	virtual pos_t portPosition(unsigned i) const;
	virtual unsigned numPorts() const { return net_nodes(); }

public: // Element
	ElementList const* scope() const {
		return Element::scope();
	}
	ElementList* scope() {
		return Element::scope();
	}

private: // Symbol
	Port& port(unsigned i) override;

public:
	std::string dev_type()const override;

public:
	virtual bool makes_own_scope()const  {return false;}
	ElementList const* subckt() const{ return _subckt; }
	ElementList* subckt(){ return _subckt; }
	void new_subckt();
	index_t net_nodes() const{return _ports.size();}

private:
	std::vector<Port*> _ports;
};
/*--------------------------------------------------------------------------*/
std::string net_name(SubcktBase const* s, std::string const& n);
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
