#ifndef SCKT_PROTO_H
#define SCKT_PROTO_H

#include "schematic_symbol.h"
#include "schematic_model.h"

// a subcircuit declaration as in a spice "subckt" or verilog "module" or
// qucsator ".Def" 
class SubcktProto : public SchematicSymbol{
public:
	SubcktProto(Element const* p) : SchematicSymbol(), Proto(p), sm(nullptr){
	}

	virtual void build()=0;

public: // accessed from netlister..
	SchematicModel const& schematicModel() const{ return sm; }

protected:
	Element const* proto() const{ return Proto; }
	SchematicModel& schematicModel(){ return sm; }

private:
	Element const* Proto;
	SchematicModel sm;
};

#endif
