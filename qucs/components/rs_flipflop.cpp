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
#include "misc.h"

RS_FlipFlop::RS_FlipFlop()
{
  Type = isDigitalComponent;
  Description = QObject::tr("RS flip flop");

  Props.push_back(qucs::Property("t", "0", false, QObject::tr("delay time")));

  Lines.push_back(qucs::Line(-20,-20, 20,-20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-20, 20, 20, 20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-20,-20,-20, 20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 20,-20, 20, 20,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-30,-10,-20,-10,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30, 10,-20, 10,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30,-10, 20,-10,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30, 10, 20, 10,QPen(Qt::darkBlue,2)));

  Texts.push_back(qucs::Text(-18,-21, "R", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-18, -1, "S", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(  6,-21, "Q", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(  6, -1, "Q", Qt::darkBlue, 12.0));
  Texts.back().over=true;

  Ports.push_back(qucs::Port(-30,-10));  // R
  Ports.push_back(qucs::Port(-30, 10));  // S
  Ports.push_back(qucs::Port( 30,-10));  // Q
  Ports.push_back(qucs::Port( 30, 10));  // nQ

  set_x1(-30);
  set_y1(-24);
  set_x2(30);
  set_y2(24);
  set_qucs_text_position(x1()+4, y2()+4);
  Model = "RSFF";
  Name  = "Y";
}

// -------------------------------------------------------
QString RS_FlipFlop::vhdlCode(int NumPorts)
{
  QString s = "";
  if(NumPorts <= 0) { // no truth table simulation ?
    QString td = prop(0).Value;     // delay time
    if(!misc::VHDL_Delay(td, Name)) return td; // time has not VHDL format
    s = td;
  }
  s += ";\n";

  s = "  " +
    port(2).getConnection()->Name + " <= " +
    port(0).getConnection()->Name + " nor " +
    port(3).getConnection()->Name + s + "  " +
    port(3).getConnection()->Name + " <= " +
    port(1).getConnection()->Name + " nor " +
    port(2).getConnection()->Name + s + '\n';
  return s;
}

// -------------------------------------------------------
QString RS_FlipFlop::verilogCode(int NumPorts)
{
  QString t = "";
  if(NumPorts <= 0) { // no truth table simulation ?
    QString td = prop(0).Value;        // delay time
    if(!misc::Verilog_Delay(td, Name)) return td; // time has not VHDL format
    t = td;
  }
  t += " ";
  
  QString l = "";

  QString s = port(1).getConnection()->Name;
  QString r = port(0).getConnection()->Name;
  QString q = port(2).getConnection()->Name;
  QString b = port(3).getConnection()->Name;
  
  l = "\n  // " + Name + " RS-flipflop\n" +
    "  assign" + t + q + " = ~(" + r + " | " + b + ");\n" +
    "  assign" + t + b + " = ~(" + s + " | " + q + ");\n\n";
  return l;
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
  BitmapFile = (char *) "rsflipflop";

  if(getNewOne)  return new RS_FlipFlop();
  return 0;
}
