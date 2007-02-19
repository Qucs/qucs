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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "libcomp.h"
#include "qucs.h"
#include "main.h"
#include "schematic.h"

#include <qdir.h>
#include <qfileinfo.h>
#include <qregexp.h>

#include <math.h>
#include <limits.h>

extern QDir QucsWorkDir;


LibComp::LibComp()
{
  Description = QObject::tr("Component taken from Qucs library");

  Ports.append(new Port(0,  0));  // dummy port because of being device

  Model = "Lib";
  Name  = "X";

  Props.append(new Property("Lib", "", true,
		QObject::tr("name of qucs library file")));
  Props.append(new Property("Comp", "", true,
		QObject::tr("name of component in library")));
}

// ---------------------------------------------------------------------
Component* LibComp::newOne()
{
  LibComp *p = new LibComp();
  p->Props.first()->Value = Props.first()->Value;
  p->Props.next()->Value = Props.next()->Value;
  p->recreate(0);
  return p;
}

// ---------------------------------------------------------------------
// Makes the schematic symbol subcircuit with the correct number
// of ports.
void LibComp::createSymbol()
{
  tx = INT_MIN;
  ty = INT_MIN;
  if(loadSymbol() > 0) {
    if(tx == INT_MIN)  tx = x1+4;
    if(ty == INT_MIN)  ty = y2+4;
  }
  else {
    // only paint a rectangle
    Lines.append(new Line(-15, -15, 15, -15, QPen(QPen::darkBlue,2)));
    Lines.append(new Line( 15, -15, 15,  15, QPen(QPen::darkBlue,2)));
    Lines.append(new Line(-15,  15, 15,  15, QPen(QPen::darkBlue,2)));
    Lines.append(new Line(-15, -15,-15,  15, QPen(QPen::darkBlue,2)));

    x1 = -18; y1 = -18;
    x2 =  18; y2 =  18;

    tx = x1+4;
    ty = y2+4;
  }
}

// ---------------------------------------------------------------------
// Loads the section with name "Name" from library file into "Section".
int LibComp::loadSection(const QString& Name, QString& Section)
{
  QDir Directory(QucsSettings.LibDir);
  QFile file(Directory.absFilePath(Props.first()->Value + ".lib"));
  if(!file.open(IO_ReadOnly))
    return -1;

  QTextStream ReadWhole(&file);
  Section = ReadWhole.read();
  file.close();


  if(Section.left(14) != "<Qucs Library ")  // wrong file type ?
    return -2;

  int Start, End = Section.find(' ', 14);
  if(End < 15) return -3;
  QString Line = Section.mid(14, End-14);
  if(!checkVersion(Line)) // wrong version number ?
    return -3;

  if(Name == "Symbol") {
    Start = Section.find("\n<", 14); // if library has default symbol, take it
    if(Start > 0)
      if(Section.mid(Start+2, 14) == "DefaultSymbol>") {
        Start += 16;
        End = Section.find("\n</DefaultSymbol>", Start);
        if(End < 0)  return -9;
        Section = Section.mid(Start, End-Start);
        return 0;
      }
  }

  // search component
  Line = "\n<Component " + Props.next()->Value + ">";
  Start = Section.find(Line);
  if(Start < 0)  return -4;  // component not found
  Start = Section.find('\n', Start);
  if(Start < 0)  return -5;  // file corrupt
  Start++;
  End = Section.find("\n</Component>", Start);
  if(End < 0)  return -6;  // file corrupt
  Section = Section.mid(Start, End-Start+1);
  
  // search model
  Start = Section.find("<"+Name+">");
  if(Start < 0)  return -7;  // symbol not found
  Start = Section.find('\n', Start);
  if(Start < 0)  return -8;  // file corrupt
  while(Section.at(++Start) == ' ') ;
  End = Section.find("</"+Name+">", Start);
  if(End < 0)  return -9;  // file corrupt
  Section = Section.mid(Start, End-Start);
  return 0;
}

// ---------------------------------------------------------------------
// Loads the symbol for the subcircuit from the schematic file and
// returns the number of painting elements.
int LibComp::loadSymbol()
{
  int z, Result;
  QString FileString, Line;
  z = loadSection("Symbol", FileString);
  if(z < 0) {
    if(z != -7)  return z;

    // If library component not defined as subcircuit, then load
    // new component and transfer data to this component.
    z = loadSection("Model", Line);
    if(z < 0)  return z;

    Component *pc = getComponentFromName(Line);
    if(pc == 0)  return -20;

    copyComponent(pc);

    pc->Arcs.setAutoDelete(false);
    pc->Lines.setAutoDelete(false);
    pc->Rects.setAutoDelete(false);
    pc->Ellips.setAutoDelete(false);
    pc->Ports.setAutoDelete(false);
    pc->Texts.setAutoDelete(false);
    pc->Props.setAutoDelete(false);
    delete pc;

    return 1;
  }


  z  = 0;
  x1 = y1 = INT_MAX;
  x2 = y2 = INT_MIN;

  QTextStream stream(&FileString, IO_ReadOnly);
  while(!stream.atEnd()) {
    Line = stream.readLine();
    Line = Line.stripWhiteSpace();
    if(Line.isEmpty())  continue;
    if(Line.at(0) != '<') return -11;
    if(Line.at(Line.length()-1) != '>') return -12;
    Line = Line.mid(1, Line.length()-2); // cut off start and end character
    Result = analyseLine(Line, 2);
    if(Result < 0) return -13;   // line format error
    z += Result;
  }

  x1 -= 4;  x2 += 4;   // enlarge component boundings a little
  y1 -= 4;  y2 += 4;
  return z;      // return number of ports
}

// -------------------------------------------------------
bool LibComp::outputSubNetlist(QTextStream *stream)
{
  int r;
  QString FileString;
  r = loadSection("Model", FileString);
  if(r < 0)  return false;
  (*stream) << "\n" << FileString << "\n";
  return true;
}

// -------------------------------------------------------
QString LibComp::netlist()
{
  QString s = "Sub:"+Name;   // output as subcircuit

  // output all node names
  for(Port *p1 = Ports.first(); p1 != 0; p1 = Ports.next())
    s += " "+p1->Connection->Name;   // node names

  // output property
  QString Type = Props.first()->Value;
  if(!QDir::isRelativePath(Type)) {
    QFileInfo Info(Type);
    Type = Info.fileName();
  }
  Type += "_" + Props.next()->Value;
  Type.replace(QRegExp("\\W"), "_");
  s += " Type=\""+Type+"\"";   // type for subcircuit

  // output user defined parameters
  for(Property *pp = Props.next(); pp != 0; pp = Props.next())
    s += " "+pp->Name+"=\""+pp->Value+"\"";

  return s + '\n';
}
