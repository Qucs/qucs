/***************************************************************************
                          subcircuit.cpp  -  description
                             -------------------
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "subcircuit.h"
#include "qucs.h"

#include <qdir.h>
#include <qfileinfo.h>

#include <limits.h>

extern QDir QucsWorkDir;


Subcircuit::Subcircuit()
{
  Description = QObject::tr("subcircuit");

  Ports.append(new Port(0,  0));  // dummy port because of being device

  Model = "Sub";
  Name  = "SUB";

  Props.append(new Property("File", "", true,
		QObject::tr("name of qucs schematic file")));
}

Subcircuit::~Subcircuit()
{
}

// ---------------------------------------------------------------------
Component* Subcircuit::newOne()
{
  Subcircuit *p = new Subcircuit();
  p->Props.getFirst()->Value = Props.getFirst()->Value;
//  p->remakeSymbol(Ports.count());
  p->recreate();
  return p;
}

/*Component* Subcircuit::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Subcircuit");
  BitmapFile = "";  // has no bitmap

  if(getNewOne)  return new Subcircuit(Props.getLast()->Value.toInt());
  return 0;
}*/

// ---------------------------------------------------------------------
// Makes the schematic symbol subcircuit with the correct number
// of ports.
void Subcircuit::recreate()
{
  int No;
  QString FileName;

  QFileInfo Info(Props.getFirst()->Value);
  if(Info.isRelative())
    FileName = QucsWorkDir.filePath(Props.getFirst()->Value);
  else
    FileName = Props.getFirst()->Value;

  if(loadSymbol(FileName) > 0) {
    performModification();
    return;
  }

  No = QucsApp::testFile(FileName);
  if(No < 0)  No = 0;

  remakeSymbol(No);
}

// ---------------------------------------------------------------------
void Subcircuit::remakeSymbol(int No)
{
  Arcs.clear();
  Lines.clear();
  Texts.clear();
  Ports.clear();

  int h = 30*((No-1)/2) + 15;
  Lines.append(new Line(-15, -h, 15, -h,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 15, -h, 15,  h,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-15,  h, 15,  h,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-15, -h,-15,  h,QPen(QPen::darkBlue,2)));
  Texts.append(new Text( -7,  0,"sub"));

  int i=0, y = 15-h;
  while(i<No) {
    i++;
    Lines.append(new Line(-30,  y,-15,  y,QPen(QPen::darkBlue,2)));
    Ports.append(new Port(-30,  y));
    Texts.append(new Text(-25,y-3,QString::number(i)));

    if(i == No) break;
    i++;
    Lines.append(new Line( 15,  y, 30,  y,QPen(QPen::darkBlue,2)));
    Ports.append(new Port( 30,  y));
    Texts.append(new Text( 20,y-3,QString::number(i)));
    y += 60;
  }

  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+2;

  performModification();
}

// ---------------------------------------------------------------------
void Subcircuit::performModification()
{
  tx = x1+4;
  ty = y2+4;

  bool mmir = mirroredX;
  int  rrot = rotated;
  if(mmir)  mirrorX();   // mirror
  for(int z=0; z<rrot; z++)  rotate(); // rotate


  rotated = rrot;   // restore properties (were changed by rotate/mirror)
  mirroredX = mmir;
}

// ---------------------------------------------------------------------
// Loads the symbol for the subcircuit from the schematic file and
// returns the number of painting elements.
int Subcircuit::loadSymbol(const QString& DocName)
{
  Arcs.clear();
  Lines.clear();
  Texts.clear();
  Ports.clear();

  QFile file(DocName);
  if(!file.open(IO_ReadOnly)) {
    return -1;
  }

  QString Line;
  // *****************************************************************
  // To strongly speed up the file read operation the whole file is
  // read into the memory in one piece.
  QTextStream ReadWhole(&file);
  QString FileString = ReadWhole.read();
  file.close();
  QTextStream stream(&FileString, IO_ReadOnly);


  // read header **************************
  do {
    if(stream.atEnd()) {
      file.close();
      return -2;
    }
    Line = stream.readLine();
    Line = Line.stripWhiteSpace();
  } while(Line.isEmpty());

  if(Line.left(16) != "<Qucs Schematic ") {  // wrong file type ?
    file.close();
    return -3;
  }

  QString s = PACKAGE_VERSION;
  s.remove('.');
  Line = Line.mid(16, Line.length()-17);
  Line.remove('.');
  if(Line > s) {  // wrong version number ? (only backward compatible)
    file.close();
    return -4;
  }

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
      file.close();
      x1 -= 4;   // enlarge component boundings a little
      x2 += 4;
      y1 -= 4;
      y2 += 4;
      return z;      // return number of ports
    }

    Line = Line.stripWhiteSpace();
    if(Line.at(0) != '<') return -1;
    if(Line.at(Line.length()-1) != '>') return -1;
    Line = Line.mid(1, Line.length()-2); // cut off start and end character
    Result = analyseLine(Line);
    if(Result < 0) return -6;   // line format error
    z += Result;
  }
  return -5;   // field not closed
}

// ---------------------------------------------------------------------
int Subcircuit::analyseLine(const QString& Row)
{
  QPen Pen;
  QString s;
  int i1, i2, i3, i4, i5, i6;

  s    = Row.section(' ',0,0);    // component type
  if(s == "PortSym") {
    if(!getIntegers(Row, &i1, &i2, &i3))  return -1;
    for(i6 = Ports.count(); i6<i3; i6++)  // if ports not in numerical order
      Ports.append(new Port(0, 0));

    Ports.at(i3-1)->x  = i1;
    Ports.current()->y = i2;

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1 > x2)  x2 = i1;
    if(i2 < y1)  y1 = i2;
    if(i2 > y2)  y2 = i2;
    return 1;
  }
  else if(s == "Line") {
    if(!getIntegers(Row, &i1, &i2, &i3, &i4))  return -1;
    if(!getPen(Row, Pen, 5))  return -1;
    i3 += i1;
    i4 += i2;
    Lines.append(new Line(i1, i2, i3, i4, Pen));

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1 > x2)  x2 = i1;
    if(i2 < y1)  y1 = i2;
    if(i2 > y2)  y2 = i2;
    if(i3 < x1)  x1 = i3;
    if(i3 > x2)  x2 = i3;
    if(i4 < y1)  y1 = i4;
    if(i4 > y2)  y2 = i4;
    return 1;
  }
  else if(s == "EArc") {
    if(!getIntegers(Row, &i1, &i2, &i3, &i4, &i5, &i6))  return -1;
    if(!getPen(Row, Pen, 7))  return -1;
    Arcs.append(new Arc(i1, i2, i3, i4, i5, i6, Pen));

    if(i1 < x1)  x1 = i1;  // keep track of component boundings
    if(i1+i3 > x2)  x2 = i1+i3;
    if(i2 < y1)  y1 = i2;
    if(i2+i4 > y2)  y2 = i2+i4;
    return 1;
  }
  else if(s == "Text") {
  }

  return 0;
}

// ---------------------------------------------------------------------
bool Subcircuit::getIntegers(const QString& s, int *i1, int *i2, int *i3,
			     int *i4, int *i5, int *i6)
{
  bool ok;
  QString n;

  if(!i1) return true;
  n  = s.section(' ',1,1);
  *i1 = n.toInt(&ok);
  if(!ok) return false;

  if(!i2) return true;
  n  = s.section(' ',2,2);
  *i2 = n.toInt(&ok);
  if(!ok) return false;

  if(!i3) return true;
  n  = s.section(' ',3,3);
  *i3 = n.toInt(&ok);
  if(!ok) return false;

  if(!i4) return true;
  n  = s.section(' ',4,4);
  *i4 = n.toInt(&ok);
  if(!ok) return false;

  if(!i5) return true;
  n  = s.section(' ',5,5);
  *i5 = n.toInt(&ok);
  if(!ok) return false;

  if(!i6) return true;
  n  = s.section(' ',6,6);
  *i6 = n.toInt(&ok);
  if(!ok) return false;

  return true;
}

// ---------------------------------------------------------------------
bool Subcircuit::getPen(const QString& s, QPen& Pen, int i)
{
  bool ok;
  QString n;

  n  = s.section(' ',i,i);    // color
  QColor co;
  co.setNamedColor(n);
  Pen.setColor(co);
  if(!Pen.color().isValid()) return false;

  i++;
  n  = s.section(' ',i,i);    // thickness
  Pen.setWidth(n.toInt(&ok));
  if(!ok) return false;

  i++;
  n  = s.section(' ',i,i);    // line style
  Pen.setStyle((Qt::PenStyle)n.toInt(&ok));
  if(!ok) return false;

  return true;
}
