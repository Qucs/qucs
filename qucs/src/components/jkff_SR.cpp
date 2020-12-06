/***************************************************************************
                              jkff_SR
                             ---------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * jkff_SR.cpp - device implementations for jkff_SR module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "jkff_SR.h"
#include "node.h"
#include "misc.h"

jkff_SR::jkff_SR()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("jk flip flop with set and reset verilog device");

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
  Model = "jkff_SR";
  Name  = "Y";
}

Component * jkff_SR::newOne()
{
  jkff_SR * p = new jkff_SR();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * jkff_SR::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("JK-FlipFlop w/ SR");
  BitmapFile = (char *) "jkff_SR";

  if(getNewOne) return new jkff_SR();
  return 0;
}

void jkff_SR::createSymbol()
{
  // put in here symbol drawing code and terminal definitions
  Lines.append(new Line(-30,-40, 30,-40,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,-40, 30, 40,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30, 40,-30, 40,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30, 40,-30,-40,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-50,-20,-30,-20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-50, 20,-30, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30, 20, 50, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,-20, 50,-20,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-50,  0,-30,  0,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-30,-10,-20,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30, 10,-20,  0,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(  0,-50,  0,-60,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, 50,  0, 60,QPen(Qt::darkBlue,2)));

  Arcs.append(new Arc(  -5, -50, 10, 10, 0, 16*360, QPen(Qt::darkBlue,2)));
  Arcs.append(new Arc(  -5,  40, 10, 10, 0, 16*360, QPen(Qt::darkBlue,2)));

  Texts.append(new Text(-25,-32, "J", Qt::darkBlue, 12.0));
  Texts.append(new Text(-25,  7, "K", Qt::darkBlue, 12.0));
  Texts.append(new Text( 11,-32, "Q", Qt::darkBlue, 12.0));
  Texts.append(new Text( -5,-39, "S", Qt::darkBlue, 12.0));
  Texts.append(new Text( 11,  7, "Q", Qt::darkBlue, 12.0));
  Texts.last()->over=true;
  Texts.append(new Text( -5, 17, "R", Qt::darkBlue, 12.0));
 
  Ports.append(new ComponentPort(  0,-60));  // S
  Ports.append(new ComponentPort(-50,-20));  // J
  Ports.append(new ComponentPort(-50, 0));   // CLK
  Ports.append(new ComponentPort(-50, 20));  // K
  Ports.append(new ComponentPort(  0, 60));  // R
  Ports.append(new ComponentPort( 50, 20));  // QB
  Ports.append(new ComponentPort( 50,-20));  // Q

  x1 = -50; y1 = -60;
  x2 =  50; y2 =  60;
}

QString jkff_SR::vhdlCode( int )
{
  QString s="";

  QString td = Props.at(2)->Value;     // delay time
  if(!misc::VHDL_Delay(td, Name)) return td; // time has not VHDL format
  td += ";\n";

  QString S     = Ports.at(0)->Connection->name();
  QString J     = Ports.at(1)->Connection->name();
  QString CLK   = Ports.at(2)->Connection->name();
  QString K     = Ports.at(3)->Connection->name();
  QString R     = Ports.at(4)->Connection->name();
  QString QB    = Ports.at(5)->Connection->name();
  QString Q     = Ports.at(6)->Connection->name();

  s = "\n  "+Name+":process ("+S+", "+CLK+", "+R+") is\n"+
      "  variable state : std_logic;\n"+
      "  begin\n" +
      "    if ("+S+" = '0') then state := '1';\n"+
      "    elsif ("+R+" = '0') then state := '0';\n"+
      "    elsif ("+CLK+" = '1' and "+CLK+"'event) then\n"+
      "      if ("+J+" = '1' and "+K+" = '1') then state := not state;\n"+
      "      elsif ("+J+" = '1' and "+K+" = '0') then state := '1';\n"+
      "      elsif ("+J+" = '0' and "+K+" = '1') then state := '0';\n"+
      "      end if;\n"+
      "    end if;\n"+ 
      "    "+Q+" <= state"+td+
      "    "+QB+" <= not state"+td+
      "  end process;\n" ;
  return s;
}

QString jkff_SR::verilogCode( int )
{
  QString td = Props.at(2)->Value;        // delay time
  if(!misc::Verilog_Delay(td, Name)) return td; // time does not have VHDL format

  QString l = "";

  QString S     = Ports.at(0)->Connection->name();
  QString J     = Ports.at(1)->Connection->name();
  QString CLK   = Ports.at(2)->Connection->name();
  QString K     = Ports.at(3)->Connection->name();
  QString R     = Ports.at(4)->Connection->name();
  QString QB    = Ports.at(5)->Connection->name();
  QString Q     = Ports.at(6)->Connection->name();

  QString QR   = "Q_reg"  + Name + Q;
  QString QBR  = "QB_reg"  + Name + QB;
  QString ST   = "Q_state" + Name;

  l = "\n  // "+Name+" jk flip-flop with set and reset\n"+
      "  assign  "+Q+" = "+QR+";\n"+
      "  reg     "+QR+" = 0;\n"+
      "  assign  "+QB+" = "+QBR+";\n"+
      "  reg     "+QBR+" = 1;\n"+
      "  reg     "+ST+" = 0;\n"+
      "  always @ (posedge "+CLK+")\n"+
      "  begin\n"+
      "    if ("+R+" == 1 && "+S+" == 1)\n"+
      "    begin\n"+
      "      if ("+J+" == 1 && "+K+" == 1) "+ST+" = ~"+ST+";\n"+
      "      if ("+J+" == 1 && "+K+" == 0) "+ST+" = 1;\n"+
      "      if ("+J+" == 0 && "+K+" == 1) "+ST+" = 0;\n"+
      "      "+QR+" <="+td+" "+ST+";\n"+
      "      "+QBR+" <="+td+" ~"+ST+";\n"+
      "    end\n"+
      "  end\n"+
      "  always @ ("+R+")\n"+
      "  begin\n"+
      "    if ("+R+" == 0) "+ST+" = 0;\n"+
      "    "+QR+" <="+td+" "+ST+";\n"+
      "    "+QBR+" <="+td+" ~"+ST+";\n"+
      "  end\n" +
      "  always @ ("+S+")\n"+
      "  begin\n"+
      "    if ("+S+" == 0) "+ST+" = 1;\n"+
      "    "+QR+" <="+td+" "+ST+";\n"+
      "    "+QBR+" <="+td+" ~"+ST+";\n"+
      "  end\n";
  return l;
}
