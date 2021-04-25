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

#include "docfmt.h"
#include "element_list.h"
#include "exception.h"
#include "misc.h"
#include "net.h"
#include "node.h"
#include "qio.h"
#include "qucs_globals.h"
#include "schematic_lang.h"
#include "sckt_base.h"
#include "settings.h"
#include "simulator.h"
#include "symbol.h"
#include "task_element.h"

#include <QString>
#include <QFile>

extern tQucsSettings QucsSettings;  // bug, settings.h
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
namespace {
/* -------------------------------------------------------------------------------- */
using namespace qucs;
/* -------------------------------------------------------------------------------- */
//	LegacyNetlister + qucsatorLanguage = qucsator netlister
class LegacyNetlister : public DocumentFormat{
	LegacyNetlister(LegacyNetlister const&) = delete;
public:
	explicit LegacyNetlister() : _qucslang(nullptr) {}
private: // Command
	void do_it(istream_t&, ElementList*) override;

private: // legacy implementation
  void createNetlist(ostream_t& stream, ElementList const*,
		std::vector<Element const*>& tasks) const;
  void prepareSave(ostream_t& stream, ElementList const* scope,
		std::map<std::string, Element const*>& declarations) const;
  void throughAllComps(ostream_t& d, ElementList const* scope,
		std::map<std::string, Element const*>& declarations) const;
  void clear() const;
  void printDeclarations(ostream_t& d,
		std::map<std::string, Element const*>& declarations) const;
private: // overrides
  void load(istream_t&, Object*) const override;
private:
//  mutable SubMap FileList; // BUG (maybe not)
  mutable Language* _qucslang;
}LNL;
static Dispatcher<Command>::INSTALL p1(&command_dispatcher, "qucsator|legacy_nl", &LNL);
/* -------------------------------------------------------------------------------- */
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

	void run(istream_t&, SimCtrl*) override{ incomplete(); }
	void do_it(istream_t&, ElementList const*) override{ incomplete(); }
	void init() override{incomplete();}
	std::string errorString() const override{ incomplete(); return "incomplete";}
	void kill() override{ incomplete(); }
	void join() override{ incomplete(); }

private: // implementation
	Simulator const* chooseBackend();
	Simulator* _wrapped_simulator;
}QS;
static Dispatcher<Data>::INSTALL p(&data_dispatcher, "legacy|legacy_sim", &QS);
/* -------------------------------------------------------------------------------- */
struct default_sim{
	default_sim(){
		QucsSettings.setSimulator(&QS);
	}
}ds;
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
NetLang const* LegacySimulator::netLang() const
{ untested();
	Language const* d = language_dispatcher["qucsator"];
	assert(d);
	auto n = prechecked_cast<NetLang const*>(d);
	assert(n);

	return n;
}
/* -------------------------------------------------------------------------------- */
// find backend the way legacy qucs does
// that would be one out of
// qucsator, qucsdigi, asco
Simulator const* LegacySimulator::chooseBackend()
{ untested();
	incomplete(); // only run qucstor for now
	auto dd = data_dispatcher["qucsator"];
	return dynamic_cast<Simulator const*>(dd);
#if 0
      if(SimOpt = findOptimization(d)) { untested();
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
Component * SimMessage::findOptimization(SchematicDoc *)
{ untested();
#if 0
  for(auto pc : Doc->components()){ untested();
    if(pc->isActive){ untested();
      if(pc->obsolete_model_hack() == ".Opt"){ untested();
	return pc;
      }
    }
  }
#endif
  return NULL;
}
#endif
}

void LegacyNetlister::clear() const
{
	//_declarations.clear();
}
/* -------------------------------------------------------------------------------- */
void LegacyNetlister::load(istream_t&, Object*) const
{ untested();
   _qucslang = language_dispatcher["qucsator"];
	assert(_qucslang);
	incomplete();
}
/* -------------------------------------------------------------------------------- */
void LegacyNetlister::do_it(istream_t& cs, ElementList* scope)
{
	trace0("LegacyNetlister::do_it");
	scope->prepare();
	std::map<std::string, Element const*> declarations;
	std::vector<Element const*> tasks;
	std::string simcmd;
	cs >> "netlist";

	size_t here = cs.cursor();
	do{
		Get(cs, "mode", &simcmd);
	} while(cs.more() && !cs.stuck(&here));

	std::string fn;
	cs >> fn;

	QFile NetlistFile(QString::fromStdString(fn));
	if(!NetlistFile.open(QIODevice::WriteOnly | QFile::Truncate)) { untested();
		fprintf(stderr, "Error: Could not open %s for writing\n", fn.c_str());
		exit(1); // BUG
	}else{
	}
	ostream_t Stream(&NetlistFile);

   _qucslang = language_dispatcher["qucsator"];
	clear();

	int SimPorts = 10;//??
	incomplete(); // HERE

	// assert(scope.owner()); // root symbol does not have owner...
	prepareSave(Stream, scope, declarations);

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

	printDeclarations(Stream, declarations);
	Stream << '\n';
	createNetlist(Stream, scope, tasks);

	assert(_qucslang);
	if(simcmd=="all"){itested();
		Stream << "# all tasks\n";
		for(auto c : tasks){itested();
			trace1("cmd", c->label());
			_qucslang->printItem(Stream, c);
		}
	}else if(simcmd=="dcop"){ untested();
		Element const* dc = element_dispatcher["DC"];
		_qucslang->printItem(Stream, dc);
		Stream << "# just dcop\n";
	}else{
	//	assert(false);
	//	throw Exception("nothing to do");
	}

}

void LegacyNetlister::printDeclarations(ostream_t& stream,
		std::map<std::string, Element const*>& declarations) const
{
   _qucslang = language_dispatcher["qucsator"];
	assert(_qucslang);

	stream << "## declarations "<< declarations.size() << "\n";
	for(auto si : declarations){

		if(!si.second){
			stream << "# unresolved symbol " << si.first << "\n";
		}else if(dynamic_cast<SubcktBase const*>(si.second)){
			stream << "### item " << si.first << "\n";
			_qucslang->printItem(stream, si.second);
		}else{
			stream << "## " << si.first << "\n";
		}
	}
	stream << "## declarations end\n";
}

// was Schematic::prepareNetlist
// visit lot of components, strange callbacks...
void LegacyNetlister::prepareSave(ostream_t& stream, ElementList const* scope,
		std::map<std::string, Element const*>& declarations) const
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
	}else if(dynamic_cast<taskElement const*>(pc)) { untested();

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

	if((allTypes) == 0) {
		if(allTypes == 0) {
			// If no simulation exists, assume analog simulation. There may
			// be a simulation within a SPICE file. Otherwise Qucsator will
			// output an error.
			// isAnalog = true;
//			allTypes |= isAnalogComponent;
			NumPorts = -1;
		}else{ untested();
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

#if 0
	// first line is documentation
	if(allTypes & isAnalogComponent)
		stream << "#";
	else if (isVerilog)
		stream << "//";
	else{ untested();
		stream << "--";
	}
#else
	stream << "#";
#endif

	std::string DocName;
	try{
//		DocName = scope.paramValue("DocName");
	}catch(qucs::ExceptionCantFind const&){ untested();
		DocName = "unknown";
	}
	stream << " Qucs " << PACKAGE_VERSION << "  "
	       << QString::fromStdString(DocName) << "\n";

	// set timescale property for verilog schematics
	if (isVerilog) { untested();
		stream << "\n`timescale 1ps/100fs\n";
	}

	// assert(scope.owner()); //root does not have owner...
	throughAllComps(stream, scope, declarations);
}

// former Schematic::createNetlist
void LegacyNetlister::createNetlist(ostream_t& stream,
		ElementList const* scope_,
		std::vector<Element const*>& tasks) const
{
	assert(scope_);
	auto mi = scope_->find_("main");
	assert(mi!=scope_->end());
	assert(*mi);
	ElementList* scope = (*mi)->scope();
	scope->prepare();

	bool isAnalog=true;
//	bool isVerilog=false;
	// FileList.clear();

	// it seems that legacy qucsator does not do recursive prototypes.
	// (do it properly in alternative netlister).
	// collectPrototypes();
	// ejectProtootypes();
	//
	/// include directives. qucsator does not seem to do that.
	// for(auto si : directives){ untested();
	// 	qucslang.printItem(si, stream);
	// }
	//
	// legacy: qucsator expects all definitions at the top

	// BUG: deduplicate. "print_module_body" or so.
	QString Time;
	assert(scope);
	trace1("qucsator netlist main", scope);
	for(auto it_ : *scope){
	//	stream << "...\n";
		auto pc = dynamic_cast<Symbol const*>(it_);
		if (auto t = dynamic_cast<TaskElement const*>(it_)){
			tasks.push_back(t);
			continue;
		} else if(pc){
		}else{
			incomplete();
			continue;
		}

		// if dynamic_cast<Label*>
		//   ignore
		if(dynamic_cast<Conductor const*>(pc)){
			// possibly a wire.
		}else if(pc->dev_type()=="GND"){
			// qucsator hack, just ignore.
		}else if(pc->dev_type()=="NodeLabel"){ untested();
			// qucsator hack, just ignore.
		}else{
			_qucslang->printItem(stream, pc);
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

// obsolete.
void LegacyNetlister::throughAllComps(ostream_t& xx, ElementList const* scope_,
		std::map<std::string, Element const*>& declarations) const
{ incomplete();
	QString s;
	bool isAnalog = true;

	ElementList const* sckt = scope_;

	auto f = scope_->find_("main");
	if(f!=scope_->end()){
		assert(*f);
		sckt = (*f)->scope();
	}else{
	}

	for(auto it_ : *sckt){
		auto pc = dynamic_cast<Symbol const*>(it_);
		if(pc){
		}else{
			incomplete();
			continue;
		}

		Symbol const* sym = pc;
		assert(pc);
		trace4("tac", pc->label(), pc, sym->owner(), pc->dev_type());

		if(!sym->owner()){
			incomplete(); //?
//			symbolSection...
			continue;
		}else{
		}
		trace2("tac", sym->label(), sym->owner()->label());

		// because they are components
//		assert(sym->owner()==&scope);
		if(sym->scope()==sckt){
		}else{ untested();
			// ?
			assert(sym->label()=="main");
		}

		if(pc->param_value_by_name("$mfactor") == "0"){
			incomplete();
			continue;
		}else if(isAnalog) {
			// check analog/digital typed components
#if 0
			if((pc->Type & isAnalogComponent) == 0) {
				incomplete();
			//
				// throw??
				// return;
			}else{
			}
#endif
//		} else if((pc->Type & isDigitalComponent) == 0) { untested();
//			return;
		}else{ untested();
		}

		if(dynamic_cast<SubcktBase const*>(sym)){ untested();
			trace2("is sckt?", sym->label(), sym->dev_type());
			incomplete();
		}else if(sym && sym->common()){
			// linked to a model go find it.
			std::string key = sym->common()->modelname();
			trace3("need shared declaration?", sym->label(), key, sym);

			// Element const* proto = sym->find_looking_out(key);
			Element const* proto = _qucslang->find_proto(key, sym);
			auto& d = declarations[key];

			// only one proto per key
			if(!d){
				d = proto;

				if(!d){
				}else if(d->scope()){
					throughAllComps(xx, d->scope(), declarations);
				}else{
				}
			}else{
				assert(d==proto);
			}
		}else if(sym && sym->subckt()){
			// really??
			std::string key = sym->dev_type();
			trace3("need declaration?", sym->label(), key, sym);

			// Element const* proto = sym->find_looking_out(key);
			Element const* proto = _qucslang->find_proto(key, sym);
			auto& d = declarations[key];

			// only one proto per key
			assert(!d || d == proto);
		  	d = proto;

		}else if(pc->dev_type() == "GND") { // BUG.
#if 0
			Net* n = pc->Ports.first()->value()->getNet();
			assert(n->label == "gnd");
#endif
		}else{
			// no.
			// find_type(pc->dev_type()); // but where?
			trace2("not sure", pc->dev_type(), pc->label());
//			_declarations.insert(pc->dev_type());
			// Symbol const* p = pc->proto(&sckt); // just expand?
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
			s = QString::fromStdString(pc->param_value_by_name("filename_maybe"));
			// tell the spice component it belongs to this schematic
			//      pc->setSchematic (this);
			if(s.isEmpty()) { untested();
				//       ErrText->appendPlainText(QObject::tr("ERROR: No file name in SPICE component \"%1\".").
				//                       arg(pc->name()));
				throw "incomplete_exception_something_about_SPICE";
			}
			QString f = "some_filename"; // pc->getSubcircuitFile();

//			SubMap::Iterator it = FileList.find(f);
//			if(it != FileList.end())
//				continue;   // insert each spice component just one time
//			FileList.insert(f, SubFile("CIR", f));

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
