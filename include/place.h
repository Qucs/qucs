/***************************************************************************
    copyright            : (C) 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_PLACE
#define QUCS_PLACE
#include "symbol.h"
/*--------------------------------------------------------------------------*/
namespace qucs {
/*--------------------------------------------------------------------------*/
class Place : public Symbol{
private:
	explicit Place(Place const& p)
		: Symbol(p){
	}
public: // construct
	explicit Place() : Symbol() { }
	//explicit Place(pos_t p) : Symbol(),  _port(0, 0){
	//	setCenter(p);
	//}
	virtual ~Place() {}

private: // Element
	virtual Element* clone()const {return new Place(*this); }
	rect_t bounding_rect() const override;
	void paint(ViewPainter*) const override;
	bool showLabel() const override;

private: // Symbol
	virtual unsigned numPorts() const {return 1;}
	void set_param_by_name(std::string const&, std::string const&) override;

public: // Node stuff
	//virtual Node* connectNode(unsigned idx, NodeMap&){incomplete(); return nullptr;}
	//virtual Node* disconnectNode(unsigned idx, NodeMap&){incomplete(); return nullptr;}

public: // Port access
	virtual pos_t portPosition(unsigned i) const{
		assert(i==0);
		return pos_t(0, 0);
	}
	virtual pos_t nodePosition(unsigned) const{
		return pos_t(center());
	}

	virtual Node const* portValue(unsigned) const{
		incomplete(); return nullptr;
	}
	Node const* portNode(unsigned) const{
		incomplete(); return nullptr;
	}
	unsigned node_degree() const;

private: // internal port access
	virtual Port& port(unsigned i){
		assert(i==0);
		return _port;
	}

private:
	Port _port;
};
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
using qucs::Place; // transition
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
