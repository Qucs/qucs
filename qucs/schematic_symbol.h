// 2018 Felix
// part of QUCS, GPLv3+
//

#ifndef QUCS_SCHEMATIC_SYMBOL_H
#define QUCS_SCHEMATIC_SYMBOL_H

#include "symbol.h"

class PaintingList;

class SchematicSymbol : public Symbol{
public:
	explicit SchematicSymbol();
	~SchematicSymbol();
public:
	virtual SchematicModel const& schematicModel() const = 0;
	virtual std::string getParameter(std::string const&) const = 0;
private:
  virtual Element* clone()const{
	  unreachable();
	  return nullptr; // new SchematicSymbol(*this);
  }
private: // hide. don't mess with this.
	SchematicSymbol(SchematicSymbol const&){};
public: //legacy hack
	PaintingList& symbolPaintings();
private:
	PaintingList* _paint; // BUG
};

#endif
