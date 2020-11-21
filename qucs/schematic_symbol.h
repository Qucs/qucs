/***************************************************************************
    copyright            : (C) 2018, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QUCS_SCHEMATIC_SYMBOL_H
#define QUCS_SCHEMATIC_SYMBOL_H
/*--------------------------------------------------------------------------*/
#include "sckt_base.h"
/*--------------------------------------------------------------------------*/
class PaintingList;
class WireList;
class NodeMap;
class DiagramList;
class PaintingList;
class CmdEltList;
class ComponentList;
/*--------------------------------------------------------------------------*/
// A symbol ("component"), formerly implicit schematic contents.
// needs work. rename to DocSymbol? or QucsDoc : Symbol?
class SchematicSymbol : public SubcktBase{
protected:
	SchematicSymbol(SchematicSymbol const& p);
public:
	explicit SchematicSymbol();
	~SchematicSymbol();

private: // Symbol
  virtual void setPort(unsigned, Node*){ incomplete(); }

public: // subckt. could rename to "model", but is it really worth it?
  // BUG: is this not in SubcktBase?
  SchematicModel* subckt() {return _subckt;}
  SchematicModel const* subckt() const {return _subckt;}

public:
	WireList const& wires() const;
	NodeMap const& nodes() const;
	DiagramList const& diagrams() const;
	PaintingList const& paintings() const;
	CmdEltList const* commands() const;
	ComponentList const& components() const; // possibly "devices". lets see.
	PaintingList const* symbolPaintings() const override;

	// commands were part of the circuit model. fix this.
	// taskElementList const& commands() const;

public:
	std::string paramValue(std::string const&n) const{
		return SubcktBase::paramValue(n);
	}
	std::string paramValue(unsigned n) const{ untested();
		return SubcktBase::paramValue(n);
	}
	virtual void setParameter(std::string const&, std::string const&){ unreachable(); }

private: // SchematicSymbol
//   bool portExists(unsigned) const override;
//   QString portName(unsigned) const override;

private:
  virtual Element* clone()const{
	  unreachable();
	  return nullptr; // new SchematicSymbol(*this);
  }

public: //legacy hack
	PaintingList& symbolPaintings();

private:
	PaintingList* _paint; // BUG
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
