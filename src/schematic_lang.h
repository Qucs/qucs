// 2018 Felix
// QUCS Project
// GPLv3+
//

#ifndef QUCS_SCHEMATIC_LANG_H
#define QUCS_SCHEMATIC_LANG_H

#include "object.h"
#include "language.h"

class SchematicModel;

// obsolete? DocumentLanguage::parseItem
class SchematicLanguage : public DocumentLanguage{
protected:
	SchematicLanguage() : DocumentLanguage() {}
public:
	virtual ~SchematicLanguage() {}
	virtual void parse(istream_t& stream, SubcktBase*) const=0;
};

#if 0
// BUG? obsolete?
template<class container>
void schematicParse(istream_t& s, SchematicSymbol& c, SchematicLanguage const* L)
{
	assert(L);
	while(!s.atEnd()){ untested();
		L->parse(s, &c);
		assert(s.atEnd()); // happens with legacy lang
	}
}
#endif


#endif
