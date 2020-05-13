#ifndef SCKT_PROTO_H
#define SCKT_PROTO_H

#include "schematic_symbol.h"
#include "schematic_model.h"

// a subcircuit declaration as in a spice "subckt" or verilog "module" or
// qucsator ".Def"
//
// FIXME class SchematicSymbol : public SubcktProto
//
class SubcktProto : public SchematicSymbol{
	SubcktProto(SubcktProto const&) = delete;
public:
	explicit SubcktProto(Element const* p);


	virtual void build()=0;

public: // accessed from netlister..
	SchematicModel const& schematicModel() const{ return sm; }

protected:
	Element const* instance() const{ untested(); incomplete(); return _instance; }
	SchematicModel* schematicModel(){ return &sm; }

//	SchematicModel* scope() override { return &sm; }
//	SchematicModel const* scope() const override { return &sm; }

private: // SchematicSymbol
//   bool portExists(unsigned) const override;
	bool portExists(unsigned i) const override;
	unsigned numPorts() const override;
public:
	QString portValue(unsigned i) const;
//   QString portName(unsigned) const override;

private:
	Element const* _instance; // why?
	SchematicModel sm;
};

#endif
