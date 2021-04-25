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

// schematic declaration as in a qucs schematic
/*--------------------------------------------------------------------------*/
#include "sckt_base.h"
#include "element_list.h"
#include "qucs_globals.h"
#include <memory>
#include "common_sckt.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
using qucs::CommonComponent;
using qucs::Element;
using qucs::ElementList;
using qucs::Node;
using qucs::Port;
using qucs::ViewPainter;
using qucs::SubcktBase;
/*--------------------------------------------------------------------------*/
class SubcktProto : public SubcktBase {
private:
	SubcktProto(SubcktProto const& p) : SubcktBase(p){
      assert(common() == p.common());

		new_subckt(); // ??
	}
public:
	explicit SubcktProto(Element const* p=nullptr); // ?

	virtual void build() { incomplete(); } // needed??
	bool makes_own_scope()const override { return true;}
	ElementList*	   scope()		{untested();return subckt();}
	const ElementList* scope()const		{return subckt();}

private:
//	Port& port(index_t) override;
//	ElementList const* scope() const override { return &sm; }
//
private: // bug, feature? is this a Symbol??
	rect_t bounding_rect() const{ return rect_t();};
	void paint(ViewPainter*) const{};

private: // Symbol
	// pos_t portPosition(index_t) const override;
	Element* clone()const override{
		return new SubcktProto(*this);
	}
	bool is_device() const override{return false;}

	void prepare() override{
		assert(scope());
		scope()->prepare();
	}
}d0;
static Dispatcher<qucs::Component>::INSTALL p(&qucs::device_dispatcher, "schematic_proto", &d0);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// bool SubcktProto::portExists(unsigned i) const
// {
// 	assert(subckt());
// 	trace4("sckt_proto::portExists", i, subckt(), this, subckt()->numPorts());
// 	return i<subckt()->numPorts(); //?
// }
/*--------------------------------------------------------------------------*/
SubcktProto::SubcktProto(Element const*)
	: SubcktBase()
{
	new_subckt();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
} // namespace
