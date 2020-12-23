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
#include "globals.h"
#include "docfmt.h"
#include "task_element.h"
#include "schematic_lang.h"
#include "schematic_model.h"
#include "net.h"
#include "sckt_base.h"

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
	if(!n){ untested();
		unreachable();
		return "(null)";
	}else if(n->hasLabel()){
		return n->label();
	}else{
		return "_net" + std::to_string(n->pos());
	}
}

class Verilog : public DocumentLanguage {
	void printtaskElement(TaskElement const*, ostream_t&) const override;
	void printSymbol(Symbol const*, ostream_t&) const override;
	void printSubckt(SubcktBase const*, ostream_t&) const override;
	void printPainting(Painting const*, ostream_t&) const override {incomplete();}
	void printDiagram(Symbol const*, ostream_t&) const override {incomplete();}
private:
	void print_ports_short(ostream_t& o, const Symbol* x) const;
private: //DocumentLanguage
	std::string findType(istream_t&) const override {incomplete(); return "incomplete";}
} V;
static Dispatcher<DocumentLanguage>::INSTALL p0(&language_dispatcher, "verilog|verilog_nl", &V);
/*--------------------------------------------------------------------------*/
class VS : public Verilog {
	void printSymbol(Symbol const*, ostream_t&) const override;
	void printSubckt(SubcktBase const* x, ostream_t& o) const override;
private:
	void print_ports_short(ostream_t& o, const Symbol* x) const;
} V_;
static Dispatcher<DocumentLanguage>::INSTALL p1(&language_dispatcher, "verilog_schematic", &V_);
/*--------------------------------------------------------------------------*/
static void printArgs(Symbol const* sym, ostream_t& s)
{
	std::string comma="";
	s << "#(";

	for(unsigned i=0; i<sym->paramCount(); ++i) {
		auto name = sym->paramName(i);
		if(name.at(0) == '$'){
		}else{
			s << comma << "." << sym->paramName(i) << "(" << sym->paramValue(i) << ")";
			comma = ", ";
		}
	}
	s << ") ";
}
/*--------------------------------------------------------------------------*/
void VS::print_ports_short(ostream_t& o, const Symbol* x) const
{
	assert(x);

	QString comma = "";
	for(unsigned i=0; i<x->numPorts(); ++i){
		trace3("...", x->label(), i, x->numPorts());
		auto p = x->nodePosition(i);
		auto x = p.first;
		auto y = p.second;
		o << comma << "net_" << x << "_" << y;
		comma = ", ";
	}

#if 0
	for (int ii = 0;  x->current_port_exists(ii);  ++ii) {untested();
		o << sep << x->current_port_value(ii);
		sep = ",";
	}
#endif
}
/*--------------------------------------------------------------------------*/
void Verilog::print_ports_short(ostream_t& o, const Symbol* x) const
{
  // print in short form ...   value only
//  o<<"xxx" << x->label() << "xxx";
  assert(x);

  std::string sep = "";
#if 1
  for (int ii = 0;  x->portExists(ii);  ++ii) {
    o << sep << netLabel(x->portValue(ii));
    sep = ", ";
  }
#else
  for(unsigned i=0; i < x->numPorts(); ++i){ untested();
	  assert(x->portExists(i));
	  o << sep << netLabel(x->portValue(i));
	  sep = ", ";
  }
#endif

#if 0
  for (int ii = 0;  x->current_port_exists(ii);  ++ii) {untested();
    o << sep << x->current_port_value(ii);
    sep = ",";
  }
#endif
}
/*--------------------------------------------------------------------------*/
#define short_label label

void Verilog::printSubckt(SubcktBase const* x, ostream_t& o) const
{
	SchematicModel const* scope = nullptr;

	if(x->label()[0] == ':'){ untested();
		o << "// skip sckt " << x->label() << "\n";
	}else if(x->subckt()){
		scope = x->subckt();
	}else if(x->scope()){
		scope = x->scope();
	}else{ untested();
	}

	if(scope){
		assert(x);

		o << "module " <<  x->short_label() << "(";
		print_ports_short(o, x);
		o << ");\n";

		for (auto ci : *scope) {
//			o << "  "; later.
			if(dynamic_cast<Conductor const*>(ci)){
			}else{
				printItem(ci, o);
			}
		}

		o << "endmodule // " << x->short_label() << "\n\n";
	}else{ untested();
		o << "// missing sckt in " << x->label() << "\n";
	}
}
/*!
 * verilog does not know about commands
 */
void Verilog::printtaskElement(TaskElement const* c, ostream_t& s) const
{
  s << "//" << c->label() << "\n";
}
/*--------------------------------------------------------------------------*/
void Verilog::printSymbol(Symbol const* sym, ostream_t& s) const
{
#if 0
	Component const* c=nullptr; // dynamic_cast<Component const*>(sym);
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

		printArgs(sym, s);
#if 0
		QString comma="";
		s << "#(";
			if(!c){ untested();
				incomplete();
			}else{ untested();
				for(auto p2 : c->params()) { untested();
					if(p2->name().at(0) == '$'){ untested();
					}else{ untested();
						s << comma << "." << p2->name() << "(" << p2->Value << ")";
						comma = ", ";
					}
				}
			}
#endif
		s << label << "(";

		print_ports_short(s, sym);
		//// QString comma="";
		//// for(unsigned i=0; i < sym->numPorts(); ++i){ untested();
		//// 	s << comma << netLabel(sym->portValue(i));
		//// 	comma = ", ";
		//// }

		s << ");\n";
	}
}
/*--------------------------------------------------------------------------*/
void VS::printSymbol(Symbol const* sym, ostream_t& s) const
{
	{
		auto label = sym->label();
		auto type = sym->typeName();
		// : is not allowed in verilog
      std::replace( type.begin(), type.end(), ':', '$');
		s << QString::fromStdString(type) << " ";

		printArgs(sym, s);
		if(label == "*"){
			// bug/feature/wtf?
			label="anonymous_gnd_hack_" + std::to_string(gndhackn++);
		}else{
		}
		s << label << "(";
		print_ports_short(s, sym);
		s << ");\n";
	}
}
/*--------------------------------------------------------------------------*/
void VS::printSubckt(SubcktBase const* x, ostream_t& o) const
{
	SchematicModel const* scope = nullptr;
//	if(x->label()[0] == ':'){ untested();
//		unreachable();
//		o << "// skip sckt " << x->label() << "\n";
//		return;
//	}else
	if(x->subckt()){ untested();
		scope = x->subckt();
	}else if(x->scope()){
		scope = x->scope();
	}else{ untested();
	}

	auto it = scope->find_("main");
	if(it == scope->end()){
	}else if(auto xx = dynamic_cast<SubcktBase const*>(*it)){
		o << "// just main scope\n";
		scope = xx->scope();
		// good idea?
	}else{ untested();
	}

	if(scope){
		assert(x);

		o << "module " <<  x->short_label() << "(";
		print_ports_short(o, x);
		o << ");\n";

		for (auto ci : *scope){
			if(dynamic_cast<Conductor const*>(ci)){
				// TODO: defer
				printItem(ci, o);
			}else if(ci->label()[0] == ':'){ untested();
			}else{
				// o << "  ";
				printItem(ci, o);
			}
		}

//		if(auto m=dynamic_cast<SchematicModel const*>(x)){ untested();
      if(scope){
			for(auto const& n : scope->nodes()){
				int a, b;
				std::tie(a, b) = n.position();
				o << "place #(.$xposition(" << a << "),"
					".$yposition(" << b << "))"
					<<  " place_" << a << "_" << b
					<< "(net_" << a << "_" << b << ");\n";
			}
		}
//		}

		o << "endmodule // " << x->short_label() << "\n\n";
	}else{ untested();
		o << "// missing sckt in " << x->label() << "\n";
	}
}
/*--------------------------------------------------------------------------*/
class VerilogSchematicFormat : public DocumentFormat{
//  void save(ostream_t& stream, Object const*) const;
  void load(istream_t& stream, Object*) const;

private: //command
  void do_it(istream_t&, SchematicModel*) override;

private: // legacy cruft
  bool isSymbolMode() const{ return false; }
//  PaintingList const& symbolPaints(SchematicSymbol const& m) const{ untested();
//    assert(m.symbolPaintings());
//    return *m.symbolPaintings();
//  }
#if 0
  DiagramList const& diagrams(SchematicSymbol const& m) const{ untested();
    return m.diagrams();
  }
  PaintingList const& paintings(SchematicSymbol const& m) const{ untested();
    return m.paintings();
  }
  NodeMap const& nodes(SchematicSymbol const& m) const{ untested();
    return m.nodes();
  }
  ElementList const& components(SchematicSymbol const& m) const{ untested();
    return m.components();
  }
#endif

private: // hacks.
  void printSymbol(Symbol const*, ostream_t&) const;
}VS;
static Dispatcher<Command>::INSTALL pp(&command_dispatcher, "v_sch", &VS);
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
void VerilogSchematicFormat::load(istream_t&, Object*) const
{ untested();
  incomplete();
}
/* -------------------------------------------------------------------------------- */
void VerilogSchematicFormat::do_it(istream_t& cs, SchematicModel* o)
{
	std::string fn;
	cs >> fn;

	QFile NetlistFile(QString::fromStdString(fn));
	if(!NetlistFile.open(QIODevice::WriteOnly | QFile::Truncate)) { untested();
		message(QucsMsgFatal, "Cannot open "+fn+" for writing\n");
		return; // throw?
	}else{
	}
	ostream_t stream(&NetlistFile);

	for(auto pc : *o){
		if(dynamic_cast<TaskElement const*>(pc)){ untested();
			unreachable();
			// BUG. a TaskElement is not a Component
			continue;
		}else if(pc->label()[0] == ':'){
			stream << "// skip sckt " << pc->label() << "\n";
		}else if(auto s=dynamic_cast<Symbol const*>(pc)){
			V_.printItem(s, stream);
		}
	}
}
/* -------------------------------------------------------------------------------- */
// similar to Verilog::printSymbol, but with the actual node names and
// coordinates.
#if 0// // obsolete?
void VerilogSchematicFormat::printSymbol(Symbol const* sym, ostream_t& s) const
{ untested();
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
	
	{ untested();
		// wrong. somehow flatten composite types.
		// Lib:Z-Diodes:1N4732A
		// (use common->modelname or such things?)
		std::string type = sym->typeName();
      std::replace( type.begin(), type.end(), ':', '$');
		std::string label = sym->label();

		s << type << " ";

		if(label == "*"){ untested();
			// bug/feature/wtf?
			label="anonymous_gnd_hack_" + std::to_string(gndhackn++);
		}else{ untested();
		}

		printArgs(sym, s);
		s << label << "(";

		QString comma = "";
		for(unsigned i=0; i<sym->numPorts(); ++i){ untested();
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
#endif
/* -------------------------------------------------------------------------------- */
} // namespace
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
