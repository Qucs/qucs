/***************************************************************************
                             qucsator.cc
                              ----------
    copyright            : (C) 2015, 2019 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// stuff collected from schematic_file.cc and others
//
// WARNING: function names are unchanged, and still counterintuitive.

#include "../sim/sim.h"
#include "node.h"
#include <QString>
#include "globals.h"
#include "command.h"
#include "docfmt.h"
#include "schematic_model.h"
#include "schematic_doc.h" // BUG
#include "schematic_lang.h"
#include "exception.h"
#include "sckt_proto.h"
#include "subcircuit.h"
#include "net.h"
#include "misc.h"

#include "paintings/paintings.h"

namespace {
// qucslang language implementation
class QucsLang : public NetLang {
private: // NetLang
  // inline void printItem(Element const* c, stream_t& s) const;

private: // local
  void printCommand(Command const*, stream_t&) const;
  void printSymbol(Symbol const*, stream_t&) const;
  void printSubckt(SubcktProto const*, stream_t&) const;
  void printComponent(Component const*, stream_t&) const;
}qucslang;
static Dispatcher<DocumentLanguage>::INSTALL pl(&doclang_dispatcher, "qucsator", &qucslang);

void QucsLang::printSymbol(Symbol const* d, stream_t& s) const
{
	if(auto c=dynamic_cast<SubcktProto const*>(d)){
		printSubckt(c, s);
	}else if(auto c=dynamic_cast<Command const*>(d)){ untested();
		printCommand(c, s);
	}else if(auto c=dynamic_cast<Component const*>(d)){
		printComponent(c, s);
	}else{ untested();
		s<<"incomplete\n";
		incomplete();
	}
}

// BUG
std::vector<QString> netLabels;

// partly from Schematic::createSubnetlistplain
void QucsLang::printSubckt(SubcktProto const* p, stream_t& s) const
{
	trace2("prinSckt", p, p->subckt());
	Symbol const* sym = p;

	s << "\n";
	s << ".Def:" << p->label();

	// print_ports();
	//
	for(unsigned i=0; sym->portExists(i); ++i){
		auto N=p->portValue(i);
//		if(N=="0"){
//			N = "gnd";
//		}else{
//		}
		s << " " << N;
	}
	s << "\n";

	for(auto pi : p->symbolPaintings()){
		incomplete();
		if(pi->name() == ".ID ") { untested();
			incomplete();
			s<<"TODO " << pi->label() << pi->name() << "\n";
	//		ID_Text *pid = (ID_Text*)pi;
	//		QList<SubParameter *>::const_iterator it;
	//		for(it = pid->Parameter.constBegin(); it != pid->Parameter.constEnd(); it++) { untested();
	//			s = (*it)->Name; // keep 'Name' unchanged
	//			(*tstream) << " " << s.replace("=", "=\"") << '"';
		}else{
		}
	//		break;
	}
	//(*tstream) << '\n';

	// TODO: deduplicate.
	trace1("sckt components", &p->schematicModel());
	trace1("sckt components", sym->scope());
	assert(sym->scope());
	for(auto i : p->schematicModel().components()){
      if(i->type() == "Port"){
		}else if(i->type() == "GND"){
		}else{
			printSymbol(i, s);
		}
	}

	s << ".Def:End\n";
}

void QucsLang::printCommand(Command const* c, stream_t& s) const
{
	{
		{ // todo: introduce proper exceptions
			// normal netlisting

			s << "." << c->name() << ":" << c->label();

			//for(auto p2 : c->params())
			for(auto p2 : c->Props){ // BUG
				if(p2->name() == "Symbol") { // hack??
				}else if(p2->name()=="p" && p2->value()==""){ untested();
					// unreachable
				}else{
					s << " " << p2->name() << "=\"" << p2->value() << "\"";
				}
			}
			s << '\n';
		}
	}
}

/*!
 * print Components in qucs language
 */
void QucsLang::printComponent(Component const* c, stream_t& s) const
{
	if(c->isActive != COMP_IS_ACTIVE){
		// comment out?
		incomplete();
	}else{
	}

	if(c->isOpen()) { untested();
		// nothing.
	}else if(c->isShort()){ untested();
		// replace by some resistors (hack?)
		incomplete();
		int z=0;
		QListIterator<Port *> iport(c->ports());
		Port *pp = iport.next();
		QString Node1 = pp->value()->label();
		while (iport.hasNext()){ untested();
			s << "R:" << c->label() << "." << QString::number(z++) << " "
				<< Node1 << " " << iport.next()->value()->label() << " R=\"0\"\n";
		}
	}else{
		if(dynamic_cast<Subcircuit const*>(c)) {
			s << "Sub:" << c->label();
		}else{
			s << QString::fromStdString(c->type()) << ":" << c->label();
		}

		Symbol const* sym=c;
		trace2("print", sym->numPorts(), sym->label());
		for(unsigned i=0; i<sym->numPorts(); ++i){
			QString N = sym->portValue(i);

//			if(N=="0"){
//				N = "gnd";
//			}else{
//			}
			s << " " << N;
		}

		for(auto p2 : c->params()) {
			if(p2->name() == "Symbol") { // hack??
			}else{
				s << " " << p2->name() << "=\"" << p2->value() << "\"";
			}
		}
		if(dynamic_cast<Subcircuit const*>(c)) {
			s << " Type=\"" << QString::fromStdString(c->type()) << "\"";
		}else{
		}
		s << '\n';
	}
}


// qucsator simulator backend
class Qucsator : public Simulator{
  NetLang const* netLang() const{return &qucslang;}
}QS;
static Dispatcher<Simulator>::INSTALL p(&simulator_dispatcher, "qucsator", &QS);

// -------------------------------------------------------------------

class LegacyNetlister : public DocumentFormat{
private: // legacy implementation
  void createNetlist(DocumentStream& stream, SchematicSymbol const& m) const;
  void prepareSave(DocumentStream& stream, SchematicSymbol const& m) const;
  void throughAllComps(DocumentStream& d, SchematicSymbol const& m) const;
  void clear() const;
  void printDeclarations(DocumentStream& d, SchematicSymbol const&m) const;
private: // overrides
  void save(DocumentStream& stream, SchematicSymbol const& m) const;
  void load(DocumentStream&, SchematicSymbol&) const{ incomplete(); }
private:
  mutable SubMap FileList; // BUG
}LNL;
static Dispatcher<DocumentFormat>::INSTALL p1(&docfmt_dispatcher, "qucsator|legacy_nl", &LNL);

void LegacyNetlister::clear() const
{
	// declarations.clear();
}

// was main::doNetlist
void LegacyNetlister::save(DocumentStream& Stream, SchematicSymbol const& m) const
{
	clear();

	qDebug() << "*** LegacyNetlister::save";

	SchematicModel const *sch = m.subckt();
	QStringList Collect;
	Collect.clear();  // clear list for NodeSets, SPICE components etc.

	int SimPorts = 10;//??
	incomplete(); // HERE

	// assert(m.owner()); // root symbol does not have owner...
	prepareSave(Stream, m);

	qDebug() << "done prep\n";

	if(SimPorts < -5) { untested();
		throw "not_enough_ports_exception"; // ?!
	}

#if 0
	// output NodeSets, SPICE simulations etc.
	for(QStringList::Iterator it = Collect.begin();
			it != Collect.end(); ++it) { untested();
		// don't put library includes into netlist...
		if ((*it).right(4) != ".lst" &&
				(*it).right(5) != ".vhdl" &&
				(*it).right(4) != ".vhd" &&
				(*it).right(2) != ".v") { untested();
			Stream << *it << '\n';
		}
	}
#endif

	printDeclarations(Stream, m);

	Stream << '\n';

	createNetlist(Stream, m);
}

// TODO: sort out
static void nodeMap(SchematicSymbol const& m)
{ untested();
	unsigned count;
	auto& sm = *m.subckt();
//	sm.throughAllNodes(count); // hack: number connected components.
//	                           // should already be numbered...

	unsigned nc=sm.numberOfNets();
	// netLabels.resize(0);
	// netLabels.resize(nc);
	qDebug() << "found" << nc << "nets";
	
	for(auto w : sm.wires()){ untested();
#if 0
		obsolete
		unsigned i=w->portValue(0)->netNumber();
		// qDebug() << "wire" << i << w->Label;
		if(!w->Label){ untested();
		}else if (netLabels[i].size()){ untested();
		}else{ untested();
			netLabels[i] = w->Label->name();
		}
#endif
	}

	for(auto pc : sm.components()){ untested();
		if(pc->type() == "GND") {
#if 0
			assert(pc->Ports.first()->value());
			Net* n = pc->Ports.first()->value()->getNet();
			assert(n);
			if (n->label().size()){ untested();
			}else{ untested();
				qDebug() << "GND: warning: overriding label" << n->label();
			}
			n->setLabel("gnd");
#endif
		}
	}

#if 0
	unsigned z=0;
	for(auto& i : netLabels){ untested();
		if(!i.size()){ untested();
			i = "_net" + QString::number(z++);
		}else{ untested();
		}
	}
#endif


	// if(allTypes & isAnalogComponent){ untested();
	// }else if (!isVerilog) { untested();
	// 	incomplete();
	// //	stream << QString::fromStdString(VHDL_LIBRARIES);
	// //	stream << "entity TestBench is\n"
	// //		<< "end entity;\n"
	// //		<< "use work.all;\n";
	// }

//	return NumPorts;
} // nodeMap


void LegacyNetlister::printDeclarations(DocumentStream& stream, SchematicSymbol const& m) const
{

	assert(m.subckt());
	for(auto si : m.subckt()->declarations()){
		//prepareSave(stream, m); // yikes
// 		if(SchematicSymbol const* sym=dynamic_cast<SchematicSymbol const*>(si)){ untested();
// 			nodeMap(*sym);
// 		}else{ untested();
// 		}

		trace2("printing declaration", si, si->label());
		qucslang.printItem(si, stream);
	}
	stream << "## declarations end\n";
}

// was Schematic::prepareNetlist
//  visit lot of components, strange callbacks...
void LegacyNetlister::prepareSave(DocumentStream& stream, SchematicSymbol const& m) const
{

	// if(showBias > 0) showBias = -1;  // do not show DC bias anymore

	bool isVerilog = false;
	bool isTruthTable = false;
	int allTypes = 0, NumPorts = 0;

	// BUG: Detect simulation domain (analog/digital) by looking at component types.
#if 0
	bool isAnalog = true;
	for(Component *pc = DocComps.first(); pc != 0; pc = DocComps.next()) { untested();
		if(pc->isActive == COMP_IS_OPEN){ untested();
			// open circuit (or so)
			//    }else if(pc->obsolete_model_hack().at(0) == '.') { untested();
	}else if(dynamic_cast<Command const*>(pc)) { untested();

		qDebug() << pc->obsolete_model_hack();
		if(pc->obsolete_model_hack() == ".Digi") { untested();
			if(allTypes & isDigitalComponent) { untested();
				ErrText->appendPlainText(
						QObject::tr("ERROR: Only one digital simulation allowed."));
				return -10;
			}else if(pc->Props.getFirst()->Value != "TimeList"){ untested();
				isTruthTable = true;
			}
			if(pc->Props.getLast()->Value != "VHDL")
				isVerilog = true;
			allTypes |= isDigitalComponent;
			isAnalog = false;
		}else{ untested();
			allTypes |= isAnalogComponent;
		}
		if((allTypes & isComponent) == isComponent) { untested();
			ErrText->appendPlainText(
					QObject::tr("ERROR: Analog and digital simulations cannot be mixed."));
			return -10;
		}
	}else if(pc->obsolete_model_hack() == "DigiSource"){ untested();
		NumPorts++;
	}else{ untested();
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
		else { untested();
			if(NumPorts < 1 && isTruthTable) { untested();
				//  ErrText->appendPlainText(
				//     QObject::tr("ERROR: Digital simulation needs at least one digital source."));
				throw "weird digital problem";
			}
			if(!isTruthTable) NumPorts = 0;
		}
	}
	else { untested();
		NumPorts = -1;
		// isAnalog = true;
	}

	// first line is documentation
	if(allTypes & isAnalogComponent)
		stream << "#";
	else if (isVerilog)
		stream << "//";
	else{ untested();
		stream << "--";
	}

	std::string DocName;
	try{
		DocName=m.getParameter("DocName");
	}catch(ExceptionCantFind const&){ untested();
		DocName="unknown";
	}
	stream << " Qucs " << PACKAGE_VERSION << "  "
	       << QString::fromStdString(DocName) << "\n";

	// set timescale property for verilog schematics
	if (isVerilog) { untested();
		stream << "\n`timescale 1ps/100fs\n";
	}

	// assert(m.owner()); //root does not have owner...
	throughAllComps(stream, m);
}

// former Schematic::createNetlist
void LegacyNetlister::createNetlist(DocumentStream& stream,
		SchematicSymbol const& m) const
{
	bool isAnalog=true;
	bool isVerilog=false;
	//  if(!isAnalog) { untested();
	//    beginNetlistDigital(stream, nl);
	//  }

	//  Signals.clear();  // was filled in "giveNodeNames()"
	FileList.clear();

	int NumPorts=0; // huh??

	/// include directives. qucsator does not seem to do that.
	// for(auto si : directives){ untested();
	// 	qucslang.printItem(si, stream);
	// }
	//
	// legacy: qucsator expects all definitions at the top

	// nodeMap(m);

	QString s, Time;
	for(auto pc : m.components()){
		// if dynamic_cast<Label*>{
		//   ignore
		if(pc->type()=="GND"){
			// qucsator hack, just ignore.
		}else if(pc->type()=="NodeLabel"){ untested();
			// qucsator hack, just ignore.
		}else{
			qucslang.printItem(pc, stream);
		}

		if(isAnalog) {
		} else { // FIXME: use different lang to print things differently
			if(pc->obsolete_model_hack() == ".Digi" && pc->isActive) {  // simulation component ?
				if(NumPorts > 0) { // truth table simulation ?
					if (isVerilog){ untested();
						Time = QString::number((1 << NumPorts));
					} else { untested();
						Time = QString::number((1 << NumPorts) - 1) + " ns";
					}
				} else { untested();
					Time = pc->Props.at(1)->Value;
					if (isVerilog) { untested();
						if(!misc::Verilog_Time(Time, pc->name())) return;
					} else { untested();
						if(!misc::VHDL_Time(Time, pc->name())) return;
					}
				}
			}
			if (isVerilog) { untested();
				s = pc->get_Verilog_Code(NumPorts);
			} else { untested();
				s = pc->get_VHDL_Code(NumPorts);
			}
			if (s.length()>0 && s.at(0) == '\xA7'){ untested();
				throw "strange_character";
			}
			stream << s;
		}
	}

	if(!isAnalog) { untested();
		// endNetlistDigital(stream, qucslang);
	}
// return Time;
}

// walk through netlist and eject prototypes?
// seems convoluted.
//
// could eject "include" statements instead, but need to convert sub schematics
// to target language somewhere else.

// some kind of expand
void LegacyNetlister::throughAllComps(DocumentStream& stream, SchematicSymbol const& m) const
{ incomplete();
	trace3("tac", m.label(), &m, m.owner());
	if(m.owner()){ untested();
	}else{
	}
	bool r;
	QString s;
	bool isAnalog=true;

	assert(m.subckt());
	auto const& sckt = *m.subckt();

	for(auto pc : sckt.components()){
		Symbol const* sym = pc;
		assert(pc);
		trace3("tac", pc->label(), pc, sym->owner());
		trace1("tac", sym->owner()->label());


		// because they are components
		assert(sym->owner()==&m);
		assert(sym->scope()==&sckt);

		if(pc->isActive != COMP_IS_ACTIVE){ untested();
			incomplete();
			continue;
		}else if(isAnalog) {
			// check analog/digital typed components
			if((pc->Type & isAnalogComponent) == 0) { untested();
				incomplete();
				return;
			}
		} else if((pc->Type & isDigitalComponent) == 0) { untested();
			return;
		}

//		QStringList Collect; // BUG
		qDebug() << "call tAC" << QString::fromStdString(pc->type());
		
		if(sym && sym->subckt()){
			trace1("need expand?", sym->label());
			// if there is a sckt, make sure it is populated.
			Symbol const* p = pc->proto(&sckt); // just expand?
			assert(p->subckt());
			//	}
		}else if(pc->obsolete_model_hack() == "GND") { // BUG.
#if 0
			qDebug() << "GND hack" << pc->portValue(0);
			assert(pc->Ports.first()->value());
			Net* n = pc->Ports.first()->value()->getNet();
			n->setLabel("gnd"); // should already be "gnd". check instead.
#endif
			// continue;
		// }else if(dynamic_cast<Subcircuit const*>(pc)) { untested();
		} else if(pc->obsolete_model_hack() == "Sub") { // BUG.

				incomplete();
			// save(pc, stream);
		}

#if 0 // does not work
		if(LibComp* lib = dynamic_cast</*const*/LibComp*>(pc)) { untested();
			if(creatingLib) { untested();
				ErrText->appendPlainText(
						QObject::tr("WARNING: Skipping library component \"%1\".").
						arg(pc->name()));
				continue;
			}
			QString scfile = pc->getSubcircuitFile();
			s = scfile + "/" + pc->Props.at(1)->Value;
			SubMap::Iterator it = FileList.find(s);
			if(it != FileList.end())
				continue;   // insert each library subcircuit just one time
			FileList.insert(s, SubFile("LIB", s));


			//FIXME: use different netlister for different purposes
			unsigned whatisit = isAnalog?1:(isVerilog?4:2);
			r = lib->createSubNetlist(stream, Collect, whatisit);
			if(!r) { untested();
				ErrText->appendPlainText(
						QObject::tr("ERROR: \"%1\": Cannot load library component \"%2\" from \"%3\"").
						arg(pc->name(), pc->Props.at(1)->Value, scfile));
				return false;
			}
			continue; // BUG
		}
#endif

		// handle SPICE subcircuit components
		if(pc->obsolete_model_hack() == "SPICE") { // BUG
			incomplete(); // move to Symbol->tac
			s = pc->Props.first()->Value;
			// tell the spice component it belongs to this schematic
			//      pc->setSchematic (this);
			if(s.isEmpty()) { untested();
				//       ErrText->appendPlainText(QObject::tr("ERROR: No file name in SPICE component \"%1\".").
				//                       arg(pc->name()));
				throw "incomplete_exception_something_about_SPICE";
			}
			QString f = pc->getSubcircuitFile();
			SubMap::Iterator it = FileList.find(f);
			if(it != FileList.end())
				continue;   // insert each spice component just one time
			FileList.insert(f, SubFile("CIR", f));

			// SpiceFile *sf = (SpiceFile*)pc;
			incomplete();
			// r = sf->createSubNetlist(stream);
			// if(!r){ untested();
			//	return false;
			// }
			// continue; // BUG
		} else if(pc->obsolete_model_hack() == "VHDL" || pc->obsolete_model_hack() == "Verilog") { untested();
			incomplete();
#if 0
			if(isVerilog && pc->obsolete_model_hack() == "VHDL")
				continue;
			if(!isVerilog && pc->obsolete_model_hack() == "Verilog")
				continue;
			s = pc->Props.getFirst()->Value;
			if(s.isEmpty()) { untested();
				ErrText->appendPlainText(QObject::tr("ERROR: No file name in %1 component \"%2\".").
						arg(pc->obsolete_model_hack()).
						arg(pc->name()));
				return false;
			}
			QString f = pc->getSubcircuitFile();
			SubMap::Iterator it = FileList.find(f);
			if(it != FileList.end())
				continue;   // insert each vhdl/verilog component just one time
			s = ((pc->obsolete_model_hack() == "VHDL") ? "VHD" : "VER");
			FileList.insert(f, SubFile(s, f));

			if(pc->obsolete_model_hack() == "VHDL") { untested();
				incomplete();
				//	VHDL_File *vf = (VHDL_File*)pc;
				//	r = vf->createSubNetlist(stream);
				//	ErrText->appendPlainText(vf->getErrorText());
				//	if(!r) { untested();
				//	  return false;
				//	}
			}else if(pc->obsolete_model_hack() == "Verilog") { untested();
				incomplete();
				//	Verilog_File *vf = (Verilog_File*)pc;
				//	r = vf->createSubNetlist(stream);
				//	ErrText->appendPlainText(vf->getErrorText());
				//	if(!r) { untested();
				//	  return false;
				//	}
			}else{ untested();
			}
			continue; // BUG
#endif
		}
	}
}

}
