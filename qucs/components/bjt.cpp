/***************************************************************************
                                bjt.cpp
                               ---------
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

#include "bjt.h"
#include "node.h"


BJT::BJT()
{
  // properties obtained from "Basic_BJT" in bjtsub.cpp
  Description = QObject::tr("bipolar junction transistor");
  createSymbol();
  set_qucs_text_position(x2()+4, y1()+4);
  Model = "_BJT";
}

// -------------------------------------------------------
Component* BJT::newOne()
{
  BJT* p = new BJT();
  p->Props.front().Value = Props.front().Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* BJT::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("npn transistor");
  BitmapFile = (char *) "npn";

  if(getNewOne)  return new BJT();
  return 0;
}

// -------------------------------------------------------
Element* BJT::info_pnp(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("pnp transistor");
  BitmapFile = (char *) "pnp";

  if(getNewOne) {
    BJT* p = new BJT();
    p->Props.front().Value = "pnp";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
void BJT::createSymbol()
{
  Lines.push_back(qucs::Line(-10,-15,-10, 15,QPen(Qt::darkBlue,3)));
  Lines.push_back(qucs::Line(-30,  0,-10,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10, -5,  0,-15,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0,-15,  0,-30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10,  5,  0, 15,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0, 15,  0, 30,QPen(Qt::darkBlue,2)));

  if(Props.front().Value == "npn") {
    Lines.push_back(qucs::Line( -6, 15,  0, 15,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line(  0,  9,  0, 15,QPen(Qt::darkBlue,2)));
  }
  else {
    Lines.push_back(qucs::Line( -5, 10, -5, 16,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line( -5, 10,  1, 10,QPen(Qt::darkBlue,2)));
  }

  Ports.push_back(qucs::Port(-30,  0));
  Ports.push_back(qucs::Port(  0,-30));
  Ports.push_back(qucs::Port(  0, 30));

  set_x1(-30);
  set_y1(-30);
  set_x2(4);
  set_y2(30);
}

// -------------------------------------------------------
QString BJT::netlist()
{
  QString s = "BJT:"+Name;

  // output all node names
  for(auto p1 = Ports.begin(); p1 != Ports.end(); ++p1)
    s += " "+p1->getConnection()->Name;   // node names
  s += " "+port(1).getConnection()->Name;  // connect substrate to collector

  // output all properties
  for(auto p2 = Props.begin(); p2 != Props.end(); ++p2)
    s += " "+p2->Name+"=\""+p2->Value+"\"";

  return s + '\n';
}
