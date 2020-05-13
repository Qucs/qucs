// 2018 Felix
// QUCS Project
// GPLv3+
//

#ifndef QUCS_SCHEMATIC_LANG_H
#define QUCS_SCHEMATIC_LANG_H

#include "object.h"
#include "language.h"
#include "schematic_symbol.h"

class SchematicModel;

// could be redundant, once schematicModels are some kind of
// symbol/object (getting there)
#if 0 // remove
class ModelInserter{
protected:
	explicit ModelInserter();
public:
	virtual void pushBack(Element*) = 0;
	virtual SchematicModel* model() const{return nullptr;} // BUG
	virtual void setParameter(std::string const&, std::string) = 0;

	virtual PaintingList& symbolPaintings() = 0; // hmmm
};
#endif

class ModelAccess : public SchematicSymbol{
protected:
	explicit ModelAccess();
private:
//   SchematicModel const* subckt() const override{
// 	  return SchematicSymbol::subckt();
//   }
};

class SchematicLanguage : public DocumentLanguage{
protected:
	SchematicLanguage() : DocumentLanguage() {}
public:
	virtual ~SchematicLanguage() {}
	virtual void parse(DocumentStream& stream, SchematicSymbol&) const=0;
};

template<class container>
void schematicParse(DocumentStream& s, SchematicSymbol& c, SchematicLanguage const* L)
{
	assert(L);
	while(!s.atEnd()){ untested();
		L->parse(s, c);
		assert(s.atEnd()); // happens with legacy lang
	}
}


#endif
