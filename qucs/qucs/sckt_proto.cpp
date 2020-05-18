
#include "sckt_proto.h"
bool SubcktProto::portExists(unsigned i) const
{
	trace3("sckt_proto::portExists", i, subckt(), this);
	assert(subckt());
	return i<subckt()->numPorts(); //?
}

unsigned SubcktProto::numPorts() const
{ untested();
	trace1("sckt_proto::numPorts", subckt()->numPorts());
	return subckt()->numPorts();
}

QString SubcktProto::portValue(unsigned i) const
{
	trace1("sckt_proto::portValue", subckt()->numPorts());
	return subckt()->portValue(i);
}

SubcktProto::SubcktProto(Element const* p)
	: SchematicSymbol(), _instance(p),
	sm(nullptr) // obsolete
{
	auto sym = dynamic_cast<Symbol const*>(p);
	assert(sym);
//	assert(sym->owner());
//	assert(dynamic_cast<Symbol const*>(sym->owner())); ///!!!
	untested();
}
