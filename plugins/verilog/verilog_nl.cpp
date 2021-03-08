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
#include <vector>
#include <map>
#include "docfmt.h"
#include "element.h"
#include "conductor.h"
#include "qucs_globals.h"
#include "element_list.h"
#include "schematic_lang.h"
#include "qio.h"
#include "net.h"
#include "exception.h"
#include "sckt_base.h"

namespace{

//using qucs::DocumentFormat;
using qucs::Language;
using qucs::Conductor;
using qucs::Symbol;

// generate a simulateable netlist in verilog
// contracts wires into nets, similar to qucsator
// TODO: generic netlister
class VerilogNetlister : public DocumentFormat{
public:
	explicit VerilogNetlister();
private: // legacy implementation
  void createNetlist(ostream_t& stream, ElementList const* m) const;
  void clear() const;
private: // Command
  void do_it(istream_t&, ElementList*) override;
private: // DocumentFormat
  void load(istream_t&, Object*) const override{ incomplete(); }
private:
  mutable std::vector<QString> netLabels;
  Language const* lang;
//  mutable ElementList const* modelhack;
}VNL;
static Dispatcher<Command>::INSTALL p1(&qucs::command_dispatcher, "verilog_nl", &VNL);
/*--------------------------------------------------------------------------*/
VerilogNetlister::VerilogNetlister() : DocumentFormat()
{
	lang = qucs::language_dispatcher["verilog"];
	// verilog = dynamic_cast<NetLang const*>(l);
	assert(lang);
}
/*--------------------------------------------------------------------------*/
void VerilogNetlister::do_it(istream_t& cs, ElementList* o)
{ untested();
	lang = qucs::language_dispatcher["verilog"];
	assert(lang);

	std::string fn;
	cs >> fn;

	trace1("VerilogNetlister::do_it", fn);
	ostream_t Stream(fn);

	assert(o);
	auto& m = *o;

	ElementList const* sch = &m;
	if (sch == NULL) {
		throw "incomplete_exception";
	}else{
	}

	createNetlist(Stream, sch);
}
/*--------------------------------------------------------------------------*/
void VerilogNetlister::createNetlist(ostream_t& stream,
		ElementList const* m) const
{
	assert(m);

	QString s, Time;
	for(auto pc : *m){
		auto sym = dynamic_cast<Symbol const*>(pc);

#if 0 // TODO
		lang->printItem(stream, pc);
#else
		if(pc->label()=="main"){
			lang->printItem(stream, pc);
		}else if(!sym){
			lang->printItem(stream, pc);
		}else if(dynamic_cast<Conductor const*>(sym)){
			// possibly a wire.
		}else if(sym->is_device()){ // hack, here?
			lang->printItem(stream, pc);
		}else{
			stream << "//else?\n";
			lang->printItem(stream, pc);
		}
#endif
	}
}
/*--------------------------------------------------------------------------*/
} // namespace
