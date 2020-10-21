/***************************************************************************
    copyright            : (C) 2015, 2019, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// stuff related to "simulation" collected from from lots of other files.

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
#include "net.h"
#include "misc.h"

namespace {

// -------------------------------------------------------------------
// PLAN/TODO: merge into (legacy) qucsator driver below
//    meant to produce a netlist including the qucsator commands and process
// (this is reminiscent of a "command", but qucs does not have commands)
class LegacyNetlister : public DocumentFormat{
	LegacyNetlister(LegacyNetlister const&) = delete;
public:
	explicit LegacyNetlister() : _qucslang(nullptr) {}
private: // legacy implementation
  void createNetlist(DocumentStream& stream, SchematicSymbol const& m) const;
  void prepareSave(DocumentStream& stream, SchematicSymbol const& m) const;
  void throughAllComps(DocumentStream& d, SchematicSymbol const& m) const;
  void clear() const;
  void printDeclarations(DocumentStream& d, SchematicSymbol const&m) const;
private: // overrides
  void save(DocumentStream& stream, SchematicSymbol const& m) const;
  void load(istream_t&, SchematicSymbol&) const;
private:
  mutable SubMap FileList; // BUG (maybe not)
  mutable DocumentLanguage* _qucslang;
}LNL;
static Dispatcher<DocumentFormat>::INSTALL p1(&docfmt_dispatcher, "qucsator|legacy_nl", &LNL);

// "simulator" backend emulating legacy behaviour
// TODO: forward to other simulator following legacy heuristic.
class LegacySimulator : public Simulator{
public:
	explicit LegacySimulator() : Simulator() {}
	LegacySimulator(LegacySimulator const&) = delete;
	~LegacySimulator(){}
private: // Simulator
	virtual Simulator* clone() const override {return new LegacySimulator();}
	NetLang const* netLang() const override;
	DocumentFormat const* netLister() const override {return &LNL;}

	void run() override{incomplete();}
	void init() override{incomplete();}


private: // implementation
	Simulator* chooseBackend();
}QS;
static Dispatcher<Simulator>::INSTALL p(&simulator_dispatcher, "legacy", &QS);
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
NetLang const* LegacySimulator::netLang() const
{ untested();
	DocumentLanguage const* d = doclang_dispatcher["qucsator"];
	assert(d);
	auto n = prechecked_cast<NetLang const*>(d);
	assert(n);

	return n;
}
/* -------------------------------------------------------------------------------- */
// find backend the way legacy qucs does
// that would be one out of
// qucsator, qucsdigi, asco
Simulator* LegacySimulator::chooseBackend()
{
	incomplete(); // only run qucstor for now
	return simulator_dispatcher["qucsator"];
#if 0
      if(SimOpt = findOptimization(d)) {
			return simulator_dispatcher["asco"];

			// move to ASCO driver (or so)
	    ((Optimize_Sim*)SimOpt)->createASCOnetlist();

        Program = QucsSettings.AscoBinDir.canonicalPath();
        Program = QDir::toNativeSeparators(Program+"/"+"asco"+QString(executableSuffix));
        // pass full path of simulator to ASCO so it does not be to be in PATH
        // and QUCSATOR environment variable is honored
        Arguments << "-qucs" << QucsSettings.QucsHomeDir.filePath("asco_netlist.txt")
                  << "-o" << "asco_out"
                  << "-s" << "\"" + QDir::toNativeSeparators(QucsSettings.Qucsator) + "\"";
      } else
#endif
}

void LegacyNetlister::clear() const
{
	// declarations.clear();
}

void LegacyNetlister::load(istream_t&, SchematicSymbol&) const
{
   _qucslang = doclang_dispatcher["qucsator"];
	assert(_qucslang);
	incomplete();
}

// was main::doNetlist, but it only works for qucsator. merge into qucsator driver.
void LegacyNetlister::save(DocumentStream& Stream, SchematicSymbol const& m) const
{
   _qucslang = doclang_dispatcher["qucsator"];
	clear();

	qDebug() << "*** LegacyNetlister::save";
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

#if 0
	if(m.doc()){ untested();
		Stream << "doc??\n";
	}else{ untested();
		Stream << "nodoc??\n";
	}
#endif
}

void LegacyNetlister::printDeclarations(DocumentStream& stream, SchematicSymbol const& m) const
{
	assert(m.subckt());
	// assert(_qucslang);
   _qucslang = doclang_dispatcher["qucsator"];
	assert(_qucslang);

	trace1("printing declarations", m.subckt()->declarations().size());
	for(auto si : m.subckt()->declarations()){
		//prepareSave(stream, m); // yikes
// 		if(SchematicSymbol const* sym=dynamic_cast<SchematicSymbol const*>(si)){ untested();
// 		}else{ untested();
// 		}

		trace2("printing declaration", si, si->label());
		_qucslang->printItem(si, stream);
	}
	stream << "## declarations end\n";
}

// was Schematic::prepareNetlist
// visit lot of components, strange callbacks...
void LegacyNetlister::prepareSave(DocumentStream& stream, SchematicSymbol const& m) const
{
	incomplete();

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
		DocName = m.paramValue("DocName");
	}catch(ExceptionCantFind const&){ untested();
		DocName = "unknown";
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

	QString s, Time;
	for(auto pc : m.components()){
		// if dynamic_cast<Label*>
		//   ignore
		if(pc->typeName()=="GND"){
			// qucsator hack, just ignore.
		}else if(pc->typeName()=="NodeLabel"){ untested();
			// qucsator hack, just ignore.
		}else{
			_qucslang->printItem(pc, stream);
		}

		if(isAnalog) {
		} else { // FIXME: use different lang to print things differently
#if 0
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
#endif
		}
	} // components

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
	QString s;
	bool isAnalog = true;

	assert(m.subckt());
	auto const& sckt = *m.subckt();

	for(auto pc : sckt.components()){
		Symbol const* sym = pc;
		assert(pc);
		trace4("tac", pc->label(), pc, sym->owner(), pc->typeName());
		trace1("tac", sym->owner()->label());

		// because they are components
		assert(sym->owner()==&m);
		assert(sym->scope()==&sckt);

		if(pc->paramValue("$mfactor") == "0"){
			incomplete();
			continue;
		}else if(isAnalog) {
			// check analog/digital typed components
			if((pc->Type & isAnalogComponent) == 0) {
				incomplete();
				// throw??
				// return;
			}else{
			}
//		} else if((pc->Type & isDigitalComponent) == 0) { untested();
//			return;
		}else{ untested();
		}

		if(sym && sym->subckt()){
			trace1("need expand?", sym->label());
			// if there is a sckt, make sure it is populated.
			Symbol const* p = pc->proto(&sckt); // just expand?
			assert(p->subckt());
			//
		}else if(pc->typeName() == "GND") { // BUG.
#if 0
			qDebug() << "GND hack" << pc->portValue(0);
			assert(pc->Ports.first()->value());
			Net* n = pc->Ports.first()->value()->getNet();
			n->setLabel("gnd"); // should already be "gnd". check instead.
#endif
			// continue;
		// }else if(dynamic_cast<Subcircuit const*>(pc)) { untested();
		} else if(pc->typeName() == "Sub") { // BUG.
				incomplete();
			// save(pc, stream);
		}else{
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
			bool r = lib->createSubNetlist(stream, Collect, whatisit);
			if(!r) { untested();
				ErrText->appendPlainText(
						QObject::tr("ERROR: \"%1\": Cannot load library component \"%2\" from \"%3\"").
						arg(pc->name(), pc->Props.at(1)->Value, scfile));
				// throw??
				return false;
			}
			continue; // BUG
		}
#endif

//		if(pc->has_obsolete_qucsator_callback()){ untested();
//		   pc->obsolete_qucsator_callback();
//		}else
//
		std::string model_hack = "notyet";
		if(model_hack == "SPICE") { // BUG
			incomplete(); // move to Symbol->tac
			//s = pc->Props.first()->Value;
			s = QString::fromStdString(pc->paramValue("filename_maybe"));
			// tell the spice component it belongs to this schematic
			//      pc->setSchematic (this);
			if(s.isEmpty()) { untested();
				//       ErrText->appendPlainText(QObject::tr("ERROR: No file name in SPICE component \"%1\".").
				//                       arg(pc->name()));
				throw "incomplete_exception_something_about_SPICE";
			}
			QString f = "some_filename"; // pc->getSubcircuitFile();
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
		} else if(model_hack == "VHDL" || model_hack == "Verilog") { untested();
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
	} // sckt component loop
}

#if 0 // stuff collected from schematicFile.
      // make qucsdigi work, then try and restore legacy hacks seperately
// ---------------------------------------------------
// Write the end of digital netlist to the text stream 'stream'.
// FIXME: use lang, not isVerilog
void SchematicDoc::endNetlistDigital(QTextStream& stream, NetLang const& /*lang*/)
{ untested();
  if (isVerilog) { untested();
  } else { untested();
    stream << "end architecture;\n";
  }
}

// ---------------------------------------------------
// Write the beginning of digital netlist to the text stream 'stream'.
// FIXME: really use lang. get rid of isVerilog
void SchematicDoc::beginNetlistDigital(QTextStream& stream, NetLang const& /*lang*/)
{ untested();
  if (isVerilog) { untested();
    stream << "module TestBench ();\n";
    QList<DigSignal> values = Signals.values();
    QList<DigSignal>::const_iterator it;
    for (it = values.constBegin(); it != values.constEnd(); ++it) { untested();
      stream << "  wire " << (*it).Name << ";\n";
    }
    stream << "\n";
  } else { untested();
    stream << "architecture Arch_TestBench of TestBench is\n";
    QList<DigSignal> values = Signals.values();
    QList<DigSignal>::const_iterator it;
    for (it = values.constBegin(); it != values.constEnd(); ++it) { untested();
      stream << "  signal " << (*it).Name << " : "
	     << ((*it).Type.isEmpty() ?
		 QString::fromStdString(VHDL_SIGNAL_TYPE) : (*it).Type) << ";\n";
    }
    stream << "begin\n";
  }

  if(Signals.find("gnd") != Signals.end()) { untested();
    if (isVerilog) { untested();
      stream << "  assign gnd = 0;\n";
    } else { untested();
      stream << "  gnd <= '0';\n";  // should appear only once
    }
  }
}
#endif

}
