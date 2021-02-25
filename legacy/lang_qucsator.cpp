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

// qucsator "language".

#include "sckt_base.h"
#include "net.h"
#include "qio.h"
#include "docfmt.h" // <<
#include "qucs_globals.h"
#include "settings.h" //??
//#include "schematic_doc.h"
#include "place.h"
#include "nodemap.h"
//#include "dat.h"
#include "painting.h"
#include "legacy_task_element.h"
#include "components/component.h" // yikes
#include "lang_dat_hack.cpp"
#include "language.cpp"
/* -------------------------------------------------------------------------------- */
namespace {
/* -------------------------------------------------------------------------------- */
static const std::string typesep(":");
static const char _typesep = ':';

// not sure what this is about
/// #ifdef __MINGW32__ // -> platform.h
/// #include <windows.h>
/// static QString pathName(QString longpath) { untested();
///   const char * lpath = QDir::toNativeSeparators(longpath).toAscii();
///   char spath[2048];
///   int len = GetShortPathNameA(lpath,spath,sizeof(spath)-1);
///   spath[len] = '\0';
///   return QString(spath);
/// }
/// #else
/// static QString pathName(QString longpath) { untested();
///   return longpath;
/// }
/// #endif
/* -------------------------------------------------------------------------------- */
// temporary kludge. why Symbol?
class QucsatorScktHack : public Symbol {
private:
	QucsatorScktHack(QucsatorScktHack const&) = default;
public:
	QucsatorScktHack() : Symbol() {}
private:
	Element* clone() const override{return new QucsatorScktHack(*this);}
private: // Symbol
	pos_t portPosition(unsigned) const {unreachable(); return pos_t(0,0);}
	unsigned numPorts() const  override{ return 0;}
	Port& port(unsigned) override{unreachable(); return *new Port();}
	void set_param_by_name(std::string const& name, std::string const& value) override{
		if(name == "qucsatorsckthack"){
			_text = value;
		}else{ untested();
			Symbol::set_param_by_name(name, value);
		}
	}
	std::string paramValue(std::string const& name) const override{
		if(name == "qucsatorsckthack"){
			return _text;
		}else{ untested();
			return Symbol::paramValue(name);
		}
	}

private: // BUG? a SubcktBase is a Painting...
	virtual rect_t bounding_rect() const override{ unreachable(); return rect_t();}
	virtual void paint(ViewPainter*) const override{ unreachable(); }

private:
	std::string _text;
}d0;
static Dispatcher<Symbol>::INSTALL p0(&symbol_dispatcher, "qucsatorScktHack", &d0);
/* -------------------------------------------------------------------------------- */
static std::string netLabel(Symbol const* s, index_t k)
{
	auto m = s->scope();
	assert(m);
	auto n = m->nodes();
	assert(n);

	std::string p = s->port_value(k);

#if 0 // TODO.
	return s->owner()->netName(p); //?
#else
	// BUG
	return n->netName(p);
#endif
}
/* -------------------------------------------------------------------------------- */
static int notalnum(char c)
{
	return !std::isalnum(c);
}
/* -------------------------------------------------------------------------------- */
static std::string mangleType(std::string& t)
{
	if(t == "_BJT"){
		// for some reason...
		t = "BJT";
	}else{
	}
	trace1("mangle", t);
	auto pos = t.find(typesep);
	std::string ret="";
	if(pos == std::string::npos){
	}else{
		auto sub = t.substr(pos+1);
		std::replace_if(sub.begin(), sub.end(), notalnum, '_');
		ret = " Type=\"" + sub + "\"";
	}
	t = t.substr(0, pos);
	if(t=="Lib"){
		t="Sub";
	}else{
	}
	return ret;
}
/* -------------------------------------------------------------------------------- */
// qucslang language implementation
class QucsatorLang : public NetLang {
private: // NetLang
  // inline void printItem(Element const* c, stream_t& s) const;
  std::string find_type_in_string(istream_t&) const override {incomplete(); return "incomplete";}

private: // local
	Element* parseItem(istream_t& s, Element* x) const override;
	Data* parseData(istream_t& s, Data* x) const;
	void printTaskElement(TaskElement const*, ostream_t&) const;

private: // local
  void printElement(Element const*, ostream_t&) const override;
  void printSymbol(Symbol const*, ostream_t&) const override;
  void printSubckt(SubcktBase const*, ostream_t&) const;
  void printComponent(Component const*, ostream_t&) const;
  void printPainting(Painting const*, ostream_t&) const override {incomplete();}
  void printDiagram(Diagram const*, ostream_t&) const override {incomplete();}
}qucslang;
static Dispatcher<DocumentFormat>::INSTALL p(&languageDispatcher, "qucsator", &qucslang);
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
// non-callback print.
static void printSymbol_(Symbol const* c, ostream_t& s)
{
	// todo: mfactor/active?
	assert(c);
	trace2("pc", c->label(), c->typeName());

	{
		std::string type = c->typeName();
		if(c->common()){
			type = c->common()->modelname(); // "netlist mode"
		}else{ untested();
		}
		std::string hack_type = mangleType(type);

		s << type << ":" << c->label();

		Symbol const* sym=c;
		trace3("print", c->label(), sym->numPorts(), sym->label());
		for(unsigned i=0; i<sym->numPorts(); ++i){
			std::string N = netLabel(sym, i);
			s << " " << N;
		}

		for(unsigned ii=0; ii<sym->paramCount(); ++ii) {
			trace3("param", c->label(), ii, sym->paramCount());
			std::string name = sym->paramName(ii);
			//trace2("param", name, value);

			if(name.at(0)=='$'){
				// hmmm
//			}else if(!sym->paramIsPrintable(ii)){ untested();
			}else if(name==""){itested();
				incomplete();
			}else if(name == "Component") {
				// hack
			}else if(name == "File") {
				// hack
			}else if(name == "Lib") {
				// hack
			}else if(name == "Symbol") { untested();
				// hack??
			}else{
				std::string value = sym->paramValue(ii);
				s << " " << name << "=\"" << value << "\"";
			}
		}

		s << hack_type;
		s << '\n';
	}
}
/* -------------------------------------------------------------------------------- */
void QucsatorLang::printSymbol(Symbol const* d, ostream_t& s) const
{
	// is_device??
	if(!d){ untested();
		incomplete();
	}else if(auto c=dynamic_cast<SubcktBase const*>(d)){ untested();
		if(c->is_device()){ untested();
			// here?
		   s << "# got sckt device " << d->label() << "\n";
			printSubckt(c, s);
		}else{ untested();
		   s << "# skip non-device " << d->label() << "\n";
		}
//	}else if(auto c=dynamic_cast<SubcktProto const*>(d)){ untested();
//		// why is this a Symbol??
//		printSubckt(c, s);
	}else if(auto c=dynamic_cast<TaskElement const*>(d)){ untested();
		// why is this a Symbol??
		printTaskElement(c, s);
	}else if(auto c=dynamic_cast<Component const*>(d)){
		// HACK
		printComponent(c, s);
	}else if(dynamic_cast<Place const*>(d)){
		// no geometry in netlist.
	}else if(dynamic_cast<Conductor const*>(d)){
		// possibly a wire.
	}else if(d){
		// TODO. all symbols here.
		printSymbol_(d, s);
	}else{ untested();
		assert(false);
		incomplete();
	}
}
/* -------------------------------------------------------------------------------- */
Data* QucsatorLang::parseData(istream_t& cs, Data* x) const
{
// 	std::string f = cs.name();
// 	_dat->set_param_by_name("filename", f);
#if 0
	if(pos <= 0) {itested();
		file.setFileName(QString::fromStdString(_fileName));
		Variable = g->_var;
	} else { untested();
		assert(false);
		incomplete();
		// is this digital stuff?? not here.
///		file.setFileName(Info.path()+QDir::separator() + g->Var.left(pos)+".dat");
///		Variable = g->Var.mid(pos+1);
	}

	Info.setFile(file);
	if(g->lastLoaded.isValid()){ untested();
		if(g->lastLoaded > Info.lastModified()){ untested();
			return this;    // dataset unchanged -> no update neccessary
		}else{ untested();
		}
	}else{ untested();
	}
#endif
	CommonData* cd = new SimOutputDat(cs);
	trace1("datparse", label());
	cd->setLabel(x->label()); // hmm
	x->attach(cd);
	return x;
}
/* -------------------------------------------------------------------------------- */
Element* QucsatorLang::parseItem(istream_t& s, Element* x) const
{
	if(auto d=dynamic_cast<Data*>(x)){
		return parseData(s, d);
	}else{
		return x;
	}
}
/* -------------------------------------------------------------------------------- */
static void printDefHack(Symbol const* p, ostream_t& s)
{
	std::string hack = p->paramValue("qucsatorsckthack");
	s << hack;
}
/* -------------------------------------------------------------------------------- */
static void print_ports(ostream_t& o, const Symbol* x)
{
	for(unsigned i=0; x->portExists(i); ++i){
		std::string N = netLabel(x, i);
		o << " " << N;
	}
	o << "\n";
}
/* -------------------------------------------------------------------------------- */
void QucsatorLang::printSubckt(SubcktBase const* p, ostream_t& s) const
{
	trace1("subckt", p->label());
	Element const* e = p;
	s << "# sckt " + p->label() + "\n";
#if 0
	if(p->label()[0] == ':'){ untested();
		return;
	}else{
	}
#endif
//	assert(!p->is_device());
	Symbol const* sym = p;
	SchematicModel const* sckt;

	sckt = e->scope();
	assert(sckt);
	std::string label = p->label();

	auto h = sckt->find_(":qucsatorsckthack:");
	if(h == sckt->end()){
	}else if(auto p = dynamic_cast<Symbol const*>(*h)){
		return printDefHack(p, s);
	}else{ untested();
	}
	h = sckt->find_("main");
	if(h == sckt->end()){
	}else{
		sckt = (*h)->scope();
		sym = prechecked_cast<Symbol const*>(*h);
		assert(sym);
	}

	s << "\n"; //?
	if(label.c_str()[3] == _typesep){
		s << ".Def:" << label.substr(4);
	}else{
		incomplete();
	}

	print_ports(s, sym);

	// somehow parameters are stashed as paintings.
	// let's see.
	if(!p->symbolPaintings()){
		s << "# Missing ID & params " << p->label() << "\n";
	}else{ untested();
#if 0
		for(auto pi : *p->symbolPaintings()){ untested();
			incomplete();
			if(pi->name() == ".ID ") { untested();
				incomplete();
				s << "# TODO ID & params" << pi->label() << pi->name() << "\n";
		//		ID_Text *pid = (ID_Text*)pi;
		//		QList<SubParameter *>::const_iterator it;
		//		for(it = pid->Parameter.constBegin(); it != pid->Parameter.constEnd(); it++) { untested();
		//			s = (*it)->Name; // keep 'Name' unchanged
		//			(*tstream) << " " << s.replace("=", "=\"") << '"';
		//			}
			}else{ untested();
			}
		//		break;
		}
#endif
	}
	//(*tstream) << '\n';
	//
	s << "# " << sckt->size() << "\n";
	for(auto it_ : *sckt){
		auto i = dynamic_cast<Symbol const*>(it_);
		if(it_){
		}else{ untested();
			incomplete();
			continue;
		}

      if(!i){
			incomplete();
		}else if(i->typeName() == "Port"){
		}else if(i->typeName() == "Wire"){ // is Conductor?
		}else if(i->typeName() == "GND"){
		}else{
			// s << "# ps" << i->typeName() << " " << i->label() << "\n";
			printSymbol(i, s);
		}
	}

	s << ".Def:End\n"; //  << p->label() << "\n";
}
/* -------------------------------------------------------------------------------- */
static void printLegacyTaskElement(LegacyTaskElement const* c, ostream_t& s)
{
	assert(c);
	s << "." << c->typeName() << ":" << c->label();

	for(auto p2 : c->Props){ // BUG
		if(p2->name() == "Symbol") { // hack??
		}else if(p2->name()=="p" && p2->value()==""){itested();
			// unreachable
		}else{itested();
			s << " " << p2->name() << "=\"" << p2->value() << "\"";
		}
	}
	s << "\n";
}
/* -------------------------------------------------------------------------------- */
void QucsatorLang::printElement(Element const* c, ostream_t& s) const
{
	if (auto t=dynamic_cast<const TaskElement*>(c)) {
		printTaskElement(t, s);
	}else{
		incomplete();
	}
}
/* -------------------------------------------------------------------------------- */
void QucsatorLang::printTaskElement(TaskElement const* c, ostream_t& s) const
{itested();
	if(auto t=dynamic_cast<LegacyTaskElement const*>(c)){
		// tmp hack
		return printLegacyTaskElement(t, s);
	}else{
	}
	assert(c);
	s << "." << c->typeName() << ":" << c->label();

#if 0
	for(auto p2 : c->Props){ // BUG
		if(p2->name() == "Symbol") { // hack??
		}else if(p2->name()=="p" && p2->value()==""){itested();
			// unreachable
		}else{itested();
			s << " " << p2->name() << "=\"" << p2->value() << "\"";
		}
	}
#endif
	s << "PROPS\n";
}

// print Component in qucsator language
// BUG: callback: untangle isShort, isActive (mfactor?).
void QucsatorLang::printComponent(Component const* c, ostream_t& s) const
{
	if(c->isActive != COMP_IS_ACTIVE){
		// comment out?
		incomplete();
	}else{
	}
	assert(c);
	trace2("pc", c->label(), c->typeName());

	if(c->isOpen()) {
		// nothing.
	}else if(c->isShort()){ untested();
		// replace by some resistors (hack?)
		incomplete();
		int z=0;
		QListIterator<ComponentPort*> iport(c->ports());
		iport.next(); // BUG
		unsigned k=0;
		std::string Node1 = netLabel(c, k);
		while (iport.hasNext()){ untested();
			++k;
			s << "R:" << c->label() << "." << QString::number(z++) << " "
				<< Node1 << " " << netLabel(c, k) << " R=\"0\"\n";
		}
	}else{
		std::string type = c->typeName();
		if(c->common()){ untested();
			type = c->common()->modelname(); // "netlist mode"
		}else{
		}
		std::string hack_type = mangleType(type);

		s << type << ":" << c->label();

		Symbol const* sym=c;
		trace3("print", c->label(), sym->numPorts(), sym->label());
		for(unsigned i=0; i<sym->numPorts(); ++i){
			std::string N = netLabel(sym, i);

			s << " " << N;
		}

		for(unsigned ii=0; ii<sym->paramCount(); ++ii) {
			trace3("param", c->label(), ii, sym->paramCount());
			std::string name = sym->paramName(ii);
			//trace2("param", name, value);

			if(name.at(0)=='$'){
				// hmmm
//			}else if(!sym->paramIsPrintable(ii)){ untested();
			}else if(name==""){itested();
				incomplete();
			}else if(name == "File") {
				// hack
			}else if(name == "Symbol") {
				// hack??
			}else{
				std::string value = sym->paramValue(ii);
				s << " " << name << "=\"" << value << "\"";
			}
		}

		s << hack_type;
		s << '\n';
	}
}
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */

}//namespace
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------- */
