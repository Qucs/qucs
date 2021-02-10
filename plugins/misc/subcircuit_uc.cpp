/***************************************************************************
    copyright            : (C) 2018, 2019, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "schematic_doc.h"
#include "misc.h"
#include <limits.h>
#include <io_trace.h>
#include "qucs_globals.h"
#include "docfmt.h"
#include "module.h"
#include "sckt_base.h"
#include "qio.h"
#include "factory.h"

namespace {

const std::string typesep(":");

// bug. use normal ports.
class ComponentPort : public Port{
public:
  ComponentPort(const ComponentPort&p) : Port(), _p(p._p), avail(p.avail),
					Type(p.Type), Name(p.Name) {}
  explicit ComponentPort(int x, int y, bool _avail=true)
	  : Port(), _p(x, y), avail(_avail)
  { untested();
    Type=""; Name="";
  }

public:
  int x_()const{return getX(_p);}
  int y_()const{return getY(_p);}
  pos_t const& position() const { return _p; }
  void setPosition(int x, int y) { _p=pos_t(x,y); }
  void setPosition(pos_t p) { _p=p; }
  int x() const{ return getX(_p); }
  int y() const{ return getY(_p); }

  QString const& name() const{return Name;}

private:
  pos_t _p;

public:
  bool  avail;
  QString Type; // BUG. type is "Port".
  QString Name; // BUG?

};
/*--------------------------------------------------------------------------*/
// possibly not needed.
class CommonSub : public CommonComponent{
public:
	explicit CommonSub(int i=0) : CommonComponent(i) {}
private:
	CommonComponent* clone() const override{ untested();
		return new CommonSub(*this);
	}
};
CommonSub cs(CC_STATIC_);
/*--------------------------------------------------------------------------*/
class SubFactory;
/*--------------------------------------------------------------------------*/
// a subcircuit instance with a factory hook
class Paramset : public Symbol {
public:
	explicit Paramset(CommonComponent* cc);
	explicit Paramset() {}
	~Paramset() {}

private:
  Paramset(Paramset const&x);

  std::string dev_type()const override { untested();
	  return "Sub";
  }

public:
  Element* clone() const override{ untested();
	  trace0("Paramset::clone");
	  return new Paramset(*this);
  }

	virtual rect_t bounding_rect() const{incomplete(); return rect_t();}
	virtual void paint(ViewPainter*) const{incomplete();}

public: // borrowed from Component. get rid of it, it does not work.
  virtual unsigned numPorts() const override{ return Ports.count(); }
  Port& port(unsigned i) override{ untested();
	  assert(i<unsigned(Ports.count()));
	  return *Ports[i];
  }
  pos_t portPosition(unsigned i) const override{ untested();
	  return Ports[i]->position();
  }
  QList<ComponentPort*> Ports; // TODO

private:
  // QString vhdlCode(int);
  void refreshSymbol(std::string const&);
  // void defaultSymbol(int No);
  // int loadSymbol(const QString&);

private: // Symbol
// unsigned numPorts() const override;
  bool portExists(unsigned) const override;
  std::string const& portName(unsigned) const override;

  void setParameter(std::string const& name, std::string const& value);
  void setParameter(unsigned i, std::string const& value) override;
  unsigned paramCount() const override;

  std::string paramName(unsigned i) const override;
  std::string paramValue(unsigned i) const override{ untested();
	  if(i == Symbol::paramCount()){ untested();
		  return _filename;
	  }else{ untested();
		  return Symbol::paramName(i);
	  }
  }
  std::string paramValue(std::string const& name) const override{ untested();
	  trace1("Paramset::paramValue", name);
	  if(name=="$tx"){ untested();
		 return "0"; // std::to_string(tx);
	  }else if(name=="$ty"){ untested();
		 return "0"; // std::to_string(ty);
	  }else{ untested();
		  incomplete();
		  return Symbol::paramValue(name);
	  }
  }

private: // overrides
	void proto(SchematicModel const* schem);

public:
	SubFactory* _factory{nullptr};

private:
	std::string _subPath;
	std::string _filename; // "File" parameter.
}p1; // Paramset
/*--------------------------------------------------------------------------*/
class SubFactory : public Element, public SymbolFactory {
public:
	explicit SubFactory() : Element(), SymbolFactory() {}
	SubFactory(SubFactory const& f) : Element(f), SymbolFactory(f)
	{ untested();
		_proto = new Paramset();
		_proto->_factory = this;
	}
	
	Element* clone_instance() const override;
	Element* clone()const{return new SubFactory(*this);}

	CommonComponent const* newCommon(std::string const& fn) const;

private:
	void set_param_by_name(std::string const& name, std::string const& v) override
	{ untested();
		trace2("SF::set_param", name, v);
		if(name=="$SUB_PATH"){ untested();
			_subPath = v;
		}else{ untested();
			throw qucs::ExceptionCantFind(name, label());
		}
	}
	void build_sckt(istream_t&, SubcktBase* proto) const;

private:
  Paramset* _proto{nullptr};
  std::string _subPath;
}d0;
static Dispatcher<Element>::INSTALL p(&element_dispatcher, "LegacySub", &d0);
static Module::INSTALL pp("stuff", &d0);
/*--------------------------------------------------------------------------*/
Element* SubFactory::clone_instance() const
{ untested();
	assert(_proto);
	auto new_instance = prechecked_cast<Paramset*>(_proto->clone());
	assert(!new_instance->subckt());

	if(this == &d0){ untested();
	}else{ untested();
		assert(new_instance->_factory == this);
	}

	return new_instance;
}
/*--------------------------------------------------------------------------*/
Paramset::Paramset(CommonComponent* cc)
	: Symbol() // sckt?
{ untested();
	attach_common(cc);
  // Description = QObject::tr("subcircuit");

//		QObject::tr("name of qucs schematic file")));

  setTypeName("Sub"); // dev_type_key.
}
/*--------------------------------------------------------------------------*/
// use common params eventually.
std::string Paramset::paramName(unsigned i) const
{ untested();
  if(i==Symbol::paramCount()){ untested();
	  return "File";
  }else if(i==Symbol::paramCount()+1){ untested();
	  return "porttype"; // or so
  }else{ untested();
	  return Symbol::paramName(i);
  }
}
/*--------------------------------------------------------------------------*/
unsigned Paramset::paramCount() const
{ untested();
  return 1 + Symbol::paramCount();
}
/*--------------------------------------------------------------------------*/
Paramset::Paramset(Paramset const&x)
  : Symbol(x),
    _factory(x._factory)
{ untested();
//  Props.append(new Property("File", "", false,
//		QObject::tr("name of qucs schematic file")));

  setTypeName("Sub"); // dev_type_key

  // new_subckt(); // triggers sckt expansion
}
/*--------------------------------------------------------------------------*/
// void Paramset::set_dev_type(const std::string& new_type){ untested();
//    hmm, type does not reflect file name, cant expand it...
// }
/*--------------------------------------------------------------------------*/
void Paramset::refreshSymbol(std::string const& fn)
{ untested();
	auto dotplace = fn.find_last_of(".");

#ifndef NDEBUG
	std::string type_name;
	if (dotplace == std::string::npos) { untested();
		// throw?? or try and recover??
		type_name = "Sub"+typesep+"invalid_filename";
	}else{ untested();
		type_name = "Sub" + typesep + fn.substr(0, dotplace);
	}
#endif

	setTypeName("Sub"); // still // dev_type_key
	assert(_factory);
	auto new_common = _factory->newCommon(fn);
	assert(new_common);
	auto cc = new_common->clone(); //really? set modelname in factory.
//	cc->set_modelname(type_name);
	assert(cc->modelname()==type_name);
	attach_common(cc); // not actually shared yet. but will.
	assert(common()->modelname()==type_name);
}
/*--------------------------------------------------------------------------*/
// create a subdevice from a file.
// if its already there, use it.
// TODO: factory needs a refresh hook.
CommonComponent const* SubFactory::newCommon(std::string const& fn) const
{ untested();
//	QString FileName(Props.getFirst()->Value);
	auto dotplace = fn.find_last_of(".");
	std::string type_name;

	if (dotplace == std::string::npos) { untested();
		incomplete();
		// throw?? or try and recover??
		type_name = "Sub"+typesep+"invalid_filename";
	}else{ untested();
		type_name = "Sub" + typesep + fn.substr(0, dotplace);
	}

	auto cached_ = _scope->find_(type_name);
	Element* cached = nullptr;
	if(cached_ != _scope->end()){ untested();
		// TODO: find_again.
		cached = *cached_;
	}else{ untested();
	}


	std::string file_found = findFile(fn, _subPath, R_OK);
	trace4("SubFactory::newCommon", label(), fn, _subPath, file_found);

	if(auto sym = dynamic_cast<Symbol const*>(cached)){ untested();
		return sym->common();
	}else if(file_found != "" ){ untested();
	  incomplete(); // rework with parser.
	  assert(owner());
	  auto os = prechecked_cast<Symbol const*>(owner());
	  assert(os);
//	  trace1("hmm", typeid(*os).name());
	  assert(os->scope());
//	  trace2("getting symbol", FileName, os->scope());

	  Symbol* ss = symbol_dispatcher.clone("subckt_proto");
	  auto s = prechecked_cast<SubcktBase*>(ss);
	  assert(s);

	  istream_t pstream(istream_t::_WHOLE_FILE, file_found);

	  build_sckt(pstream, s);

	  s->set_dev_type(type_name);
	  s->setLabel(type_name);

	  assert(_scope);

	  trace3("aa", s->label(), type_name, s->typeName());
	  assert(_scope->find_(type_name) == _scope->end());
	  _scope->push_back(s);

	  assert(s->label()==type_name);

#if 0
	  // BUG // not cached. why?
	  if(loadSymbol(FileName) > 0) { untested();
		  if(tx == INT_MIN)  tx = x1+4;
		  if(ty == INT_MIN)  ty = y2+4;
		  // remove unused ports
		  QMutableListIterator<ComponentPort *> ip(Ports);
		  ComponentPort *pp;
		  while (ip.hasNext()) { untested();
			  pp = ip.next();
			  if(!pp->avail) { untested();
				  pp = ip.peekNext();
				  ip.remove();
			  }
		  }
#endif
	  return s->common();

#if 0 // no file found?
	  // TODO: default symbol if this fails.
  }else if(have_number_of_ports or so){ untested();
	  No = SchematicDoc::testFile(FileName);
	  if(No < 0)  No = 0;
	  Ports.clear();
	  defaultSymbol(No);  // no symbol was found -> create standard symbol
#endif
  }else{ untested();
  }
}
// ---------------------------------------------------------------------
#if 0
void Paramset::defaultSymbol(int No)
{ untested();
	// sort of default symbol if there is no symbol section?
	incomplete();
  int h = 30*((No-1)/2) + 15;
  Lines.append(new Line(-15, -h, 15, -h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 15, -h, 15,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-15,  h, 15,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-15, -h,-15,  h,QPen(Qt::darkBlue,2)));
  Texts.append(new Text(-10, -6,"sub"));

  int i=0, y = 15-h;
  while(i<No) { untested();
    i++;
    Lines.append(new Line(-30,  y,-15,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new ComponentPort(-30,  y));
    Texts.append(new Text(-25,y-14,QString::number(i)));

    if(i == No) break;
    i++;
    Lines.append(new Line( 15,  y, 30,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new ComponentPort( 30,  y));
    Texts.append(new Text( 19,y-14,QString::number(i)));
    y += 60;
  }

  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+2;
  tx = x1+4;
  ty = y2+4;
}

// ---------------------------------------------------------------------
// Loads the symbol for the subcircuit from the schematic file and
// returns the number of painting elements.
int Paramset::loadSymbol(const QString& DocName)
{ untested();
  QFile file(DocName);
  if(!file.open(QIODevice::ReadOnly)){ untested();
    return -1;
  }else{ untested();
  }

  QString Line;
  // *****************************************************************
  // To strongly speed up the file read operation the whole file is
  // read into the memory in one piece.
  QTextStream ReadWhole(&file);
  QString FileString = ReadWhole.readAll();
  file.close();
  QTextStream stream(&FileString, QIODevice::ReadOnly);


  // read header **************************
  do { untested();
    if(stream.atEnd()) return -2;
    Line = stream.readLine();
    Line = Line.trimmed();
  } while(Line.isEmpty());

  if(Line.left(16) != "<Qucs Schematic "){ untested();
	  // wrong file type ?
    return -3;
  }else{ untested();
  }

  Line = Line.mid(16, Line.length()-17);
  VersionTriplet SymbolVersion = VersionTriplet(Line);
  if (SymbolVersion > QucsVersion){ untested();
    return -4;
  }else{ untested();
  }

  // read content *************************
  while(!stream.atEnd()) { untested();
    Line = stream.readLine();
    if(Line == "<Symbol>") break;
  }

  x1 = y1 = INT_MAX;
  x2 = y2 = INT_MIN;

  int z=0, Result;
  while(!stream.atEnd()) { untested();
    Line = stream.readLine();
    if(Line == "</Symbol>") { untested();
      x1 -= 4;   // enlarge component boundings a little
      x2 += 4;
      y1 -= 4;
      y2 += 4;
      return z;      // return number of ports
    }else{ untested();
	 }

    Line = Line.trimmed();
    if(Line.at(0) != '<') return -5;
    if(Line.at(Line.length()-1) != '>') return -6;
    Line = Line.mid(1, Line.length()-2); // cut off start and end character
    Result = analyseLine(Line, 1);
    if(Result < 0) return -7;   // line format error
    z += Result;
  }

  return -8;   // field not closed
}
#endif

/*--------------------------------------------------------------------------*/
#if 0 // obsolete
void Paramset::proto(SchematicModel const* scope)
{ untested();
   auto t = typeName();
	trace1("Paramset::proto", typeName());
//	auto p = scope->findProto(t);
   assert(owner());
	Symbol const* p = nullptr;
	try{ untested();
		p = dynamic_cast<Symbol const*>(find_looking_out(t));
	}catch(qucs::ExceptionCantFind const&){ untested();
		incomplete();
	}

	if(p){ untested();
		trace1("cached", typeName());
	}else{ untested();
		trace3("not cached", typeName(), scope, dev_type());
//		Symbol const* ownersym = dynamic_cast<Symbol const*>(owner());
//		auto* s = new SubcktProto(this);
		Symbol* ss = symbol_dispatcher.clone("subckt_proto");
		auto s = prechecked_cast<SubcktBase*>(ss);
		assert(s);

		build_sckt(s);
		trace3("Subcircuit built proto", s->typeName(), owner(), s->common()->modelname());
		assert(!s->subckt());
		assert(ss->scope());

		QString t = Props.first()->Value;
		
		s->setLabel(typeName()); // so we can find it.
		s->set_dev_type(typeName());
		trace3("sckt::proto install", label(), this, s->common());
		trace3("sckt::proto install", typeName(), s->typeName(), s->common()->modelname());
		trace3("sckt::proto install", typeName(), s->dev_type(), dev_type());
		assert(s->dev_type() == typeName());
		setTypeName("Sub"); // key.

		assert(s->common());
		attach_common(s->common()->clone());
		_scope->pushBack(s);

		p = s;
	}

	{ untested();
		auto q = prechecked_cast<Symbol*>(p->clone_instance());
		assert(q);
		assert(q->common() == p->common());
		attach_common(q->mutable_common());
		delete q;
	}

//	_proto = p;?
//	set_type(p->type())?

}
#endif

void SubFactory::build_sckt(istream_t& cs, SubcktBase* proto) const
{ untested();
	assert(owner());

	auto cmd = commandDispatcher["leg_sch"];
	auto D = prechecked_cast<DocumentFormat const*>(cmd);
	assert(D);

#if 0
	D->load(cs, proto);
#else
//	proto->new_subckt(); wrong.
	auto LL = languageDispatcher["leg_sch"]; // use command instead?
	auto L = dynamic_cast<SchematicLanguage const*>(LL);
	assert(L);
	cs.read_line();
	while(!cs.is_end()){itested();
		trace1("parse schematic subckt", cs.fullstring());
		Element* e = proto;
		assert(e->scope());
		// assert(proto->scope() == proto->subckt()); nope
		// L->parse_top_item(s, _root->subckt());
		L->new__instance(cs, proto, e->scope());
		cs.read_line();
	}
#endif

//	proto->setTypeName(s.toStdString());
	assert(proto->common());
}

bool Paramset::portExists(index_t i) const
{ untested();
	return i<numPorts();
	assert(scope());
	trace1("Paramset::portExists", i);
	incomplete();
	return false;
}
/*--------------------------------------------------------------------------*/
static std::string invalid_ = "invalid";

std::string const& Paramset::portName(index_t) const
{ untested();
	incomplete();
	// throw?
	return invalid_;
}
/*--------------------------------------------------------------------------*/
void Paramset::setParameter(std::string const& name, std::string const& v)
{ untested();
	trace2("Paramset::setParameter", name, v);
	if(name=="$SUB_PATH"){ untested();
		_subPath = v;
	}else if(name=="File"){ untested();
		_filename = v;
		trace1("Paramset::setParameter2", v);
		refreshSymbol(v);
	}else{ untested();
		Symbol::setParameter(name, v);
	}
}
/*--------------------------------------------------------------------------*/
void Paramset::setParameter(unsigned i, std::string const& v)
{ untested();
	trace2("Paramset::setParameter", i, v);
	if(i==Symbol::paramCount()){ untested();
		_filename = v;
		trace1("Paramset::setParameter", v);
		refreshSymbol(v);
	}else if(i==4){ // needed by legacy schematic parser...
		_filename = v;
		trace1("Paramset::setParameter", v);
		refreshSymbol(v);
	}else{ untested();
		incomplete();
	}
}
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
