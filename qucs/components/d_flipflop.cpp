/***************************************************************************
                              d_flipflop.cpp
                             ----------------
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
#include "d_flipflop.h"
#include "node.h"
#include "misc.h"

D_FlipFlop::D_FlipFlop()
{
  Type = isDigitalComponent;
  Description = QObject::tr("D flip flop with asynchron reset");

  Props.push_back(qucs::Property("t", "0", false, QObject::tr("delay time")));

  Lines.push_back(qucs::Line(-20,-20, 20,-20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-20, 20, 20, 20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-20,-20,-20, 20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 20,-20, 20, 20,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-30,-10,-20,-10,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30, 10,-20, 10,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30,-10, 20,-10,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0, 20,  0, 30,QPen(Qt::darkBlue,2)));

  Texts.push_back(qucs::Text(-18,-21, "D", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(  6,-21, "Q", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text( -4,  4, "R", Qt::darkBlue, 9.0));
  Lines.push_back(qucs::Line(-20,  6,-12, 10,QPen(Qt::darkBlue,0)));
  Lines.push_back(qucs::Line(-20, 14,-12, 10,QPen(Qt::darkBlue,0)));

  Ports.push_back(qucs::Port(-30,-10));  // D
  Ports.push_back(qucs::Port(-30, 10));  // Clock
  Ports.push_back(qucs::Port( 30,-10));  // Q
  Ports.push_back(qucs::Port(  0, 30));  // Reset

  x1 = -30; y1 = -24;
  x2 =  30; y2 =  30;
  set_qucs_text_position(x1+4, y2+4);
  Model = "DFF";
  Name  = "Y";
}

// -------------------------------------------------------
QString D_FlipFlop::vhdlCode(int NumPorts)
{
  QString s = "";
  if(NumPorts <= 0) { // no truth table simulation ?
    QString td = prop(0).Value;     // delay time
    if(!misc::VHDL_Delay(td, Name)) return td; // time has not VHDL format
    s += td;
  }
  s += ";\n";

  s = "  " + Name + " : process (" +
      port(0).getConnection()->Name + ", " +
      port(1).getConnection()->Name + ")\n  begin\n    if (" +
      port(3).getConnection()->Name + "='1') then  " +
      port(2).getConnection()->Name + " <= '0'" + s +"    elsif (" +
      port(1).getConnection()->Name + "='1' and " +
      port(1).getConnection()->Name + "'event) then\n      " +
      port(2).getConnection()->Name + " <= " +
      port(0).getConnection()->Name + s + "    end if;\n  end process;\n\n";
  return s;
}

// -------------------------------------------------------
QString D_FlipFlop::verilogCode(int NumPorts)
{
  QString t = "";
  if(NumPorts <= 0) { // no truth table simulation ?
    QString td = prop(0).Value;        // delay time
    if(!misc::Verilog_Delay(td, Name)) return td; // time has not VHDL format
    if(!td.isEmpty()) t = "   " + td  + ";\n";
  }
  
  QString s = "";
  QString q = port(2).getConnection()->Name;
  QString d = port(0).getConnection()->Name;
  QString r = port(3).getConnection()->Name;
  QString c = port(1).getConnection()->Name;
  QString v = "net_reg" + Name + q;
  
  s = "\n  // " + Name + " D-flipflop\n" +
    "  assign  " + q + " = " + v + ";\n" +
    "  reg     " + v + " = 0;\n" +
    "  always @ (" + c + " or " + r + ") begin\n" + t +
    "    if (" + r + ") " + v + " <= 0;\n" +
    "    else if (~" + r + " && " + c + ") " + v + " <= " + d + ";\n" +
    "  end\n\n";
  return s;
}

// -------------------------------------------------------
Component* D_FlipFlop::newOne()
{
  return new D_FlipFlop();
}

// -------------------------------------------------------
Element* D_FlipFlop::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("D-FlipFlop");
  BitmapFile = (char *) "dflipflop";

  if(getNewOne)  return new D_FlipFlop();
  return 0;
}
