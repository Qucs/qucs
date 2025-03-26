/***************************************************************************
                             comp_2bit
                            -----------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * comp_2bit.cpp - device implementations for comp_2bit module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "comp_2bit.h"
#include "node.h"
#include "misc.h"

comp_2bit::comp_2bit()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("2bit comparator verilog device");

  Props.push_back(qucs::Property("TR", "6", false,
    QObject::tr ("transfer function high scaling factor")));
  Props.push_back(qucs::Property("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));
 
  createSymbol ();
  set_qucs_text_position(x1+19, y2+4);
  Model = "comp_2bit";
  Name  = "Y";
}

Component * comp_2bit::newOne()
{
  comp_2bit * p = new comp_2bit();
  p->Props.front().Value = Props.front().Value; 
  p->recreate(0); 
  return p;
}

Element * comp_2bit::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("2Bit Comparator");
  BitmapFile = (char *) "comp_2bit";

  if(getNewOne) return new comp_2bit();
  return 0;
}

void comp_2bit::createSymbol()
{
  Lines.push_back(qucs::Line(-40, -50, 40,-50,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 40, -50, 40, 60,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 40,  60,-40, 60,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-40,  60,-40, -50,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-60,-10,-40,-10,QPen(Qt::darkBlue,2)));  // X0
  Lines.push_back(qucs::Line(-60, 10,-40, 10,QPen(Qt::darkBlue,2)));  // X1
  Lines.push_back(qucs::Line(-60, 30,-40, 30,QPen(Qt::darkBlue,2)));  // Y0
  Lines.push_back(qucs::Line(-60, 50,-40, 50,QPen(Qt::darkBlue,2)));  // Y1
  Lines.push_back(qucs::Line( 40, 30, 60, 30,QPen(Qt::darkBlue,2)));  // L
  Lines.push_back(qucs::Line( 40, 10, 60, 10,QPen(Qt::darkBlue,2)));  // G
  Lines.push_back(qucs::Line( 40,-10, 60,-10,QPen(Qt::darkBlue,2)));  // E

  Texts.push_back(qucs::Text(-25,-45, "COMP", Qt::darkBlue, 12.0));

  Texts.push_back(qucs::Text(-25,-20,   "{",  Qt::darkBlue, 16.0));
  Texts.push_back(qucs::Text(-15,-15,   "X",  Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-35,-23,   "0",  Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-35, -3,   "1",  Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-25, 22,   "{",  Qt::darkBlue, 16.0));
  Texts.push_back(qucs::Text(-15, 27,   "Y",  Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-35, 17,   "0",  Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-35, 37,   "1",  Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(  5, 17, "X<Y",  Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(  5, -3, "X>Y", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(  5,-23, "X=Y", Qt::darkBlue, 12.0));

  Ports.push_back(qucs::Port(-60,-10));  // X0
  Ports.push_back(qucs::Port(-60, 10));  // X1
  Ports.push_back(qucs::Port(-60, 30));  // Y0
  Ports.push_back(qucs::Port(-60, 50));  // Y1
  Ports.push_back(qucs::Port( 60, 30));  // L
  Ports.push_back(qucs::Port( 60, 10));  // G
  Ports.push_back(qucs::Port( 60,-10));  // E

  x1 = -60; y1 = -54;
  x2 =  60; y2 =  64;
}

QString comp_2bit::vhdlCode( int )
{
  QString s="";

  QString td = prop(1).Value;     // delay time
  if(!misc::VHDL_Delay(td, Name)) return td; // time has not VHDL format
  td += ";\n";

  QString X0   = port(0).getConnection()->Name;
  QString X1   = port(1).getConnection()->Name;
  QString Y0   = port(2).getConnection()->Name;
  QString Y1   = port(3).getConnection()->Name;
  QString L    = port(4).getConnection()->Name;
  QString G    = port(5).getConnection()->Name;
  QString E    = port(6).getConnection()->Name;
 
  s = "\n  "+Name+":process ("+X0+", "+X1+", "+Y0+", "+Y1+")\n"+
      "  begin\n"+
      "    "+L+" <= ((not "+X1+") and "+Y1+") or (not ("+X1+" xor "+Y1+") and (not ("+X0+") and "+Y0+"))"+td+
      "    "+G+" <= ("+X1+" and (not "+Y1+"))"+" or (not ("+X1+" xor "+Y1+") and ("+X0+" and (not "+Y0+")))"+td+
      "    "+E+" <= (not ("+X1+" xor "+Y1+")) and (not ("+X0+" xor "+Y0+"))"+td+  
      "  end process;\n";
  return s;
}

QString comp_2bit::verilogCode( int )
{
  QString l="";

  QString td = prop(1).Value;        // delay time
  if(!misc::Verilog_Delay(td, Name)) return td; // time does not have VHDL format

  QString X0    = port(0).getConnection()->Name;
  QString X1    = port(1).getConnection()->Name;
  QString Y0    = port(2).getConnection()->Name;
  QString Y1    = port(3).getConnection()->Name;
  QString L     = port(4).getConnection()->Name;
  QString G     = port(5).getConnection()->Name;
  QString E     = port(6).getConnection()->Name;

  QString LR  = "L_reg"  + Name + L;
  QString GR  = "G_reg"  + Name + G;
  QString ER  = "E_reg"  + Name + E;

  l = "\n  // "+Name+" 2bit comparator\n"+
      "  assign  "+L+" = "+LR+";\n"+
      "  reg     "+LR+" = 0;\n"+
      "  assign  "+G+" = "+GR+";\n"+
      "  reg     "+GR+" = 0;\n"+
      "  assign  "+E+" = "+ER+";\n"+
      "  reg     "+ER+" = 0;\n"+
      "  always @ ("+X0+" or "+X1+" or "+Y0+" or "+Y1+")\n"+
      "  begin\n"+
      "    "+LR+" <="+td+" ((~"+X1+") && "+Y1+") || (~("+X1+" ^ "+Y1+") && (~("+X0+") && "+Y0+"));\n"+
      "    "+GR+" <="+td+" ("+X1+" && (~"+Y1+"))"+" || (~("+X1+" ^ "+Y1+") && ("+X0+" && (~"+Y0+")));\n"+
      "    "+ER+" <="+td+" (~("+X1+" ^ "+Y1+")) && (~("+X0+" ^ "+Y0+"));\n"+  
      "  end\n";
  
  return l;
}
