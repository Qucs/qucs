/***************************************************************************
                                mosfet.cpp
                               ------------
    begin                : Fri Jun 4 2004
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

#include "mosfet.h"
#include "node.h"


MOSFET::MOSFET()
{
  // properties obtained from "Basic_MOSFET" in mosfet_sub.cpp
  Description = QObject::tr("MOS field-effect transistor");
  createSymbol();
  tx = x2+4;
  ty = y1+4;
  Model = "_MOSFET";
}

// -------------------------------------------------------
Component* MOSFET::newOne()
{
  MOSFET* p = new MOSFET();
  p->Props.at(0)->Value = Props.at(0)->Value;
  p->Props.at(1)->Value = Props.at(1)->Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
GraphicItem* MOSFET::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("n-MOSFET");
  BitmapFile = (char *) "nmosfet";

  if(getNewOne)  return new MOSFET();
  return 0;
}

// -------------------------------------------------------
GraphicItem* MOSFET::info_p(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("p-MOSFET");
  BitmapFile = (char *) "pmosfet";

  if(getNewOne) {
    MOSFET* p = new MOSFET();
    p->Props.at(0)->Value = "pfet";
    p->Props.at(1)->Value = "-1.0 V";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
GraphicItem* MOSFET::info_depl(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("depletion MOSFET");
  BitmapFile = (char *) "dmosfet";

  if(getNewOne) {
    MOSFET* p = new MOSFET();
    p->Props.at(0);
    p->Props.at(1)->Value = "-1.0 V";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
void MOSFET::createSymbol()
{
  Lines.append(new Line(-14,-13,-14, 13,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-30,  0,-14,  0,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-10,-11,  0,-11,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0,-11,  0,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10, 11,  0, 11,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0,  0,  0, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10,  0,  0,  0,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-10,-16,-10, -7,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-10,  7,-10, 16,QPen(Qt::darkBlue,3)));

  if(Props.at(0)->Value == "nfet") {
    Lines.append(new Line( -9,  0, -4, -5,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( -9,  0, -4,  5,QPen(Qt::darkBlue,2)));
  }
  else {
    Lines.append(new Line( -1,  0, -6, -5,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( -1,  0, -6,  5,QPen(Qt::darkBlue,2)));
  }

  if((Props.at(1)->Value.trimmed().at(0) == '-') ==
     (Props.at(0)->Value == "nfet"))
    Lines.append(new Line(-10, -8,-10,  8,QPen(Qt::darkBlue,3)));
  else
    Lines.append(new Line(-10, -4,-10,  4,QPen(Qt::darkBlue,3)));
  
  Ports.append(new Port(-30,  0));
  Ports.append(new Port(  0,-30));
  Ports.append(new Port(  0, 30));

  x1 = -30; y1 = -30;
  x2 =   4; y2 =  30;
}

// -------------------------------------------------------
QString MOSFET::netlist()
{
  QString s = "MOSFET:"+Name;

  // output all node names
  foreach(Port *p1, Ports)
    s += " "+p1->Connection->Name;   // node names
  s += " "+Ports.at(2)->Connection->Name;  // connect substrate to source

  // output all properties
  foreach(Property *p2, Props)
    s += " "+p2->Name+"=\""+p2->Value+"\"";

  return s + '\n';
}
