/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2018, 2019, 2020 Felix Salfelder / QUCS team
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sckt_proto.h"
#include "qucs.h"
#include "schematic_doc.h"
#include "misc.h"

#include <QTextStream>
#include <QFileInfo>
#include <QMutex>

#include <limits.h>
#include <io_trace.h>
#include "globals.h"
#include "docfmt.h"
#include "module.h"


#include "subcircuit.h" // possibly unneeded. let's see.

Subcircuit::Subcircuit() : Component() // gaah sckt_base
{
  Type = isComponent;   // both analog and digital
  Description = QObject::tr("subcircuit");

//  Props.append(new Property("File", "", false,
//		QObject::tr("name of qucs schematic file")));

  setType("Sub");

  // Do NOT call createSymbol() here. But create port to let it rotate.
  Ports.append(new Port(0, 0, false));

  new_subckt(); // triggers sckt expansion
}

// ---------------------------------------------------------------------
Subcircuit::Subcircuit(Subcircuit const&x) : Component(x)
{
  Props.append(new Property("File", "", false,
		QObject::tr("name of qucs schematic file")));

  setType("Sub");

  new_subckt(); // triggers sckt expansion
}

// ---------------------------------------------------------------------
Component* Subcircuit::newOne()
{
	unreachable();
  Subcircuit *p = new Subcircuit();
  p->setLabel("-newOne-"); // TODO
  p->Props.getFirst()->Value = Props.getFirst()->Value;
  trace1("Subcircuit::clone", Props.getFirst()->Value);

  // p->recreate(0); // does not make sense.
  return p;
}

// -------------------------------------------------------
Element* Subcircuit::info(QString& Name, char* &BitmapFile, bool getNewOne)
{ untested();
	assert(false);
  Name = QObject::tr("Subcircuit");
  BitmapFile = (char *) "subcircuit";

  if(getNewOne) { untested();
	  incomplete();
    Subcircuit *p = new Subcircuit();
	 p->setLabel("-getnewOne-"); // TODO
    p->recreate(0);   // createSymbol() is NOT called in constructor !!!
    return p;
  }
  return 0;
}

// ---------------------------------------------------------------------
// Makes the schematic symbol subcircuit with the correct number
// of ports.
void Subcircuit::createSymbol() // SchematicModel const& scope)
{
  int No;
  QString FileName(Props.getFirst()->Value);

  qDebug() << "FileName" << FileName; // seems okay, sth like opa227.sch

  if(FileName.length()<4){ unreachable();
	  // throw?
	  return;
  }else{

	  //  trace2("sckt::createSymbol", this, scope);
	  incomplete();
	  assert(owner());
	  auto os = prechecked_cast<Symbol*>(owner());
	  assert(os);
	  trace1("hmm", typeid(*os).name());
	  assert(os->subckt());
	  FileName = getSubcircuitFile(os->subckt());
	  trace2("getting symbol", FileName, os->subckt());

	  tx = INT_MIN;
	  ty = INT_MIN;
	  // BUG // not cached. why?
	  if(loadSymbol(FileName) > 0) {  // try to load subcircuit symbol
		  if(tx == INT_MIN)  tx = x1+4;
		  if(ty == INT_MIN)  ty = y2+4;
		  // remove unused ports
		  QMutableListIterator<Port *> ip(Ports);
		  Port *pp;
		  while (ip.hasNext()) {
			  pp = ip.next();
			  if(!pp->avail) { untested();
				  pp = ip.peekNext();
				  ip.remove();
			  }
		  }
	  }else{
		  No = SchematicDoc::testFile(FileName);
		  if(No < 0)  No = 0;
		  Ports.clear();
		  remakeSymbol(No);  // no symbol was found -> create standard symbol
	  }
  }
}

// ---------------------------------------------------------------------
void Subcircuit::remakeSymbol(int No)
{
	incomplete();
  int h = 30*((No-1)/2) + 15;
  Lines.append(new Line(-15, -h, 15, -h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 15, -h, 15,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-15,  h, 15,  h,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-15, -h,-15,  h,QPen(Qt::darkBlue,2)));
  Texts.append(new Text(-10, -6,"sub"));

  int i=0, y = 15-h;
  while(i<No) {
    i++;
    Lines.append(new Line(-30,  y,-15,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port(-30,  y));
    Texts.append(new Text(-25,y-14,QString::number(i)));

    if(i == No) break;
    i++;
    Lines.append(new Line( 15,  y, 30,  y,QPen(Qt::darkBlue,2)));
    Ports.append(new Port( 30,  y));
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
int Subcircuit::loadSymbol(const QString& DocName)
{
  QFile file(DocName);
  if(!file.open(QIODevice::ReadOnly))
    return -1;

  QString Line;
  // *****************************************************************
  // To strongly speed up the file read operation the whole file is
  // read into the memory in one piece.
  QTextStream ReadWhole(&file);
  QString FileString = ReadWhole.readAll();
  file.close();
  QTextStream stream(&FileString, QIODevice::ReadOnly);


  // read header **************************
  do {
    if(stream.atEnd()) return -2;
    Line = stream.readLine();
    Line = Line.trimmed();
  } while(Line.isEmpty());

  if(Line.left(16) != "<Qucs Schematic ")  // wrong file type ?
    return -3;

  Line = Line.mid(16, Line.length()-17);
  VersionTriplet SymbolVersion = VersionTriplet(Line);
  if (SymbolVersion > QucsVersion) // wrong version number ?
    return -4;

  // read content *************************
  while(!stream.atEnd()) {
    Line = stream.readLine();
    if(Line == "<Symbol>") break;
  }

  x1 = y1 = INT_MAX;
  x2 = y2 = INT_MIN;

  int z=0, Result;
  while(!stream.atEnd()) {
    Line = stream.readLine();
    if(Line == "</Symbol>") {
      x1 -= 4;   // enlarge component boundings a little
      x2 += 4;
      y1 -= 4;
      y2 += 4;
      return z;      // return number of ports
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

// -------------------------------------------------------
// BUG: obsolete callback
QString Subcircuit::netlist() const
{ untested();
	incomplete();
	return "bla";
#if 0
  QString s = Model+":"+Name;

  // output all node names
  foreach(Port *p1, Ports)
    s += " "+p1->Connection->name();   // node names

  // type for subcircuit
  QString f = misc::properFileName(Props.first()->Value);
  s += " Type=\""+misc::properName(f)+"\"";

  // output all user defined properties
  for(Property *pp = Props.next(); pp != 0; pp = Props.next())
    s += " "+pp->name()+"=\""+pp->Value+"\"";
  return s + '\n';
#endif
}

// -------------------------------------------------------
QString Subcircuit::vhdlCode(int)
{ untested();
	return "bla";
#if 0
  QString f = misc::properFileName(Props.first()->Value);
  QString s = "  " + name() + ": entity Sub_" + misc::properName(f);

  // output all user defined properties
  Property *pr = Props.next();
  if (pr) { untested();
    s += " generic map (";
    s += pr->Value;
    for(pr = Props.next(); pr != 0; pr = Props.next())
      s += ", " + pr->Value;
    s += ")";
  }

  // output all node names
  s += " port map (";
  QListIterator<Port *> iport(Ports);
  Port *pp = iport.next();
  if(pp){ untested();
    s += pp->Connection->name();
  }else{ untested();
  }
  while (iport.hasNext()) { untested();
    pp = iport.next();
    s += ", "+pp->Connection->name();   // node names
  }

  s += ");\n";
  return s;
#endif
}

// -------------------------------------------------------
QString Subcircuit::verilogCode(int)
{ untested();
#if 0
  QString f = misc::properFileName(Props.first()->Value);
  QString s = "  Sub_" + misc::properName(f);

  // output all user defined properties
  Property *pr = Props.next();
  if (pr) { untested();
    s += " #(";
    s += misc::Verilog_Param(pr->Value);
    for(pr = Props.next(); pr != 0; pr = Props.next())
      s += ", " + misc::Verilog_Param(pr->Value);
    s += ")";
  }

  // output all node names
  s +=  " " + name() + " (";
  QListIterator<Port *> iport(Ports);
  Port *pp = iport.next();
  if(pp)
    s += pp->Connection->name();
  while (iport.hasNext()) { untested();
    pp = iport.next();
    s += ", "+pp->Connection->name();   // node names
  }

  s += ");\n";
  return s;
#endif
}

// -------------------------------------------------------
QString Subcircuit::getSubcircuitFile(SchematicModel const* scp) const
{
  trace3("getSubcircuitFile", this, scp, scope());

	if(scp){
	}else{ untested();
		assert(owner());
		auto scp=scope();
	}
	assert(scp);
  // construct full filename
  QString FileName = Props.getFirst()->Value;

  if (FileName.isEmpty()) { untested();
	  incomplete();
      return misc::properAbsFileName(FileName);
  }else{
  }

  QFileInfo FileInfo(FileName);

  if (FileInfo.exists()) { untested();
      // the file must be an absolute path to a schematic file
     return FileInfo.absoluteFilePath();
  } else {
	  qDebug() << FileName << "doesnt exist";
    // get the complete base name (everything except the last '.'
    // and whatever follows
    QString baseName = FileInfo.completeBaseName();

    // if only a file name is supplied, first check if it is in the
    // same directory as the schematic file it is a part of
    if (FileInfo.fileName () != FileName) { untested();
        // the file has no path information, just the file name
		 qDebug() << "no path info";
	 }else if (scp) {
		 qDebug() << "trying to inherit path from sch";
		 // check if a file of the same name is in the same directory
		 // as the schematic file, if we have a pointer to it, in
		 // which case we use this one
		 QFileInfo schematicFileInfo = scp->getFileInfo ();
		 QFileInfo localFIleInfo (schematicFileInfo.canonicalPath () + "/" + baseName + ".sch");
		 qDebug() << schematicFileInfo.canonicalPath() << "seems to exist";
		 if (localFIleInfo.exists ()) {
			 // return the subcircuit saved in the same directory
			 // as the schematic file
			 return localFIleInfo.absoluteFilePath();
		 }else{ untested();
		 }
	 }else{ untested();
		 qDebug() << "no schematic";
	 }

    // look up the hash table for the schematic file as
    // it does not seem to be an absolute path, this will also
    // search the home directory which is always hashed
    QMutex mutex;
    mutex.lock();
    QString hashsearchresult = "";
    // check if GUI is running and there is something in the search path lookup
    if ( (QucsMain != 0) && !QucsMain->schNameHash.isEmpty() )
      hashsearchresult = QucsMain->schNameHash.value(baseName);
    mutex.unlock();

    if (hashsearchresult.isEmpty()) { untested();
        // the schematic was not found in the hash table, return
        // what would always have been returned in this case
        return misc::properAbsFileName(FileName);
    } else { untested();
        // we found an entry in the hash table, check it actually still exists
        FileInfo.setFile(hashsearchresult);

        if (FileInfo.exists()) { untested();
            // it does exist so return the absolute file path
            return FileInfo.absoluteFilePath();
        } else { untested();
            // the schematic file does not actually exist, return
            // what would always have been returned in this case
            return misc::properAbsFileName(FileName);
        }
    }
  }
}

static SubMap FileList;

void Subcircuit::tAC(QTextStream& stream, SchematicModel const* schem, QStringList&
		Collect, int& countInit, int NumPorts, NetLang const& nl)
{untested();
	assert(false); // obsolete
#if 0
	stream << "# subckt declaration\n";
	Component* pc=this;
	int i;
	// tell the subcircuit it belongs to this schematic
	Subcircuit* sckt=prechecked_cast<Subcircuit*>(pc);
	assert(sckt);

	auto schem_ = const_cast<SchematicModel*>(schem);
//	sckt->setSchematic(schem_); // BUG. assert only
	assert(scope() == schem);

	QString f = pc->getSubcircuitFile();
	trace2("Subcircuit::tAC", schem, f);
	SubMap::Iterator it = FileList.find(f);
	if(it != FileList.end()) { untested();
		if (!it.value().PortTypes.isEmpty())
		{ untested();
			i = 0;
			// apply in/out signal types of subcircuit
			foreach(Port *pp, pc->Ports)
			{ untested();
				pp->Type = it.value().PortTypes[i];
				incomplete();
				//pp->Connection->DType = pp->Type;
				i++;
			}
		}
	}

	// The subcircuit has not previously been added
	SubFile sub = SubFile("SCH", f);
	FileList.insert(f, sub);


	// load subcircuit schematic
	QString s=pc->Props.first()->Value;
	SchematicModel* d = _subckt;
	assert(d);

	// todo: error handling.
	QString scktfilename(pc->getSubcircuitFile());
	QFile file(scktfilename);
	trace2("getting sckt definition", scktfilename, s);
	file.open(QIODevice::ReadOnly);
	DocumentStream pstream(&file);
	// d->setFileInfo(scktfilename);
	d->parse(pstream);
	d->setDevType(s);

	// d->setDocName(s);
	// d->isVerilog = isVerilog;
	// d->isAnalog = isAnalog;
	// d->creatingLib = creatingLib; wtf?
	bool creatingLib=false;
	auto r = d->createSubNetlist(stream, countInit, Collect, nullptr, NumPorts, creatingLib, nl);
	if (r) { untested();
		i = 0;
		// save in/out signal types of subcircuit
		foreach(Port *pp, pc->Ports)
		{ untested();
			//if(i>=d->PortTypes.count())break;
			pp->Type = d->portType(i);
			incomplete();
			//pp->Connection->DType = pp->Type;
			i++;
		}
		incomplete();
		//sub.PortTypes = d->PortTypes;
		FileList.insert(f, sub);
	}else{ untested();
	}
#endif
}

namespace{

// maybe not here. the sckt is within SchematicModel
//
class pr : public SubcktProto{
public:
  	pr(Element const* e) : SubcktProto(e){
	}
	~pr() { untested(); }

	void build() override{
		assert(owner());
		assert(scope()==subckt());

		qDebug() << "pr::build";
		int i;

		Subcircuit const* inst=dynamic_cast<Subcircuit const*>(instance());
		Component const* pc=inst;
		assert(inst);

#if 0
		// don't know what this is.
		SubMap::Iterator it = FileList.find(f);
		if(it != FileList.end()) { untested();
			if (!it.value().PortTypes.isEmpty())
			{ untested();
				i = 0;
				// apply in/out signal types of subcircuit
				foreach(Port *pp, pc->Ports)
				{ untested();
					pp->Type = it.value().PortTypes[i];
					incomplete();
					//pp->Connection->DType = pp->Type;
					i++;
				}
			}
		}
#endif

		trace2("gSsf", inst, inst->scope());
		QString f = inst->getSubcircuitFile(inst->scope());
		qDebug() << "sckt" << f;

		// load subcircuit schematic
		QString s=pc->Props.first()->Value;
		SchematicModel* d=schematicModel();
		SchematicModel const* dc=schematicModel();
		_subckt = d; // bug. new sckt?

		// todo: error handling.
		QString scktfilename(f);
		assert(scktfilename!="");
		QFile file(scktfilename);
		qDebug() << "getting sckt definition from" << scktfilename << "type" << s;
		file.open(QIODevice::ReadOnly);
		DocumentStream pstream(&file);

		auto D=docfmt_dispatcher["leg_sch"];
		assert(D);

		qDebug() << "reading subckt .sch";
		D->load(pstream, *this);
		//d->parse(pstream);

		qDebug() << "got" << dc->components().count() << "components";
		d->setDevType(s);

		// Ports.resize(5);
	}

	SchematicModel* scope() override{return _subckt;};

private:
	std::string getParameter(std::string const&) const{ untested();
		return "incomplete";
	}
	Port& port(unsigned){ untested();
		unreachable();
		return *new Port(0,0);
	}
};

}

void Subcircuit::build()
{
	incomplete();
	createSymbol(); // why?!
}

// partially tAC. build a new sckt proto.
// does not fullymake sense.
Symbol const* Subcircuit::proto(SchematicModel const* scope) const
{
   auto t = QString::fromStdString(type());
	auto p = scope->findProto(t);
	if(p){
		trace1("cached", type());
		return p;
	}else{
		Symbol const* ownersym = dynamic_cast<Symbol const*>(owner());
		Symbol* s = new pr(this);

		assert(owner());
		// same owner. good idea? needed?
		Element* mutable_owner_hack = const_cast<Element*>(owner());
		s->setOwner(mutable_owner_hack);
		s->build();
		assert(s->subckt());

		QString t = Props.first()->Value;
		trace3("Subcircuit::proto", t, owner(), typeid(*owner()).name());
		
		s->setLabel(type());
		trace2("sckt::proto", type(), s->type());

		assert(s->subckt());
		assert(scope);
		scope->cacheProto(s);
		p = s;
	}

//	_proto = p;?
//	set_type(p->type())?

	return p;
}

bool Subcircuit::portExists(unsigned i) const
{ untested();
	assert(scope());
	trace1("Subcircuit::portExists", i);
	incomplete();
	return false;
}

QString Subcircuit::portName(unsigned) const
{ untested();
	incomplete();
	return "invalid";
}

void Subcircuit::setParameter(unsigned i, std::string const& value)
{
	QString v = QString::fromStdString(value);
	if(i==0){
		trace1("Subcircuit::setParameter", v);
///		setType(value.toStdString());
/// possibly bad idea. do in "expand" instead.
		setType(v.left(v.length()-4).toStdString());
	}else{
		incomplete();
	}
}

namespace{
Subcircuit D;
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "Sub", &D);
static Module::INSTALL pp("stuff", &D);
}
