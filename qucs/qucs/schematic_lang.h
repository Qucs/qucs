// 2018 Felix
// QUCS Project
// GPLv3+
//

#ifndef QUCS_SCHEMATIC_LANG_H
#define QUCS_SCHEMATIC_LANG_H

#include "object.h"
#include "language.h"

#define SchematicModel Schematic // fix later.
class SchematicModel;

class SchematicLanguage : public DocumentLanguage{
protected:
	SchematicLanguage() : DocumentLanguage() {}
public:
	virtual ~SchematicLanguage() {}
	virtual void parse(DocumentStream& stream, SchematicModel*) const=0;
};

#endif
