/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
	                           2018, 20 Felix Salfelder / QUCS
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QUCS_CONDUCTOR_H
#define QUCS_CONDUCTOR_H

#include "element.h"
//#include "node.h"
// access nodes connected to a Node though a Conductor
//
class Conductor;
class Net;
class AdjConductorIterator{
public:
	typedef std::list<Element*>::iterator elt_iter;
public:
	AdjConductorIterator(elt_iter b, elt_iter e)
		:_elt(b), _end(e){
		skip();
	}
public:
	AdjConductorIterator(AdjConductorIterator const& o)
		: _elt(o._elt),
		  _end(o._end){
	}
public:
	Conductor* operator*();
	
	AdjConductorIterator& operator++(){
		assert(_elt!=_end);
		++_elt;
		skip();
		return *this;
	}
	bool operator==(AdjConductorIterator const& o){
		return _elt==o._elt;
	}
	bool operator!=(AdjConductorIterator const& o){
		return !(*this == o);
	}
	void skip();
private:
	elt_iter _elt;
	elt_iter _end;
};

class AdjConductorRange{
public:
	explicit AdjConductorRange(Conductor& c);
public:
	AdjConductorRange(AdjConductorRange const&o)
		: _cond_begin(o._cond_begin),
		  _cond_end(o._cond_end){
	}
public:
	AdjConductorIterator begin(){
		return AdjConductorIterator(_cond_begin, _cond_end);
	}
	AdjConductorIterator end(){
		return AdjConductorIterator(_cond_end, _cond_end);
	}

private:
	AdjConductorIterator::elt_iter _cond_begin;
	AdjConductorIterator::elt_iter _cond_end;
};

// nodes and wires?
// (this does not make sense, move to Node, rebase Wire)
class Conductor {
public:
  enum{
	  INVALID=-1u
  };
protected:
  explicit Conductor()  {}
  ~Conductor();
public:
//  unsigned netNumber();
//  void resetNetNumber(){ _cn = INVALID; }
//   void setNet(Net* x);
//   bool hasNet() const { return _net; }
// 

public:
  virtual std::list<Element*>::iterator connectionsBegin() = 0;
  virtual std::list<Element*>::iterator connectionsEnd() = 0;

public:
  WireLabel *Label; // BUG
};

#endif // guard
