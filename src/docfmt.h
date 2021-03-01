// 2019 Felix Salfelder
// QUCS Project
// GPLv3+
//

#ifndef QUCS_DOCFMT_H
#define QUCS_DOCFMT_H

#include "object.h"
#include "command.h"

class SchematicSymbol;
class istream_t;

// various list commands?
class DocumentFormat : public Command{
protected:
	DocumentFormat() : Command() {}
public:
	virtual ~DocumentFormat() {}
	virtual void load(istream_t&, Object*) const=0;
	virtual void save(ostream_t&, Object const*) const{assert(false);}
};

#include <QTextStream>
// here?
// inline QTextStream& operator<< (QTextStream& o, std::string const& s)
// {
// 	return o << QString::fromStdString(s);
// }


#endif
