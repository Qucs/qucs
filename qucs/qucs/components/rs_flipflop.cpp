/***************************************************************************
                              rs_flipflop.cpp
                             -----------------
    begin                : Fri Jan 06 2006
    copyright            : (C) 2006 by Michael Margraf
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

#include "rs_flipflop.h"
#include "node.h"

RS_FlipFlop::RS_FlipFlop()
{
  Type = isDigitalComponent;
  Description = QObject::tr("RS flip flop");

  Props.append(new Property("t", "0", false, QObject::tr("delay time")));

  Lines.append(new Line(-20,-25, 20,-25,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-20, 25, 20, 25,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-20,-25,-20, 25,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 20,-25, 20, 25,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-30,-20,-20,-20,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30, 20,-20, 20,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,-20, 20,-20,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30, 20, 20, 20,QPen(QPen::darkBlue,2)));

  Texts.append(new Text(-17,-25, "R", QPen::darkBlue, 12.0));
  Texts.append(new Text(-17,  9, "S", QPen::darkBlue, 12.0));
  Texts.append(new Text(  6,-25, "Q", QPen::darkBlue, 12.0));
  Texts.append(new Text(  6,  9, "Q", QPen::darkBlue, 12.0));
  Lines.append(new Line(  7,  9, 16,  9,QPen(QPen::darkBlue,1)));

  Ports.append(new Port(-30,-20));  // R
  Ports.append(new Port(-30, 20));  // S
  Ports.append(new Port( 30,-20));  // Q
  Ports.append(new Port( 30, 20));  // nQ

  x1 = -30; y1 = -29;
  x2 =  30; y2 =  29;
  tx = x1+4;
  ty = y2+4;
  Model = "RSFF";
  Name  = "Y";
}

// -------------------------------------------------------
QString RS_FlipFlop::VHDL_Code(int NumPorts)
{
  QString s = ";\n";
  if(NumPorts <= 0)  // no truth table simulation ?
    if(strtod(Props.getFirst()->Value.latin1(), 0) != 0.0)  // delay time
      s = " after " + Props.getFirst()->Value + ";\n";

  s = "  " +
    Ports.at(2)->Connection->Name + " <= " +
    Ports.at(0)->Connection->Name + " nor " +
    Ports.at(3)->Connection->Name + s + "  " +
    Ports.at(3)->Connection->Name + " <= " +
    Ports.at(1)->Connection->Name + " nor " +
    Ports.at(2)->Connection->Name + s;
  return s;
}

// -------------------------------------------------------
Component* RS_FlipFlop::newOne()
{
  return new RS_FlipFlop();
}

// -------------------------------------------------------
Element* RS_FlipFlop::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("RS-FF");
  BitmapFile = "rsflipflop";

  if(getNewOne)  return new RS_FlipFlop();
  return 0;
}
