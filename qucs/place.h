/***************************************************************************
    copyright            : (C) 2016, 2019, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// place.
#ifndef QUCS_PLACE_H
#define QUCS_PLACE_H
/*--------------------------------------------------------------------------*/
#include "symbol.h"
#include "exception.h"
#include "geometry.h"
/*--------------------------------------------------------------------------*/
class Place : public Symbol{
private:
	explicit Place(Place const& p)
		: Symbol(p),
	     _port(p._port){
	}
public: // construct
	explicit Place(pos_t p) : _port(0, 0){
		setCenter(p);
	}
	virtual ~Place() {}

private: // Element
	virtual Element* clone()const {return new Place(*this); }
	void paint(ViewPainter*) const override {incomplete();}

private: // Symbol
	virtual unsigned numPorts() const {return 1;}

public: // Node stuff
	virtual Node* connectNode(unsigned idx, NodeMap&){incomplete(); return nullptr;}
	virtual Node* disconnectNode(unsigned idx, NodeMap&){incomplete(); return nullptr;}

public: // Port access
	virtual pos_t portPosition(unsigned i) const{
		assert(i==0);
		return pos_t(0, 0);
	}
	virtual pos_t nodePosition(unsigned) const{
		return pos_t(center());
	}

	virtual Net const* portValue(unsigned) const{
		incomplete(); return nullptr;
	}
	Node const* portNode(unsigned) const{
		incomplete(); return nullptr;
	}

private: // internal port access
	virtual Port& port(unsigned i){
		assert(i==0);
		return _port;
	}

private:
	Port _port;
}; // symbol
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
