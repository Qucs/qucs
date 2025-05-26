/***************************************************************************
                               libcomp.cpp
                              -------------
    begin                : Fri Jun 10 2005
    copyright            : (C) 2005 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "libcomp.h"
#include "qucs.h"
#include "schematic.h"
#include "misc.h"
#include "trace.h"

#include <limits.h>

#include <QTextStream>
#include <QDir>
#include <QRegExp>
#include <QDebug>

LibComp::LibComp()
{
  Type = isComponent;   // both analog and digital
  Description = QObject::tr("Component taken from Qucs library");

  Ports.push_back(qucs::Port(0,  0));  // dummy port because of being device

  Model = "Lib";
  Name  = "X";

  assert(!Props.size());
  Props.push_back(qucs::Property("Lib", "", true,
		QObject::tr("name of qucs library file")));
  Props.push_back(qucs::Property("Comp", "", true,
		QObject::tr("name of component in library")));
}
// ---------------------------------------------------------------------
LibComp::LibComp(LibComp const& p) : MultiViewComponent(p)
{ untested();
	incomplete(); // not reached?!
}

// ---------------------------------------------------------------------
#if 0 // TODO
std::string LibComp::param_is_printable(int i) const
{ untested();
	if(i==0){ untested();
		return false; "Lib" is an attibute, not a parameter
	if(i==1){ untested();
		return false; type is not a parameter.
	}else{ untested();
		return true;
	}
}
#endif

// ---------------------------------------------------------------------

std::string LibComp::param_name(int i) const
{
	if(i==0){
		return "Lib"; // BUG, it's a file name
	}else if(i==1){
		return "Comp"; // BUG, isn't this the type?
	}else{
		return Component::param_name(i);
	}
}

// ---------------------------------------------------------------------
std::string LibComp::param_value(int i) const
{
	if(i < int(Props.size())){
		return Component::param_value(i);
	}else{ untested();
		return "NA";
	}
}


// ---------------------------------------------------------------------
Component* LibComp::newOne()
{ untested();
  LibComp *p = new LibComp();
  assert(Props.size()>1);
  p->prop(0).Value = prop(0).Value;
  p->prop(1).Value = prop(1).Value;
  p->recreate(0);
  assert(Props.size()>1);
  return p;
}

// ---------------------------------------------------------------------
std::string LibComp::dev_type() const
{
	assert((Props.size() > 1));
	return prop(1).Value.toStdString();
}

// ---------------------------------------------------------------------
void LibComp::set_dev_type(std::string const& t)
{
	assert((Props.size() > 1));
	prop(1).Value = QString::fromStdString(t);
}

// ---------------------------------------------------------------------
// Makes the schematic symbol subcircuit with the correct number
// of ports.
void LibComp::createSymbol()
{
  assert(Props.size()>1);
  set_qucs_text_position(INT_MIN, INT_MIN);
  if(loadSymbol() > 0) {
    if(tx() == INT_MIN) { untested();
		 set_qucs_text_position(x1()+4, ty());
	 }else{
	 }
    if(ty() == INT_MIN) { untested();
		 set_qucs_text_position(tx(), y2()+4);
	 }else{
	 }
    assert(Props.size()>1);
  } else { untested();
    // only paint a rectangle
    Lines.push_back(qucs::Line(-15, -15, 15, -15, QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line( 15, -15, 15,  15, QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line(-15,  15, 15,  15, QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line(-15, -15,-15,  15, QPen(Qt::darkBlue,2)));

    set_x1(-18);
    set_y1(-18);
    set_x2(18);
    set_y2(18);

	 set_qucs_text_position(x1()+4, y2()+4);
    assert(Props.size()>1);
  }

  if (Props.size()==0) { untested();
	  // BUG: props got lost.
	  Props.push_back(qucs::Property("Lib", "BUG", true,
			QObject::tr("name of qucs library file")));
  }else{
  }
  if (Props.size()==1) { untested();
	  // BUG: props got lost.
	  Props.push_back(qucs::Property("Comp", "BUG", true,
			QObject::tr("name of component in library")));
  }else{
  }

}

// ---------------------------------------------------------------------
// Loads the section with name "Name" from library file into "Section".
int LibComp::loadSection(const QString& Name, QString& Section,
			 QStringList *Includes)
{
  QDir Directory(QucsSettings.LibDir);
  QFile file(Directory.absoluteFilePath(prop(0).Value + ".lib"));
  if(!file.open(QIODevice::ReadOnly))
    return -1;

  QString libDefaultSymbol;

  QTextStream ReadWhole(&file);
  Section = ReadWhole.readAll();
  file.close();


  if(Section.left(14) != "<Qucs Library ")  // wrong file type ?
    return -2;

  int Start, End = Section.indexOf(' ', 14);
  if(End < 15) return -3;
  QString Line = Section.mid(14, End-14); // extract version string
  VersionTriplet LibVersion = VersionTriplet(Line);
  if (LibVersion > QucsVersion) // wrong version number ?
    return -3;

  if(Name == "Symbol") {
    Start = Section.indexOf("\n<", 14); // library has default symbol
    if(Start > 0)
      if(Section.mid(Start+2, 14) == "DefaultSymbol>") {
        Start += 16;
        End = Section.indexOf("\n</DefaultSymbol>", Start);
        if(End < 0)  return -9;
        libDefaultSymbol = Section.mid(Start, End-Start);
      }
  }

  // search component
  Line = "\n<Component " + prop(1).Value + ">";
  Start = Section.indexOf(Line);
  if(Start < 0)  return -4;  // component not found
  Start = Section.indexOf('\n', Start);
  if(Start < 0)  return -5;  // file corrupt
  Start++;
  End = Section.indexOf("\n</Component>", Start);
  if(End < 0)  return -6;  // file corrupt
  Section = Section.mid(Start, End-Start+1);
  
  // search model includes
  if(Includes) {
    int StartI, EndI;
    StartI = Section.indexOf("<"+Name+"Includes");
    if(StartI >= 0) {  // includes found
      StartI = Section.indexOf('"', StartI);
      if(StartI < 0)  return -10;  // file corrupt
      EndI = Section.indexOf('>', StartI);
      if(EndI < 0)  return -11;  // file corrupt
      StartI++; EndI--;
      QString inc = Section.mid(StartI, EndI-StartI);
      QStringList f = inc.split(QRegExp("\"\\s+\""));
      for(QStringList::Iterator it = f.begin(); it != f.end(); ++it ) { untested();
	Includes->append(*it);
      }
    }
  }

  // search model
  Start = Section.indexOf("<"+Name+">");
  if(Start < 0) {
    if((Name == "Symbol") && (!libDefaultSymbol.isEmpty())) {
      // component does not define its own symbol but the library defines a default symbol
      Section = libDefaultSymbol;
      return 0;
    } else { untested();
      return -7;  // symbol not found
    }
  }
  Start = Section.indexOf('\n', Start);
  if(Start < 0)  return -8;  // file corrupt
  while(Section.at(++Start) == ' ') ;
  End = Section.indexOf("</"+Name+">", Start);
  if(End < 0)  return -9;  // file corrupt

  // snip actual model
  Section = Section.mid(Start, End-Start);
  return 0;
}

// ---------------------------------------------------------------------
// Loads the symbol for the subcircuit from the schematic file and
// returns the number of painting elements.
int LibComp::loadSymbol()
{
  assert(Props.size()>1);
  int z, Result;
  QString FileString, Line;
  z = loadSection("Symbol", FileString);
  assert(Props.size()>1);
  if(z < 0) { untested();
    if(z != -7)  return z;

    // If library component not defined as subcircuit, then load
    // new component and transfer data to this component.
    z = loadSection("Model", Line);
    assert(Props.size()>1);
    if(z < 0)  return z;

    // Note: the component returned from getComponentFromName does not have an ownership yet.
    std::shared_ptr<Component> pc(getComponentFromName(Line));
    if(!pc)  return -20;
    assert(Props.size()>1);
    copyComponent(*pc);
    assert(Props.size()>1);

    return 1;
  }else{
  }


  z  = 0;
  set_x1(INT_MAX);
  set_y1(INT_MAX);
  set_x2(INT_MIN);
  set_y2(INT_MIN);

  assert(Props.size()>1);
  if(Props.size()==2){
	  // BUG: analyseLine erases the trailing parameter?!
	  // (wtf)
	 // Props.push_back(qucs::Property("dummy", "", false));
  }else{
  }
  QTextStream stream(&FileString, QIODevice::ReadOnly);
  while(!stream.atEnd()) {
    Line = stream.readLine();
    Line = Line.trimmed();
    if(Line.isEmpty())  continue;
    if(Line.at(0) != '<') return -11;
    if(Line.at(Line.length()-1) != '>') return -12;
    Line = Line.mid(1, Line.length()-2); // cut off start and end character
	 assert(Props.size()>1);
    Result = analyseLine(Line, 2);
	 assert(Props.size()>1);
    if(Result < 0) return -13;   // line format error
    z += Result;
  }
  assert(Props.size()>1);

  set_x1(x1() - 4);
  set_x2(x2() + 4);   // enlarge component boundings a little
  set_y1(y1() - 4);
  set_y2(y2() + 4);
  return z;      // return number of ports
}

// -------------------------------------------------------
QString LibComp::getSubcircuitFile()
{
  QDir Directory(QucsSettings.LibDir);
  QString FileName = Directory.absoluteFilePath(prop(0).Value);
  return misc::properAbsFileName(FileName);
}

// -------------------------------------------------------
bool LibComp::createSubNetlist(QTextStream *stream, QStringList &FileList,
			       int type)
{
  int r = -1;
  QString FileString;
  QStringList Includes;
  if(type&1) {
    r = loadSection("Model", FileString, &Includes);
  } else if(type&2) { untested();
    r = loadSection("VHDLModel", FileString, &Includes);
  } else if(type&4) { untested();
    r = loadSection("VerilogModel", FileString, &Includes);
  }
  if(r < 0)  return false;

  // also include files
  int error = 0;
  for(QStringList::Iterator it = Includes.begin();
      it != Includes.end(); ++it ) { untested();
    QString s = getSubcircuitFile()+"/"+*it;
    if(FileList.indexOf(s) >= 0) continue;
    FileList.append(s);

    // load file and stuff into stream
    QFile file(s);
    if(!file.open(QIODevice::ReadOnly)) { untested();
      error++;
    } else { untested();
      QByteArray FileContent = file.readAll();
      file.close();
      //?stream->writeRawBytes(FileContent.value(), FileContent.size());
      (*stream) << FileContent.data();
      qDebug() << "hi from libcomp";
    }
  }

  (*stream) << "\n" << FileString << "\n";
  return error > 0 ? false : true;
}

// -------------------------------------------------------
std::string LibComp::attr_get() const
{
  std::string ret = Component::attr_get();
  ret += ", qucs_type=\"Lib\"";
  return ret;
}

// -------------------------------------------------------
QString LibComp::createType()
{
  assert(Props.size()>0);
  QString Type = misc::properFileName(prop(0).Value);
  if(Props.size() < 2) { untested();
    return misc::properName(Type);
  } else {
    return misc::properName(Type + "_" + prop(1).Value);
  }
}

// -------------------------------------------------------
QString LibComp::netlist()
{
  QString s = "Sub:"+Name;   // output as subcircuit

  // output all node names
  for(auto p1 = Ports.begin(); p1 != Ports.end(); ++p1)
    s += " "+p1->getConnection()->Name;   // node names

  // output property
  s += " Type=\""+createType()+"\"";   // type for subcircuit

  // output user defined parameters
  auto pp = Props.begin();
  ++pp;
  ++pp;
  for( ; pp != Props.end(); ++pp)
    s += " "+pp->Name+"=\""+pp->Value+"\"";

  return s + '\n';
}

// -------------------------------------------------------
QString LibComp::verilogCode(int)
{ untested();
  QString s = "  Sub_" + createType() + " " + Name + " (";

  // output all node names
  auto iport = Ports.begin();
  if (iport != Ports.end()) { untested();
    s += iport->getConnection()->Name;
    while (++iport != Ports.end())
      s += ", "+iport->getConnection()->Name;   // node names
  }

  s += ");\n";
  return s;
}

// -------------------------------------------------------
QString LibComp::vhdlCode(int)
{ untested();
  QString s = "  " + Name + ": entity Sub_" + createType() + " port map (";

  // output all node names
  auto iport = Ports.begin();
  if (iport != Ports.end()) { untested();
    s += iport->getConnection()->Name;
    while (++iport != Ports.end())
      s += ", "+iport->getConnection()->Name;   // node names
  }

  s += ");\n";
  return s;
}
