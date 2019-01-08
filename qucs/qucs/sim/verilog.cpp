/***************************************************************************
                              verilog.cc
                              ----------
    begin                : yes
    copyright            : (C) 2015 by Felix Salfelder
    email                : felix@salfelder.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sim.h"
#include "node.h"
#include <QString>
#include "globals.h"
#include "docfmt.h"
#include "command.h"
#include "schematic_lang.h"
#include "schematic_model.h"

// #include "schematic.h" // BUG, transition

unsigned gndhackn=0;

class PaintingList;
class DiagramList;
class WireList;
class NodeList;
class ComponentList;

namespace {
class Verilog : public NetLang
{
  virtual void printCommand(Command const*, QTextStream&) const;
  virtual void printSymbol(Symbol const*, QTextStream&) const;
} V;

static Dispatcher<DocumentLanguage>::INSTALL p(&doclang_dispatcher, "verilog", &V);

/*!
 * verilog does not know about commands
 */
void Verilog::printCommand(Command const* c, QTextStream& s) const
{
  s << "//" << c->label() << "\n";
}

void Verilog::printSymbol(Symbol const* sym, QTextStream& s) const
{ untested();
	Component const* c=dynamic_cast<Component const*>(sym);
	if(!c){
		incomplete();
		return;
	}else if(c->isOpen()) { untested();
		unreachable(); // wrong place.
	}else if(c->isShort()){ untested();
		// replace by some resistors (hack?)
		int z=0;
		QListIterator<Port *> iport(c->ports());
		Port *pp = iport.next();
		QString Node1 = pp->Connection->name();
		while (iport.hasNext()){
			s << "R:" << c->label() << "." << QString::number(z++) << " "
				<< Node1 << " " << iport.next()->Connection->name() << " R=\"0\"\n";
		}
	}else{ untested();
		s << QString::fromStdString(c->type()) << " ";

		QString comma="";
		s << "#(";
		for(auto p2 : c->params()) {
			s << comma << "." << p2->name() << "(" << p2->Value << ")";
			comma = ", ";
		}
		s << ") ";
		s << c->label() << "(";

		comma = "";
		for(Port *p1 : c->ports()){
			s << comma << p1->Connection->label();
			if(p1->name()!=""){
				s << "?";
			}else{
			  	s << " /*" << p1->Name << "*/"; // later.
			}
			comma = ", ";
		}

		s << ");\n";
	}
}

/// --------------------------------------------------------
class VerilogSchematicFormat : public DocumentFormat{
  void save(DocumentStream& stream, SchematicSymbol const&) const;
  void load(DocumentStream& stream, SchematicSymbol&) const;

private: // legacy cruft
  bool isSymbolMode() const{ return false; }
  PaintingList const& symbolPaints(SchematicSymbol const& m) const{
    return m.schematicModel().symbolPaints();
  }
  DiagramList const& diagrams(SchematicSymbol const& m) const{
    return m.schematicModel().diagrams();
  }
  PaintingList const& paintings(SchematicSymbol const& m) const{
    return m.schematicModel().paintings();
  }
  WireList const& wires(SchematicSymbol const& m) const{
    return m.schematicModel().wires();
  }
  NodeList const& nodes(SchematicSymbol const& m) const{
    return m.schematicModel().nodes();
  }
  ComponentList const& components(SchematicSymbol const& m) const{
    return m.schematicModel().components();
  }

private: // hacks.
  void printSymbol(Symbol const*, stream_t&) const;
}VS;

static Dispatcher<DocumentFormat>::INSTALL
pp(&docfmt_dispatcher, "v_sch", &VS);
// ------------------------------------------------------

void VerilogSchematicFormat::load(DocumentStream& stream, SchematicSymbol& s) const
{
  incomplete();
}

void VerilogSchematicFormat::save(DocumentStream& stream, SchematicSymbol const& m) const{
  for(auto pc : components(m)){ untested();
	  if(dynamic_cast<Command const*>(pc)){
		  unreachable();
		  // BUG. a Command is not a Component
		  continue;
	  }
	  printSymbol(pc, stream); // BUG: use V::printItem
	                           // but uses different port names...
  }
}

// similar to Verilog::printSymbol, but with the actual node names and
// coordinates.
void VerilogSchematicFormat::printSymbol(Symbol const* sym, stream_t& s) const
{ untested();
	Component const* c=dynamic_cast<Component const*>(sym);
	if(!c){
		incomplete();
		unreachable();
		return;
	}else if(c->isOpen()) { untested();
		// parameter?
		incomplete();
		// }else if(c->isShort()){ untested();
		//   // parameter?
		//   incomplete();
	}else{ untested();
		std::string type = c->type();
		std::string label = c->label().toStdString();
		s << QString::fromStdString(type) << " ";

		if(label == "*"){
			// bug/feature/wtf?
			label="anonymous_gnd_hack_" + std::to_string(gndhackn++);
		}else{
		}

		QString comma="";
		s << "#(";
		for(auto p2 : c->params()) {
			s << comma << "." << p2->name() << "(" << p2->Value << ")";
			comma = ", ";
		}
		s << ") ";
		s << QString::fromStdString(label) << "(";

		comma = "";
		for(unsigned i=0; i<sym->portCount(); ++i){
			auto p1=sym->port(i); // BUG
			s << comma << "net_" << p1->value()->cx() << "_" <<  p1->value()->cy();
			comma = ", ";
		}

		s << ");\n";
	}
}

} // namespace
