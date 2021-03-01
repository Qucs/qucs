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
  set_label("JK flip flop with asynchron set and reset");

  Props.append(new Property("t", "0", false, QObject::tr("delay time")));

  Lines.append(new Line(-20,-30, 20,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20, 30, 20, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20,-30,-20, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 20,-30, 20, 30,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-30,-20,-20,-20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30, 20,-20, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,-20, 20,-20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30, 20, 20, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  0,-20,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0,-30,  0,-40,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, 30,  0, 40,QPen(Qt::darkBlue,2)));

  Texts.append(new Text( -4,-29, "S", Qt::darkBlue,  9.0));
  Texts.append(new Text( -4, 14, "R", Qt::darkBlue,  9.0));
  Texts.append(new Text(-18,-31, "J", Qt::darkBlue, 12.0));
  Texts.append(new Text(-18,  8, "K", Qt::darkBlue, 12.0));
  Texts.append(new Text(  6,-31, "Q", Qt::darkBlue, 12.0));
  Texts.append(new Text(  6,  8, "Q", Qt::darkBlue, 12.0));
  Texts.last()->over=true;
  Lines.append(new Line(-20, -4,-12,  0,QPen(Qt::darkBlue,0)));
  Lines.append(new Line(-20,  4,-12,  0,QPen(Qt::darkBlue,0)));

  Ports.append(new ComponentPort(-30,-20));  // J
  Ports.append(new ComponentPort(-30, 20));  // K
  Ports.append(new ComponentPort( 30,-20));  // Q
  Ports.append(new ComponentPort( 30, 20));  // nQ
  Ports.append(new ComponentPort(-30,  0));  // Clock
  Ports.append(new ComponentPort(  0,-40));  // set
  Ports.append(new ComponentPort(  0, 40));  // reset

  x1 = -30; y1 = -40;
  x2 =  30; y2 =  40;
  tx = x1+4;
  ty = y2+4;
  Model = "JKFF";
  Name  = "Y";
}

// -------------------------------------------------------
QString JK_FlipFlop::vhdlCode(int)
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
      Ports.at(5)->Connection->name() + ", " +
      Ports.at(6)->Connection->name() + ", " +
      Ports.at(4)->Connection->name() + ")\n  begin\n    if (" +
      Ports.at(6)->Connection->name() + "='1') then  " +
      Ports.at(2)->Connection->name() + " <= '0'" + s +"    elsif (" +
      Ports.at(5)->Connection->name() + "='1') then  " +
      Ports.at(2)->Connection->name() + " <= '1'" + s +"    elsif (" +
      Ports.at(4)->Connection->name() + "='1' and " +
      Ports.at(4)->Connection->name() + "'event) then\n      " +
      Ports.at(2)->Connection->name() + " <= (" +
      Ports.at(0)->Connection->name() + " and not " +
      Ports.at(2)->Connection->name() + ") or (not " +
      Ports.at(1)->Connection->name() + " and " +
      Ports.at(2)->Connection->name() + ")" + s +
      "    end if;\n  end process;\n  " +
      Ports.at(3)->Connection->name() + " <= not " +
      Ports.at(2)->Connection->name() + ";\n\n";
  return s;
#endif
  return QString("incomplet");
}

// -------------------------------------------------------
QString JK_FlipFlop::verilogCode(int)
{
#if 0
  QString t = "";
  if(NumPorts <= 0) { // no truth table simulation ?
    QString td = Props.at(0)->Value;        // delay time
    if(!misc::Verilog_Delay(td, name())) return td; // time has not VHDL format
    if(!td.isEmpty()) t = "   " + td + ";\n";
  }

  QString l = "";

  QString s = Ports.at(5)->Connection->name();
  QString r = Ports.at(6)->Connection->name();
  QString j = Ports.at(0)->Connection->name();
  QString k = Ports.at(1)->Connection->name();
  QString q = Ports.at(2)->Connection->name();
  QString b = Ports.at(3)->Connection->name();
  QString c = Ports.at(4)->Connection->name();
  QString v = "net_reg" + name() + q;
  
  l = "\n  // " + name() + " JK-flipflop\n" +
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
#endif
  return "";
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
