// 2018 Felix
// part of QUCS, GPLv3+
//

#ifndef QUCS_SCHEMATIC_SYMBOL_H
#define QUCS_SCHEMATIC_SYMBOL_H

#include "symbol.h"

class PaintingList;

class SchematicSymbol : public Symbol{
private: // hide. don't mess with this.
	SchematicSymbol(SchematicSymbol const&){};
public:
	explicit SchematicSymbol();
	~SchematicSymbol();

private: // Symbol
  virtual void setPort(unsigned, Node*){ incomplete(); }

public:
	virtual SchematicModel const& schematicModel() const = 0;
	virtual SchematicModel* schematicModel() = 0;
	virtual std::string getParameter(std::string const&) const = 0;
	virtual void setParameter(std::string const&, std::string const&){ unreachable(); }

private:
  virtual Element* clone()const{
	  unreachable();
	  return nullptr; // new SchematicSymbol(*this);
  }

public: //legacy hack
	PaintingList& symbolPaintings();
	PaintingList const& symbolPaintings() const;
private:
	PaintingList* _paint; // BUG
};

#endif
