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
#ifndef SCKT_BASE_H
#define SCKT_BASE_H
/*--------------------------------------------------------------------------*/
#include "symbol.h"
/*--------------------------------------------------------------------------*/
class SubcktBase : public Symbol {
public:
	explicit SubcktBase() {} // HACK
	~SubcktBase(){
		for(auto pp : _ports){
			delete pp;
		}
	}
protected:
	SubcktBase(SubcktBase const& p) : Symbol(p) {}
private:
	virtual Element* clone()const {unreachable(); return nullptr;}
//	Port& port(unsigned) override {unreachable(); return *new Port();}
	virtual pos_t portPosition(unsigned i) const;
public:
	virtual unsigned numPorts() const { incomplete(); return 0; }

public: // Element
	SchematicModel const* scope() const {
		return Element::scope();
	}

private: // Symbol
	Port& port(unsigned i) override;

public:
	virtual bool makes_own_scope()const  {return false;}
	SchematicModel const* subckt() const{ return _subckt; }
	SchematicModel* subckt(){ return _subckt; }
	void new_subckt();

private:
	std::vector<Port*> _ports;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
