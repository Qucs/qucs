// 2019 Felix Salfelder
// QUCS Project
// GPLv3+
//

#ifndef QUCS_DOCFMT_H
#define QUCS_DOCFMT_H

#include "object.h"

class SchematicSymbol;
class DocumentStream;
class istream_t;

class DocumentFormat : public Object{
protected:
	DocumentFormat() : Object() {}
public:
	virtual ~DocumentFormat() {}
	virtual void load(istream_t&, SchematicSymbol&) const=0;
	virtual void save(DocumentStream&, SchematicSymbol const&) const{incomplete();}
};

#include <QTextStream>
// here?
inline QTextStream& operator<< (QTextStream& o, std::string const& s)
{
	return o << QString::fromStdString(s);
}


#endif
