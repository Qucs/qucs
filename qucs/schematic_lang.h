// 2018 Felix
// QUCS Project
// GPLv3+
//

#ifndef QUCS_SCHEMATIC_LANG_H
#define QUCS_SCHEMATIC_LANG_H

#include "object.h"
#include "language.h"

class SchematicModel;
class Schematic; // BUG

class ModelInserter{
protected:
	explicit ModelInserter();
public:
	virtual void pushBack(Element*) = 0;
	virtual SchematicModel* model() const{return nullptr;} // BUG
	virtual void setParameter(std::string const&, std::string const&) = 0;
};

class ModelAccess{
protected:
	explicit ModelAccess();
public:
	virtual SchematicModel const& schematicModel() const = 0;
	virtual std::string getParameter(std::string const&) const = 0;
};

class SchematicLanguage : public DocumentLanguage{
protected:
	SchematicLanguage() : DocumentLanguage() {}
public:
	virtual ~SchematicLanguage() {}
	virtual void parse(DocumentStream& stream, ModelInserter&) const=0;
};

template<class container>
void schematicParse(DocumentStream& s, ModelInserter& c, SchematicLanguage const* L)
{
	assert(L);
	while(!s.atEnd()){ untested();
		L->parse(s, c);
		assert(s.atEnd()); // happens with legacy lang
	}
}


#endif
