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
#include "tff_SR.h"
#include "node.h"
#include "misc.h"

tff_SR::tff_SR()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("T flip flop with set and reset verilog device");

  Props.push_back(qucs::Property("TR_H", "6", false,
    QObject::tr ("cross coupled gate transfer function high scaling factor")));
  Props.push_back(qucs::Property("TR_L", "5", false,
    QObject::tr ("cross coupled gate transfer function low scaling factor")));
  Props.push_back(qucs::Property("Delay", "1 ns", false,
    QObject::tr ("cross coupled gate delay")
    +" ("+QObject::tr ("s")+")"));
 
  createSymbol ();
  set_qucs_text_position(x1+4, y2+4);
  Model = "tff_SR";
  Name  = "Y"; 
}

Component * tff_SR::newOne()
{
  tff_SR * p = new tff_SR();
  p->Props.front().Value = Props.front().Value; 
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
  Lines.push_back(qucs::Line(-30, -40, 30,-40,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30, -40, 30, 40,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30,  40,-30, 40,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30,  40,-30,-40,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-50,-20,-30,-20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-50, 20,-30, 20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30, 20, 50, 20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30,-20, 50,-20,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line( -30, 10,-20, 20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -30, 30,-20, 20,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(  0, -50,  0,-60,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0,  50,  0, 60,QPen(Qt::darkBlue,2)));

  Arcs.push_back(qucs::Arc( -5, -50, 10, 10, 0, 16*360, QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc( -5,  40, 10, 10, 0, 16*360, QPen(Qt::darkBlue,2)));

  Texts.push_back(qucs::Text(-25,-32, "T", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text( 11,-32, "Q", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text( -5,-39, "S", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text( 11,  7, "Q", Qt::darkBlue, 12.0));
  Texts.back().over=true;
  Texts.push_back(qucs::Text( -5, 17, "R", Qt::darkBlue, 12.0));
 
  Ports.push_back(qucs::Port(  0,-60));  // S
  Ports.push_back(qucs::Port(-50,-20));  // T
  Ports.push_back(qucs::Port(-50, 20));  // CLK
  Ports.push_back(qucs::Port(  0, 60));  // R
  Ports.push_back(qucs::Port( 50, 20));  // QB
  Ports.push_back(qucs::Port( 50,-20));  // Q

  x1 = -50; y1 = -60;
  x2 =  50; y2 =  60;
}

QString tff_SR::vhdlCode( int )
{
  QString s="";

  QString td = prop(2).Value;     // delay time
  if(!misc::VHDL_Delay(td, Name)) return td; // time has not VHDL format
  td += ";\n";

  QString S     = port(0).getConnection()->Name;
  QString T     = port(1).getConnection()->Name;
  QString CLK   = port(2).getConnection()->Name;
  QString R     = port(3).getConnection()->Name;
  QString QB    = port(4).getConnection()->Name;
  QString Q     = port(5).getConnection()->Name;

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
  QString td = prop(2).Value;        // delay time
  if(!misc::Verilog_Delay(td, Name)) return td; // time does not have VHDL format
  
  QString l = "";
 
  QString S     = port(0).getConnection()->Name;
  QString T     = port(1).getConnection()->Name;
  QString CLK   = port(2).getConnection()->Name;
  QString R     = port(3).getConnection()->Name;
  QString QB    = port(4).getConnection()->Name;
  QString Q     = port(5).getConnection()->Name;

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
