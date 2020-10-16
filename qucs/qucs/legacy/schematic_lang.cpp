// Felix 2018, GPLv3+
// Part of QUCS

#include "qucsdoc.h"
#include "schematic_model.h"
#include "schematic_lang.h"
#include "schematic_symbol.h"
#include "schematic_model.h" /// hmm
#include "globals.h"
#include "command.h"
#include "paintings/painting.h"
#include "diagram.h" // BUG
#include "docfmt.h" // BUG

#ifdef DO_TRACE
#include <typeinfo>
#endif

bool PaintingList::load(QTextStream& str)
{ untested();
	auto stream=&str;
	auto List=this;
	Painting *p=0;
	QString Line, cstr;
	while(!stream->atEnd()) { untested();
		Line = stream->readLine();
		if(Line.at(0) == '<') if(Line.at(1) == '/') return true;

		Line = Line.trimmed();
		if(Line.isEmpty()) continue;
		if( (Line.at(0) != '<') || (Line.at(Line.length()-1) != '>')) { untested();
			incomplete();
			// QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nWrong 'painting' line delimiter!"));
			return false;
		}else{ untested();
		}
		Line = Line.mid(1, Line.length()-2);  // cut off start and end character

		cstr = Line.section(' ',0,0);    // painting type
		qDebug() << cstr;
		if(Painting const* pp=painting_dispatcher[cstr.toStdString()]){ untested();
			p=prechecked_cast<Painting*>(pp->clone());
			assert(p);
		}else{ untested();
			// incomplete();
			// QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nUnknown painting " + cstr));
			return false;
		}

		if(!p->load(Line)) { untested();
			incomplete();
			// QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nWrong 'painting' line format!"));
			delete p;
			return false;
		}else{ untested();
		}
		qDebug() << "got painting" << cstr << p->name();
		List->append(p);
	}

	incomplete();
	// QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\n'Painting' field is not closed!"));
	return false;
}

namespace{

class LegacySchematicLanguage : public SchematicLanguage {
public:
	LegacySchematicLanguage() : SchematicLanguage(){ untested();
	}
private: // stuff saved from schematic_file.cpp
	Diagram* loadDiagram(QString const& Line, DocumentStream& /*, DiagramList *List */) const;
private: // stuff from component.cc
	void loadProperties(QTextStream& stream, SchematicSymbol& m) const;
	Component* parseComponentObsoleteCallback(const QString& _s, Component* c) const;
	CmdElement* loadCommand(const QString& _s, CmdElement* c) const;
	Element* loadElement(const QString& _s, Element* e) const;
	Element* getComponentFromName(QString& Line) const;
private: // overrides
	// BUG: need parseItem
	void parse(DocumentStream& stream, SchematicSymbol& s) const;

private:
	void printSymbol(Symbol const*, stream_t&) const override;
	void printCommand(CmdElement const*, stream_t&) const override;
	void printPainting(Painting const*, stream_t&) const override {incomplete();}
   void printDiagram(Symbol const*, stream_t&) const override {incomplete();}
}defaultSchematicLanguage_;
static Dispatcher<DocumentLanguage>::INSTALL
    p(&doclang_dispatcher, "leg_sch", &defaultSchematicLanguage_);
// ------------------------------------------------------
// // "parseElement"?
Element* LegacySchematicLanguage::loadElement(const QString& _s, Element* e) const
{ untested();
	if(CmdElement* c=dynamic_cast<CmdElement*>(e)){ untested();
		c = loadCommand(_s, c);
		// incomplete();
	}else if(Component* c=dynamic_cast<Component*>(e)){ untested();
		// legacy components
		// will not work non-qucs-.sch languages
		incomplete();
		c = parseComponentObsoleteCallback(_s, c);

		QString cstr = c->name();   // is perhaps changed in "recreate" (e.g. subcircuit)
		int x = c->tx;
		int y = c->ty;
		// c->setSchematic (p);
	//	s->recreate(); // half-expand subcircuits? why not "precalc"?
	//	               // why not in constructor? it needs parameters.
		c->setLabel(cstr);
		c->tx = x;
		c->ty = y;
	}else{ untested();
		incomplete();
		return e;
	}
	return nullptr;
}

static std::list<Element*> implicit_hack;

static bool obsolete_wireload(Symbol* w, const QString& sc)
{ untested();
	Symbol* sym = w;
	QString s(sc);
	trace1("obsolete_wireload", s);
	bool ok;

	if(s.at(0) != '<'){ untested();
		throw ExceptionCantParse();
	}else if(s.at(s.length()-1) != '>'){ untested();
		throw ExceptionCantParse();
	}
	s = s.mid(1, s.length()-2);   // cut off start and end character

	QString n;
	n  = s.section(' ',0,0);
	int x1 = n.toInt(&ok);
	if(!ok) { untested();
		return false; // BUG: throw
	}else{ untested();
	}
	sym->setParameter("$xposition", std::to_string(x1));

	n  = s.section(' ',1,1);    // y1
	int y1 = n.toInt(&ok);
	if(!ok) { untested();
		return false; // BUG: throw
	}else{ untested();
	}
	sym->setParameter("$yposition", std::to_string(y1));

	n  = s.section(' ',2,2);    // x2
	int x2 = n.toInt(&ok);
	if(!ok) { untested();
		return false; // BUG: throw
	}else{ untested();
	}

	assert(x1<=x2); // possibly the case in all legacy files
	sym->setParameter("deltax", std::to_string(x2 - x1));

	n  = s.section(' ',3,3);    // y2
	int y2 = n.toInt(&ok);
	if(!ok) { untested();
		return false; // BUG: throw
	}else{ untested();
	}

	if(y1<=y2){ untested();
		// possibly the case in all legacy files
	}else{ untested();
		// nothing wrong with that, really?
	}
	sym->setParameter("deltay", std::to_string(y2 - y1));

	n = s.section('"',1,1);
	trace1("parse node label", n);
	if(!n.isEmpty()) { untested();
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
	}else{ untested();
	}

	return true;
}

// BUG: this is schematicFormat
void LegacySchematicLanguage::parse(DocumentStream& stream, SchematicSymbol& owner) const
{ untested();
	assert(!implicit_hack.size());
	QString Line;

	// mode: a throwback to the legacy format:
	//       connect legacy "parsers".
	// this is not needed in a proper SchematicLanguage
	char mode='\0';
	while(!stream.atEnd()) { untested();
		Line = stream.readLine();
		Line = Line.trimmed();
		if(Line.size()<2){ untested();
		}else if(Line.at(0) == '<'
				&& Line.at(1) == '/'){ untested();
			qDebug() << "endtag?" << Line;
		}else if(Line.isEmpty()){ untested();
		}else if(Line == "<Components>") { untested();
			mode='C';
		}else if(Line == "<Symbol>") { untested();
			mode='S';
		}else if(Line == "<Wires>") { untested();
			mode='W';
		}else if(Line == "<Diagrams>") { untested();
			mode='D';
		}else if(Line == "<Properties>") { untested();
			mode='Q';
		}else if(Line == "<Paintings>") { untested();
			mode='P';
		}else{ untested();

			/// \todo enable user to load partial schematic, skip unknown components
			Element*c=nullptr;
			if(mode=='C'){ untested();
				c = getComponentFromName(Line);
				if(c){ untested();
					c->setOwner(&owner);
				}else{ untested();
				}
				if(Symbol* sym=dynamic_cast<Symbol*>(c) ){ untested();
					//always do this?

//					assert(s.scope());
					// what are those?!
					sym->recreate(); // re? create symbol gfx and random other things. needs owner
					sym->build(); // what's this?!
				}else{ untested();
				}
			}else if(mode=='S'){ untested();
				incomplete();
				try{ untested();
					qDebug() << "symbol Paintings";
					owner.symbolPaintings().load(stream);
					c = nullptr;
				}catch(...){ untested();
					incomplete();
				}
			}else if(mode=='W'){ untested();
				qDebug() << "wire parse?" << Line;
				Symbol* sw= symbol_dispatcher.clone("Wire");
				assert(sw);
				sw->setOwner(&owner);
				bool err = obsolete_wireload(sw, Line);
				if(!err){ untested();
					incomplete();
					delete(sw);
				}else{ untested();
					c = sw;
				}
			}else if(mode=='D'){ untested();
				trace1("diagram parse?", Line);

				Diagram* d=loadDiagram(Line, stream);
				if(d){ untested();
					c = d;
					c->setOwner(&owner);
				}else{ untested();
					incomplete();
				}

			}else if(mode=='Q'){ untested();

			}else{ untested();
				qDebug() << "LSL::parse" <<  Line;
				incomplete();
			}

			assert(owner.subckt());
			if(c){itested();
				trace2("pushing back", c->label(), typeid(*c).name());
				Element const* cc = c;
				assert(cc->owner() == &owner);
				owner.subckt()->pushBack(c);
			}else{itested();
			}
		}
	}

	/// some components are implicit, collect them here
	assert(owner.subckt());
	while(implicit_hack.size()){ untested();
		auto cc = implicit_hack.front();
		cc->setOwner(&owner);
		implicit_hack.pop_front();
		owner.subckt()->pushBack(cc);
	}
}

Diagram* LegacySchematicLanguage::loadDiagram(QString const& line_in,
		DocumentStream& stream)const
{itested();
	Diagram *d;
	QString Line=line_in;
	QString cstr;
	if(!stream.atEnd()) {itested();
		trace1("diagram?", Line);
		if(Line.at(0) == '<' && Line.at(1) == '/'){ untested();
			return nullptr;
		}else{ untested();
			untested();
		}
		Line = Line.trimmed();
		if(Line.isEmpty()){ untested();
			return nullptr;
		}else{ untested();
		}
		cstr = Line.section(' ',0,0);    // diagram type
		std::string what=cstr.toStdString();

		auto type = what.c_str()+1;
		if(auto x=diagram_dispatcher[what.c_str()+1]){itested();
			d=prechecked_cast<Diagram*>(x->clone());
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
		}else{ untested();
		}
		return d;
	}

	return nullptr;
}
//
//
// -------------------------------------------------------
static const std::string typesep(":");
static std::string mangle(std::string t)
{ untested();
	auto pos = t.find(typesep);
	std::string ret="";
	return t.substr(0, pos);
}

void LegacySchematicLanguage::printCommand(CmdElement const* c, stream_t& s) const
{ untested();
	s << "  <." << c->Name << " ";

	{ untested();
		s << c->label();
	}
	s << " ";

	int i = 0;
	if(!c->showName){ untested();
		i = 4;
	}else{ untested();
	}
	i |= c->isActive;
	s << QString::number(i);
	s << " "+QString::number(c->cx())+" "+QString::number(c->cy());
	s << " "+QString::number(c->tx)+" "+QString::number(c->ty);
	s << " ";
	s << " 0"; // mirrored
	s << " 0"; // rotated

	// write all properties
	// FIXME: ask element for properties, not for dictionary
	auto cc=const_cast<CmdElement*>(c); // BUGBUGBUGBUG
	// cannot access Props without this hack
	for(Property *p1 = cc->Props.first(); p1 != 0; p1 = cc->Props.next()) { untested();
		if(p1->Description.isEmpty()){ untested();
			s << " \""+p1->Name+"="+p1->Value+"\"";   // e.g. for equations
		}else{ untested();
			s << " \""+p1->Value+"\"";
		}
		s << " ";
		if(p1->display){ untested();
			s << "1";
		}else{ untested();
			s << "0";
		}
	}

	s << ">";
}

static void printArgs(Symbol const* sym, stream_t& s)
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
	if(hflip==1){ untested();
		s << " " << (1+vflip) / 2;
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
	for(unsigned i=0; i<sym->paramCount(); ++i){ untested();
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
// was: void Schematic::saveComponent(QTextStream& s, Component const* c) const
void LegacySchematicLanguage::printSymbol(Symbol const* sym, stream_t& s) const
{ untested();
	s << "  <" << mangle(sym->typeName()) << " ";

	if(sym->label()==""){ untested();
		s << "*";
	}else{
		s << sym->label();
	}

	Component const* c=dynamic_cast<Component const*>(sym);
	Component* cc=const_cast<Component*>(c); // BUGBUGBUGBUG
	if(c && c->useObsoleteProps()){ untested();
		s << " ";
		int i=0;
		if(!c->showName){ untested();
			i = 4;
		}else{ untested();
		}
		i |= c->isActive;
		s << QString::number(i);
		s << " "+QString::number(c->cx())+" "+QString::number(c->cy());
		s << " "+QString::number(c->tx)+" "+QString::number(c->ty);
		s << " ";
		if(c->mirroredX){ untested();
			s << "1";
		}else{
			s << "0";
		}
		s << " " << QString::number(c->rotated());
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
	}else{ untested();
		printArgs(sym, s);
	}

	s << ">";
} // printSymbol

CmdElement* LegacySchematicLanguage::loadCommand(const QString& _s, CmdElement* c) const
{ untested();
	bool ok;
	int  ttx, tty, tmp;
	QString s = _s;

	if(s.at(0) != '<'){ untested();
		return NULL;
	}else if(s.at(s.length()-1) != '>'){ untested();
		return NULL;
	}else{ untested();
		s = s.mid(1, s.length()-2);   // cut off start and end character

		QString label=s.section(' ',1,1);
		trace1("NAME", label);
///		c->setName(label);//???
		c->setLabel(label);

		QString n;
		n  = s.section(' ',2,2);      // isActive
		tmp = n.toInt(&ok);
		if(!ok){ untested();
			return NULL;
		}
		c->isActive = tmp & 3;

		if(tmp & 4){ untested();
			c->showName = false;
		}else{ untested();
			// use default, e.g. never show name for GND (bug?)
		}

		n  = s.section(' ',3,3);    // cx
		int cx = n.toInt(&ok);
		if(!ok) return NULL;

		n  = s.section(' ',4,4);    // cy
		c->setCenter(cx, n.toInt(&ok));
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

		unsigned int z=0, counts = s.count('"');
		// FIXME. use c->paramCount()

		/// BUG FIXME. dont use Component parameter dictionary.
		for(; tmp<=(int)counts/2; tmp++){ untested();
			c->Props.append(new Property("p", "", true, " "));
		}

		// load all properties
		Property *p1;
		qDebug() << "load command props" << s;
		for(p1 = c->Props.first(); p1 != 0; p1 = c->Props.next()) { untested();
			qDebug() << "load command props" << z;
			z++;
			n = s.section('"',z,z);    // property value
			z++;
			//qDebug() << "LOAD: " << p1->Description;

			// not all properties have to be mentioned (backward compatible)
			if(z > counts) { untested();
				if(p1->Description.isEmpty()){ untested();
					c->Props.remove();    // remove if allocated in vain
				}else{ untested();
				}

				return c;
			}else{ untested();
			}

			p1->Value = n;

			n  = s.section('"',z,z);    // display
			p1->display = (n.at(1) == '1');
		}

		return c;
	}
}

// BUG raise exceptions if something goes wrong.
Component* LegacySchematicLanguage::parseComponentObsoleteCallback(const QString& _s, Component* c) const
{ untested();
	Symbol* sym = c;
	qDebug() << "parseComponentObsoleteCallback" << _s;
	bool ok;
	int  ttx, tty, tmp;
	QString s = _s;

	if(s.at(0) != '<'){ untested();
		return NULL;
	}else if(s.at(s.length()-1) != '>'){ untested();
		return NULL;
	}
	s = s.mid(1, s.length()-2);   // cut off start and end character

	QString label=s.section(' ',1,1);
	c->obsolete_name_override_hack(label); //??
	trace1("loadComp", label);
	c->setLabel(label);

	QString n;
	n  = s.section(' ',2,2);      // flags
	tmp = n.toInt(&ok);
	if(!ok){ untested();
		return NULL;
	}
	c->isActive = tmp & 3;
	// sym->setParameter("is_active", std::to_string(tmp & 3));

	if(tmp & 4){ untested();
	// sym->setParameter("hide_label", std::to_string(tmp & 4));
		c->showName = false;
	}else{ untested();
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
	assert(c->obsolete_model_hack().at(0) != '.');

	{ untested();
		n  = s.section(' ',7,7);    // mirror y axis
		if(n.toInt(&ok) == 1){ untested();
			c->mirrorX();
		}
		if(!ok) return NULL;

		n  = s.section(' ',8,8);    // rotated
		tmp = n.toInt(&ok);
		if(!ok){ untested();
			return NULL;
		}else if(int(c->rotated()) > tmp){ untested();
		  	// neccessary because of historical flaw in ...
		  	// ... components like "volt_dc"
			// ????
			tmp += 4;
		}

		tmp *= 90;
		sym->setParameter("$angle", std::to_string(tmp));
		assert(sym->paramValue("$angle") == std::to_string(tmp));
		trace2("DBG rot", c->rotated(), n);
	}

	c->tx = ttx;
	c->ty = tty; // restore text position (was changed by rotate/mirror)

	QString Model = c->obsolete_model_hack(); // BUG: don't use names

	unsigned int z=0, counts = s.count('"');
	// FIXME. use c->paramCount()
	if(Model == "Sub"){ untested();
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
	}else{ untested();
		// "+1" because "counts" could be zero
		tmp = counts + 1;
	}
//	assert(tmp == c->ParamPositionHack()); //?

	/// BUG FIXME. dont use Component parameter dictionary.
	for(; tmp<=(int)counts/2; tmp++){ untested();
		c->Props.append(new Property("p", "", true, " "));
		//	sym->setParameter("p__" + std::to_string(tmp), "");
	}

	// set parameters.
	Property *p1;
	unsigned position=0;
	for(p1 = c->Props.first(); p1 != 0; p1 = c->Props.next()) { untested();
		z++;
		n = s.section('"',z,z);    // property value. gaah parse over and over again?
		z++;
		//qDebug() << "LOAD: " << p1->Description;

		// not all properties have to be mentioned (backward compatible)
		if(z > counts) { untested();
			if(p1->Description.isEmpty()){ untested();
				c->Props.remove();    // remove if allocated in vain
			}else{ untested();
			}

			if(Model == "Diode") { // BUG: don't use names
				incomplete();
				if(counts < 56) {  // backward compatible
					counts >>= 1;
					p1 = c->Props.at(counts-1);
					for(; p1 != 0; p1 = c->Props.current()) { untested();
						if(counts-- < 19){ untested();
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
			}else{ untested();
			}

			return c;
		}else{ untested();
			// z <= counts
		}

		// for equations
		qDebug() << "Model" << Model;
#if 1
		if(Model != "EDD" && Model != "RFEDD" && Model != "RFEDD2P")
			if(p1->Description.isEmpty()) {  // unknown number of properties ?
				p1->Name = n.section('=',0,0);
				n = n.section('=',1);
				// allocate memory for a new property (e.g. for equations)
				if(c->Props.count() < (counts>>1)) { untested();
					c->Props.insert(z >> 1, new Property("y", "1", true));
					c->Props.prev();
				}
			}
#endif
		if(z == 6)  if(counts == 6)     // backward compatible
			if(Model == "R") { untested();
				c->Props.getLast()->Value = n;
				return c;
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

Element* LegacySchematicLanguage::getComponentFromName(QString& Line) const
{ untested();
	qDebug() << "component?" << Line;
	Element *e = 0;

	Line = Line.trimmed();
	if(Line.at(0) != '<') { untested();
		throw "notyet_exception"
			"Format Error:\nWrong line start!";
	}else{ untested();
	}

	QString type = Line.section (' ',0,0); // component type
	type.remove (0,1);    // remove leading "<"
	std::string typestring = type.toStdString();

	// TODO: get rid of the exceptional cases.
	if (type == "Lib"){ untested();
		incomplete();
		// c = new LibComp ();
	}else if (type == "Eqn"){ untested();
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

	// fetch proto from dictionary.
	Element const* s=symbol_dispatcher[typestring];

	if(Component const* sc=dynamic_cast<Component const*>(s)){ untested();
		// legacy component
		Element* k=sc->clone(); // memory leak?
		e = prechecked_cast<Element*>(k);

		// set_type?
	}else if(CmdElement const* sc=dynamic_cast<CmdElement const*>(s)){ untested();
		// legacy component
		Element* k = sc->clone(); // memory leak?
		e = prechecked_cast<Element*>(k);
	}else if(typestring.size() == 0){ untested();
		incomplete();
	}else if(typestring.c_str()[0] == '.'){ untested();
		std::string type = typestring.substr(1); // drop dot.
		e = command_dispatcher.clone(type);
		if(auto ce = dynamic_cast<CmdElement*>(e)){ untested();
			// should use setType lower down. drop name.
			ce->setTypeName(QString::fromStdString(type));
		}else{ untested();
			untested();
		}
	}

	if(e) { untested();
		incomplete();
		loadElement(Line, e);
		// setType()
	}else{ untested();
		qDebug() << "error with" << type;
		message(QucsWarningMsg,
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
	if(CmdElement* cmd=command(e)){ untested();
		p->loadCommand(Line, cmd);
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

// was Schematic::loadProperties
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

} // namespace
