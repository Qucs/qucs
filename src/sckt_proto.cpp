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

// subcircuit declaration as in a spice "subckt" or verilog "module" or
// qucsator ".Def".
/*--------------------------------------------------------------------------*/
#include "sckt_base.h"
#include "schematic_model.h"
#include "qucs_globals.h"

typedef unsigned index_t;
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class CommonSubckt : public CommonComponent{
public:
	explicit CommonSubckt(int x)
		: CommonComponent(x),
        _subckt() {}
	~CommonSubckt(){
		for(auto& p : _ports){
			delete p;
			p = nullptr;
		}
	}
public:
	SchematicModel* subckt(){ return &_subckt; }
	CommonComponent* clone()const override{
		assert(false); // not yet.
		incomplete();
		return new CommonSubckt(0);
	}
   bool operator==(const CommonComponent&)const override{
		return false;
	}
	pos_t const& portPosition(index_t i) const;
	Port& port(index_t i){
		trace2("cport", i, _ports.size());
		if(i<_ports.size()){
		}else{
			_ports.resize(i+1);
		}
		if(_ports[i]){
			trace2("oldport", i, _ports.size());
		}else{
			trace2("newport", i, _ports.size());
			_ports[i] = new Port();
		}
		return *_ports[i];
	}
	unsigned numPorts() const {
		return _ports.size();
	}
private:
	SchematicModel _subckt;
	std::vector<Port*> _ports;
};
CommonSubckt comms(CC_STATIC_);
/*--------------------------------------------------------------------------*/
pos_t const& CommonSubckt::portPosition(index_t i) const
{
	// TODO: find a port connected to the node at position i..
	//  (maybe use precalc?)
	auto p = _subckt.portValue(i); // BUG.
	if(p){ untested();
		incomplete();
		// return p->position();
	}else{ untested();
		throw qucs::ExceptionCantFind("position", "subckt", std::to_string(i));
	}
	// a port is a place. return its position?
	unreachable();
	return *new pos_t(0, 0);
}
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
	bool makes_own_scope()const override { return true;}
	SchematicModel* scope() override;

public: // repeat (why?!)
	SchematicModel const* scope() const {
		auto m = const_cast<SubcktProto*>(this);
		return m->scope();
	}
private:
	Port& port(unsigned) override;
//	SchematicModel const* scope() const override { return &sm; }
//
private: // bug, feature? is this a Symbol??
	rect_t bounding_rect() const{};
	void paint(ViewPainter*) const{};

private: // Symbol
	pos_t portPosition(unsigned) const;
	Element* clone()const override{
		auto a=new SubcktProto(*this);
		a->attach_common(new CommonSubckt(0));
		return a;
	}
	Element* clone_instance()const override{
		return new SubcktProto(*this);
	}
//   bool portExists(unsigned) const override;
//	bool portExists(unsigned i) const override;
	unsigned numPorts() const override;
	bool is_device() const override{return false;}

private: // internal
	SchematicModel* subckt() { untested();
		CommonComponent* cc = mutable_common();
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
// bool SubcktProto::portExists(unsigned i) const
// {
// 	assert(subckt());
// 	trace4("sckt_proto::portExists", i, subckt(), this, subckt()->numPorts());
// 	return i<subckt()->numPorts(); //?
// }
/*--------------------------------------------------------------------------*/
unsigned SubcktProto::numPorts() const
{
	// assert(subckt());
	// if(makes_own_scope()){
	// 	trace1("sckt_proto::numPorts", scope()->numPorts());
	// 	return scope()->numPorts();
	// }else{
	// 	trace1("sckt_proto::numPorts", subckt()->numPorts());
	// 	return subckt()->numPorts();
	// }
	CommonComponent const* cc = common();
	assert(cc);
	auto cs = prechecked_cast<CommonSubckt const*>(cc);
	assert(cs);
	return cs->numPorts();
}
/*--------------------------------------------------------------------------*/
Port& SubcktProto::port(index_t i)
{
	CommonComponent* cc = mutable_common();
	assert(cc);
	auto cs = prechecked_cast<CommonSubckt*>(cc);
	assert(cs);
	return cs->port(i);
}
/*--------------------------------------------------------------------------*/
pos_t SubcktProto::portPosition(unsigned i) const
{
	std::string n = portName(i);
	SchematicModel const* s = scope();

	if(s){
//		return subckt()->portValue(i)->position();
		auto i = s->find_(n);
		if(i!=s->end()){
			return (*i)->position();
		}else{
		}

	}else{ untested();
	}
	unreachable();
	return pos_t(0,0);
#if 0
	auto cs = prechecked_cast<CommonSubckt const*>(common());
	if(cs){ untested();
		return cs->portPosition(i);
	}else if(subckt()){
		incomplete();
		return pos_t(0, 0);
		//return subckt()->portPosition(i);
	}else{
		unreachable();
		return pos_t(0, 0);
	}
#endif
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
	CommonComponent* cc = mutable_common();
	assert(cc);
	auto cs = prechecked_cast<CommonSubckt*>(cc);
	assert(cs);

	assert(cs->subckt());
	return cs->subckt();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
} // namespace
