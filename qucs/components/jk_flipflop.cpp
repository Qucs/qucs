/***************************************************************************
                              jk_flipflop.cpp
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

#include "jk_flipflop.h"
#include "node.h"
#include "misc.h"

JK_FlipFlop::JK_FlipFlop()
{
  Type = isDigitalComponent;
  Description = QObject::tr("JK flip flop with asynchron set and reset");

  Props.push_back(qucs::Property("t", "0", false, QObject::tr("delay time")));

  Lines.push_back(qucs::Line(-20,-30, 20,-30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-20, 30, 20, 30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-20,-30,-20, 30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 20,-30, 20, 30,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-30,-20,-20,-20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30, 20,-20, 20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30,-20, 20,-20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30, 20, 20, 20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30,  0,-20,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0,-30,  0,-40,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0, 30,  0, 40,QPen(Qt::darkBlue,2)));

  Texts.push_back(qucs::Text( -4,-29, "S", Qt::darkBlue,  9.0));
  Texts.push_back(qucs::Text( -4, 14, "R", Qt::darkBlue,  9.0));
  Texts.push_back(qucs::Text(-18,-31, "J", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-18,  8, "K", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(  6,-31, "Q", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(  6,  8, "Q", Qt::darkBlue, 12.0));
  Texts.back().over=true;
  Lines.push_back(qucs::Line(-20, -4,-12,  0,QPen(Qt::darkBlue,0)));
  Lines.push_back(qucs::Line(-20,  4,-12,  0,QPen(Qt::darkBlue,0)));

  Ports.push_back(qucs::Port(-30,-20));  // J
  Ports.push_back(qucs::Port(-30, 20));  // K
  Ports.push_back(qucs::Port( 30,-20));  // Q
  Ports.push_back(qucs::Port( 30, 20));  // nQ
  Ports.push_back(qucs::Port(-30,  0));  // Clock
  Ports.push_back(qucs::Port(  0,-40));  // set
  Ports.push_back(qucs::Port(  0, 40));  // reset

  set_x1(-30);
  set_y1(-40);
  set_x2(30);
  set_y2(40);
  set_qucs_text_position(x1()+4, y2()+4);
  Model = "JKFF";
  Name  = "Y";
}

// -------------------------------------------------------
QString JK_FlipFlop::vhdlCode(int NumPorts)
{
  QString s = "";
  if(NumPorts <= 0) { // no truth table simulation ?
    QString td = prop(0).Value;     // delay time
    if(!misc::VHDL_Delay(td, Name)) return td; // time has not VHDL format
    s += td;
  }
  s += ";\n";

  s = "  " + Name + " : process (" +
      port(5).getConnection()->Name + ", " +
      port(6).getConnection()->Name + ", " +
      port(4).getConnection()->Name + ")\n  begin\n    if (" +
      port(6).getConnection()->Name + "='1') then  " +
      port(2).getConnection()->Name + " <= '0'" + s +"    elsif (" +
      port(5).getConnection()->Name + "='1') then  " +
      port(2).getConnection()->Name + " <= '1'" + s +"    elsif (" +
      port(4).getConnection()->Name + "='1' and " +
      port(4).getConnection()->Name + "'event) then\n      " +
      port(2).getConnection()->Name + " <= (" +
      port(0).getConnection()->Name + " and not " +
      port(2).getConnection()->Name + ") or (not " +
      port(1).getConnection()->Name + " and " +
      port(2).getConnection()->Name + ")" + s +
      "    end if;\n  end process;\n  " +
      port(3).getConnection()->Name + " <= not " +
      port(2).getConnection()->Name + ";\n\n";
  return s;
}

// -------------------------------------------------------
QString JK_FlipFlop::verilogCode(int NumPorts)
{
  QString t = "";
  if(NumPorts <= 0) { // no truth table simulation ?
    QString td = prop(0).Value;        // delay time
    if(!misc::Verilog_Delay(td, Name)) return td; // time has not VHDL format
    if(!td.isEmpty()) t = "   " + td + ";\n";
  }

  QString l = "";

  QString s = port(5).getConnection()->Name;
  QString r = port(6).getConnection()->Name;
  QString j = port(0).getConnection()->Name;
  QString k = port(1).getConnection()->Name;
  QString q = port(2).getConnection()->Name;
  QString b = port(3).getConnection()->Name;
  QString c = port(4).getConnection()->Name;
  QString v = "net_reg" + Name + q;
  
  l = "\n  // " + Name + " JK-flipflop\n" +
    "  assign  " + q + " = " + v + ";\n" +
    "  assign  " + b + " = ~" + q + ";\n" +
    "  reg     " + v + " = 0;\n" +
    "  always @ (" + c + " or " + r + " or " + s + ") begin\n" + t +
    "    if (" + r + ") " + v + " <= 0;\n" +
    "    else if (" + s + ") " + v + " <= 1;\n" +
    "    else if (" + c + ")\n" + 
    "      " + v + " <= (" + j + " && ~" + q + ") || (~" +
    k + " && " + q + ");\n" +
    "  end\n\n";
  return l;
}

// -------------------------------------------------------
Component* JK_FlipFlop::newOne()
{
  return new JK_FlipFlop();
}

// -------------------------------------------------------
Element* JK_FlipFlop::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("JK-FlipFlop");
  BitmapFile = (char *) "jkflipflop";

  if(getNewOne)  return new JK_FlipFlop();
  return 0;
}
