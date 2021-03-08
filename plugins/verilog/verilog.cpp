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

// verilog languages with bits from lang_verilog.cc

#include "node.h"
#include "qucs_globals.h"
#include "docfmt.h"
#include "task_element.h"
#include "schematic_lang.h"
#include "element_list.h"
#include "net.h"
#include "nodemap.h" // BUG
#include "sckt_base.h"
#include "dot.h"
#include "place.h"
#include "qio.h"
#include <QFile> // BUG

unsigned gndhackn=0;

class DiagramList;
class WireList;
class ComponentList;

namespace qucs{
	class Conductor;
	class Painting;
	class Diagram;
	class TaskElement;
}

namespace {
/* -------------------------------------------------------------------------------- */
using qucs::Conductor;
using qucs::Net;
using qucs::Node;
using qucs::Element;
using qucs::Language;
using qucs::Painting;
using qucs::TaskElement;
using qucs::Diagram;
using qucs::Symbol;
/* -------------------------------------------------------------------------------- */
static std::string netLabel(Node const* nn)
{ untested();
	if(!nn){ untested();
		return "(null)";
	}else{ untested();
	}
	Net const* n = nn->net();
	if(!n){ untested();
		unreachable();
		return "(null)";
	}else if(n->hasLabel()){ untested();
		return n->short_label();
	}else{ untested();
		return "_net" + std::to_string(n->pos());
	}
}
/* -------------------------------------------------------------------------------- */
class Verilog : public Language {
protected: // in
   DEV_DOT* parseCommand(istream_t&, DEV_DOT*) const override;

public:
	SubcktBase* parse_module(istream_t& cmd, SubcktBase* x) const;
	SubcktBase* parse_model(istream_t&, SubcktBase*) const override;
	Symbol* parseSymbol(istream_t& cs, Symbol* sym) const;

private:
	void printElement(Element const*, ostream_t&) const override;
	void printSymbol(Symbol const*, ostream_t&) const override;
	void printSubckt(SubcktBase const*, ostream_t&) const override;
	void printPainting(Painting const*, ostream_t&) const override;
	void print_command(ostream_t&, DEV_DOT const*) const override;
	void printDiagram(Diagram const*, ostream_t&) const override {incomplete();}

private: // local
	void printTaskElement(TaskElement const*, ostream_t&) const;
	void print_ports_short(ostream_t& o, const Symbol* x) const;

private: //DocumentLanguage
	std::string find_type_in_string(istream_t&) const override;
	Element* parseItem(istream_t&, Element*) const override;
}lang_verilog;
static Dispatcher<Language>::INSTALL p0(&qucs::language_dispatcher,
                                                "verilog|verilog_nl", &lang_verilog);
/* -------------------------------------------------------------------------------- */
class VS : public Verilog {
public:
	explicit VS() : Verilog(){
		set_label("verilog_schematic");
	}

private:
//	std::string find_type_in_string(istream_t&) const override;
//	Element* parseItem(istream_t&, Element*) const override;
//	SubcktBase* parse_model(istream_t&, SubcktBase*) const override;
//	Symbol* parseSymbol(istream_t& cs, Symbol* sym) const;

private:
	void printSymbol(Symbol const*, ostream_t&) const override;
	void printSubckt(SubcktBase const* x, ostream_t& o) const override;

	void print_ports_short(ostream_t& o, const Symbol* x) const;
	void print_args(ostream_t&, Symbol const* sym) const;
} V_;
static Dispatcher<Language>::INSTALL p1(&qucs::language_dispatcher, "verilog_schematic|.vs", &V_);
/*--------------------------------------------------------------------------*/
DEV_DOT* Verilog::parseCommand(istream_t& cmd, DEV_DOT* x) const
{
	assert(x);
	x->set(cmd.fullstring());
	ElementList* scope = x->scope();

	if(scope){
	}else if(!x->owner()){ untested();
	}else if(auto s=dynamic_cast<SubcktBase*>(x->owner())){
		scope = s->subckt();
	}else{
		incomplete();
		// : &ElementList::card_list;
	}

	cmd.reset();
	Command::cmdproc(cmd, scope);
	delete x;
	return NULL;
}
/*--------------------------------------------------------------------------*/
std::string Verilog::find_type_in_string(istream_t& cmd) const
{
	size_t here = cmd.cursor();
	std::string type;
	if ((cmd >> "//")) {
		assert(here == 0);
		type = "dev_comment";
	}else{
		cmd >> type;
	}
	cmd.reset(here);
	trace1("verilog ftis", type);
	return type;
}
/*--------------------------------------------------------------------------*/
static void parse_type(istream_t& cmd, Element* x)
{
  assert(x);
  std::string new_type;
  cmd >> new_type;
  // x->set_dev_type(new_type); TODO
}
/*--------------------------------------------------------------------------*/
static void parse_args_instance(istream_t& cmd, Element* x)
{
	assert(x);

	if (cmd >> "#(") {
		if (cmd.match1('.')) {
			// by name
			while (cmd >> '.') {
				size_t here = cmd.cursor();
				std::string name  = cmd.ctos("(", "", "");
				std::string value = cmd.ctos(",)", "(", ")");
				cmd >> ',';
				try{
					trace2("pai", name, value);
					x->set_param_by_name(name, value);
				}catch (qucs::ExceptionNoMatch&) {untested();
					cmd.warn(bDANGER, here, x->label() + ": bad parameter " + name + " ignored");
				}
			}
		}else{
			// by order
			int index = 1;
			while (cmd.is_alnum() || cmd.match1("+-.")) { untested();
				size_t here = cmd.cursor();
				try{ untested();
					std::string value = cmd.ctos(",)", "", "");
					trace2("pai", index, value);
					x->set_param_by_index(x->param_count() - index++, value); // , 0/*offset*/);
				}catch (qucs::Exception_Too_Many& e) {untested();
					cmd.warn(bDANGER, here, e.message());
				}
			}
		}
		cmd >> ')';
	}else{ untested();
		// no args
	}
}
/*--------------------------------------------------------------------------*/
static void parse_label(istream_t& cmd, Element* x)
{
	trace1("parse_label", cmd.fullstring());
	assert(x);
	std::string my_name;
	if (cmd >> my_name) {
		x->set_label(my_name);
	}else{ untested();
		//x->set_label(x->id_letter() + std::string("_unnamed")); //BUG// not unique
		x->set_label(std::string("_unnamed")); //BUG// not unique
		cmd.warn(bDANGER, "label required");
	}
}
/*--------------------------------------------------------------------------*/
static void parse_ports(istream_t& cmd, Symbol* x, bool all_new)
{
	assert(x);
	trace4("parse_ports", cmd.fullstring(), x->label(), x->scope(), x->owner());

	if (cmd >> '(') {
		if (cmd.is_alnum()) {
			// by order
			int index = 0;
			while (cmd.is_alnum()) {
				size_t here = cmd.cursor();
				try{
					std::string value;
					cmd >> value;
					x->set_port_by_index(index, value);
					if (all_new) { untested();
						//    if (x->node_is_grounded(index)) { untested();
						//      cmd.warn(bDANGER, here, "node 0 not allowed here");
						//    }else
						if (x->subckt() && x->subckt()->nodes()->size() != index+1) { untested();
							cmd.warn(bDANGER, here, "duplicate port name, skipping");
						}else{ untested();
							++index;
						}
					}else{
						++index;
					}
				}catch (qucs::Exception_Too_Many& e) { untested();
					cmd.warn(bDANGER, here, e.message());
				}
			}
//			if (index < x->min_nodes()) { untested();
//				cmd.warn(bDANGER, "need " + to_string(x->min_nodes()-index) +" more nodes, grounding");
//				for (int iii = index;  iii < x->min_nodes();  ++iii) { untested();
//					x->set_port_to_ground(iii);
//				}
//			}else{ untested();
//			}
		}else{
			// by name
			while (cmd >> '.') { untested();
				size_t here = cmd.cursor();
				try{ untested();
					std::string name, value;
					cmd >> name >> '(' >> value >> ')' >> ',';
					x->set_port_by_name(name, value);
				}catch (qucs::ExceptionNoMatch const&) {untested();
					cmd.warn(bDANGER, here, "mismatch, ignored");
				}
			}
//			for (int iii = 0;  iii < x->min_nodes();  ++iii) { untested();
//				if (!(x->node_is_connected(iii))) {untested();
//					cmd.warn(bDANGER, x->port_name(iii) + ": port unconnected, grounding");
//					x->set_port_to_ground(iii);
//				}else{ untested();
//				}
//			}
		}
		cmd >> ')';
	}else{ untested();
		cmd.warn(bDANGER, "'(' required (parse ports) (grounding)");
		incomplete();
//		for (int iii = 0;  iii < x->min_nodes();  ++iii) { untested();
//			if (!(x->node_is_connected(iii))) { untested();
//				cmd.warn(bDANGER, x->port_name(iii) + ": port unconnected, grounding");
//				x->set_port_to_ground(iii);
//			}else{ untested();
//				unreachable();
//			}
//		}
	}
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
Symbol* Verilog::parseSymbol(istream_t& cmd, Symbol* x) const
{
	trace1("VS::parseSymbol", cmd.fullstring());
	// parse_instance...
	try{
		assert(x);
		cmd.reset();
		parse_type(cmd, x);
		parse_args_instance(cmd, x);
		parse_label(cmd, x);
		parse_ports(cmd, x, false/*allow dups*/);
		cmd >> ';';
		cmd.check(bWARNING, "what's this?");
		return x;
	}catch (qucs::Exception& e) {
		trace1("parseSymbol", e.message());
		incomplete();
		cmd.warn(bDANGER, e.message());
		return nullptr;
	}
}
/*--------------------------------------------------------------------------*/
SubcktBase* Verilog::parse_model(istream_t&, SubcktBase*) const
{ untested();
	incomplete();
	return nullptr;
}
/*--------------------------------------------------------------------------*/
Element* Verilog::parseItem(istream_t& cmd, Element* e) const
{
	assert(e);
	if(auto s=dynamic_cast<Symbol*>(e)){
		return parseSymbol(cmd, s);
	}else if(auto s=dynamic_cast<DEV_DOT*>(e)){
		return parseCommand(cmd, s);
	}else{ untested();
		incomplete();
		return nullptr;
	}
}
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
		auto v = x->port_value(i);
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
	 }else{ untested();
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
	ElementList const* scope = nullptr;

	scope = x->scope();

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
/*--------------------------------------------------------------------------*/
void Verilog::printElement(Element const* c, ostream_t& s) const
{
	if(auto t=dynamic_cast<TaskElement const*>(c)){
		printTaskElement(t, s);
	}else{
		incomplete();
	}
}
/*--------------------------------------------------------------------------*/
// verilog does not know about commands
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
		if(sym->common()){
			type = sym->common()->modelname(); // "netlist mode"
		}else{
		}
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
		auto type = sym->typeName(); // dev_type_key
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
	ElementList const* scope = nullptr;
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
	}else if(dynamic_cast<SubcktBase const*>(*it)){ untested();
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

//		if(auto m=dynamic_cast<ElementList const*>(x)){ untested();
		for(auto ci : places){
				printItem(o, ci);
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
  void do_it(istream_t&, ElementList*) override;

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
static Dispatcher<Command>::INSTALL pp(&qucs::command_dispatcher, "v_sch", &VS);
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
void VerilogSchematicFormat::load(istream_t&, Object*) const
{ untested();
  incomplete();
}
/* -------------------------------------------------------------------------------- */
void VerilogSchematicFormat::do_it(istream_t& cs, ElementList* scope)
{
	std::string fn;
	cs >> fn;
	ostream_t stream(fn);

	auto it = scope->find_("main");
	if(it == scope->end()){ untested();
	}else if(auto main = dynamic_cast<SubcktBase*>(*it)){
		// scope = xx->scope();
		V_.printItem(stream, main);
	}else{ untested();
	}

#if 0
	for(auto pc : *o){ untested();
		if(dynamic_cast<TaskElement const*>(pc)){ untested();
			unreachable();
			// BUG. a TaskElement is not a Component
			continue;
		}else if(pc->label()[0] == ':'){ untested();
			stream << "// skip sckt " << pc->label() << "\n";
		}else if(auto s=dynamic_cast<Symbol const*>(pc)){ untested();
			V_.printItem(stream, s);
		}
	}
#endif
}
/* -------------------------------------------------------------------------------- */
/* "module" <name> "(" <ports> ")" ";"
 *    <declarations>
 *    <netlist>
 * "endmodule"
 */
//BUG// strictly one device per line

SubcktBase* Verilog::parse_module(istream_t& cmd, SubcktBase* x) const
{
	assert(x);

	// header
	cmd.reset();
	(cmd >> "module |macromodule ");
	parse_label(cmd, x);
	parse_ports(cmd, x, true/*all new*/);
	cmd >> ';';

	// body
	for (;;) {
		cmd.get_line("verilog-module>");

		if (cmd >> "endmodule ") {
			break;
		}else{
			trace1("new_instance", cmd.fullstring());
			new__instance(cmd, x, x->scope());
		}
	}
	return x;
}
/*--------------------------------------------------------------------------*/
class CMD_MODULE : public Command {
	void do_it(istream_t& cmd, ElementList* Scope) override
	{
		// auto new_module = dynamic_cast<SubcktBase*>(device_dispatcher.clone("subckt"));
		auto new_module = dynamic_cast<SubcktBase*>(qucs::symbol_dispatcher.clone("subckt_proto"));
		assert(new_module);
		assert(!new_module->owner());

		{ // a hack...
			cmd.reset();
			(cmd >> "module |macromodule ");
			parse_label(cmd, new_module);

			trace2("new_module", new_module->label(), new_module);
			auto f = Scope->find_(new_module->label());
			if(f == Scope->end()){
			}else{
				new_module->set_owner((*f)->owner());
				Scope->erase(f);
			}
		}

		assert(new_module->scope());
		assert(new_module->scope()->is_empty());
		assert(!new_module->is_device());
		lang_verilog.parse_module(cmd, new_module);
		Scope->push_back(new_module);
	}
} p2;
Dispatcher<Command>::INSTALL d2(&qucs::command_dispatcher, "module|macromodule", &p2);
/*--------------------------------------------------------------------------*/
} // namespace
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
