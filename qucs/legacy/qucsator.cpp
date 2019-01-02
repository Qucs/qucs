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
#include "schematic.h" // BUG. fixed in qt branch
#include "schematic_lang.h"
#include "exception.h"
#include "misc.h"

#include "components/subcircuit.h" //  BUG

namespace {
// qucslang language implementation
class QucsLang : public NetLang {
private: // NetLang
  // inline void printItem(Element const* c, stream_t& s) const;

private: // local
  void printCommand(Command const*, stream_t&) const;
  void printSymbol(Symbol const*, stream_t&) const;
  void printComponent(Component const*, stream_t&) const;
}qucslang;
static Dispatcher<DocumentLanguage>::INSTALL pl(&doclang_dispatcher, "qucsator", &qucslang);

void QucsLang::printSymbol(Symbol const* d, stream_t& s) const
{ untested();
  if(auto c=dynamic_cast<Command const*>(d)){
    if(auto cc=dynamic_cast<Component const*>(d)){
      incomplete(); // legacy hack
      //printComponent(cc, s);
      printCommand(c, s);
    }else{
      printCommand(c, s);
    }
  }else if(auto c=dynamic_cast<Component const*>(d)){
    printComponent(c, s);
  }else{
    incomplete();
  }
}

void QucsLang::printCommand(Command const* c, stream_t& s) const
{
  if(c->isOpen()) {
    // nothing.
  }else if(c->isShort()){
    unreachable();
  }else{
    { // todo: introduce proper exceptions
      // normal netlisting

      s << "." << c->type() << ":" << c->label();

      //for(auto p2 : c->params())
      for(auto p2 : c->Props){ // BUG
	if(p2->name() != "Symbol") { // hack.
	  s << " " << p2->name() << "=\"" << p2->Value << "\"";
	}
      }
      s << '\n';
    }
  }
}

std::vector<QString> netLabels; // BUG
/*!
 * print Components in qucs language
 */
void QucsLang::printComponent(Component const* c, stream_t& s) const
{
  qDebug() << "pC" << c << c->label();

  // BUG
  assert(c->isActive == COMP_IS_ACTIVE);

  if(c->isOpen()) {
    // nothing.
  }else if(c->isShort()){
    // replace by some resistors (hack?)
    int z=0;
    QListIterator<Port *> iport(c->ports());
    Port *pp = iport.next();
    QString Node1 = pp->Connection->label();
    while (iport.hasNext()){
      s << "R:" << c->label() << "." << QString::number(z++) << " "
	<< Node1 << " " << iport.next()->Connection->label() << " R=\"0\"\n";
    }
//  }else if(Subcircuit const* sub=dynamic_cast<Subcircuit const*>(c)){
//    incomplete();
  }else{
    { // todo: introduce proper exceptions
      // normal netlisting

      s << c->type() << ":" << c->label();

      // output all node names
      for(Port *p1 : c->ports()){
			s << " ";
			if(p1->Connection->hasNumber()){
				s << netLabels[p1->Connection->number()];
			}else{
				// happens in list_entries ...
				s << "open";
			}
      }

      for(auto p2 : c->params()) {
	if(p2->name() != "Symbol") { // hack.
	  s << " " << p2->name() << "=\"" << p2->Value << "\"";
	}
      }
      s << '\n';
    }
  }
}


// qucsator simulator backend
class Qucsator : public Simulator
{
  NetLang const* netLang() const{return &qucslang;}
}QS;
static Dispatcher<Simulator>::INSTALL p(&simulator_dispatcher, "qucsator", &QS);

// -------------------------------------------------------------------

class LegacyNetlister : public DocumentFormat{
private: // legacy implementation
  void createNetlist(DocumentStream& stream, ModelAccess const& m) const;
  void prepareSave(DocumentStream& stream, ModelAccess const& m) const;
  void throughAllComps(DocumentStream& d, ModelAccess const& m) const;
private: // overrides
  void save(DocumentStream& stream, ModelAccess const& m) const;
private:
  mutable SubMap FileList;
}LNL;
static Dispatcher<DocumentFormat>::INSTALL p1(&docfmt_dispatcher, "legacy_nl", &LNL);

// was main::doNetlist
void LegacyNetlister::save(DocumentStream& Stream, ModelAccess const& m) const
{
  qDebug() << "*** LegacyNetlister::save";

  SchematicModel const *sch = &m.schematicModel();
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

  Stream << '\n';

  createNetlist(Stream, m);
}

// was Schematic::prepareNetlist
//  visit lot of components, strange callbacks...
void LegacyNetlister::prepareSave(DocumentStream& stream, ModelAccess const& m) const
{

	// if(showBias > 0) showBias = -1;  // do not show DC bias anymore

	bool isVerilog = false;
	bool isAnalog = true;
	bool isTruthTable = false;
	int allTypes = 0, NumPorts = 0;

	// BUG: Detect simulation domain (analog/digital) by looking at component types.
#if 0
	for(Component *pc = DocComps.first(); pc != 0; pc = DocComps.next()) {
		if(pc->isActive == COMP_IS_OPEN){
			// open circuit (or so)
			//    }else if(pc->obsolete_model_hack().at(0) == '.') {
	}else if(dynamic_cast<Command const*>(pc)) {

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
			isAnalog = true;
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
		isAnalog = true;
	}

	// first line is documentation
	if(allTypes & isAnalogComponent)
		stream << "#";
	else if (isVerilog)
		stream << "//";
	else{
		stream << "--";
	}

	std::string DocName;
	try{
		DocName=m.getParameter("DocName");
	}catch(ExceptionCantFind const&){
		DocName="unknown";
	}
	stream << " Qucs " << PACKAGE_VERSION << "  "
	       << QString::fromStdString(DocName) << "\n";

	// set timescale property for verilog schematics
	if (isVerilog) {
		stream << "\n`timescale 1ps/100fs\n";
	}

	int countInit = 0;  // counts the nodesets to give them unique names

	// BUG: giveNodeNames ejects subcircuit declarations (WTF?)
	// giveNodeNames(&stream, m);
	
	unsigned count;
	auto& sm=m.schematicModel();
	sm.throughAllNodes(count); // hack: number connected components.
	                           // should already be numbered...

	unsigned nc=sm.numberOfNets();
	netLabels.resize(nc);
	qDebug() << "found" << nc << "nets";
	
	for(auto w : sm.wires()){
		assert(w->Port1->number()==w->Port1->number());
		unsigned i=w->Port1->number();
		qDebug() << "wire" << i << w->Label;
		if(!w->Label){
		}else if (netLabels[i].size()){
		}else{
			netLabels[i] = w->Label->name();
		}
	}

	unsigned z=0;
	for(auto& i : netLabels){
		if(!i.size()){
			i = "_net" + QString::number(z++);
		}else{
		}
	}

	throughAllComps(stream, m);

	if(allTypes & isAnalogComponent){
	}else if (!isVerilog) {
		incomplete();
	//	stream << QString::fromStdString(VHDL_LIBRARIES);
	//	stream << "entity TestBench is\n"
	//		<< "end entity;\n"
	//		<< "use work.all;\n";
	}

//	return NumPorts;
}

// former Schematic::createNetlist
void LegacyNetlister::createNetlist(DocumentStream& stream,
		ModelAccess const& m) const
{ untested();
	bool isAnalog=true;
	bool isVerilog=false;
	//  if(!isAnalog) {
	//    beginNetlistDigital(stream, nl);
	//  }

	//  Signals.clear();  // was filled in "giveNodeNames()"
	FileList.clear();

	int NumPorts=0; // huh??

	QString s, Time;
	for(auto pc : m.schematicModel().components()){
		if(isAnalog) {
			if(pc->type()=="GND"){ // qucsator hack
			}else{
				qucslang.printItem(pc, stream);
			}
		} else { // FIXME: use different lang to print things differently
			if(pc->obsolete_model_hack() == ".Digi" && pc->isActive) {  // simulation component ?
				if(NumPorts > 0) { // truth table simulation ?
					if (isVerilog){
						Time = QString::number((1 << NumPorts));
					} else {
						Time = QString::number((1 << NumPorts) - 1) + " ns";
					}
				} else {
					Time = pc->Props.at(1)->Value;
					if (isVerilog) {
						if(!misc::Verilog_Time(Time, pc->name())) return;
					} else {
						if(!misc::VHDL_Time(Time, pc->name())) return;
					}
				}
			}
			if (isVerilog) {
				s = pc->get_Verilog_Code(NumPorts);
			} else {
				s = pc->get_VHDL_Code(NumPorts);
			}
			if (s.length()>0 && s.at(0) == '\xA7'){
				throw "strange_character";
			}
			stream << s;
		}
	}

	if(!isAnalog) {
		// endNetlistDigital(stream, qucslang);
	}
// return Time;
}

void LegacyNetlister::throughAllComps(DocumentStream& stream, ModelAccess const& m) const
{ incomplete();
	bool r;
	QString s;
	bool isAnalog=true;

	// give the ground nodes the name "gnd", and insert subcircuits etc.
	for(auto it : m.schematicModel().components()){ untested();
		Component *pc=it;

		if(pc->isActive != COMP_IS_ACTIVE) continue;

		// check analog/digital typed components
		if(isAnalog) {
			if((pc->Type & isAnalogComponent) == 0) {
				incomplete();
				return;
			}
		} else if((pc->Type & isDigitalComponent) == 0) {
			return;
		}

		QStringList Collect; // BUG
		int countInit = 0;  // ??
		int NumPorts=0; // ??!
		pc->tAC(stream, &m.schematicModel(), Collect, countInit, NumPorts, qucslang); //?!!
		// handle ground symbol
		if(pc->obsolete_model_hack() == "GND") { // BUG.
			qDebug() << "GND hack" << pc->Ports.first()->Connection->name();
			pc->Ports.first()->Connection->setName("gnd");
			continue;
#if 0 // moved to Subcircuit::tAC
		}else if(dynamic_cast<Subcircuit const*>(pc)) {
			assert(pc->obsolete_model_hack()=="Sub"); // really?
			int i;
			// tell the subcircuit it belongs to this schematic
			pc->setSchematic (this);
			QString f = pc->getSubcircuitFile();
			SubMap::Iterator it = FileList.find(f);
			if(it != FileList.end())
			{
				if (!it.value().PortTypes.isEmpty())
				{
					i = 0;
					// apply in/out signal types of subcircuit
					foreach(Port *pp, pc->Ports)
					{
						pp->Type = it.value().PortTypes[i];
						pp->Connection->DType = pp->Type;
						i++;
					}
				}
				continue;   // insert each subcircuit just one time
			}

			// The subcircuit has not previously been added
			SubFile sub = SubFile("SCH", f);
			FileList.insert(f, sub);


			// load subcircuit schematic
			s = pc->Props.first()->Value;

			//      qDebug() << "reading subckt schematic" << pc->getSubcircuitFile();
			Schematic *d = new Schematic(0, pc->getSubcircuitFile());
			if(!d->loadDocument()) { // BUG. "try".
				delete d;
				/// \todo implement error/warning message dispatcher for GUI and CLI modes.
				QString message = QObject::tr("ERROR: Cannot load subcircuit \"%1\".").arg(s);
				if (QucsMain) // GUI is running
					ErrText->appendPlainText(message);
				else // command line
					qCritical() << "Schematic::throughAllComps" << message;
				return false;
			}else{
			}
			d->DocName = s;
			d->isVerilog = isVerilog;
			d->isAnalog = isAnalog;
			d->creatingLib = creatingLib;
			r = d->createSubNetlist(stream, countInit, Collect, ErrText, NumPorts, nl);
			if (r)
			{
				i = 0;
				// save in/out signal types of subcircuit
				foreach(Port *pp, pc->Ports)
				{
					//if(i>=d->PortTypes.count())break;
					pp->Type = d->PortTypes[i];
					pp->Connection->DType = pp->Type;
					i++;
				}
				sub.PortTypes = d->PortTypes;
				FileList.insert(f, sub);
			}
			delete d;
			if(!r)
			{
				return false;
			}
			continue; // BUG
		}else{
#endif
		}

#if 0 // does not work
		if(LibComp* lib = dynamic_cast</*const*/LibComp*>(pc)) {
			if(creatingLib) {
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
			if(!r) {
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
			if(s.isEmpty()) {
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
			// if(!r){
			//	return false;
			// }
			// continue; // BUG
		} else if(pc->obsolete_model_hack() == "VHDL" || pc->obsolete_model_hack() == "Verilog") {
			incomplete();
#if 0
			if(isVerilog && pc->obsolete_model_hack() == "VHDL")
				continue;
			if(!isVerilog && pc->obsolete_model_hack() == "Verilog")
				continue;
			s = pc->Props.getFirst()->Value;
			if(s.isEmpty()) {
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

			if(pc->obsolete_model_hack() == "VHDL") {
				incomplete();
				//	VHDL_File *vf = (VHDL_File*)pc;
				//	r = vf->createSubNetlist(stream);
				//	ErrText->appendPlainText(vf->getErrorText());
				//	if(!r) {
				//	  return false;
				//	}
			}else if(pc->obsolete_model_hack() == "Verilog") {
				incomplete();
				//	Verilog_File *vf = (Verilog_File*)pc;
				//	r = vf->createSubNetlist(stream);
				//	ErrText->appendPlainText(vf->getErrorText());
				//	if(!r) {
				//	  return false;
				//	}
			}else{
			}
			continue; // BUG
#endif
		}
	}
}

}
