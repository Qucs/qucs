/***************************************************************************
                              tff_SR
                             --------
    begin                : January 2009
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * tff_SR.cpp - device implementations for tff_SR module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include <stdlib.h>

#include "tff_SR.h"
#include "node.h"
#include "main.h"

tff_SR::tff_SR()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("T flip flop with set and reset verilog device");

  Props.append (new Property ("TR_H", "6", false,
    QObject::tr ("cross coupled gate transfer function high scaling factor")));
  Props.append (new Property ("TR_L", "5", false,
    QObject::tr ("cross coupled gate transfer function low scaling factor")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("cross coupled gate delay")
    +" ("+QObject::tr ("s")+")"));
 
  createSymbol ();
  tx = x1 + 4;
  ty = y2 + 4;
  Model = "tff_SR";
  Name  = "Y"; 
}

Component * tff_SR::newOne()
{
  tff_SR * p = new tff_SR();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * tff_SR::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("T-FlipFlop w/ SR");
  BitmapFile = (char *) "tff_SR";

  if(getNewOne) return new tff_SR();
  return 0;
}

void tff_SR::createSymbol()
{
  // put in here symbol drawing code and terminal definitions
  Lines.append(new Line(-30, -40, 30,-40,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30, -40, 30, 40,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,  40,-30, 40,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  40,-30,-40,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-45,-25,-30,-25,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-45, 25,-30, 25,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30, 25, 45, 25,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,-25, 45,-25,QPen(QPen::darkBlue,2)));

  Lines.append(new Line( -30, 15,-20, 25,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -30, 35,-20, 25,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(  0, -50,  0,-60,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,  50,  0, 60,QPen(QPen::darkBlue,2)));

  Arcs.append(new Arc( -5, -50, 10, 10, 0, 16*360, QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( -5,  40, 10, 10, 0, 16*360, QPen(QPen::darkBlue,2)));

  Texts.append(new Text(-25,-37, "T", QPen::darkBlue, 12.0));
  Texts.append(new Text( 13,-37, "Q", QPen::darkBlue, 12.0));
  Texts.append(new Text( -5,-39, "S", QPen::darkBlue, 12.0));
  Texts.append(new Text( 13, 12, "Q", QPen::darkBlue, 12.0));
  Texts.current()->over=true;
  Texts.append(new Text( -5, 17, "R", QPen::darkBlue, 12.0));
 
  Ports.append(new Port(  0,-60));  // S
  Ports.append(new Port(-45,-25));  // T
  Ports.append(new Port(-45, 25));  // CLK
  Ports.append(new Port(  0, 60));  // R
  Ports.append(new Port( 45, 25));  // QB
  Ports.append(new Port( 45,-25));  // Q

  x1 = -45; y1 = -60;
  x2 =  45; y2 =  60;
}

QString tff_SR::vhdlCode( int )
{
  QString s="";
  QString td=";\n";

  if(strtod(Props.at(2)->Value.latin1(), 0) != 0.0) { // delay time
    td = Props.at(2)->Value;
    if(!VHDL_Time(td, Name))
      return td;    // Time does not have VHDL format.
    td = " after " + td + ";\n";
  }

  QString S     = Ports.at(0)->Connection->Name;
  QString T     = Ports.at(1)->Connection->Name;
  QString CLK   = Ports.at(2)->Connection->Name;
  QString R     = Ports.at(3)->Connection->Name;
  QString QB    = Ports.at(4)->Connection->Name;
  QString Q     = Ports.at(5)->Connection->Name;

  s = "\n  "+Name+" : process ("+S+", "+CLK+", "+R+") is\n"+
      "  variable state : std_logic;\n"+
      "  begin\n" +
      "    if ("+S+" = '0') then\n"+
      "      state := '1';\n"+
      "    elsif ("+R+" = '0') then\n"+
      "      state := '0';\n"+
      "    elsif ("+CLK+" = '1' and "+CLK+"'event) then\n"+
      "      if ("+T+" = '1') then state := not state;\n"+
      "      end if;\n"+
      "    end if;\n"+ 
      "    "+Q+" <= state"+td+
      "    "+QB+" <= not state"+td+
      "  end process;\n";
  return s;
}

QString tff_SR::verilogCode( int )
{
  QString td = "";

  if(strtod(Props.at(2)->Value.latin1(), 0) != 0.0) { // delay time
    td = Props.at(2)->Value;
    if(!Verilog_Time(td, Name))
      return td;    // Time does not have VHDL format.
    td = " #" + td ;
  }
  
  QString l = "";
 
  QString S     = Ports.at(0)->Connection->Name;
  QString T     = Ports.at(1)->Connection->Name;
  QString CLK   = Ports.at(2)->Connection->Name;
  QString R     = Ports.at(3)->Connection->Name;
  QString QB    = Ports.at(4)->Connection->Name;
  QString Q     = Ports.at(5)->Connection->Name;

  QString QR   = "Q_reg"  + Name + Q;
  QString QBR  = "QB_reg"  + Name + QB;
  QString ST   = "Q_state" + Name;

  l = "\n  // "+Name+" t flip flop with set and reset\n"+
      "  assign  "+Q+" = "+QR+";\n"+
      "  reg     "+QR+" = 0;\n"+
      "  assign  "+QB+" = "+QBR+";\n"+
      "  reg     "+QBR+" = 1;\n"+
      "  reg     "+ST+" = 0;\n"+
      "  always @ (posedge "+CLK+")\n"+
      "  begin\n"+
      "    if ("+T+" == 1 && "+R+" == 1 && "+S+" == 1)\n"+
      "    begin\n"+
      "      "+ST+" = ~"+ST+";\n"+
      "      "+QR+" <="+td+" "+ST+";\n"+
      "      "+QBR+" <="+td+" ~"+ST+";\n"+
      "    end\n"+
      "  end\n"+
      "  always @ ("+R+")\n"+
      "  begin\n"+
      "    if ("+R+" == 0) "+ST+" = 0;\n"+
      "    "+QR+" <="+td+" "+ST+";\n"+
      "    "+QBR+" <="+td+" ~"+ST+";\n"+
      "  end\n"+
      "  always @ ("+S+")\n"+
      "  begin if ("+S+" == 0) "+ST+" = 1;\n"+
      "    "+QR+" <="+td+" "+ST+";\n"+
      "    "+QBR+" <="+td+" ~"+ST+";\n"+
      "  end\n";
  return l;
}
