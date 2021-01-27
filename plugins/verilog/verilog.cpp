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
#include "qucs_globals.h"
#include "docfmt.h"
#include "task_element.h"
#include "schematic_lang.h"
#include "schematic_model.h"
#include "net.h"
#include "nodemap.h" // BUG
#include "sckt_base.h"
#include "dot.h"
#include "place.h"
#include "qio.h"
#include <QFile> // BUG

unsigned gndhackn=0;

class PaintingList;
class DiagramList;
class WireList;
class ComponentList;

namespace {
/* -------------------------------------------------------------------------------- */
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
/* -------------------------------------------------------------------------------- */
class Verilog : public DocumentLanguage {
	void printTaskElement(TaskElement const*, ostream_t&) const override;
	void printSymbol(Symbol const*, ostream_t&) const override;
	void printSubckt(SubcktBase const*, ostream_t&) const override;
	void printPainting(Painting const*, ostream_t&) const override;
	void print_command(ostream_t&, DEV_DOT const*) const override;
	void printDiagram(Symbol const*, ostream_t&) const override {incomplete();}

private:
	void print_ports_short(ostream_t& o, const Symbol* x) const;

private: //DocumentLanguage
	std::string findType(istream_t&) const override {incomplete(); return "incomplete";}
} V;
static Dispatcher<DocumentLanguage>::INSTALL p0(&languageDispatcher, "verilog|verilog_nl", &V);
/*--------------------------------------------------------------------------*/
class VS : public Verilog {
	void printSymbol(Symbol const*, ostream_t&) const override;
	void printSubckt(SubcktBase const* x, ostream_t& o) const override;
private:
	void print_ports_short(ostream_t& o, const Symbol* x) const;
	void print_args(ostream_t&, Symbol const* sym) const;
} V_;
static Dispatcher<DocumentLanguage>::INSTALL p1(&languageDispatcher, "verilog_schematic", &V_);
/*--------------------------------------------------------------------------*/
void VS::print_args(ostream_t& s, Symbol const* sym) const
{
	std::string comma="";
	s << "#(";

	for(unsigned i=0; i<sym->paramCount(); ++i) {
		auto name = sym->paramName(i);
		s << comma << "." << sym->paramName(i) << "(" << sym->paramValue(i) << ")";
		comma = ", ";
	}
	s << ") ";
}
/*--------------------------------------------------------------------------*/
static void print_args(Symbol const* sym, ostream_t& s)
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
//		auto p = x->nodePosition(i);
		auto v = x->port_value(i);
//		auto x = p.first;
//		auto y = p.second;
		/// o << comma << "net_" << x << "_" << y;
		o << comma << v; // "net_" << x << "_" << y;
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
void Verilog::print_command(ostream_t& o, DEV_DOT const* d) const
{
	o << "// " + d->s() << "\n";
}
/*--------------------------------------------------------------------------*/
void Verilog::printPainting(Painting const*, ostream_t& o) const
{
	o << "painting incomplete\n";
}
/*--------------------------------------------------------------------------*/
void Verilog::print_ports_short(ostream_t& o, const Symbol* x) const
{
  // print in short form ...   value only
//  o<<"xxx" << x->label() << "xxx";
  assert(x);

  std::string sep = "";
  auto s = x->scope();
  assert(s);
  auto n = s->nodes();
  assert(n);
  for (int ii = 0;  x->portExists(ii);  ++ii) {
	  std::string p = x->port_value(ii);

	 if(1){
		 o << sep << n->netName(p); // BUG. use owner.
	 }else{
		 o << sep << p;
	 }
    sep = ", ";
  }

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

#if 0 // no. too late.
	if(x->label()[0] == ':'){ untested();
		o << "// skip sckt " << x->label() << "\n";
	}else
#endif
	Element const* e = x;

	// BUG?
	if(x->subckt()){
		scope = x->subckt();
	}else if(e->scope()){
		scope = e->scope();
	}else{ untested();
	}

	if(scope){
		assert(x);

		o << "module " <<  x->short_label() << "(";
		print_ports_short(o, x);
		o << ");\n";

		for (auto ci : *scope) { itested();
//			o << "  "; later.
			if(dynamic_cast<Conductor const*>(ci)){
			}else if(dynamic_cast<Place const*>(ci)){
			}else{ itested();
				printItem(o, ci);
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
void Verilog::printTaskElement(TaskElement const* c, ostream_t& s) const
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

		print_args(sym, s);
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

		print_args(s, sym);
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
//		return;
//	}else
	o << "// VS::printSubckt " << x->label() << "\n";
	Element const* e = x;

	// makes_own_scope?
	if(x->subckt()){ untested();
		scope = x->subckt();
	}else if(e->scope()){
		scope = e->scope();
	}else{ untested();
	}

	auto it = scope->find_("main");
	if(it == scope->end()){
	}else if(dynamic_cast<SubcktBase const*>(*it)){
		o << "// just main scope\n";
		scope = (*it)->scope();
		// good idea?
	}else{ untested();
	}

	std::vector<Element*> places;
	if(scope){
		assert(x);

		o << "module " <<  x->short_label() << "(";
		print_ports_short(o, x);
		o << ");\n";

		for (auto ci : *scope){
			if(dynamic_cast<Place const*>(ci)){
				places.push_back(ci);
			}else if(dynamic_cast<Conductor const*>(ci)){
				// TODO: defer
				printItem(o, ci);
			}else if(ci->label()[0] == ':'){ untested();
				// ?
				printItem(o, ci);
			}else{
				// o << "  ";
				printItem(o, ci);
			}
		}

//		if(auto m=dynamic_cast<SchematicModel const*>(x)){ untested();
#if 1
		for(auto ci : places){
				printItem(o, ci);
		}

#else
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
#endif
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
static Dispatcher<Command>::INSTALL pp(&commandDispatcher, "v_sch", &VS);
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
void VerilogSchematicFormat::load(istream_t&, Object*) const
{ untested();
  incomplete();
}
/* -------------------------------------------------------------------------------- */
void VerilogSchematicFormat::do_it(istream_t& cs, SchematicModel* scope)
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

	auto it = scope->find_("main");
	if(it == scope->end()){
	}else if(auto main = dynamic_cast<SubcktBase*>(*it)){
		// scope = xx->scope();
		V_.printItem(stream, main);
	}else{ untested();
	}

#if 0
	for(auto pc : *o){
		if(dynamic_cast<TaskElement const*>(pc)){ untested();
			unreachable();
			// BUG. a TaskElement is not a Component
			continue;
		}else if(pc->label()[0] == ':'){
			stream << "// skip sckt " << pc->label() << "\n";
		}else if(auto s=dynamic_cast<Symbol const*>(pc)){
			V_.printItem(stream, s);
		}
	}
#endif
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

		print_args(sym, s);
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
