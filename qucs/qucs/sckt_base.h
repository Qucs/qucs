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

#include "symbol.h"

class SubcktBase : public Symbol {
private:
	virtual Element* clone()const {unreachable(); return nullptr;}
	virtual unsigned numPorts() const { incomplete(); return 0; }
	virtual pos_t portPosition(unsigned) const{ unreachable(); return pos_t(0,0);}
	Port& port(unsigned i){unreachable(); return *new Port();}
public:
	SchematicModel const* subckt() const{ return _subckt; }
	SchematicModel* subckt(){ return _subckt; }
	void new_subckt();
};

#endif
