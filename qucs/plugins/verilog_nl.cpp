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
#include "exception.h"

namespace{

// generate a simulateable netlist in verilog
// contracts wires into nets, similar to qucsator
// TODO: generic netlister
class VerilogNetlister : public DocumentFormat{
public:
	explicit VerilogNetlister();
private: // legacy implementation
  void createNetlist(DocumentStream& stream, SchematicModel const* m) const;
  void throughAllComps(DocumentStream& d, SchematicSymbol const& m) const;
  void clear() const;
private: // Command
  void do_it(istream_t&, SchematicModel*) override;
private: // internal
  void printDeclarations(DocumentStream& d) const;
private: // DocumentFormat
  void load(istream_t&, Object*) const override{ incomplete(); }
private:
  mutable std::map<std::string, Element*> declarations;
  mutable std::vector<QString> netLabels;
  DocumentLanguage const* lang;
//  mutable SchematicModel const* modelhack;
}VNL;
static Dispatcher<Command>::INSTALL p1(&command_dispatcher, "verilog_nl", &VNL);
/*--------------------------------------------------------------------------*/
VerilogNetlister::VerilogNetlister() : DocumentFormat()
{
	lang = doclang_dispatcher["verilog"];
	// verilog = dynamic_cast<NetLang const*>(l);
	assert(lang);
}
/*--------------------------------------------------------------------------*/
void VerilogNetlister::clear() const
{
	declarations.clear();
}
/*--------------------------------------------------------------------------*/
void VerilogNetlister::do_it(istream_t& cs, SchematicModel* o)
{
	lang = doclang_dispatcher["verilog"];
	assert(lang);

	std::map<std::string, Element const*> declarations;
	std::string fn;
	cs >> fn;

	QFile NetlistFile(QString::fromStdString(fn));
	if(!NetlistFile.open(QIODevice::WriteOnly | QFile::Truncate)) { untested();
		fprintf(stderr, "Error: Could write to %s\n", fn.c_str());
		exit(1); // BUG
	}else{
	}

	ostream_t Stream(&NetlistFile);

	assert(o);
	auto& m = *o;

	SchematicModel const* sch = &m;
	if (sch == NULL) {
		throw "incomplete_exception";
	}else{
	}

	//printDeclarations(Stream);
	createNetlist(Stream, sch);
}
/*--------------------------------------------------------------------------*/
void VerilogNetlister::printDeclarations(DocumentStream& stream) const
{
	for(auto si : declarations){
		if(SchematicSymbol* sym=dynamic_cast<SchematicSymbol*>(si.second)){
		}else{
		}

		lang->printItem(si.second, stream);
	}
}
/*--------------------------------------------------------------------------*/
void VerilogNetlister::createNetlist(DocumentStream& stream,
		SchematicModel const* m) const
{
	assert(m);

	/// include directives. qucsator does not seem to do that.
	// for(auto si : directives){
	// 	lang->printItem(si, stream);
	// }
	//
	// legacy: qucsator expects all definitions at the top

	QString s, Time;
	for(auto pc : *m){
		auto sym = dynamic_cast<Symbol const*>(pc);

		if(pc->label()=="main"){
			lang->printItem(pc, stream);
		}else if(!sym){
			lang->printItem(pc, stream);
		}else if(dynamic_cast<Conductor const*>(sym)){
			// possibly a wire.
		}else if(sym->is_device()){ // hack, here?
			lang->printItem(pc, stream);
		}else{
		}
	}
}
/*--------------------------------------------------------------------------*/
void VerilogNetlister::throughAllComps(DocumentStream& stream, SchematicSymbol const& m) const
{ incomplete();
	auto const& sckt = *m.subckt();

	for(auto it_ : sckt){
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
