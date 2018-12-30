// 2018 Felix
// part of QUCS, GPLv3+
//

#ifndef QUCS_SCHEMATIC_SYMBOL_H
#define QUCS_SCHEMATIC_SYMBOL_H

#include "element.h"

class PaintingList;

class SchematicSymbol : public Element{
public:
	explicit SchematicSymbol();
	~SchematicSymbol();
private:
  virtual Element* clone()const{
	  return new SchematicSymbol(*this);
  }
private: // hide. don't mess with this.
	SchematicSymbol(SchematicSymbol const&){};
public: //legacy hack
	PaintingList& symbolPaintings();
private:
	PaintingList* _paint; // BUG
};

#endif
