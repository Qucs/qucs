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

#include "schematic_symbol.h"
#include "schematic_model.h"
#include "globals.h"
namespace {

// a subcircuit declaration as in a spice "subckt" or verilog "module" or
// qucsator ".Def"
//
class CommonSubckt : public COMMON_COMPONENT{
public:
	explicit CommonSubckt(int x)
		: COMMON_COMPONENT(x), _subckt() {}
public:
	SchematicModel* subckt(){ return &_subckt; }
	COMMON_COMPONENT* clone()const override{
		return new CommonSubckt(0);
	}
   bool  operator==(const COMMON_COMPONENT&)const override{
		return false;
	}
private:
	SchematicModel _subckt;
};
CommonSubckt comms(CC_STATIC);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class SubcktProto : public SubcktBase {
private:
	SubcktProto(SubcktProto const& p) : SubcktBase(p){
//		attach_common(comms.clone()); // really?
//		attach_common(new CommonSubckt(0)); // really?
	}
public:
	explicit SubcktProto(Element const* p=nullptr); // ?

	virtual void build() { incomplete(); } // needed??
	SchematicModel* scope() override;
	SchematicModel const* scope() const {
		auto m = const_cast<SubcktProto*>(this);
		return m->scope();
	}
private:
	Port& port(unsigned) override{ incomplete(); throw "a";}
//	SchematicModel const* scope() const override { return &sm; }

private: // Symbol
	pos_t portPosition(unsigned) const{ assert(false); return pos_t(0,0);}
	Element* clone()const override{
		auto a=new SubcktProto(*this);
		a->attach_common(new CommonSubckt(0));
		return a;
	}
	Element* clone_instance()const override{
		return new SubcktProto(*this);
	}
	bool makes_own_scope()const override { return true;}
//   bool portExists(unsigned) const override;
	bool portExists(unsigned i) const override;
	unsigned numPorts() const override;
	bool is_device() const override{return false;}

private: // internal
	SchematicModel* subckt() { untested();
		COMMON_COMPONENT* cc = mutable_common();
		assert(cc);
		auto cs = prechecked_cast<CommonSubckt*>(cc);
		assert(cs);
		return cs->subckt();
	}
	SchematicModel const* subckt() const{
		auto s=const_cast<SubcktProto*>(this);
		return s->scope();
	}

public:
   Node const* portValue(unsigned) const override;
//	QString portName(unsigned i) const;

private:
	Element const* _instance; // why?
	//SchematicModel sm;
}d0;
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "subckt_proto", &d0);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
bool SubcktProto::portExists(unsigned i) const
{
	assert(subckt());
	trace4("sckt_proto::portExists", i, subckt(), this, subckt()->numPorts());
	return i<subckt()->numPorts(); //?
}
/*--------------------------------------------------------------------------*/
unsigned SubcktProto::numPorts() const
{
	assert(subckt());
	if(makes_own_scope()){
		trace1("sckt_proto::numPorts", scope()->numPorts());
		return scope()->numPorts();
	}else{
		trace1("sckt_proto::numPorts", subckt()->numPorts());
		return subckt()->numPorts();
	}
}
/*--------------------------------------------------------------------------*/
Node const* SubcktProto::portValue(unsigned i) const
{
	trace2("sckt_proto::portValue", subckt()->numPorts(), i);
	// return node[i]->net()??
	return subckt()->portValue(i);
}
/*--------------------------------------------------------------------------*/
SubcktProto::SubcktProto(Element const*)
	: SubcktBase()
{
//	auto sym = dynamic_cast<Symbol const*>(p);
//	assert(sym);
	attach_common(new CommonSubckt(0)); // really?
//	assert(sym->owner());
//	assert(dynamic_cast<Symbol const*>(sym->owner())); ///!!!
}
/*--------------------------------------------------------------------------*/
SchematicModel* SubcktProto::scope()
{
	COMMON_COMPONENT* cc = mutable_common();
	assert(cc);
	auto cs = prechecked_cast<CommonSubckt*>(cc);
	assert(cs);

	assert(cs->subckt());
	return cs->subckt();
}

} // namespace
