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
  set_qucs_text_position(x2+4, y1+4);
  Model = "_MOSFET";
}

// -------------------------------------------------------
Component* MOSFET::newOne()
{
  MOSFET* p = new MOSFET();
  p->prop(0).Value = prop(0).Value;
  p->prop(1).Value = prop(1).Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* MOSFET::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("n-MOSFET");
  BitmapFile = (char *) "nmosfet";

  if(getNewOne)  return new MOSFET();
  return 0;
}

// -------------------------------------------------------
Element* MOSFET::info_p(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("p-MOSFET");
  BitmapFile = (char *) "pmosfet";

  if(getNewOne) {
    MOSFET* p = new MOSFET();
    p->prop(0).Value = "pfet";
    p->prop(1).Value = "-1.0 V";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
Element* MOSFET::info_depl(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("depletion MOSFET");
  BitmapFile = (char *) "dmosfet";

  if(getNewOne) {
    MOSFET* p = new MOSFET();
    p->prop(1).Value = "-1.0 V";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
void MOSFET::createSymbol()
{
  Lines.push_back(qucs::Line(-14,-13,-14, 13,QPen(Qt::darkBlue,3)));
  Lines.push_back(qucs::Line(-30,  0,-14,  0,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-10,-11,  0,-11,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0,-11,  0,-30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10, 11,  0, 11,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0,  0,  0, 30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10,  0,  0,  0,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-10,-16,-10, -7,QPen(Qt::darkBlue,3)));
  Lines.push_back(qucs::Line(-10,  7,-10, 16,QPen(Qt::darkBlue,3)));

  if(prop(0).Value == "nfet") {
    Lines.push_back(qucs::Line( -9,  0, -4, -5,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line( -9,  0, -4,  5,QPen(Qt::darkBlue,2)));
  }
  else {
    Lines.push_back(qucs::Line( -1,  0, -6, -5,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line( -1,  0, -6,  5,QPen(Qt::darkBlue,2)));
  }

  if((prop(1).Value.trimmed().at(0) == '-') ==
     (prop(0).Value == "nfet"))
    Lines.push_back(qucs::Line(-10, -8,-10,  8,QPen(Qt::darkBlue,3)));
  else
    Lines.push_back(qucs::Line(-10, -4,-10,  4,QPen(Qt::darkBlue,3)));
  
  Ports.push_back(qucs::Port(-30,  0));
  Ports.push_back(qucs::Port(  0,-30));
  Ports.push_back(qucs::Port(  0, 30));

  x1 = -30; y1 = -30;
  x2 =   4; y2 =  30;
}

// -------------------------------------------------------
QString MOSFET::netlist()
{
  QString s = "MOSFET:"+Name;

  // output all node names
  for(auto p1 = Ports.begin(); p1 != Ports.end(); ++p1)
    s += " "+p1->getConnection()->Name;   // node names
  s += " "+port(2).getConnection()->Name;  // connect substrate to source

  // output all properties
  for(auto p2 = Props.begin(); p2 != Props.end(); ++p2)
    s += " "+p2->Name+"=\""+p2->Value+"\"";

  return s + '\n';
}
