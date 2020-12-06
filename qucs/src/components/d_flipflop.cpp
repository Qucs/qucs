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

  Props.append(new Property("t", "0", false, QObject::tr("delay time")));

  Lines.append(new Line(-20,-20, 20,-20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20, 20, 20, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20,-20,-20, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 20,-20, 20, 20,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-30,-10,-20,-10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30, 10,-20, 10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,-10, 20,-10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, 20,  0, 30,QPen(Qt::darkBlue,2)));

  Texts.append(new Text(-18,-21, "D", Qt::darkBlue, 12.0));
  Texts.append(new Text(  6,-21, "Q", Qt::darkBlue, 12.0));
  Texts.append(new Text( -4,  4, "R", Qt::darkBlue, 9.0));
  Lines.append(new Line(-20,  6,-12, 10,QPen(Qt::darkBlue,0)));
  Lines.append(new Line(-20, 14,-12, 10,QPen(Qt::darkBlue,0)));

  Ports.append(new ComponentPort(-30,-10));  // D
  Ports.append(new ComponentPort(-30, 10));  // Clock
  Ports.append(new ComponentPort( 30,-10));  // Q
  Ports.append(new ComponentPort(  0, 30));  // Reset

  x1 = -30; y1 = -24;
  x2 =  30; y2 =  30;
  tx = x1+4;
  ty = y2+4;
  Model = "DFF";
  Name  = "Y";
}

// -------------------------------------------------------
QString D_FlipFlop::vhdlCode(int)
{
#if 0
  QString s = "";
  if(NumPorts <= 0) { // no truth table simulation ?
    QString td = Props.at(0)->Value;     // delay time
    if(!misc::VHDL_Delay(td, Name)) return td; // time has not VHDL format
    s += td;
  }
  s += ";\n";

  s = "  " + Name + " : process (" +
      Ports.at(0)->Connection->name() + ", " +
      Ports.at(1)->Connection->name() + ")\n  begin\n    if (" +
      Ports.at(3)->Connection->name() + "='1') then  " +
      Ports.at(2)->Connection->name() + " <= '0'" + s +"    elsif (" +
      Ports.at(1)->Connection->name() + "='1' and " +
      Ports.at(1)->Connection->name() + "'event) then\n      " +
      Ports.at(2)->Connection->name() + " <= " +
      Ports.at(0)->Connection->name() + s + "    end if;\n  end process;\n\n";
  return s;
#endif
  return "";
}

// -------------------------------------------------------
QString D_FlipFlop::verilogCode(int)
{
#if 0
  QString t = "";
  if(NumPorts <= 0) { // no truth table simulation ?
    QString td = Props.at(0)->Value;        // delay time
    if(!misc::Verilog_Delay(td, name())) return td; // time has not VHDL format
    if(!td.isEmpty()) t = "   " + td  + ";\n";
  }
  
  QString s = "";
  QString q = Ports.at(2)->Connection->name();
  QString d = Ports.at(0)->Connection->name();
  QString r = Ports.at(3)->Connection->name();
  QString c = Ports.at(1)->Connection->name();
  QString v = "net_reg" + name() + q;
  
  s = "\n  // " + name() + " D-flipflop\n" +
    "  assign  " + q + " = " + v + ";\n" +
    "  reg     " + v + " = 0;\n" +
    "  always @ (" + c + " or " + r + ") begin\n" + t +
    "    if (" + r + ") " + v + " <= 0;\n" +
    "    else if (~" + r + " && " + c + ") " + v + " <= " + d + ";\n" +
    "  end\n\n";
  return s;
#endif
  return "";
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
