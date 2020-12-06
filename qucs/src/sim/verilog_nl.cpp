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
//#include "components/component.h" // BUG
#include "globals.h"
#include "schematic_symbol.h"
#include "schematic_model.h"
#include "schematic_lang.h"
#include "io.h"
#include "net.h"
#include "sckt_proto.h"
#include "exception.h"

namespace{

// generate a simulateable netlist in verilog
// contracts wires into nets, similar to qucsator
// // TODO: one netlister for both languages.
class VerilogNetlister : public DocumentFormat{
public:
	explicit VerilogNetlister();
private: // legacy implementation
  void createNetlist(DocumentStream& stream, SchematicSymbol const& m) const;
  void prepareSave(DocumentStream& stream, SchematicSymbol const& m) const;
  void throughAllComps(DocumentStream& d, SchematicSymbol const& m) const;
  void clear() const;
  void createDeclarations(DocumentStream& d) const;
private: // Command
  void do_it(istream_t&, SchematicModel*) override{incomplete();}
private: // DocumentFormat
  void save(ostream_t& stream, Object const* m) const override;
  void load(istream_t&, Object*) const override{ incomplete(); }
private: // implementation
  void nodeMap(SchematicSymbol const& m) const;
private:
  mutable std::map<std::string, Element*> declarations;
  mutable std::vector<QString> netLabels;
  DocumentLanguage const* verilog;
  mutable SchematicModel const* modelhack;
}VNL;
static Dispatcher<Command>::INSTALL p1(&command_dispatcher, "verilog_nl", &VNL);

VerilogNetlister::VerilogNetlister() : DocumentFormat()
{
	verilog=doclang_dispatcher["verilog"];
	// verilog = dynamic_cast<NetLang const*>(l);
	assert(verilog);
}

void VerilogNetlister::clear() const
{
	declarations.clear();
}
// was main::doNetlist
void VerilogNetlister::save(DocumentStream& Stream, Object const* o) const
{
	auto m_ = dynamic_cast<SchematicSymbol const*>(o);
	if(!m_){
		incomplete();
		return;
	}else{
	}
	auto& m = *m_;

	modelhack = m.subckt();
	clear();

	qDebug() << "*** VerilogNetlister::save";

	SchematicModel const *sch = m.subckt();
	if (sch == NULL) {
		throw "incomplete_exception";
	}else{
	}

	QStringList Collect;

	Collect.clear();  // clear list for NodeSets, SPICE components etc.

	int SimPorts = 10;//??
	prepareSave(Stream, m);

	qDebug() << "done prep\n";

	if(SimPorts < -5) {
		throw "not_enough_ports_exception"; // ?!
	}

#if 0
	// output NodeSets, SPICE simulations etc.
	for(QStringList::Iterator it = Collect.begin();
			it != Collect.end(); ++it) {
		// don't put library includes into netlist...
		if ((*it).right(4) != ".lst" &&
				(*it).right(5) != ".vhdl" &&
				(*it).right(4) != ".vhd" &&
				(*it).right(2) != ".v") {
			Stream << *it << '\n';
		}
	}
#endif

	createDeclarations(Stream);

	Stream << '\n';

	nodeMap(m);

	createNetlist(Stream, m);
}

void VerilogNetlister::nodeMap(SchematicSymbol const& m) const
{
	return;
	assert(false);
	auto& sm = *m.subckt();
//	sm.throughAllNodes(count); // hack: number connected components.
	                           // should already be numbered...
//	sm.updateNetLabels(); // HACK: should already be named.

	unsigned nc=sm.numberOfNets();
	netLabels.resize(0);
	netLabels.resize(nc);
	qDebug() << "found" << nc << "nets";
	
#if 0
	for(auto w : sm.wires()){
		assert(w->portValue(0)->getNet()==w->portValue(1)->getNet());
		unsigned i=w->portValue(0)->netNumber();
		//qDebug() << "wire" << i << w->Label;
		if(!w->Label){
		}else if (netLabels[i].size()){
		}else{
			netLabels[i] = w->Label->name();
		}
	}
#endif

	// maybe just s/gnd/0/??
#if 0
	for(auto pc : sm.components()){
		if(pc->typeName() == "GND") { untested();
			assert(pc->Ports.first()->Connection);
			Net* n = pc->Ports.first()->Connection->getNet();
			assert(n);
			if (n->label().size()){ untested();
			}else{ untested();
				qDebug() << "GND: warning: overriding label" << n->label();
			}
			n->setLabel("0");
		}
	}
#endif

#if 0
	unsigned z=0;
	for(auto& i : netLabels){
		if(!i.size()){
			i = "_net" + QString::number(z++);
		}else{
		}
	}
#endif

} // nodeMap


void VerilogNetlister::createDeclarations(DocumentStream& stream) const
{
	for(auto si : declarations){
		//prepareSave(stream, m); // yikes
		if(SchematicSymbol* sym=dynamic_cast<SchematicSymbol*>(si.second)){
			nodeMap(*sym);
		}

		verilog->printItem(si.second, stream);
	}
}

// was Schematic::prepareNetlist
//  visit lot of components, strange callbacks...
void VerilogNetlister::prepareSave(DocumentStream& stream, SchematicSymbol const& m) const
{

	// if(showBias > 0) showBias = -1;  // do not show DC bias anymore

	bool isVerilog = false;
	bool isTruthTable = false;
	int allTypes = 0, NumPorts = 0;

	// BUG: Detect simulation domain (analog/digital) by looking at component types.
#if 0
	bool isAnalog = true;
	for(Component *pc = DocComps.first(); pc != 0; pc = DocComps.next()) {
		if(pc->isActive == COMP_IS_OPEN){
			// open circuit (or so)
			//    }else if(pc->obsolete_model_hack().at(0) == '.') {
	}else if(dynamic_cast<taskElement const*>(pc)) {

		qDebug() << pc->obsolete_model_hack();
		if(pc->obsolete_model_hack() == ".Digi") {
			if(allTypes & isDigitalComponent) {
				ErrText->appendPlainText(
						QObject::tr("ERROR: Only one digital simulation allowed."));
				return -10;
			}else if(pc->Props.getFirst()->Value != "TimeList"){
				isTruthTable = true;
			}
			if(pc->Props.getLast()->Value != "VHDL")
				isVerilog = true;
			allTypes |= isDigitalComponent;
			isAnalog = false;
		}else{
			allTypes |= isAnalogComponent;
		}
		if((allTypes & isComponent) == isComponent) {
			ErrText->appendPlainText(
					QObject::tr("ERROR: Analog and digital simulations cannot be mixed."));
			return -10;
		}
	}else if(pc->obsolete_model_hack() == "DigiSource"){
		NumPorts++;
	}else{
	}
	}
#endif

	if((allTypes & isAnalogComponent) == 0) {
		if(allTypes == 0) {
			// If no simulation exists, assume analog simulation. There may
			// be a simulation within a SPICE file. Otherwise Qucsator will
			// output an error.
			// isAnalog = true;
			allTypes |= isAnalogComponent;
			NumPorts = -1;
		}
		else {
			if(NumPorts < 1 && isTruthTable) {
				//  ErrText->appendPlainText(
				//     QObject::tr("ERROR: Digital simulation needs at least one digital source."));
				throw "weird digital problem";
			}
			if(!isTruthTable) NumPorts = 0;
		}
	}
	else {
		NumPorts = -1;
		// isAnalog = true;
	}

	std::string DocName;
	try{
		DocName=m.paramValue("DocName");
	}catch(ExceptionCantFind const&){
		DocName="unknown";
	}
	stream << "// Qucs " << PACKAGE_VERSION << "  "
	       << QString::fromStdString(DocName) << "\n";

	// set timescale property for verilog schematics
	if (isVerilog) {
		stream << "\n`timescale 1ps/100fs\n";
	}

	// BUG: giveNodeNames ejects subcircuit declarations (WTF?)
	// giveNodeNames(&stream, m);
	throughAllComps(stream, m);
	
}

// former Schematic::createNetlist
void VerilogNetlister::createNetlist(DocumentStream& stream,
		SchematicSymbol const& m) const
{

	/// include directives. qucsator does not seem to do that.
	// for(auto si : directives){
	// 	verilog->printItem(si, stream);
	// }
	//
	// legacy: qucsator expects all definitions at the top

	nodeMap(m);

	QString s, Time;
	for(auto pc : m.components()){
		assert(verilog);
		auto sym = dynamic_cast<Symbol const*>(pc);

		if(!sym){
			verilog->printItem(pc, stream);
		}else if(dynamic_cast<Conductor const*>(sym)){
			// possibly a wire.
		}else if(sym->is_device()){ // hack, here?
			verilog->printItem(pc, stream);
		}else{
		}
	}
}

/// hmm same as qucsator? cleanup and share.
void VerilogNetlister::throughAllComps(DocumentStream& stream, SchematicSymbol const& m) const
{ incomplete();
	QString s;

	auto const& sckt = *m.subckt();

	for(auto it_ : sckt.components()){

		auto it = dynamic_cast<Symbol const*>(it_);
		if(it){
		}else{
			incomplete();
			continue;
		}

		if(it->paramValue("$mfactor") == "0") {
			stream << "#ifdef QUCS_INACTIVE\n";
		}else{
		}

		QStringList Collect; // BUG
		//int countInit = 0;  // ??
		// int NumPorts=0; // ??!
		qDebug() << "call tAC" << QString::fromStdString(it->typeName());

		auto sym = it; // dynamic_cast<Symbol const*>(pc);
		if(sym && sym->subckt()){
			trace1("need expand?", sym->label());
			// if there is a sckt, make sure it is populated.
			sym->proto(&sckt); // just expand?
		}else if(it->typeName() == "GND") { // BUG, use a rail?

			// it->Ports.first()->Connection->setName("gnd");
		}

		if(it->paramValue("$mfactor") == "0") {
			stream << "#endif // QUCS_INACTIVE\n";
		}else{
		}
	}
}

} // namespace
