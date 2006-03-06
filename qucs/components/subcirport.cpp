/***************************************************************************
                               subcirport.cpp
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

#include "subcirport.h"
#include "node.h"
#include "qucsdoc.h"

SubCirPort::SubCirPort()
{
  Type = isComponent;   // both analog and digital
  Description = QObject::tr("port of a subcircuit");

  // This property must be the first one !
  Props.append(new Property("Num", "1", true,
		QObject::tr("number of the port within the subcircuit")));
  // This property must be the second one !
  Props.append(new Property("Type", "analog", false,
		QObject::tr("type of the port (for digital simulation only)")
		+" [analog, in, out, inout]"));

  createSymbol();
  tx = x1+4;
  ty = y2+4;
  Model = "Port";
  Name  = "P";
}

SubCirPort::~SubCirPort()
{
}

void SubCirPort::createSymbol()
{
  if(Props.at(1)->Value == "in") {
    Lines.append(new Line( -9,  0,  0,  0,QPen(QPen::darkBlue,2)));
    Lines.append(new Line(-14, -5, -9,  0,QPen(QPen::darkGreen,2)));
    Lines.append(new Line(-14,  5, -9,  0,QPen(QPen::darkGreen,2)));
    Lines.append(new Line(-25, -5,-14, -5,QPen(QPen::darkGreen,2)));
    Lines.append(new Line(-25,  5,-14,  5,QPen(QPen::darkGreen,2)));
    Lines.append(new Line(-25, -5,-25,  5,QPen(QPen::darkGreen,2)));
  }
  else if(Props.at(1)->Value == "out") {
    Lines.append(new Line( -9,  0,  0,  0,QPen(QPen::darkBlue,2)));
    Lines.append(new Line(-20, -5,-25,  0,QPen(QPen::red,2)));
    Lines.append(new Line(-20,  5,-25,  0,QPen(QPen::red,2)));
    Lines.append(new Line(-20, -5, -9, -5,QPen(QPen::red,2)));
    Lines.append(new Line(-20,  5, -9,  5,QPen(QPen::red,2)));
    Lines.append(new Line( -9, -5, -9,  5,QPen(QPen::red,2)));
  }
  else {
    Arcs.append(new Arc(-25, -6, 13, 13,  0, 16*360,QPen(QPen::darkBlue,2)));
    Lines.append(new Line(-14,  0,  0,  0,QPen(QPen::darkBlue,2)));
  }

  Ports.append(new Port(  0,  0));

  x1 = -27; y1 = -8;
  x2 =   0; y2 =  8;
}

Component* SubCirPort::newOne()
{
  return new SubCirPort();
}

// -------------------------------------------------------
Element* SubCirPort::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Subcircuit Port");
  BitmapFile = "subport";

  if(getNewOne)  return new SubCirPort();
  return 0;
}

// -------------------------------------------------------
QString SubCirPort::NetList()
{
  return QString("");
}

// -------------------------------------------------------
QString SubCirPort::VHDL_Code(int)
{
  if(Props.at(1)->Value != "out")  return QString("");

  // Insert dummy buffer to avoid reading from an output port.
  QString s = "  nnout_";
  Node *pn = Ports.getFirst()->Connection;
  s += pn->Name + " <= ";
  s += pn->Name + " or '0';";
  return s;
}

// -------------------------------------------------------
void SubCirPort::recreate(QucsDoc *Doc)
{
  if(Doc) {
    Doc->Comps->setAutoDelete(false);
    Doc->deleteComp(this);
  }

  Arcs.clear();
  Ports.clear();
  Lines.clear();
  createSymbol();
  performModification();  // rotate and mirror

  if(Doc) {
    Doc->insertRawComponent(this);
    Doc->Comps->setAutoDelete(true);
  }
}
