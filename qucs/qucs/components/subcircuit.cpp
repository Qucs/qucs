/***************************************************************************
                               subcircuit.cpp
                              ----------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
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

#include "subcircuit.h"
#include "qucs.h"
#include "schematic.h"
#include "misc.h"

#include <QTextStream>
#include <QFileInfo>
#include <QMutex>

#include <limits.h>
#include <io_trace.h>



Subcircuit::Subcircuit()
{
  Type = isComponent;   // both analog and digital
  Description = QObject::tr("subcircuit");

  Props.append(new Property("File", "", false,
		QObject::tr("name of qucs schematic file")));

  Model = "Sub";
  Name  = "SUB";

  // Do NOT call createSymbol() here. But create port to let it rotate.
  Ports.append(new Port(0, 0, false));
}

// ---------------------------------------------------------------------
Component* Subcircuit::newOne()
{
  Subcircuit *p = new Subcircuit();
  p->Props.getFirst()->Value = Props.getFirst()->Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* Subcircuit::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Subcircuit");
  BitmapFile = (char *) "subcircuit";

  if(getNewOne) {
    Subcircuit *p = new Subcircuit();
    p->recreate(0);   // createSymbol() is NOT called in constructor !!!
    return p;
  }
  return 0;
}

// ---------------------------------------------------------------------
// Makes the schematic symbol subcircuit with the correct number
// of ports.
void Subcircuit::createSymbol()
{
  int No;
  QString FileName(Props.getFirst()->Value);
  FileName = getSubcircuitFile();

  tx = INT_MIN;
  ty = INT_MIN;
  if(loadSymbol(FileName) > 0) {  // try to load subcircuit symbol
    if(tx == INT_MIN)  tx = x1+4;
    if(ty == INT_MIN)  ty = y2+4;
    // remove unused ports
    QMutableListIterator<Port *> ip(Ports);
    Port *pp;
    while (ip.hasNext()) {
      pp = ip.next();
      if(!pp->avail) {
          pp = ip.peekNext();
          ip.remove();
      }
    }
  }
  else {
    No = Schematic::testFile(FileName);
    if(No < 0)  No = 0;

    Ports.clear();
    remakeSymbol(No);  // no symbol was found -> create standard symbol
  }
}

// ---------------------------------------------------------------------
void Subcircuit::remakeSymbol(int No)
{
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
{
  QString s = Model+":"+Name;

  // output all node names
  foreach(Port *p1, Ports)
    s += " "+p1->Connection->Name;   // node names

  // type for subcircuit
  QString f = misc::properFileName(Props.first()->Value);
  s += " Type=\""+misc::properName(f)+"\"";

  // output all user defined properties
  for(Property *pp = Props.next(); pp != 0; pp = Props.next())
    s += " "+pp->Name+"=\""+pp->Value+"\"";
  return s + '\n';
}

// -------------------------------------------------------
QString Subcircuit::vhdlCode(int)
{
  QString f = misc::properFileName(Props.first()->Value);
  QString s = "  " + Name + ": entity Sub_" + misc::properName(f);

  // output all user defined properties
  Property *pr = Props.next();
  if (pr) {
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
  if(pp)
    s += pp->Connection->Name;
  while (iport.hasNext()) {
    pp = iport.next();
    s += ", "+pp->Connection->Name;   // node names
  }

  s += ");\n";
  return s;
}

// -------------------------------------------------------
QString Subcircuit::verilogCode(int)
{
  QString f = misc::properFileName(Props.first()->Value);
  QString s = "  Sub_" + misc::properName(f);

  // output all user defined properties
  Property *pr = Props.next();
  if (pr) {
    s += " #(";
    s += misc::Verilog_Param(pr->Value);
    for(pr = Props.next(); pr != 0; pr = Props.next())
      s += ", " + misc::Verilog_Param(pr->Value);
    s += ")";
  }

  // output all node names
  s +=  " " + Name + " (";
  QListIterator<Port *> iport(Ports);
  Port *pp = iport.next();
  if(pp)
    s += pp->Connection->Name;
  while (iport.hasNext()) {
    pp = iport.next();
    s += ", "+pp->Connection->Name;   // node names
  }

  s += ");\n";
  return s;
}

// -------------------------------------------------------
QString Subcircuit::getSubcircuitFile() const
{
  // construct full filename
  QString FileName = Props.getFirst()->Value;

  if (FileName.isEmpty())
  {
      return misc::properAbsFileName(FileName);
  }

  QFileInfo FileInfo(FileName);

  if (FileInfo.exists())
  {
      // the file must be an absolute path to a schematic file
     return FileInfo.absoluteFilePath();
  }
  else
  {
    // get the complete base name (everything except the last '.'
    // and whatever follows
    QString baseName = FileInfo.completeBaseName();

    // if only a file name is supplied, first check if it is in the
    // same directory as the schematic file it is a part of
    if (FileInfo.fileName () == FileName)
    {
        // the file has no path information, just the file name
        if (containingSchematic)
        {
            // check if a file of the same name is in the same directory
            // as the schematic file, if we have a pointer to it, in
            // which case we use this one
            QFileInfo schematicFileInfo = containingSchematic->getFileInfo ();
            QFileInfo localFIleInfo (schematicFileInfo.canonicalPath () + "/" + baseName + ".sch");
            if (localFIleInfo.exists ())
            {
                // return the subcircuit saved in the same directory
                // as the schematic file
                return localFIleInfo.absoluteFilePath();
            }
        }
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

    if (hashsearchresult.isEmpty())
    {
        // the schematic was not found in the hash table, return
        // what would always have been returned in this case
        return misc::properAbsFileName(FileName);
    }
    else
    {
        // we found an entry in the hash table, check it actually still exists
        FileInfo.setFile(hashsearchresult);

        if (FileInfo.exists())
        {
            // it does exist so return the absolute file path
            return FileInfo.absoluteFilePath();
        }
        else
        {
            // the schematic file does not actually exist, return
            // what would always have been returned in this case
            return misc::properAbsFileName(FileName);
        }
    }

  }

}

static SubMap FileList;

// moved from schematic_file.cpp
void Subcircuit::tAC(QTextStream& stream, Schematic* schem, QStringList&
		Collect, int& countInit, int NumPorts, NetLang const& nl){
	Component* pc=this;
	assert(pc->obsolete_model_hack()=="Sub"); // really?
	int i;
	// tell the subcircuit it belongs to this schematic
	pc->setSchematic(schem);
	QString f = pc->getSubcircuitFile();
	SubMap::Iterator it = FileList.find(f);
	if(it != FileList.end())
	{
		if (!it.value().PortTypes.isEmpty())
		{
			i = 0;
			// apply in/out signal types of subcircuit
			foreach(Port *pp, pc->Ports)
			{
				pp->Type = it.value().PortTypes[i];
				pp->Connection->DType = pp->Type;
				i++;
			}
		}
//		continue;   // wtf?
	}

	// The subcircuit has not previously been added
	SubFile sub = SubFile("SCH", f);
	FileList.insert(f, sub);


	// load subcircuit schematic
	QString s=pc->Props.first()->Value;

	//      qDebug() << "reading subckt schematic" << pc->getSubcircuitFile();
	Schematic *d = new Schematic(0, pc->getSubcircuitFile());
	if(!d->loadDocument()) { // BUG. "try".
		delete d;
		/// \todo implement error/warning message dispatcher for GUI and CLI modes.
		QString message = QObject::tr("ERROR: Cannot load subcircuit \"%1\".").arg(s);
		if (QucsMain) // GUI is running
			qDebug() << "ErrText->appendPlainText(message)";
		else // command line
			qCritical() << "Schematic::throughAllComps" << message;
		throw "something wrong in sckt"; // TODO
	}else{
	}
	d->DocName = s;
	d->isVerilog = false; // isVerilog;
	d->isAnalog = true; // isAnalog;
	d->creatingLib = false; // creatingLib; //!?
	auto r = d->createSubNetlist(&stream, countInit, Collect, nullptr, NumPorts, nl);
	if (r) {
		i = 0;
		// save in/out signal types of subcircuit
		foreach(Port *pp, pc->Ports)
		{
			//if(i>=d->PortTypes.count())break;
			pp->Type = d->portType(i);
			pp->Connection->DType = pp->Type;
			i++;
		}
		sub.PortTypes = d->PortTypes;
		FileList.insert(f, sub);
	}
	delete d; // BUG
	if(!r) {
		// throw?
	}
}
