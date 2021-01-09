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
#include "schematic_model.h"
#include "schematic_lang.h"
#include "schematic_model.h" /// hmm
#include "globals.h"
#include "task_element.h"
#include "painting.h"
#include "diagram.h" // BUG
#include "docfmt.h" // BUG
#include "d_dot.h"
#include "components/component.h"
#include "sckt_base.h"

#ifdef DO_TRACE
#include <typeinfo>
#endif
/*--------------------------------------------------------------------------*/
static Element* parseCommand(istream_t& c, DEV_DOT*x)
{
	QString Line = QString::fromStdString(c.fullString());
#if 0 // not sure.
	auto scope = x->owner()->subckt();
#else
	auto scope = x->scope();
	if(scope){
	}else{ untested();
	}
#endif
//	assert(p);

	std::string s;
	c.reset();
	c.skipbl();
	c.skip1('<');
	c >> s;

	// c.reset(here);
	if (!command_dispatcher[s]) { untested();
		unreachable(); // for now
		trace3("uuh", s, Line, c.cursor());
		assert(false);
		//    cmd.skip();
		//    ++here;
	}else{
	}

	//istream_t c(istream_t::_STRING, Line.toStdString());
	c.reset();
	c.skipbl();
	c.skip1('<');
	Command::cmdproc(c, scope);
	// assert(false);

	delete x;
	return nullptr;
}
/*--------------------------------------------------------------------------*/
static void parsePainting(QString Line, Painting*p)
{
	assert(p);
	// BUG: callback
	if(!p->load(Line)) { untested();
		trace2("ERROR parsePainting", Line, p->label());
		incomplete();
		// QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nWrong 'painting' line format!"));
		throw Exception("cannot parse painting");

	}else{
		trace2("parsePainting", Line, p->center());
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
class LegacySchematicLanguage : public SchematicLanguage {
public:
	LegacySchematicLanguage(bool mod=false) : SchematicLanguage(), _lib_mod(mod){
	}
private: // stuff saved from schematic_file.cpp
	Diagram* loadDiagram(Diagram* d, istream_t& /*, DiagramList *List */) const;
private: // stuff from component.cc
	void loadProperties(QTextStream& stream, SchematicSymbol& m) const;
//	Component* parseComponentObsoleteCallback(const QString& _s, Component* c) const;
//	Element* getComponentFromName(QString& Line) const;

private: // overrides
	void parse_top_item(istream_t& stream, SchematicModel* sckt) const override;
	std::string findType(istream_t&) const override;
	Element* parseItem(istream_t&, Element*) const override;
   DEV_DOT* parseCommand(istream_t&, DEV_DOT*) const override;

private: // local/incomplete
	Symbol* parseSymbol(istream_t&, Symbol*) const;
	Element* loadElement_(const QString& _s, Element* e) const;

private:
	void printSymbol(Symbol const*, ostream_t&) const override;
	void printTaskElement(TaskElement const*, ostream_t&) const override;
	void printPainting(Painting const*, ostream_t&) const override;
	void printSubckt(SubcktBase const*, ostream_t&) const override;
   void printDiagram(Symbol const*, ostream_t&) const override {incomplete();}
	bool _lib_mod; // HACK HACK
}d0;
static Dispatcher<DocumentLanguage>::INSTALL
    p0(&language_dispatcher, "leg_sch", &d0);
LegacySchematicLanguage d1(true);
static Dispatcher<DocumentLanguage>::INSTALL
    p1(&language_dispatcher, "legacy_lib", &d1);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static Component* parseComponentObsoleteCallback(const QString& _s, Component* c);
static TaskElement* loadtaskElement(const QString& _s, TaskElement* c);
/*--------------------------------------------------------------------------*/
static std::list<Element*> implicit_hack;
/*--------------------------------------------------------------------------*/
static bool obsolete_wireload(Symbol* w, const QString& sc)
{
	trace1("ow", sc);
	Symbol* sym = w;
	QString s(sc);
	bool ok;

//	if(s.at(0) != '<'){ untested();
//		throw ExceptionCantParse();
//	}else if(s.at(s.length()-1) != '>'){ untested();
//		throw ExceptionCantParse();
//	}
//	s = s.mid(1, s.length()-2);   // cut off start and end character

	QString n;
	n  = s.section(' ',0,0);
	int x1 = n.toInt(&ok);
	if(!ok) { untested();
		return false; // BUG: throw
	}else{
	}
	sym->setParameter("$xposition", std::to_string(x1));

	n  = s.section(' ',1,1);    // y1
	int y1 = n.toInt(&ok);
	if(!ok) { untested();
		return false; // BUG: throw
	}else{
	}
	sym->setParameter("$yposition", std::to_string(y1));

	n  = s.section(' ',2,2);    // x2
	int x2 = n.toInt(&ok);
	if(!ok) { untested();
		return false; // BUG: throw
	}else{
	}

	if(x1<=x2){
		// possibly the case in all legacy files..
	}else{ itested();
		// but not really necessary
	}
	sym->setParameter("deltax", std::to_string(x2 - x1));

	n  = s.section(' ',3,3);    // y2
	int y2 = n.toInt(&ok);
	if(!ok) { untested();
		return false; // BUG: throw
	}else{
	}

	if(y1<=y2){
		// possibly the case in all legacy files
	}else{ untested();
		// nothing wrong with that, really?
	}
	sym->setParameter("deltay", std::to_string(y2 - y1));

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
		auto nx = s.section(' ',5,5);
		auto ny = s.section(' ',6,6);
		auto delta = s.section(' ',7,7);
		auto sth = s.section('"',3,3);

		// nl->setLabel(n, s.section('"',3,3), delta, nx, ny);  // Wire Label

		// not sure what the parameters mean, need to compute label position and
		// node position from it.
		auto nn = s.section('"',3,3);
		trace7("hack push", s, sth, delta, nx, ny, nn, n);
		sym->setParameter("netname", n); // what is this?
		sym->setParameter("delta", delta); // what is this?
		sym->setParameter("nx", nx); // not the node position, maybe the label position?
		sym->setParameter("ny", ny); // not the node position, maybe the label position?
		sym->expand(); //always?
	}else{
	}

	return true;
}
/*--------------------------------------------------------------------------*/
void LegacySchematicLanguage::parse_top_item(istream_t& stream, SchematicModel* sckt) const
{
	QString Line;
	assert(sckt);
	Symbol* owner = nullptr;

	new__instance(stream, owner, sckt);
}
/*--------------------------------------------------------------------------*/
Diagram* LegacySchematicLanguage::loadDiagram(Diagram* d, istream_t& stream)const
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
		if(diagram_dispatcher[what.c_str()+1]){
			// d=prechecked_cast<Diagram*>(x->clone());
			assert(d);
			qDebug() << "got diagram" << what.c_str();
			d->setName(type); // yuck
		}else{ untested();
			trace1("diagram doesntexist", type);
			incomplete();
			// throw ...
			return nullptr;
		}

		if(!d->load(Line, stream)) { untested();
			trace1("loadfail", Line);
			incomplete();
			delete d;
			return nullptr;
		}else{
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
void LegacySchematicLanguage::printSubckt(SubcktBase const*, ostream_t&) const
{ untested();
	unreachable();
}
/*--------------------------------------------------------------------------*/
void LegacySchematicLanguage::printTaskElement(TaskElement const* c, ostream_t& s) const
{
	s << "  <." << c->Name << " ";

	if(c->label()==""){
		s << "MISSING_LABEL";
		unreachable();
	}else{
		s << c->label();
	}
	s << " ";

	int i = 0;
	if(!c->showName){ untested();
		i = 4;
	}else{
	}
	std::string active = c->paramValue("$mfactor");
	i |= atoi(active.c_str());
	s << std::to_string(i);
	s << " "+QString::number(c->cx())+" "+QString::number(c->cy());
	s << " "+QString::number(c->tx)+" "+QString::number(c->ty);
	s << " ";
	s << " 0"; // mirrored
	s << " 0"; // rotated

	// write all properties
	// FIXME: ask element for properties, not for dictionary
	auto cc=const_cast<TaskElement*>(c); // BUGBUGBUGBUG
	// cannot access Props without this hack
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

	s << ">\n";
}
/*--------------------------------------------------------------------------*/
void LegacySchematicLanguage::printPainting(Painting const* pp, ostream_t& s) const
{
	auto mp = const_cast<Painting*>(pp);
	s << "  <" << mp->save() << ">\n";
}
/*--------------------------------------------------------------------------*/
static void printArgs(Symbol const* sym, ostream_t& s)
{
	s << " " << sym->paramValue("$mfactor");
	s << " " << sym->paramValue("$xposition");
	s << " " << sym->paramValue("$yposition");
	s << " " << sym->paramValue("$tx");
	s << " " << sym->paramValue("$ty");
	int angle = atoi(sym->paramValue("$angle").c_str());
	int hflip = atoi(sym->paramValue("$hflip").c_str());
	int vflip = atoi(sym->paramValue("$vflip").c_str());
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

	int show = atoi(sym->paramValue("$param_display").c_str());
	for(unsigned i=0; i<sym->paramCount(); ++i){
		trace3("display", i, show, sym->paramName(i));
		// if(sym->paramIsPrintabl(i))
		std::string n = sym->paramName(i);
		if(n.at(0)=='$'){
		}else{
			s << " \"" << sym->paramValue(i) << "\" " << (show % 2);
		}
		show /= 2;
	}
}
static void printwirehack(Symbol const* w, ostream_t& d)
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

//  int cx = atoi(sym->paramValue("$xposition").c_str());
//  int cy = atoi(sym->paramValue("$yposition").c_str());
//  int dx = atoi(sym->paramValue("deltax").c_str());
//  int dy = atoi(sym->paramValue("deltay").c_str());

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
// was: void Schematic::saveComponent(QTextStream& s, Component const* c) const
void LegacySchematicLanguage::printSymbol(Symbol const* sym, ostream_t& s) const
{
	trace1("printSymbol", sym->typeName());
	if(sym->typeName()=="wire"){
		// hack hack
		return printwirehack(sym, s);
	}else{
	}
	s << "  <" << mangle(sym->typeName()) << " ";

	if(sym->label()==""){ untested();
		s << "*";
	}else{
		s << sym->label();
	}

	Component const* c=dynamic_cast<Component const*>(sym);
	Component* cc=const_cast<Component*>(c); // BUGBUGBUGBUG
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
		if(c->mirroredX){
			s << "1";
		}else{
			s << "0";
		}
		s << " " << QString::number(c->rotated());
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

static TaskElement* loadtaskElement(const QString& _s, TaskElement* c)
{
	trace1("loadtaskElement", c->label());
	bool ok;
	int  ttx, tty, tmp;
	QString s = _s;

	if(s.at(0) != '<'){
	}else if(s.at(s.length()-1) != '>'){ untested();
	}else{ untested();
		s = s.mid(1, s.length()-2);   // cut off start and end character
	}

	if(1){

		QString label=s.section(' ',1,1);
		trace1("TASK NAME", label);
///		c->setName(label);//???
		c->setLabel(label.toStdString());

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

		/// BUG FIXME. dont use Component parameter dictionary.
		for(; tmp<=(int)counts/2; tmp++){
			c->Props.append(new Property("p", "", true, " "));
		}

		// load all properties
		Property *p1;
		qDebug() << "load command props" << s;
		for(p1 = c->Props.first(); p1 != 0; p1 = c->Props.next()) {
			qDebug() << "load command props" << z;
			z++;
			n = s.section('"',z,z);    // property value
			z++;
			//qDebug() << "LOAD: " << p1->Description;

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

		return c;
	}
}
/*--------------------------------------------------------------------------*/
// decluttered parseComponentObsoleteCallback
Symbol* LegacySchematicLanguage::parseSymbol(istream_t& cs, Symbol* sym) const
{
	QString Line = QString::fromStdString( cs.fullString());
	trace1("LegacySchematicLanguage::parseItem", Line);

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

	if(sym->typeName()=="wire"){
		// yikes.
		obsolete_wireload(sym, s);
	}else if(auto cc=dynamic_cast<Component*>(sym)){
		trace2("compon callback", s, cc->label());
		// HACK
		parseComponentObsoleteCallback(s, cc);
	}else{

		QString label=s.section(' ',1,1);
		sym->setLabel(label.toStdString());

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
			throw Exception("xposition parse");
		}else{
			sym->setParameter("$xposition", std::to_string(cx));
			//	sym->setParameter(3, std::to_string(cx));
		}

		n  = s.section(' ',4,4);    // cy
		int cy=n.toInt(&ok);
		if(!ok){ untested();
			throw Exception("yposition parse");
		}else{
			sym->setParameter("$yposition", std::to_string(cy));
			//	sym->setParameter(4, std::to_string(cy));
		}

		n  = s.section(' ',5,5);    // tx
		tmp = n.toInt(&ok);
		if(!ok){ untested();
			throw Exception("tx parse");
		}else{
			(void)tmp;
			//		sym->setParameter("$ttx", std::to_string(tmp));
		}

		n  = s.section(' ',6,6);    // ty
		tmp = n.toInt(&ok);
		if(!ok){ untested();
			throw Exception("ty parse");
		}else{
			(void)tmp;
			//		sym->setParameter("$tty", std::to_string(tmp));
		}

		{
			n  = s.section(' ',7,7);    // mirror y axis
			int nn = n.toInt(&ok);
			if(!ok){ untested();
				throw Exception("vflip parse");
			}else{
				int vflip = 1-2*nn;
				assert(vflip==1 || vflip==-1);
				sym->setParameter("$vflip", std::to_string(vflip));
			}

			n  = s.section(' ',8,8);    // rotated
			tmp = n.toInt(&ok);
			if(!ok){ untested();
				return NULL;
			}else{
			}

			tmp *= 90;
			sym->setParameter("$angle", std::to_string(tmp));
			assert(sym->paramValue("$angle") == std::to_string(tmp));
		}

		// set parameters.
		unsigned position = 0;

		// skip the first 4. x y tx ty.
		unsigned offset = 4; // Symbol::paramCountBase()
		unsigned int z=0;
		int counts = s.count('"');
		trace2("set?", s, counts);
		for(; int(position)<counts/2; ++ position){
			z++;
			n = s.section('"',z,z);    // property value. gaah parse over and over again?
			z++;

			trace2("legacy:set", position, n);
			try{
				sym->setParameter(position + offset, n);
			}catch(ExceptionCantFind const*){ untested();
				incomplete(); // CS has error messages...
				error(5, "cannot parse Symbol param " +
						std::to_string(position + offset) + " in " + sym->label());
				throw; // BUG
			}

			n  = s.section('"',z,z);    // display
		}

		trace1("done sym parse", sym->label());
	}

	// setPort?
	auto Scope = sym->scope();
	assert(Scope);
	for(unsigned i=0; i<sym->numPorts(); ++i){
		sym->connectNode(i, Scope->nodes());
	}

	return sym;
} // parseSymbol

// BUG raise exceptions if something goes wrong.
static Component* parseComponentObsoleteCallback(const QString& _s, Component* c)
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

	QString label=s.section(' ',1,1);
	c->obsolete_name_override_hack(label); //??
	trace1("loadComp", label);
	c->setLabel(label.toStdString());

	QString n;
	n  = s.section(' ',2,2);      // flags
	tmp = n.toInt(&ok);
	if(!ok){ untested();
		return NULL;
	}
	c->isActive = tmp & 3;
	// sym->setParameter("is_active", std::to_string(tmp & 3));

	if(tmp & 4){itested();
	// sym->setParameter("hide_label", std::to_string(tmp & 4));
		c->showName = false;
	}else{
		// use default, e.g. never show name for GND (bug?)
	}

	n  = s.section(' ',3,3);    // cx
	int cx=n.toInt(&ok);
	qDebug() << "cx" << cx;
	sym->setParameter("$xposition", std::to_string(cx));
//	sym->setParameter(3, std::to_string(cx));
	if(!ok) return NULL;

	n  = s.section(' ',4,4);    // cy
	int cy=n.toInt(&ok);
	sym->setParameter("$yposition", std::to_string(cy));
//	sym->setParameter(4, std::to_string(cy));
	if(!ok) return NULL;

	n  = s.section(' ',5,5);    // tx
	ttx = n.toInt(&ok);
	if(!ok) return NULL;

	n  = s.section(' ',6,6);    // ty
	tty = n.toInt(&ok);
	if(!ok) return NULL;

	assert(c);
//	assert(c->obsolete_model_hack().at(0) != '.');

	{
		n  = s.section(' ',7,7);    // mirror y axis
		if(n.toInt(&ok) == 1){
			c->mirrorX();
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
		sym->setParameter("$angle", std::to_string(tmp));
		if(sym->paramValue("$angle") == std::to_string(tmp)){
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
	}else if(Model == "EDD"){ untested();
		tmp = 5;
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
	unsigned position=4;
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
		if(Model != "EDD" && Model != "RFEDD" && Model != "RFEDD2P")
			if(p1->Description.isEmpty()) {  // unknown number of properties ?
				p1->Name = n.section('=',0,0);
				n = n.section('=',1);
				// allocate memory for a new property (e.g. for equations)
				if(int(c->Props.count()) < (int(counts)>>1)) {
					c->Props.insert(z >> 1, new Property("y", "1", true));
					c->Props.prev();
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
		sym->setParameter(position++, n); // BUG: also do for non-Components.

		n  = s.section('"',z,z);    // display
		p1->display = (n.at(1) == '1');
	}

	trace1("done legacy load", c->label());
	return c;
}
/*--------------------------------------------------------------------------*/
Element* LegacySchematicLanguage::parseItem(istream_t& c, Element* e) const
{
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
		loadtaskElement(l, t);
	}else if(auto d=dynamic_cast<Diagram*>(e)){
		loadDiagram(d, c);
	}else if(auto s=dynamic_cast<Painting*>(e)){
		::parsePainting(l, s);
	}else if(auto s=dynamic_cast<DEV_DOT*>(e)){
		return ::parseCommand(c, s);
	}else{ untested();
		return DocumentLanguage::parseItem(c, e);
	}


	return e; // wrong.
} // parseItem
/*--------------------------------------------------------------------------*/
DEV_DOT* LegacySchematicLanguage::parseCommand(istream_t& c, DEV_DOT* x) const
{ untested();
  assert(x);
  // x->set(cmd.fullstring());
  DEV_DOT const* cx = x;
  assert(cx->owner());
#if 0 // not sure.
  auto scope = x->owner()->subckt();
#else
  auto scope = x->scope();
#endif

//  cmd.reset();
//  skip_pre_stuff(cmd);

  size_t here = c.cursor();

  std::string s;
  c >> s;
  trace2("ctos?", c.fullString(), s);
  c.reset(here);
  if (!command_dispatcher[s]) { untested();
	  unreachable(); // for now
	  trace2("uuh", s, c.fullString());
	  assert(false);
  }else{ untested();
  }
  Command::cmdproc(c, scope); // where is x->owner gone?

  delete x;
  return NULL;
}
/*--------------------------------------------------------------------------*/
std::string LegacySchematicLanguage::findType(istream_t& c) const
{
	std::string l = c.fullString();
	auto Line = QString::fromStdString(l);

	Line = Line.trimmed();
	if(Line.at(0) != '<') {
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
	}else{
		trace3("findType", c.fullString(), typestring, type.at(0));
	}

	// untangle non-commands.
	if(typestring == ".AC"){
		return "AC";
	}else if(typestring == ".TR"){
		return "TR";
	}else if(typestring == ".DC"){
		return "DC";
	}else if(typestring == ".SP"){
		return "SP";
	}else if(typestring == ".SW"){
		return "SW";
	}else{
		return typestring;
	}

}
/*--------------------------------------------------------------------------*/
// findProto??
#if 0
Element* LegacySchematicLanguage::getComponentFromName(QString& Line) const
{ untested();
	qDebug() << "component?" << Line;
	Element *e = nullptr;

	Line = Line.trimmed();
	if(Line.at(0) != '<') { untested();
		throw "notyet_exception"
			"Format Error:\nWrong line start!";
	}else{ untested();
	}

	QString type = Line.section (' ',0,0); // component type
	type.remove (0,1);    // remove leading "<"
	std::string typestring = type.toStdString();

///	// TODO: get rid of the exceptional cases.
	if (type == "Eqn"){ untested();
		incomplete();
		// c = new Equation ();
	}else if (type == "SPICE"){ untested();
		incomplete();
		// c = new SpiceFile();
	}else if (type.left (6) == "SPfile" && type != "SPfile"){ untested();
		incomplete();
		// backward compatible
		//c = new SParamFile ();
		//c->Props.getLast()->Value = type.mid (6);
	}

	// BUG: findProto
	Element const* s = symbol_dispatcher[typestring];
	if(!s){ untested();
		s = element_dispatcher[typestring];
	}else{ untested();
	}

	trace2("get", typestring, s);

	if(Component const* sc=dynamic_cast<Component const*>(s)){ untested();
		// legacy component
		Element* k = sc->clone(); // memory leak?
		e = prechecked_cast<Element*>(k);

		// set_type?
	}else if(TaskElement const* sc=dynamic_cast<TaskElement const*>(s)){ untested();
		trace1("TaskElement", Line);
		Element* k = sc->clone(); // memory leak?
		e = prechecked_cast<Element*>(k);
	}else if(typestring.size() == 0){ untested();
		incomplete();
	}else if(typestring.c_str()[0] == '.'){ untested();
		std::string type = typestring.substr(1); // drop dot.
		e = element_dispatcher.clone(type);
		if(auto ce = dynamic_cast<TaskElement*>(e)){ untested();
			// should use setType lower down. drop name.
			ce->setTypeName(QString::fromStdString(type));
		}else{ untested();
			untested();
		}
	}else if(s){ untested();
		e = s->clone_instance();
	}else{ untested();
	}

	if(e) { untested();
		trace1("e bug", Line);
		incomplete();
		loadElement_(Line, e);
		// setType()
	}else{ untested();
		qDebug() << "error with" << type;
		message(QucsMsgWarning,
			"Format Error:\nUnknown component!\n"
			"%1\n\n"
			"Do you want to load schematic anyway?\n"
			"Unknown components will be replaced \n"
			"by dummy subcircuit placeholders.");

		incomplete();
		// c = new Subcircuit();
		// // Hack: insert dummy File property before the first property
		// int pos1 = Line.indexOf('"');
		// QString filestr = QString("\"%1.sch\" 1 ").arg(cstr);
		// Line.insert(pos1,filestr);
	}


#if 0 // legacy cruft?
	// BUG: don't use schematic.
	if(TaskElement* cmd=command(e)){ untested();
		p->loadtaskElement(Line, cmd);
	}else if(Component* c=component(e)){ untested();
		if(!p->parseComponentObsoleteCallback(Line, c)) { untested();
			QMessageBox::critical(0, QObject::tr("Error"),
					QObject::tr("Format Error:\nWrong 'component' line format!"));
			delete e;
			return 0;
		}else{ untested();
		}
		cstr = c->name();   // is perhaps changed in "recreate" (e.g. subcircuit)
		int x = c->tx, y = c->ty;
		c->setSchematic (p);
		c->recreate(0);
		c->obsolete_name_override_hack(cstr);
		c->tx = x;  c->ty = y;
	}
#endif

	return e;
}
#endif
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
	void do_it(istream_t& cs, SchematicModel* s) override{
	  auto fullstring = cs.fullString();
	  trace1("Section", fullstring);

	  Symbol* sc = symbol_dispatcher.clone("subckt_proto");
	  auto* sym = dynamic_cast<SubcktBase*>(sc);
	  assert(sym);
	  sym->new_subckt();
	  sym->setLabel(":Paintings:");
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

	  s->pushBack(sym);

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
	void do_it(istream_t& cs, SchematicModel* s) override{
		auto fullstring = cs.fullString();
		trace1("Section", fullstring);

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
			sym->setLabel("main");
			//sym->setOwner(..);
			s->pushBack(sym);
			assert(s);
		}

		auto lang = language_dispatcher["legacy_lib"];
		assert(lang);


		while(true){
			cs.read_line();
			if(cs.umatch("</Diagrams>")){
				break;
			}else{
				trace2("Diag parse", sym->label(), cs.fullstring());
				cs.skipbl();
				lang->new__instance(cs, sym, sym->scope());
			}
		}
		trace1("Diag parse", sym->subckt()->size());
	}
}d4;
Dispatcher<Command>::INSTALL p3_(&command_dispatcher, "Diagrams", &d4);
Dispatcher<Command>::INSTALL p4_(&command_dispatcher, "Diagrams>", &d4); // BUG
Dispatcher<Command>::INSTALL p5_(&command_dispatcher, "<Diagrams>", &d4); // ...
/*--------------------------------------------------------------------------*/
} // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
