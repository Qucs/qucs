/***************************************************************************
    copyright            : (C) 2019, 2020, 2021 Felix Salfelder
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
#include <memory>
#include "common_sckt.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
namespace qucs {
/*--------------------------------------------------------------------------*/
	class Node;
/*--------------------------------------------------------------------------*/
CommonSubckt comms(CC_STATIC_);
/*--------------------------------------------------------------------------*/
std::string CommonSubckt::port_value(unsigned i) const
{
	trace2("port_value", short_label(), i);
	Port const& p = port(i);
	return p.nodeLabel();
}
/*--------------------------------------------------------------------------*/
pos_t CommonSubckt::portPosition(index_t i) const
{
	// TODO: find a port connected to the node at position i..
	//  (maybe use precalc?)
	std::string n = port_value(i);
	ElementList const* s = subckt();

	if(n==""){
		incomplete();
	}else if(s){
		trace2("CommonSubckt::portPosition", i, n);
//		return subckt()->portValue(i)->position();
		auto ii = s->find_(n);
		if(ii!=s->end()){
			trace1("CommonSub::portPosition hit", i);
			return (*ii)->position();
		}else if(subckt()){
			trace1("CommonSub::portPosition miss", i);
			auto p_ = subckt()->find_(":SymbolSection:");
			if(p_==subckt()->end()){

			}else if(auto p = dynamic_cast<SubcktBase const*>(*p_)){
				return p->portPosition(i);
			}else{
//				find_again?
			}
		}else{
//			find_again?
		}
	}else{ untested();
		throw qucs::ExceptionCantFind("position", "subckt", std::to_string(i));
	}
	// a port is a place. return its position?
	unreachable();
	return pos_t(0, 0);
}
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
using qucs::Port;
using qucs::Node;
using qucs::ViewPainter;
using qucs::CommonSubckt;
using qucs::CommonComponent;
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
	ElementList* scope() override;

public: // repeat (why?!)
	ElementList const* scope() const {
		auto m = const_cast<SubcktProto*>(this);
		return m->scope();
	}
private:
	Port& port(index_t) override;
//	ElementList const* scope() const override { return &sm; }
//
private: // bug, feature? is this a Symbol??
	rect_t bounding_rect() const{ return rect_t();};
	void paint(ViewPainter*) const{};

private: // Symbol
	pos_t portPosition(index_t) const;
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
	index_t numPorts() const override;
	bool is_device() const override{return false;}

private: // internal
	ElementList* subckt() { untested();
		CommonComponent* cc = mutable_common();
		assert(cc);
		auto cs = prechecked_cast<CommonSubckt*>(cc);
		assert(cs);
		return cs->subckt();
	}
	ElementList const* subckt() const{ untested();
		auto s=const_cast<SubcktProto*>(this);
		return s->scope();
	}

public:
   Node const* portValue(index_t) const override;
	std::string const portName(unsigned i) const override{
		auto cs = prechecked_cast<CommonSubckt const*>(common());
		assert(cs);
		return cs->port_value(i);
	}

private:
//	Element const* _instance; // why?
	//ElementList sm;
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
index_t SubcktProto::numPorts() const
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
pos_t SubcktProto::portPosition(index_t i) const
{
#if 1
	std::string n = portName(i);
	ElementList const* s = scope();

	if(s){
//		return subckt()->portValue(i)->position();
		auto ii = s->find_(n);
		if(ii!=s->end()){
			trace1("SubcktProto::portPosition hit", i);
			return (*ii)->position();
		}else{
			trace2("SubcktProto::portPosition miss", i, n);
		}

	}else{ untested();
	}
	unreachable();
	return pos_t(0,0);
#else
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
Node const* SubcktProto::portValue(index_t) const
{
	assert(false);
//	trace2("sckt_proto::portValue", subckt()->numPorts(), i);
//	// return node[i]->net()??
//	return subckt()->portValue(i);
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
ElementList* SubcktProto::scope()
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
