// 2018 Felix
// part of QUCS, GPLv3+
//

#ifndef QUCS_SCHEMATIC_SYMBOL_H
#define QUCS_SCHEMATIC_SYMBOL_H

#include "symbol.h"

class PaintingList;
class WireList;
class NodeMap;
class DiagramList;
class PaintingList;
class CmdEltList;
class ComponentList;

class SchematicSymbol : public Symbol{
protected:
	SchematicSymbol(SchematicSymbol const& p);
public:
	explicit SchematicSymbol();
	~SchematicSymbol();

private: // Symbol
  virtual void setPort(unsigned, Node*){ incomplete(); }

protected:
public: // subckt. could rename to "model", but is it really worth it?
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
	// CommandList const& commands() const;

public:
	std::string paramValue(std::string const&n) const{ untested();
		return Symbol::paramValue(n);
	}
	std::string paramValue(unsigned n) const{ untested();
		return Symbol::paramValue(n);
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

#endif
