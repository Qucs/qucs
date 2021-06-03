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

#include "diagram.h"
#include "docfmt.h"
#include "dot.h"
#include "element_list.h"
#include "model.h"
#include "net.h"
#include "node.h"
#include "nodemap.h" // BUG
#include "place.h"
#include "qio.h"
#include "qucs_globals.h"
#include "schematic_lang.h"
#include "sckt_base.h"
#include "task_element.h"
/* -------------------------------------------------------------------------------- */
unsigned gndhackn = 0;
/* -------------------------------------------------------------------------------- */
namespace qucs {
	class Conductor;
	class Painting;
	class Diagram;
	class TaskElement;
}
/* -------------------------------------------------------------------------------- */
namespace {
/* -------------------------------------------------------------------------------- */
using qucs::Component;
using qucs::Conductor;
using qucs::Diagram;
using qucs::Element;
using qucs::Language;
using qucs::Model;
using qucs::Net;
using qucs::Node;
using qucs::Painting;
using qucs::SubcktBase;
using qucs::Symbol;
using qucs::TaskElement;
using qucs::command_dispatcher;
/* -------------------------------------------------------------------------------- */
#if 0
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
#endif
/* -------------------------------------------------------------------------------- */
class Verilog : public Language {
protected: // in
   DEV_DOT* parseCommand(istream_t&, DEV_DOT*) const override;

public:
	SubcktBase* parse_module(istream_t& cmd, SubcktBase* x) const;
	SubcktBase* parse_model(istream_t&, SubcktBase*) const override;
	Model* parse_paramset(istream_t&, Model*) const; //  override;
	Symbol* parseSymbol(istream_t& cs, Symbol* sym) const;

private:
	void printElement(Element const*, ostream_t&) const override;
	void print_instance(ostream_t&, Component const*) const override;
	void print_paramset(ostream_t&, Model const*) const override;
	void print_module(ostream_t&, SubcktBase const*) const override;
	void printPainting(Painting const*, ostream_t&) const override;
	void print_command(ostream_t&, DEV_DOT const*) const override;
	void print_diagram(ostream_t&, Diagram const*) const override;

private: // local
	void printTaskElement(TaskElement const*, ostream_t&) const;
	void print_ports_short(ostream_t& o, Component const* x) const;
	void print_args(ostream_t&, Component const* sym) const;
	void print_args(ostream_t&, Model const* sym) const;

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
	void print_instance(ostream_t&, Component const*) const override;
	void print_module(ostream_t&, SubcktBase const*) const override;

	void print_ports_short(ostream_t& o, const Component* x) const;
	void print_args(ostream_t&, Component const* sym) const;
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
		// assert(here == 0); why?
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
  x->set_dev_type(new_type);
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
static void parse_ports(istream_t& cmd, Component* x, bool all_new)
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
					if (all_new) {
						//    if (x->node_is_grounded(index)) { untested();
						//      cmd.warn(bDANGER, here, "node 0 not allowed here");
						//    }else
						if (x->subckt() && int(x->subckt()->nodes()->size()) != index+1) { untested();
							cmd.warn(bDANGER, here, "duplicate port name, skipping");
						}else{
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
		incomplete();
		cmd.warn(bDANGER, e.message());
		return nullptr;
	}
}
/*--------------------------------------------------------------------------*/
static void parse_args_paramset(istream_t& cmd, Model* x)
{
	assert(x);

	while (cmd >> '.') {
		size_t here = cmd.cursor();
		std::string name, value;
		try{
			cmd >> name >> '=' >> value >> ';';
			x->set_param_by_name(name, value);
		}catch (qucs::ExceptionNoMatch&) {untested();
			cmd.warn(bDANGER, here, x->long_label() + ": bad parameter " + name + " ignored");
		}
	}
}
/*--------------------------------------------------------------------------*/
/* "paramset" <my_name> <base_name> ";"
 *    <paramset_item_declaration>*
 *    <paramset_statement>*
 *  "endparamset"
 */
//BUG// no paramset_item_declaration, falls back to spice mode

Model* Verilog::parse_paramset(istream_t& cmd, Model* x) const
{
  assert(x);
  cmd.reset();
  cmd >> "paramset ";
  parse_label(cmd, x);
  parse_type(cmd, x);
  cmd >> ';';

  for (;;) {
    parse_args_paramset(cmd, x);
    if (cmd >> "endparamset ") {
      break;
    }else if (!cmd.more()) {
      cmd.get_line("verilog-paramset>");
    }else{untested();
      cmd.check(bWARNING, "what's this?");
      break;
    }
  }
  return x;
}
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
void VS::print_args(ostream_t& s, Component const* sym) const
{
	std::string comma="";
	s << "#(";

	for(index_t i=sym->param_count(); i--;) {
		auto name = sym->param_name(i);
		if (name.size() == 0){
			unreachable();
		}else if (sym->param_is_printable(i)) {
			s << comma << "." << sym->param_name(i) << "(" << sym->param_value(i) << ")";
			comma = ", ";
		}else{
		}
	}
	s << ") ";
}
/*--------------------------------------------------------------------------*/
// same as VS::, but skip '$.*'
void Verilog::print_args(ostream_t& s, Component const* x) const
{
	std::string comma="";
	s << "#(";

	for(index_t i=x->param_count(); i--;) {
		auto name = x->param_name(i);
		if(name.size() ==0){
			unreachable();
		}else if (name.at(0) == '$'){
		}else if (x->param_is_printable(i)) {
			s << comma << "." << x->param_name(i) << "(" << x->param_value(i) << ")";
			comma = ", ";
		}else{
		}
	}
	s << ") ";
}
/*--------------------------------------------------------------------------*/
void Verilog::print_args(ostream_t& o, const Model* x) const
{
	assert(x);
	for (int ii = x->param_count();  ii--;) {
		if (x->param_is_printable(ii)) {
			std::string arg = " ." + x->param_name(ii) + "=" + x->param_value(ii) + ";\n";
			o << arg;
		}else{
		}
	}
}
/*--------------------------------------------------------------------------*/
void VS::print_ports_short(ostream_t& o, Component const* x) const
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
void Verilog::print_diagram(ostream_t& o, Diagram const* d) const
{
	o << "// `ifdef QUCS_DIAGRAMS\n"
	  << "// " << d->dev_type() << "\n"
	  << "// " << d->short_label() << "\n"
	  << "// `endif\n";
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
void Verilog::print_ports_short(ostream_t& o, Component const* x) const
{
  // print in short form ...   value only
//  o<<"xxx" << x->label() << "xxx";
  assert(x);

  std::string sep = "";
  auto s = x->scope();
  if(!s){
	  unreachable();
	  return;
  }else{
  }
  auto n = s->nodes();
  assert(n);
  for (int ii = 0;  x->port_exists(ii);  ++ii) {
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
void Verilog::print_module(ostream_t& o, SubcktBase const* x) const
{
	ElementList const* scope = nullptr;

	if(x->makes_own_scope()){
		scope = x->scope();
	}else{ untested();
		scope = x->subckt();
	}

	if(scope){
		auto label = x->short_label();
		auto v = scope->find_(label);
	  	if(v == scope->end()){
			// BUG check type and if valid.
			assert(x);
			std::replace( label.begin(), label.end(), ':', '$');

			o << "module " << label << "(";
			print_ports_short(o, x);
			o << ");\n";

			for (auto ci : *scope) {
	//			o << "  "; later.
				if(dynamic_cast<Conductor const*>(ci)){
				}else if(dynamic_cast<Place const*>(ci)){
				}else{
					printItem(o, ci);
				}
			}

			o << "endmodule // " << x->short_label() << "\n\n";
		}else if(auto m=dynamic_cast<Model const*>(*v)){
			print_paramset(o, m);
		}
	}else{
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
void Verilog::print_paramset(ostream_t& o, Model const* x) const
{
	incomplete();
	assert(x);
  // _mode = mPARAMSET;
	std::string label = x->short_label();
	std::replace( label.begin(), label.end(), ':', '$');
	o << "paramset " << label << ' ' << x->dev_type() << ";\n";
	print_args(o, x);
   o << "endparmset //"<< label <<"\n\n";
  // _mode = mDEFAULT;
}
/*--------------------------------------------------------------------------*/
void Verilog::print_instance(ostream_t& s, Component const* sym) const
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
		auto type = sym->dev_type();
		if(sym->common()){
			type = sym->common()->modelname(); // "netlist mode"
		}else{
		}
		// : is not allowed in verilog
      std::replace( type.begin(), type.end(), ':', '$');
		s << QString::fromStdString(type) << " ";

		print_args(s, sym);
		s << label << "(";
		print_ports_short(s, sym);
		s << ");\n";
	}
}
/*--------------------------------------------------------------------------*/
void VS::print_instance(ostream_t& s, Component const* sym) const
{
	{
		auto label = sym->label();
		auto type = sym->dev_type();
		// : is not allowed in verilog
 //     std::replace( type.begin(), type.end(), ':', '$');
		s << QString::fromStdString(type) << " ";

		print_args(s, sym);
		if(label == "*"){
			// bug/feature/wtf?
			label="anonymous_gnd_hack_" + std::to_string(gndhackn++);
		}else{
//			std::replace( label.begin(), label.end(), ':', '$');
		}
		s << label << "(";
		print_ports_short(s, sym);
		s << ");\n";
	}
}
/*--------------------------------------------------------------------------*/
void VS::print_module(ostream_t& o, SubcktBase const* x) const
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
//		(*it)->prepare();
		o << "// just main scope\n";
		scope = (*it)->scope();
		// good idea?
	}else{ untested();
	}

	std::vector<Element const*> places;
//	Element const* main_;
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
			}else if(ci->label() == "main"){ untested();
				assert(ci->scope());
				ci->scope()->prepare();
				// main_ = ci;
				printItem(o, ci);
			}else if(ci->label()[0] == ':'){ untested();
				// ?
				//printItem(o, ci);
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
	trace2("Verilog::parse_module", cmd.fullstring(), x->numPorts());
	cmd >> ';';

	// body
	for (;;) {
		cmd.get_line("verilog-module>");
		trace2("verilog body", cmd.fullstring(), cmd.cursor());

		if (cmd >> "endmodule ") {
			break;
		}else if (cmd >> "parameter |param ") {
			cmd >> "real";
			std::string name;
			cmd >> name;
			DEV_DOT* d = new DEV_DOT();
			d->set(cmd.fullstring());
			x->scope()->push_back(d);
		}else{
			trace1("new_instance", cmd.fullstring());
			new__instance(cmd, x, x->scope());
		}
	}
	return x;
}
/*--------------------------------------------------------------------------*/
class CMD_MODULE : public Command {
	void do_it(istream_t& cmd, ElementList* Scope) override {
		// auto new_module = dynamic_cast<SubcktBase*>(device_dispatcher.clone("subckt"));
		Component* dd = qucs::device_dispatcher.clone("subckt_proto");
		auto new_module = dynamic_cast<SubcktBase*>(dd);
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
Dispatcher<Command>::INSTALL d2(&command_dispatcher, "module|macromodule", &p2);
/*--------------------------------------------------------------------------*/
class CMD_VERILOG : public Command {
public:
  void do_it(CS&, CARD_LIST* Scope)
  {
    command("options lang=verilog", Scope);
  }
} p8;
DISPATCHER<Command>::INSTALL d8(&command_dispatcher, "verilog", &p8);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class CMD_PARAMSET : public Command {
  void do_it(CS& cmd, ElementList* Scope) {
	  // already got "paramset"
	  std::string my_name, base_name;
	  cmd >> my_name;
	  size_t here = cmd.cursor();
	  cmd >> base_name;

	  //const MODEL_CARD* p = model_dispatcher[base_name];
	  const Element* p = lang_verilog.find_proto(base_name, NULL);
	  if (p) {
		  Element* cl = p->clone();
		  auto new_card = dynamic_cast<Model*>(cl);
		  if (new_card) {
			  assert(!new_card->owner());
			  lang_verilog.parse_paramset(cmd, new_card);
			  Scope->push_back(new_card);
		  }else{
			  delete(cl);
			  cmd.warn(bDANGER, here, "paramset: base has incorrect type");
		  }
	  }else{untested();
		  cmd.warn(bDANGER, here, "paramset: no match");
	  }
  }
}p3;
Dispatcher<Command>::INSTALL d3(&command_dispatcher, "paramset", &p3);
/*--------------------------------------------------------------------------*/
} // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
