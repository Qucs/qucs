#ifndef SCKT_PROTO_H
#define SCKT_PROTO_H

#include "schematic_symbol.h"
#include "schematic_model.h"

class SubcktProto : public SchematicSymbol{
public:
	SubcktProto(Element const* p) : SchematicSymbol(), Proto(p), sm(nullptr){}
	virtual void build()=0;

protected:
	SchematicModel const& schematicModel() const{ return sm; }

protected:
	Element const* proto() const{ return Proto; }
	SchematicModel& schematicModel(){ return sm; }

private:
	Element const* Proto;
	SchematicModel sm;
};

#endif
