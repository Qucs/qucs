/***************************************************************************
    copyright            : (C) 2015, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "node.h"
#include <QString>
#include "globals.h"
#include "docfmt.h"
#include "task_element.h"
#include "schematic_lang.h"
#include "schematic_model.h"
#include "net.h"
#include "simulator.h" // BUG. just NetLang

// #include "schematic.h" // BUG, transition

unsigned gndhackn=0;

class PaintingList;
class DiagramList;
class WireList;
class ComponentList;

namespace {

static std::string netLabel(Node const* nn)
{
	if(!nn){
		return "(null)";
	}else{
	}
	Net const* n = nn->net();
	if(!n){
		unreachable();
		return "(null)";
	}else if(n->hasLabel()){
		return n->label().toStdString();
	}else{
		return "_net" + std::to_string(n->pos());
	}
}

class Verilog : public NetLang {
	void printtaskElement(TaskElement const*, ostream_t&) const override;
	void printSymbol(Symbol const*, ostream_t&) const override;
	void printPainting(Painting const*, ostream_t&) const override {incomplete();}
	void printDiagram(Symbol const*, ostream_t&) const override {incomplete();}

private: //NetLang
	std::string findType(istream_t&) const override {incomplete(); return "incomplete";}
} V;

static Dispatcher<DocumentLanguage>::INSTALL p(&doclang_dispatcher, "verilog", &V);

/*!
 * verilog does not know about commands
 */
void Verilog::printtaskElement(TaskElement const* c, ostream_t& s) const
{
  s << "//" << c->label() << "\n";
}

void Verilog::printSymbol(Symbol const* sym, ostream_t& s) const
{
	Component const* c=dynamic_cast<Component const*>(sym);
#if 0
	if(!c){ untested();
		/// wires...
		incomplete();
		return;
// 	}else if(!c->is_device) { ?
	}else if(c->isOpen()) { untested();
		unreachable(); // wrong place.
	}else if(c->isShort()){ untested();
		// replace by some resistors (hack?)
		int z=0;
		QListIterator<Port *> iport(c->ports());
		Port *pp = iport.next();
		QString Node1 = pp->netLabel();
		while (iport.hasNext()){ untested();
			s << "R:" << c->label() << "." << QString::number(z++) << " "
				<< Node1 << " " << iport.next()->netLabel() << " R=\"0\"\n";
		}
	}else
#endif
	{
		auto label = sym->label();
		auto type = sym->typeName();
		// : is not allowed in verilog
      std::replace( type.begin(), type.end(), ':', '$');
		s << QString::fromStdString(type) << " ";
		QString comma="";
		s << "#(";

			// DUPLICATE		print_args
			if(!c){
				incomplete();
			}else
		for(auto p2 : c->params()) {
			if(p2->name().at(0) == '$'){
			}else{
				s << comma << "." << p2->name() << "(" << p2->Value << ")";
				comma = ", ";
			}
		}
		s << ") ";
		s << label << "(";

		// printPorts()
		comma = "";
		for(unsigned i=0; i < sym->numPorts(); ++i){
			s << comma << netLabel(sym->portValue(i));
			comma = ", ";
		}

		s << ");\n";
	}
}

/// --------------------------------------------------------
class VerilogSchematicFormat : public DocumentFormat{
  void save(DocumentStream& stream, SchematicSymbol const&) const;
  void load(istream_t& stream, SchematicSymbol&) const;

private: //command
  void do_it(istream_t&, SchematicModel*) override{incomplete();}

private: // legacy cruft
  bool isSymbolMode() const{ return false; }
  PaintingList const& symbolPaints(SchematicSymbol const& m) const{ untested();
    assert(m.symbolPaintings());
    return *m.symbolPaintings();
  }
  DiagramList const& diagrams(SchematicSymbol const& m) const{ untested();
    return m.diagrams();
  }
  PaintingList const& paintings(SchematicSymbol const& m) const{ untested();
    return m.paintings();
  }
  WireList const& wires(SchematicSymbol const& m) const{
    return m.wires();
  }
  NodeMap const& nodes(SchematicSymbol const& m) const{
    return m.nodes();
  }
  ComponentList const& components(SchematicSymbol const& m) const{
    return m.components();
  }

private: // hacks.
  void printSymbol(Symbol const*, ostream_t&) const;
}VS;
static Dispatcher<Command>::INSTALL pp(&command_dispatcher, "v_sch", &VS);
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
void VerilogSchematicFormat::load(istream_t&, SchematicSymbol&) const
{ untested();
  incomplete();
}

void VerilogSchematicFormat::save(DocumentStream& stream, SchematicSymbol const& m) const
{
  for(auto pc : components(m)){
	  if(dynamic_cast<TaskElement const*>(pc)){ untested();
		  unreachable();
		  // BUG. a TaskElement is not a Component
		  continue;
	  }
	  printSymbol(pc, stream); // BUG: use V::printItem
	                           // but uses different port names...
  }
  for(auto w : wires(m)){
	  printSymbol(w, stream); // BUG: use V::printItem
  }
  for(auto const& n : nodes(m)){
	  int x, y;
	  std::tie(x, y) = n.position();
	  stream << "place #(.$xposition(" << x << "),"
		                 ".$yposition(" << y << "))"
							 <<  " place_" << x << "_" << y
							 << "(net_" << x << "_" << y << ");\n";
  }
}

//// not here.
//QTextStream& operator<<(QTextStream& o, std::string const& s)
//{ untested();
//		return o << QString::fromStdString(s);
//}

// similar to Verilog::printSymbol, but with the actual node names and
// coordinates.
void VerilogSchematicFormat::printSymbol(Symbol const* sym, ostream_t& s) const
{
#if 0
	Component const* c=dynamic_cast<Component const*>(sym);
	if(!c){ untested();
		incomplete();
		unreachable();
		return;
	}else if(c->isOpen()) { untested();
		// parameter?
		incomplete();
		// }else if(c->isShort()){ untested();
		//   // parameter?
		//   incomplete();
	}else
#endif
	
	{
		std::string type = sym->typeName();
      std::replace( type.begin(), type.end(), ':', '$');
		std::string label = sym->label().toStdString();

		s << QString::fromStdString(type) << " ";

		if(label == "*"){
			// bug/feature/wtf?
			label="anonymous_gnd_hack_" + std::to_string(gndhackn++);
		}else{
		}

		QString comma="";
		s << "#(";


			// DUPLICATE		print_args
		for(unsigned i=0; i<sym->paramCount(); ++i) {
			auto name = sym->paramName(i);
			if(name.at(0) == '$'){
			}else{
				s << comma << "." << sym->paramName(i) << "(" << sym->paramValue(i) << ")";
				comma = ", ";
			}
		}
		s << ") ";
		// s << QString::fromStdString(label) << "(";
		s << label << "(";

		comma = "";
		for(unsigned i=0; i<sym->numPorts(); ++i){
			trace3("...", sym->label(), i, sym->numPorts());
			auto p = sym->nodePosition(i);
			auto x = p.first;
			auto y = p.second;
			s << comma << "net_" << x << "_" << y;
			comma = ", ";
		}

		s << ");\n";
	}
}

} // namespace
