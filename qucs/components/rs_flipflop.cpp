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

#include <stdlib.h>

#include "rs_flipflop.h"
#include "node.h"

RS_FlipFlop::RS_FlipFlop()
{
  Type = isDigitalComponent;
  Description = QObject::tr("RS flip flop");

  Props.append(new Property("t", "0", false, QObject::tr("delay time")));

  Lines.append(new Line(-20,-20, 20,-20,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-20, 20, 20, 20,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-20,-20,-20, 20,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 20,-20, 20, 20,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-30,-10,-20,-10,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30, 10,-20, 10,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,-10, 20,-10,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30, 10, 20, 10,QPen(QPen::darkBlue,2)));

  Texts.append(new Text(-17,-17, "R", QPen::darkBlue, 12.0));
  Texts.append(new Text(-17,  3, "S", QPen::darkBlue, 12.0));
  Texts.append(new Text(  6,-17, "Q", QPen::darkBlue, 12.0));
  Texts.append(new Text(  6,  3, "Q", QPen::darkBlue, 12.0));
  Lines.append(new Line(  7,   3, 15,   3,QPen(QPen::darkBlue,1)));

  Ports.append(new Port(-30,-10));  // R
  Ports.append(new Port(-30, 10));  // S
  Ports.append(new Port( 30,-10));  // Q
  Ports.append(new Port( 30, 10));  // nQ

  x1 = -30; y1 = -24;
  x2 =  30; y2 =  24;
  tx = x1+4;
  ty = y2+4;
  Model = "RSFF";
  Name  = "Y";
}

// -------------------------------------------------------
QString RS_FlipFlop::vhdlCode(int NumPorts)
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
    Ports.at(2)->Connection->Name + s + '\n';
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
  Name = QObject::tr("RS-FlipFlop");
  BitmapFile = "rsflipflop";

  if(getNewOne)  return new RS_FlipFlop();
  return 0;
}
