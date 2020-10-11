/***************************************************************************
    copyright            : (C) 2019, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
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
	Element const* instance() const{ incomplete(); return _instance; }
	SchematicModel* schematicModel(){ return &sm; }

//	SchematicModel* scope() override { return &sm; }
//	SchematicModel const* scope() const override { return &sm; }

private: // SchematicSymbol
//   bool portExists(unsigned) const override;
	bool portExists(unsigned i) const override;
	unsigned numPorts() const override;
public:
   Net const* portValue(unsigned) const override;
//	QString portName(unsigned i) const;

private:
	Element const* _instance; // why?
	SchematicModel sm;
};

#endif
