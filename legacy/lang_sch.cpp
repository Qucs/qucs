/***************************************************************************
    copyright            : (C) 2018, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "qucsdoc.h"
#include "element_list.h"
#include "schematic_lang.h"
#include "element_list.h" /// hmm
#include "qucs_globals.h"
#include "task_element.h"
#include "legacy_painting.h"
#include "diagram.h" // BUG
#include "docfmt.h" // BUG
#include "dot.h"
#include "model.h"
#include "components/component.h"
#include "sckt_base.h"
#include "place.h"
#include "qio.h"
#include "data.h"
#include "legacy_task_element.h"

#ifdef DO_TRACE
#include <typeinfo>
#endif
/*--------------------------------------------------------------------------*/
static void parsePainting(QString Line, Painting*p)
{
	assert(p);
	auto lp=dynamic_cast<LegacyPainting*>(p);
	// BUG: callback
	if(!lp){ untested();
		incomplete();
	}else if(!lp->load(Line)) { untested();
		trace2("ERROR parsePainting", Line, lp->label());
		incomplete();
		// QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nWrong 'painting' line format!"));
		throw qucs::Exception("cannot parse painting");

	}else{
//		trace2("parsePainting", Line, p->center());
	}
}
/*--------------------------------------------------------------------------*/
#if 0
static bool PaintingListLoad(QString Line, PaintingList& List)
{ untested();
	Painting *p=0;
	QString cstr;
	{ untested();
		if(Line.isEmpty()){ untested();
			return true;
		}else

	 // not here.
		if(Line.at(0) == '<' && Line.at(1) == '/'){ untested();
			return true;
		}else{ untested();
		}

		Line = Line.trimmed();
		if(Line.isEmpty()) { untested();
			return true;
		}else if( (Line.at(0) != '<') || (Line.at(Line.length()-1) != '>')) { untested();
			incomplete();
			// QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nWrong 'painting' line delimiter!"));
			return false;
		}else{ untested();
		}
		Line = Line.mid(1, Line.length()-2);  // cut off start and end character


		cstr = Line.section(' ',0,0);    // painting type
		qDebug() << cstr;
		if(Painting const* pp = painting_dispatcher[cstr.toStdString()]){ untested();
			p=prechecked_cast<Painting*>(pp->clone());
			assert(p);
		}else{ untested();
			trace1("no painting", cstr);
			// incomplete();
			// QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nUnknown painting " + cstr));
			return false;
		}

		::parsePainting(Line, p);

		List.append(p);
	}	

	// incomplete();
	// QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\n'Painting' field is not closed!"));
	return false;
}
#endif

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
using qucs::Component;
using qucs::Model;
/*--------------------------------------------------------------------------*/
class LegacySchematicLanguage : public SchematicLanguage {
public:
	LegacySchematicLanguage(bool mod=false) : SchematicLanguage(), _lib_mod(mod){
		set_label("legacy_schematic");
	}
private: // stuff from component.cc
	void loadProperties(QTextStream& stream, SchematicSymbol& m) const;
//	Component* parseComponentObsoleteCallback(const QString& _s, Component* c) const;
//	Element* getComponentFromName(QString& Line) const;

private: // overrides
	void parse_top_item(istream_t& stream, ElementList* sckt) const override;
	std::string find_type_in_string(istream_t&) const override;
	Element* parseItem(istream_t&, Element*) const override;
   DEV_DOT* parseCommand(istream_t&, DEV_DOT*) const override;

private: // local/incomplete
	Diagram* parseDiagram(Diagram* d, istream_t&) const;
	Symbol* parseSymbol(istream_t&, Symbol*) const;
//	Element* loadElement_(const QString& _s, Element* e) const;
	void printLegacyTaskElement(LegacyTaskElement const*, ostream_t&) const;
	Element* parseElement(istream_t&, Element*) const;
	void printTaskElement(TaskElement const*, ostream_t&) const;
	void printData(Data const*, ostream_t&) const;

private:
	void print_instance(ostream_t&, Component const*) const override;
	void print_paramset(ostream_t&, Model const*) const override;
	void printElement(Element const*, ostream_t&) const override;
	void printPainting(Painting const*, ostream_t&) const override;
	void print_module(ostream_t&, SubcktBase const*) const override;
   void print_diagram(ostream_t&, Diagram const*) const override;
	bool _lib_mod; // HACK HACK
}leg_sch;
static Dispatcher<Language>::INSTALL
    p0(&language_dispatcher, "leg_sch|.sch", &leg_sch);
/*--------------------------------------------------------------------------*/
struct set_default{
	set_default(){
		OPT::language = &leg_sch;
	}
}sd;
/*--------------------------------------------------------------------------*/
	 // obsolete??
LegacySchematicLanguage d1(true);
static Dispatcher<Language>::INSTALL
    p1(&language_dispatcher, "legacy_lib", &d1);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static ::Component* parseComponentObsoleteCallback(const QString& _s, ::Component* c);
//static TaskElement* loadtaskElement(const QString& _s, TaskElement* c);
/*--------------------------------------------------------------------------*/
static std::list<Element*> implicit_hack;
/*--------------------------------------------------------------------------*/
static bool obsolete_wireload(Symbol* w, const QString& sc)
{
	trace1("ow", sc);
	Symbol* sym = w;
	QString s(sc);
	bool ok;

	QString n;
	n  = s.section(' ',0,0);
	int x1 = n.toInt(&ok);
	if(!ok) { untested();
		return false; // BUG: throw
	}else{
	}
	sym->set_param_by_name("$xposition", std::to_string(x1));

	n  = s.section(' ',1,1);    // y1
	int y1 = n.toInt(&ok);
	if(!ok) { untested();
		return false; // BUG: throw
	}else{
	}
	sym->set_param_by_name("$yposition", std::to_string(y1));

	n  = s.section(' ',2,2);    // x2
	int x2 = n.toInt(&ok);
	if(!ok) { untested();
		return false; // BUG: throw
	}else{
	}

	if(x1<=x2){
		// possibly the case in all legacy files..
	}else{
		// but not really necessary
	}
	sym->set_param_by_name("deltax", std::to_string(x2 - x1));

	n  = s.section(' ',3,3);    // y2
	int y2 = n.toInt(&ok);
	if(!ok) { untested();
		throw qucs::Exception("value error"); // CS_Exception?
	}else{
	}

	if(y1<=y2){
		// possibly the case in all legacy files
	}else{
		// nothing wrong with that, really?
	}
	sym->set_param_by_name("deltay", std::to_string(y2 - y1));

	assert(sym->nodePosition(0) == pos_t(x1, y1));
	trace2("debug obsolete load", sym->nodePosition(1), pos_t(x2, y2));
	assert(sym->nodePosition(1) == pos_t(x2, y2));

	n = s.section('"',1,1);
	if(!n.isEmpty()) {
		Symbol* sym=w;
		// there is a label hidden in this wire. go get it.
		//
		// a label has a position and is connected to a node.
		// nodelabel #(.$xposition(x), .$yposition(y)) name(node);
		// name may be used to identify the net connected to node in a flat netlist.
		auto nx = s.section(' ',5,5).toInt(&ok);
		auto ny = s.section(' ',6,6).toInt(&ok);
		if(!ok) { untested();
			throw qucs::Exception("value error"); // CS_Exception?
		}else{
		}

		// TODO: emulate legacy qucs...
		nx -= x1;
		ny -= y1;
		auto delta = s.section(' ',7,7);
		auto sth = s.section('"',3,3);

		// nl->set_label(n, s.section('"',3,3), delta, nx, ny);  // Wire Label

		// not sure what the parameters mean, need to compute label position and
		// node position from it.
		auto nn = s.section('"',3,3);
		trace7("hack push", s, sth, delta, nx, ny, nn, n);
		sym->set_param_by_name("netname", n.toStdString()); // what is this?
		sym->set_param_by_name("delta", delta.toStdString()); // what is this?
		sym->set_param_by_name("$tx", std::to_string(nx));
		sym->set_param_by_name("$ty", std::to_string(ny));
		sym->expand(); //always?
	}else{
	}

	return true;
}
/*--------------------------------------------------------------------------*/
void LegacySchematicLanguage::parse_top_item(istream_t& cmd, ElementList* sckt) const
{
	QString Line;
	assert(sckt);
	Symbol* owner = nullptr;

	cmd.get_line("qucs-sch>");
	new__instance(cmd, owner, sckt);
}
/*--------------------------------------------------------------------------*/
// TODO: remove. not getting here for new-style diagrams
Diagram* LegacySchematicLanguage::parseDiagram(Diagram* d, istream_t& stream)const
{
	QString Line = QString::fromStdString(stream.fullString());
	QString cstr;
	if(!stream.atEnd()) {
		trace1("diagram?", Line);
		if(Line.at(0) == '<' && Line.at(1) == '/'){ untested();
			return nullptr;
		}else{
		}
		Line = Line.trimmed();
		if(Line.isEmpty()){ untested();
			return nullptr;
		}else{
		}
		cstr = Line.section(' ',0,0);    // diagram type
		std::string what=cstr.toStdString();

		auto type = what.c_str()+1;
		if(diagram_dispatcher[type]) {
			trace2("diagram parse", type, stream.fullString());
			assert(d); // BUG
			d->set_label(type); // yuck there is no label. put the type in
		}else{ untested();
			trace1("diagram doesntexist", type);
			incomplete();
			// throw ...
			return nullptr;
		}

		trace1("load call", Line);
		if(!d->load(Line, stream)) { untested();
			trace1("loadfail", Line);
			incomplete();
			// delete d;
			// return nullptr; // no. just use the half parsed thing.
		}else{
		}

		auto& cs = stream;
		while(true){
			cs.read_line();
			trace2("diag cmd" , cs.fullstring(), type);
			if(cs.umatch(std::string("</")+type+">")){
				break;
			}else{
				trace2("Rect parse", short_label(), cs.fullstring());
				cs.skipbl();
				new__instance(cs, d, d->scope());
			}
		}
		return d;
	}

	return nullptr;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static const std::string typesep(":");
static std::string mangle(std::string t)
{
	auto pos = t.find(typesep);
	std::string ret="";
	return t.substr(0, pos);
}
/*--------------------------------------------------------------------------*/
void LegacySchematicLanguage::print_diagram(ostream_t& cs, Diagram const* x) const
{
	cs << "  " << x->save(); // BUG
	Element const* e = x;
	for(auto ii : *e->scope()){
		printItem(cs, ii);
	}
	cs << "  </" << x->label() << ">\n";
}
/*--------------------------------------------------------------------------*/
void LegacySchematicLanguage::print_module(ostream_t& s, SubcktBase const* x) const
{ untested();
	incomplete();
	s << "  " << x->short_label() << "\n";
}
/*--------------------------------------------------------------------------*/
// catchall for non-circuit stuff? let's see.
void LegacySchematicLanguage::printElement(Element const* c, ostream_t& s) const
{
	if(auto t=dynamic_cast<TaskElement const*>(c)){
		return printTaskElement(t, s);
	}else if(auto d=dynamic_cast<Data const*>(c)){
		return printData(d, s);
	}else{
		trace1("printElement??", c->label());
		// painting here?
		incomplete();
	}
}
/*--------------------------------------------------------------------------*/
static void print_args(ostream_t& s, Data const* x)
{
	for(index_t i=x->param_count(); i--;){
		s << " " << x->param_value(i);
	}
}
/*--------------------------------------------------------------------------*/
void LegacySchematicLanguage::printData(Data const* x, ostream_t& s) const
{
	s << "\t<\"" << x->label() << "\"";
	print_args(s, x);
	s << ">\n";
}
/*--------------------------------------------------------------------------*/
void LegacySchematicLanguage::printTaskElement(TaskElement const* c, ostream_t& s) const
{
	s << "  <." << c->dev_type() << " ";

	if(c->label()==""){
		s << "MISSING_LABEL";
		unreachable();
	}else{
		s << c->label();
	}
	s << " ";
	if (auto lc=dynamic_cast<LegacyTaskElement const*>(c)){
		printLegacyTaskElement(lc, s);
	}else{
		incomplete();
		s << "taskElement incomplete\n";
	}
}
/*--------------------------------------------------------------------------*/
void LegacySchematicLanguage::printLegacyTaskElement(LegacyTaskElement const* c, ostream_t& s) const
{
	int i = 0;
	if(!c->showName){ untested();
		i = 4;
	}else{
	}
	incomplete();
	std::string active = c->param_value_by_name("$active");
	i |= atoi(active.c_str());
	s << std::to_string(i);
	s << " "+QString::number(c->cx())+" "+QString::number(c->cy());
	s << " "+QString::number(c->tx)+" "+QString::number(c->ty);
	s << " ";
	s << " 0"; // mirrored
	s << " 0"; // rotated

	// write all properties
	// FIXME: ask element for properties, not for dictionary
	auto cc=const_cast<LegacyTaskElement*>(c); // BUGBUGBUGBUG
	// cannot access Props without this hack
#if 1
	// used in .AC statement.
	for(Property *p1 = cc->Props.first(); p1 != 0; p1 = cc->Props.next()) {
		if(p1->Description.isEmpty()){ untested();
			s << " \""+p1->Name+"="+p1->Value+"\"";   // e.g. for equations
		}else{
			s << " \""+p1->Value+"\"";
		}
		s << " ";
		if(p1->display){
			s << "1";
		}else{
			s << "0";
		}
	}
#endif

	s << ">\n";
}
/*--------------------------------------------------------------------------*/
void LegacySchematicLanguage::printPainting(Painting const* p, ostream_t& s) const
{
	if(auto pp=dynamic_cast<LegacyPainting const*>(p)){
		auto mp = const_cast<LegacyPainting*>(pp);
		s << "  <" << mp->save() << ">\n";
	}else{
		incomplete();
	}
}
/*--------------------------------------------------------------------------*/
static void printArgs(qucs::Component const* sym, ostream_t& s)
{
	s << " " << sym->param_value_by_name("$mfactor");
	s << " " << sym->param_value_by_name("$xposition");
	s << " " << sym->param_value_by_name("$yposition");
	s << " " << sym->param_value_by_name("$tx");
	s << " " << sym->param_value_by_name("$ty");
	int angle = atoi(sym->param_value_by_name("$angle").c_str());
	int hflip = atoi(sym->param_value_by_name("$hflip").c_str());
	int vflip = atoi(sym->param_value_by_name("$vflip").c_str());
	assert(hflip);
	assert(vflip);
	if(hflip==1){
		s << " " << (1-vflip) / 2;
		s << " " << (angle/90) % 4;
	}else if(vflip==1){ untested();
		assert(hflip==-1);
		s << " 1";
		s << " TODO" << ((180-angle)/90) % 4;
	}else{ untested();
		assert(hflip==-1);
		assert(vflip==-1);
		s << " 0";
		s << " TODO" << ((180+angle)/90) % 4;
	}

	int show = atoi(sym->param_value_by_name("$param_display").c_str());
	for(index_t i=sym->param_count(); i--;){
		trace4("printArgs display", sym->short_label(), i, show, sym->param_name(i));
		// if(sym->paramIsPrintabl(i))
		std::string n = sym->param_name(i);
		if(n.size() == 0){
			unreachable();
		}else if(n.at(0)=='$'){
		}else if(sym->param_is_printable(i)){
			s << " \"" << sym->param_value(i) << "\" " << (show % 2);
		}else{
		}
		show /= 2;
	}
}
/*--------------------------------------------------------------------------*/
static void printwirehack(qucs::Symbol const* w, ostream_t& d)
{
	assert(w);
	// Symbol const* sym = w;
  int x1=0, x2=0, y1=0, y2=0;
  trace1("wirehack", w);
  std::pair<int, int> X = w->portPosition(0);
  trace1("wirehack", X.first);
  x1 = X.first;
  y1 = X.second;
  X = w->portPosition(1);
  x2 = X.first;
  y2 = X.second;
  std::tie(x1, y1) = w->nodePosition(0);
  std::tie(x2, y2) = w->nodePosition(1);

//  int cx = atoi(sym->param_value_by_name("$xposition").c_str());
//  int cy = atoi(sym->param_value_by_name("$yposition").c_str());
//  int dx = atoi(sym->param_value_by_name("deltax").c_str());
//  int dy = atoi(sym->param_value_by_name("deltay").c_str());

  d << "<" << x1 << " " << y1
    << " " << x2 << " " << y2;

  if(false) { untested();
          // d << " \""+Label->name()+"\" ";
          // d << QString::number(Label->x1_())+" "+QString::number(Label->y1_())+" ";
          // d << QString::number(Label->cx_()-x1 + Label->cy_()-y1);
          // d << " \""+Label->initValue+"\">";
  } else {
	  d << " \"\" 0 0 0 \"\">\n";
  }
}
/*--------------------------------------------------------------------------*/
void LegacySchematicLanguage::print_paramset(ostream_t& s, Model const* x) const
{
	unreachable(); // there are no paramsets in a schematic atm.
	s << "incomplete model " << x->short_label()  << "\n";
}
/*--------------------------------------------------------------------------*/
void LegacySchematicLanguage::print_instance(ostream_t& s, Component const* sym) const
{
	trace1("printSymbol", sym->dev_type());
	if(sym->dev_type()!="wire"){
	}else if(auto ww=dynamic_cast<Symbol const*>(sym)){
		// hack hack
		return printwirehack(ww, s);
	}else{
	}

	s << "  <" << mangle(sym->dev_type()) << " ";

	if(sym->label()==""){ untested();
		s << "*";
	}else{
		s << sym->label();
	}

	auto c = dynamic_cast<::Component const*>(sym);
	auto cc = const_cast<::Component*>(c); // BUGBUGBUGBUG
	if(c && c->useObsoleteProps()){
		s << " ";
		int i=0;
		if(!c->showName){
			i = 4;
		}else{
		}
		i |= c->isActive;
		s << QString::number(i);
		s << " "+QString::number(c->cx())+" "+QString::number(c->cy());
		s << " "+QString::number(c->tx)+" "+QString::number(c->ty);
		s << " ";
		if(!sym->legacyTransformHack()){
			int angle = atoi(sym->param_value_by_name("$angle").c_str());
			int hflip = atoi(sym->param_value_by_name("$hflip").c_str());
			int vflip = atoi(sym->param_value_by_name("$vflip").c_str());
			assert(hflip);
			assert(vflip);
			int a = (angle/90) % 4;
			
			// only vflip in legacy format. convert.
			// (this is implemented somewhere else already...)
			if(hflip==-1 and vflip==-1 and a==3){ untested();
				s << "0 1";
			}else if(hflip==1){
				s << (1-vflip) / 2;
				s << " " << (angle/90) % 4;
			}else if(vflip==1){ untested();
				assert(hflip==-1);
				s << "1";
				s << " TODO" << ((180-angle)/90) % 4;
			}else{ untested();
				assert(hflip==-1);
				assert(vflip==-1);
				s << "0";
				s << " TODO" << ((180+angle)/90) % 4;
			}
		}else{ untested();
			if(c->mirroredX){ untested();
				s << "1";
			}else{ untested();
				s << "0";
			}
			s << " " << QString::number(c->rotated());
		}
		for(Property *p1 = cc->Props.first(); p1 != 0; p1 = cc->Props.next()) {
			if(p1->Description.isEmpty()){
				s << " \""+p1->Name+"="+p1->Value+"\"";   // e.g. for equations
			}else{
				s << " \""+p1->Value+"\"";
			}
			s << " ";
			if(p1->display){
				s << "1";
			}else{
				s << "0";
			}
		}
	}else{
		printArgs(sym, s);
	}

	s << ">\n";
} // printSymbol
/*--------------------------------------------------------------------------*/
static TaskElement* loadLegacyTaskElement(const QString& _s, LegacyTaskElement* c)
{
	trace1("loadLegacyTaskElement", c->label());
	bool ok;
	int  ttx, tty, tmp;
	QString s = _s;

	if(s.at(0) != '<'){
	}else if(s.at(s.length()-1) != '>'){ untested();
	}else{ untested();
		s = s.mid(1, s.length()-2);   // cut off start and end character
	}

	if(1){

		QString type = s.section(' ',0,0);
		if(type[0]=='.'){
			type = type.section('.',1,1);
		}else{
		}
		c->set_dev_type(type.toStdString());

		QString label = s.section(' ',1,1);
		trace1("TASK NAME", label);
///		c->setName(label);//???
		c->set_label(label.toStdString());

		QString n;
		n  = s.section(' ',2,2);      // isActive
		tmp = n.toInt(&ok);
		if(!ok){ untested();
			return NULL;
		}
		c->isActive = tmp & 3;

		if(tmp & 4){ untested();
			c->showName = false;
		}else{
			// use default, e.g. never show name for GND (bug?)
		}

		n  = s.section(' ',3,3);    // cx
		int cx = n.toInt(&ok);
		if(!ok) return NULL;

		n  = s.section(' ',4,4);    // cy
		c->setPosition(pos_t(cx, n.toInt(&ok)));
		if(!ok) return NULL;

		n  = s.section(' ',5,5);    // tx
		ttx = n.toInt(&ok);
		if(!ok) return NULL;

		n  = s.section(' ',6,6);    // ty
		tty = n.toInt(&ok);
		if(!ok) return NULL;

		assert(c);

		c->tx = ttx;
		c->ty = tty; // restore text position (was changed by rotate/mirror)

		unsigned z = 0;
		unsigned counts = s.count('"');
		// FIXME. use c->paramCount()

		tmp=c->Props.size(); // ???
		/// BUG FIXME. dont use Component parameter dictionary.
		for(; tmp<=(int)counts/2; tmp++){
			trace2("taskhack append", tmp, c->Props.size());
			c->Props.append(new Property("p", "", true, " "));
		}

		// load all properties
		Property *p1;
#if 1
		for(p1 = c->Props.first(); p1 != 0; p1 = c->Props.next()) {
			z++;
			n = s.section('"',z,z);    // property value
			trace3("load legacy command props", z, n, p1->Name);
			z++;
			//qDebug() << "LOAD: " << p1->Description;
			//
			// c->set_param_by_index( z/2, n); ?

			// not all properties have to be mentioned (backward compatible)
			if(z > counts) {
				if(p1->Description.isEmpty()){ untested();
					c->Props.remove();    // remove if allocated in vain
				}else{
				}

				return c;
			}else{
			}

			p1->Value = n;

			n  = s.section('"',z,z);    // display
			p1->display = (n.at(1) == '1');
		}
#endif

		return c;
	}
}
/*--------------------------------------------------------------------------*/
Place const* place_at(pos_t p, Symbol* m)
{
	std::string ps = "net_" + std::to_string(getX(p)) + "_" + std::to_string(getY(p));
	auto scope = m->scope();
	assert(scope);
	auto i = scope->find_(ps);
	Place const* ret = nullptr;

	assert(m->owner());

	if(i == scope->end()){
	}else if(auto p=dynamic_cast<Place const*>(*i)){
		ret = p;
	}else{
		incomplete();// find_again...
		assert(false); //for now.
	}

	if(!ret){
		Symbol* c = symbol_dispatcher.clone("place");
		auto s = prechecked_cast<Place*>(c);
		assert(s);
		s->setPosition(p);
		s->set_dev_type("place");
		s->set_label(ps);
		s->set_owner(m->owner());
		s->set_port_by_index(0, ps);
		scope->push_back(s);

		ret = s;
	}else{
	}

	return ret;

}
/*--------------------------------------------------------------------------*/
Element* LegacySchematicLanguage::parseElement(istream_t& cmd, Element* x) const
{
	trace1("LegacySchematicLanguage::parseElement", cmd.fullstring());

	cmd.skip1('<');
	std::string s;
	cmd >> s;

	x->set_label(s);

	for(index_t i=x->param_count(); i--;){
		s = cmd.ctos(">", "=", ">");
		trace3("LegacySchematicLanguage::parseElement", i, s, x->param_name(i));

		x->set_param_by_index(i, s);
	}
	return x;
}
/*--------------------------------------------------------------------------*/
// decluttered parseComponentObsoleteCallback
Symbol* LegacySchematicLanguage::parseSymbol(istream_t& cs, Symbol* sym) const
{
	QString Line = QString::fromStdString( cs.fullString());
	trace1("LegacySchematicLanguage::parseSymbol", Line);

	QString l = Line.trimmed();
	if(l.isEmpty()) { untested();
		delete sym;
		return nullptr;
	}else if( (l.at(0) != '<') || (l.at(l.length()-1) != '>')) { untested();
		incomplete();
		// QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nWrong 'painting' line delimiter!"));
		delete sym;
		return nullptr;
	}else{
	}
	QString s  = l.mid(1, l.length()-2);  // cut off start and end character
	bool ok;
	int tmp;

//	if(s.at(0) != '<'){ untested();
//		return NULL;
//	}else if(s.at(s.length()-1) != '>'){ untested();
//		return NULL;
//	}
//	s = s.mid(1, s.length()-2);   // cut off start and end character
	bool use_obsolete_callback = dynamic_cast<::Component*>(sym);

	if(sym->dev_type()=="EDD"){
		trace0("EDD disable callback");
		use_obsolete_callback = false;
	}else{
	}

	if(sym->dev_type()=="wire"){
		// yikes.
		obsolete_wireload(sym, s);
	}else if(use_obsolete_callback){
		auto cc=dynamic_cast<::Component*>(sym);
		trace2("compon callback", s, cc->label());
		// HACK
		parseComponentObsoleteCallback(s, cc);
	}else{
		std::string type = s.section(' ',0,0).toStdString();
		std::string label = s.section(' ',1,1).toStdString();
		sym->set_dev_type(type);
		sym->set_label(label);

		trace2("nonobsolete parse", type, sym->short_label());

		QString n;
		n  = s.section(' ',2,2);      // flags
		tmp = n.toInt(&ok);
		if(!ok){ untested();
			return NULL;
		}
		// sym->setParameter("is_active", std::to_string(tmp & 3));
		// sym->setParameter("$hide_label", std::to_string(tmp & 4));

		n  = s.section(' ',3,3);    // cx
		int cx=n.toInt(&ok);
		qDebug() << "cx" << cx;
		if(!ok){ untested();
			throw qucs::Exception("xposition parse");
		}else{
			sym->set_param_by_name("$xposition", std::to_string(cx));
			//	sym->setParameter(3, std::to_string(cx));
		}

		n  = s.section(' ',4,4);    // cy
		int cy=n.toInt(&ok);
		if(!ok){ untested();
			throw qucs::Exception("yposition parse");
		}else{
			sym->set_param_by_name("$yposition", std::to_string(cy));
			//	sym->setParameter(4, std::to_string(cy));
		}

		n  = s.section(' ',5,5);    // tx
		tmp = n.toInt(&ok);
		if(!ok){ untested();
			throw qucs::Exception("tx parse");
		}else{
			sym->set_param_by_name("$tx", std::to_string(tmp));
		}

		n  = s.section(' ',6,6);    // ty
		tmp = n.toInt(&ok);
		if(!ok){ untested();
			throw qucs::Exception("ty parse");
		}else{
			sym->set_param_by_name("$ty", std::to_string(tmp));
		}

		{
			n  = s.section(' ',7,7);    // mirror y axis
			int nn = n.toInt(&ok);
			if(!ok){ untested();
				throw qucs::Exception("vflip parse");
			}else{
				int vflip = 1-2*nn;
				assert(vflip==1 || vflip==-1);
				trace2("spbn $vflip", vflip, sym->label());
				sym->set_param_by_name("$vflip", std::to_string(vflip));
			}

			n  = s.section(' ',8,8);    // rotated
			tmp = n.toInt(&ok);
			if(!ok){ untested();
				return NULL;
			}else{
			}

			tmp *= 90;
			sym->set_param_by_name("$angle", std::to_string(tmp));
			assert(sym->param_value_by_name("$angle") == std::to_string(tmp));
		}

		// set parameters.
		// skip the first 4. x y (tx ty) mirror rotate
		index_t position = 0;
		index_t z = 0;

		int counts = s.count('"');
		trace2("set?", s, counts);
		for(; int(position)<counts/2; ++position){
			z++;
			n = s.section('"',z,z);    // property value. gaah parse over and over again?
			z++;

			try{
				index_t i = sym->param_count() - 1 - 2 - position;
				trace5("legacy:set", sym->param_count(), position, n, i, sym->param_name(i));
				sym->set_param_by_index(i, n.toStdString());
			}catch(qucs::ExceptionCantFind const*){ untested();
				trace1("wrong position", position);
				incomplete(); // CS has error messages...
				cs.warn(MsgFatal, "cannot parse Symbol param " +
						std::to_string(position) + " in " + sym->label());
				throw; // BUG
			}

			n  = s.section('"',z,z);    // display
		}
	}

	auto Scope = sym->scope();
	assert(Scope);
	trace2("lang_sch connect ports", sym->label(), sym->numPorts());
	for(index_t i=0; i<sym->numPorts(); ++i){
		pos_t p = sym->nodePosition(i);
		auto q = place_at(p, sym);
		
		std::string const& l = q->label();
		sym->set_port_by_index(i, l);
		trace4("portplace", sym->label(), i, p, l);
	}

	return sym;
} // parseSymbol

// BUG raise exceptions if something goes wrong.
static ::Component* parseComponentObsoleteCallback(const QString& _s, ::Component* c)
{
	Symbol* sym = c;
	trace1("parseComponentObsoleteCallback", _s);
	bool ok;
	int  ttx, tty, tmp;
	QString s = _s;

	if(s.at(0) != '<'){
	}else if(s.at(s.length()-1) != '>'){ untested();
	}else{ untested();
		s = s.mid(1, s.length()-2);   // cut off start and end character
	}

	if(1){
		// really?
		QString type=s.section(' ',0,0);
		sym->set_dev_type(type.toStdString());
	}else{
	}

	QString label=s.section(' ',1,1);
	// c->obsolete_name_override_hack(label); //??
	trace1("loadComp", label);
	c->set_label(label.toStdString());

	QString n;
	n  = s.section(' ',2,2);      // flags
	tmp = n.toInt(&ok);
	if(!ok){ untested();
		return NULL;
	}
	c->isActive = tmp & 3;
	// sym->setParameter("is_active", std::to_string(tmp & 3));

	if(tmp & 4){
	// sym->setParameter("hide_label", std::to_string(tmp & 4));
		c->showName = false;
	}else{
		// use default, e.g. never show name for GND (bug?)
	}

	n  = s.section(' ',3,3);    // cx
	int cx=n.toInt(&ok);
	qDebug() << "cx" << cx;
	sym->set_param_by_name("$xposition", std::to_string(cx));
//	sym->setParameter(3, std::to_string(cx));
	if(!ok) return NULL;

	n  = s.section(' ',4,4);    // cy
	int cy=n.toInt(&ok);
	sym->set_param_by_name("$yposition", std::to_string(cy));
//	sym->setParameter(4, std::to_string(cy));
	if(!ok){
		return NULL;
	}else{
	}

	n  = s.section(' ',5,5);    // tx
	ttx = n.toInt(&ok);
	if(!ok){
		return NULL;
	}else{
	}

	n  = s.section(' ',6,6);    // ty
	tty = n.toInt(&ok);
	if(!ok){
		return NULL;
	}else{
	}

	assert(c);
//	assert(c->obsolete_model_hack().at(0) != '.');

	{
		n  = s.section(' ',7,7);    // mirror y axis
		int nn = n.toInt(&ok);
		if(!ok){
			incomplete();
			// throw?
		}else if(!sym->legacyTransformHack()){
			int vflip = 1-2*nn;
			assert(vflip==1 || vflip==-1);
			trace2("spbn $vflip", vflip, sym->label());
			sym->set_param_by_name("$vflip", std::to_string(vflip));
		}else if(nn == 1){
			c->mirrorX(); // yikes.
		}else{
		}
		if(!ok) return NULL;

		n  = s.section(' ',8,8);    // rotated
		tmp = n.toInt(&ok);
		if(!ok){ untested();
			return NULL;
		}else if(int(c->rotated()) > tmp){
		  	// neccessary because of historical flaw in ...
		  	// ... components like "volt_dc"
			// ????
			tmp += 4;
		}

		tmp *= 90;
		sym->set_param_by_name("$angle", std::to_string(tmp));
		if(sym->param_value_by_name("$angle") == std::to_string(tmp)){
		}else{
			unreachable();
		}
		trace2("DBG rot", c->rotated(), n);
	}

	c->tx = ttx;
	c->ty = tty; // restore text position (was changed by rotate/mirror)

	QString Model = c->obsolete_model_hack(); // BUG: don't use names

	unsigned int z=0;
	int counts = s.count('"');
	// FIXME. use c->paramCount()
	if(Model == "Sub"){
		tmp = 2;   // first property (File) already exists
	}else if(Model == "Lib"){ untested();
		tmp = 3;
	}else if(Model == "RFEDD"){ untested();
		tmp = 8;
	}else if(Model == "VHDL"){ untested();
		tmp = 2;
	}else if(Model == "MUTX"){ untested();
		tmp = 5; // number of properties for the default MUTX (2 inductors)
	}else{
		// "+1" because "counts" could be zero
		tmp = counts + 1;
	}
//	assert(tmp == c->ParamPositionHack()); //?

	/// BUG FIXME. dont use Component parameter dictionary.
	for(; tmp<=(int)counts/2; tmp++){
		incomplete();
		c->Props.append(new Property("p", "", true, " "));
		//	sym->setParameter("p__" + std::to_string(tmp), "");
	}

	// set parameters.
	Property *p1;
	index_t position = c->Symbol::param_count() + 2 /*tx ty*/;
	trace4("position hack", c->dev_type(), position, c->param_count(), c->Symbol::param_count());
	for(p1 = c->Props.first(); p1 != 0; p1 = c->Props.next()) {
		z++;
		n = s.section('"',z,z);    // property value. gaah parse over and over again?
		z++;
		//qDebug() << "LOAD: " << p1->Description;

		// not all properties have to be mentioned (backward compatible)
		if(int(z) > int(counts)) {
			if(p1->Description.isEmpty()){ untested();
				c->Props.remove();    // remove if allocated in vain
			}else{
			}

			if(Model == "Diode") { // BUG: don't use names
				incomplete();
				if(counts < 56) {  // backward compatible
					counts >>= 1;
					p1 = c->Props.at(counts-1);
					for(; p1 != 0; p1 = c->Props.current()) {
						if(counts-- < 19){
							break;
						}

						n = c->Props.prev()->Value;
						p1->Value = n;
					}

					p1 = c->Props.at(17);
					p1->Value = c->Props.at(11)->Value;
					c->Props.current()->Value = "0";
				}
			}else if(Model == "AND" || Model == "NAND" || Model == "NOR" ||
					Model == "OR" ||  Model == "XNOR"|| Model == "XOR") { untested();
				if(counts < 10) {   // backward compatible
					counts >>= 1;
					p1 = c->Props.at(counts);
					for(; p1 != 0; p1 = c->Props.current()) { untested();
						if(counts-- < 4)
							break;
						n = c->Props.prev()->Value;
						p1->Value = n;
					}
					c->Props.current()->Value = "10";
				}
			}else if(Model == "Buf" || Model == "Inv") { untested();
				if(counts < 8) {   // backward compatible
					counts >>= 1;
					p1 = c->Props.at(counts);
					for(; p1 != 0; p1 = c->Props.current()) { untested();
						if(counts-- < 3)
							break;
						n = c->Props.prev()->Value;
						p1->Value = n;
					}
					c->Props.current()->Value = "10";
				}
			}else{
			}

			return c;
		}else{
			// z <= counts
		}

		// for equations
		qDebug() << "Model" << Model;
#if 1
		// gaah. breaks Eqn.
		if(Model != "RFEDD" && Model != "RFEDD2P"){
			if(p1->Description.isEmpty()) {  // unknown number of properties ?
				p1->Name = n.section('=',0,0);
				n = n.section('=',1);
				// allocate memory for a new property (e.g. for equations)
				if(int(c->Props.count()) < (int(counts)>>1)) {
					c->Props.insert(z >> 1, new Property("y", "1", true));
					c->Props.prev();
				}
			}
		}
#endif
		if(z == 6 && counts == 6) {
			// backward compatible
			if(Model == "R") {
				c->Props.getLast()->Value = n;
				return c;
			}
		}else{
		}
		p1->Value = n; // TODO: remove

		Symbol* sym = c;
		trace6("callback spbi", c->dev_type(), c->label(), sym->param_count(), position, n, sym->param_name(position));

		sym->set_param_by_index(position, n.toStdString()); // BUG: also do for non-Components.
		position++;

		n  = s.section('"',z,z);    // display
		p1->display = (n.at(1) == '1');
	} // props loop

	trace3("done legacy load", c->label(), s, z);
	return c;
}
/*--------------------------------------------------------------------------*/
static TaskElement* loadTaskElement(const QString& _s, TaskElement* c)
{
	if(auto t=dynamic_cast<LegacyTaskElement*>(c)){
		loadLegacyTaskElement(_s, t);
	}else{ untested();
		incomplete();
	}
	return c;
}
/*--------------------------------------------------------------------------*/
Element* LegacySchematicLanguage::parseItem(istream_t& c, Element* e) const
{
	if(auto s=dynamic_cast<DEV_DOT*>(e)){
		return parseCommand(c, s);
	}else{
	}
	QString Line = QString::fromStdString( c.fullString());
	trace1("LegacySchematicLanguage::parseItem", Line);

	QString l = Line.trimmed();
	if(l.isEmpty()) { untested();
		delete e;
		return nullptr;
	}else if( (l.at(0) != '<') || (l.at(l.length()-1) != '>')) { untested();
		incomplete();
		// QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nWrong 'painting' line delimiter!"));
		delete e;
		return nullptr;
	}else{
	}
	l = l.mid(1, l.length()-2);  // cut off start and end character

	if(auto s=dynamic_cast<Symbol*>(e)){
		parseSymbol(c, s);
	}else if(auto t=dynamic_cast<TaskElement*>(e)){
		loadTaskElement(l, t);
	}else if(auto d=dynamic_cast<Diagram*>(e)){
		parseDiagram(d, c);
	}else if(auto s=dynamic_cast<DEV_DOT*>(e)){
		return parseCommand(c, s);
	}else if(auto s=dynamic_cast<LegacyPainting*>(e)){
		::parsePainting(l, s);
	}else if(auto s=dynamic_cast<Element*>(e)){
		// Data here?
		return parseElement(c, s);
	}else{ untested();
		return Language::parseItem(c, e);
	}


	return e; // wrong.
} // parseItem
/*--------------------------------------------------------------------------*/
DEV_DOT* LegacySchematicLanguage::parseCommand(istream_t& c, DEV_DOT* x) const
{
	QString Line = QString::fromStdString(c.fullString());
	trace2("command?", x->s(), Line);
#if 0 // not sure.
	auto scope = x->owner()->subckt();
#else
	auto scope = x->scope();
	if(scope){
	}else{
	}
#endif

#if 0
  if(auto o=dynamic_cast<Element*>(x->owner())){ untested();
	  assert(scope == x->scope());
  }else{ untested();
	  // yikes. getting here from c_get.
	  assert(false);
  }
#endif

	std::string s;
	c.reset();
	c.skipbl();
	c.skip1('<');
	c >> s;

	c.reset();
	c.skipbl();
	c.skip1('<');

	if (!command_dispatcher[s]) {
		trace3("command miss", s, Line, c.cursor());
		//    cmd.skip();
		//    ++here;
		c.reset();
		c.skipbl();
	}else{
	}


	Command::cmdproc(c, scope); // new__instance??

	delete x;
	return nullptr;
}
/*--------------------------------------------------------------------------*/
std::string LegacySchematicLanguage::find_type_in_string(istream_t& c) const
{
	std::string l = c.fullString();
	auto Line = QString::fromStdString(l);

	Line = Line.trimmed();
	if(Line.size() == 0){ untested();
		return "";
	}else if(Line.at(0) != '<') {
		std::string r;
		c >> r;
		return r;
		return "unknown_type";
	}else{
	}

	QString type = Line.section (' ',0,0); // component type
	type.remove (0,1);    // remove leading "<"
	std::string typestring = type.toStdString();
	if('0' <= type.at(0) && type.at(0) <= '9'){
		return "Wire";
	}else if('-' == type.at(0)){
		return "Wire";
	}else if('"' == type.at(0)){
		trace1("findType diagramvariable", c.fullString());
		return "diagramvariable";
	}else{
	}

	// untangle non-commands.
	if(typestring == ".AC"){
		typestring = "AC";
	}else if(typestring == ".TR"){
		typestring = "TR";
	}else if(typestring == ".DC"){
		typestring = "DC";
	}else if(typestring == ".SP"){
		typestring = "SP";
	}else if(typestring == ".SW"){
		typestring = "SW";
	}else if(typestring == "Diagrams>"){
		typestring = "<Diagrams>";
	}else{
	}

	trace2("legsch::findType", c.fullstring(), typestring);
	return typestring;
}
/*--------------------------------------------------------------------------*/
// was Schematic::loadProperties
#if 0
void LegacySchematicLanguage::loadProperties(QTextStream& s_in,
		SchematicSymbol& m) const
{ untested();
	auto stream=&s_in;
	bool ok = true;
	QString Line, cstr, nstr;
	while(!stream->atEnd()) { untested();
		Line = stream->readLine();
		if(Line.at(0) == '<') if(Line.at(1) == '/') return; //?!
		Line = Line.trimmed();
		if(Line.isEmpty()) continue;

		// move up.
		if(Line.size() < 2){ untested();
			throw "incomplete_exception1";
		}else if(Line.at(0) != '<') { untested();
			throw "incomplete_exception1";
		}else if(Line.at(Line.length()-1) != '>') { untested();
			throw "incomplete_exception2";
		}
		Line = Line.mid(1, Line.length()-2);   // cut off start and end character

		cstr = Line.section('=',0,0);    // property type
		nstr = Line.section('=',1,1);    // property value
		if(cstr == "View") { untested();
			incomplete(); // setParameter
		 	QString tmp;
			tmp = nstr.section(',',0,0).toInt(&ok);
			m.setParameter("ViewX1", tmp.toStdString());
			tmp = nstr.section(',',1,1).toInt(&ok);
			m.setParameter("ViewY1", tmp.toStdString());
			tmp = nstr.section(',',2,2).toInt(&ok);
			m.setParameter("ViewX2", tmp.toStdString());
			tmp = nstr.section(',',2,2).toInt(&ok);
			m.setParameter("ViewY2", tmp.toStdString());

		// 					Scale  = nstr.section(',',4,4).toDouble(&ok); if(ok) { untested();
		// 						tmpViewX1 = nstr.section(',',5,5).toInt(&ok); if(ok)
		// 							tmpViewY1 = nstr.section(',',6,6).toInt(&ok); }
		} else if(cstr == "Grid") { untested();
			m.setParameter("GridX", nstr.section(',',0,0).toStdString());
			m.setParameter("GridY", nstr.section(',',1,1).toStdString());
			m.setParameter("GridOn",nstr.section(',',2,2).toStdString());
		}else if(cstr == "DataSet"
		       ||cstr == "Script"
		       ||cstr == "FrameText0"
		       ||cstr == "FrameText1"
		       ||cstr == "FrameText2"
		       ||cstr == "FrameText3"
		       ||cstr == "showFrame"
		       ||cstr == "DataDisplay"
		       ||cstr == "OpenDisplay"){ untested();
			m.setParameter(cstr.toStdString(), nstr.toStdString());
		}
		// else if(cstr == "OpenDisplay") // bool SimOpenDpl
		// else if(cstr == "RunScript") // bool simRunScript
		// else if(cstr == "showFrame") // bool showFrame
		//else if(cstr == "FrameText0") misc::convert2Unicode(Frame_Text0 = nstr);
		//else if(cstr == "FrameText1") misc::convert2Unicode(Frame_Text1 = nstr);
		//else if(cstr == "FrameText2") misc::convert2Unicode(Frame_Text2 = nstr);
		//else if(cstr == "FrameText3") misc::convert2Unicode(Frame_Text3 = nstr);
		else { untested();
			throw "incomplete_exception" + cstr.toStdString();
		}
		if(!ok) { untested();
			incomplete();
					//QObject::tr("Format Error:\nNumber expected in property field!"));
			throw "something wrong";
		}else{ untested();
		}
	}

	throw "QObject::tr(\"Format Error:\n'Property' field is not closed!\")";
}
#endif
/*--------------------------------------------------------------------------*/
class PaintingCommand : public Command{
	void do_it(istream_t& cs, ElementList* s) override{
	  auto fullstring = cs.fullString();
	  trace1("Section", fullstring);

	  qucs::Component* sc = device_dispatcher.clone("subckt_proto");
	  auto* sym = dynamic_cast<SubcktBase*>(sc);
	  assert(sym);
	  sym->new_subckt();
	  sym->set_label(":Paintings:");
	  assert(s);

	  auto lang = language_dispatcher["legacy_lib"];
	  assert(lang);

	  while(true){
		  cs.read_line();
		  if(cs.umatch("</Paintings>")){
			  break;
		  }else{
			  cs.skipbl();
			  lang->new__instance(cs, sym, sym->subckt());
		  }
	  }

	  s->push_back(sym);

	  trace1("find DOT", sym->label());
	  for(auto i : *sym->subckt()){
		  if(auto d = dynamic_cast<DEV_DOT*>(i)){ untested();
			  trace1("DOT incomplete", d->s());
//			  sym->setParam(k, name); //?
		  }else{
		  }
	  }
  }
}d3;
Dispatcher<Command>::INSTALL _p0(&command_dispatcher, "Paintings", &d3);
Dispatcher<Command>::INSTALL _p1(&command_dispatcher, "Paintings>", &d3); // BUG
Dispatcher<Command>::INSTALL _p2(&command_dispatcher, "<Paintings>", &d3); // ...
/*--------------------------------------------------------------------------*/
class DiagramCommand : public Command{
	void do_it(istream_t& cs, ElementList* s) override{
		auto fullstring = cs.fullString();
		trace1("Section", fullstring);

		SubcktBase* sym = nullptr;
		auto p_ = s->find_("main");
		if(p_!=s->end()){
			sym = dynamic_cast<SubcktBase*>(*p_);
			assert(sym);
		}else{ untested();
			Symbol* sc = symbol_dispatcher.clone("subckt_proto");
			sym = dynamic_cast<SubcktBase*>(sc);
			assert(sym);
			sym->set_label("main");
			//sym->set_owner(..);
			s->pushBack(sym);
			assert(s);
		}

		auto lang = &leg_sch; // language_dispatcher["legacy_lib"];
		assert(lang);

		Element* e = sym;

		while(true){
			cs.read_line();
			if(cs.umatch("</Diagrams>")){
				break;
			}else{
				trace2("Diag parse", sym->label(), cs.fullstring());
				cs.skipbl();
				lang->new__instance(cs, sym, e->scope());
			}
		}

		e->scope()->set_owner(sym);
		trace1("Diag parse", e->scope()->size());
	}
}d4;
Dispatcher<Command>::INSTALL p5_(&command_dispatcher, "<Diagrams>", &d4); // ...
/*--------------------------------------------------------------------------*/
class WireCommand : public Command{
	void do_it(istream_t& cs, ElementList* s) override{
		auto fullstring = cs.fullString();
		trace1("WireCommand", fullstring);

		SubcktBase* sym = nullptr;
		auto p_ = s->find_("main");
		if(p_!=s->end()){
			sym = dynamic_cast<SubcktBase*>(*p_);
		}else{
			// "headless" mode
			// create main, but no project.
			//  (defer expansion of components that need a project)
//			Symbol* sc = mainSection.clone();
			Symbol* sc = symbol_dispatcher.clone("subckt_proto");

			sym = dynamic_cast<SubcktBase*>(sc);
			assert(sym);
			sym->set_label("main");
			//sym->set_owner(..);
			s->pushBack(sym);
			assert(s);
		}

		auto lang = language_dispatcher["legacy_lib"];
		assert(lang);

		Element* e = sym;

		while(true){
			cs.read_line();
			if(cs.umatch("</Wires>")){
				break;
			}else{
				cs.skipbl();
				trace2("compcmd", cs.fullstring(), e->scope()->size());
				lang->new__instance(cs, sym, e->scope());
			}
		}

		trace1("find DOT", sym->label());
		for(auto i : *e->scope()){
			if(auto d = dynamic_cast<DEV_DOT*>(i)){ untested();
				trace1("DOT incomplete", d->s());
				//			  sym->setParam(k, name); //?
			}else{
			}
		}
	}
}dw0;
Dispatcher<Command>::INSTALL pw0(&command_dispatcher, "Wires", &dw0);
Dispatcher<Command>::INSTALL pw1(&command_dispatcher, "Wires>", &dw0); // BUG
Dispatcher<Command>::INSTALL pw2(&command_dispatcher, "<Wires>", &dw0); // ...
/*--------------------------------------------------------------------------*/
class DescriptionCommand : public Command{
	void do_it(istream_t& cs, ElementList* s) override{
		auto description = cs.fullString();

		while(true) {
			// this may throw. And it should, if Description is not closed
			cs.read_line();

			description += cs.fullString(); // newline?
			if(cs.umatch("</Description>")) {
				break;
			}else{
			}
		}
		
		trace1("Description", description);
	}
}d5;
Dispatcher<Command>::INSTALL pd0(&command_dispatcher, "Description>", &d5); // BUG
Dispatcher<Command>::INSTALL pd1(&command_dispatcher, "<Description>", &d5); // ...
/*--------------------------------------------------------------------------*/
} // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
