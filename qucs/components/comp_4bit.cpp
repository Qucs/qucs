/***************************************************************************
                              comp_4bit
                             -----------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * comp_4bit.cpp - device implementations for comp_4bit module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "comp_4bit.h"
#include "node.h"
#include "misc.h"

comp_4bit::comp_4bit()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("4bit comparator verilog device");

  Props.push_back(qucs::Property("TR", "6", false,
    QObject::tr ("transfer function high scaling factor")));
  Props.push_back(qucs::Property("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));

  createSymbol ();
  set_qucs_text_position(x1()+19, y2()+4);
  Model = "comp_4bit";
  Name  = "Y";
}

Component * comp_4bit::newOne()
{
  comp_4bit * p = new comp_4bit();
  p->Props.front().Value = Props.front().Value; 
  p->recreate(0); 
  return p;
}

Element * comp_4bit::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("4Bit Comparator");
  BitmapFile = (char *) "comp_4bit";

  if(getNewOne) return new comp_4bit();
  return 0;
}

void comp_4bit::createSymbol()
{
  Lines.push_back(qucs::Line(-40, -90, 40,-90,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 40, -90, 40,100,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 40, 100,-40,100,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-40, 100,-40,-90,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-60,-50,-40,-50,QPen(Qt::darkBlue,2)));  // X0
  Lines.push_back(qucs::Line(-60,-30,-40,-30,QPen(Qt::darkBlue,2)));  // X1
  Lines.push_back(qucs::Line(-60,-10,-40,-10,QPen(Qt::darkBlue,2)));  // X2
  Lines.push_back(qucs::Line(-60, 10,-40, 10,QPen(Qt::darkBlue,2)));  // X3
  Lines.push_back(qucs::Line(-60, 30,-40, 30,QPen(Qt::darkBlue,2)));  // Y0
  Lines.push_back(qucs::Line(-60, 50,-40, 50,QPen(Qt::darkBlue,2)));  // Y1
  Lines.push_back(qucs::Line(-60, 70,-40, 70,QPen(Qt::darkBlue,2)));  // Y2
  Lines.push_back(qucs::Line(-60, 90,-40, 90,QPen(Qt::darkBlue,2)));  // Y3
  Lines.push_back(qucs::Line( 40, 30, 60, 30,QPen(Qt::darkBlue,2)));  // L
  Lines.push_back(qucs::Line( 40, 10, 60, 10,QPen(Qt::darkBlue,2)));  // G
  Lines.push_back(qucs::Line( 40,-10, 60,-10,QPen(Qt::darkBlue,2)));  // E

  Texts.push_back(qucs::Text(-25,-85, "COMP", Qt::darkBlue, 12.0));

  Texts.push_back(qucs::Text(-25,-40,   "{",  Qt::darkBlue, 16.0));
  Texts.push_back(qucs::Text(-15,-35,   "X",  Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-35,-63,   "0",  Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-35,-43,   "1",  Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-35,-23,   "2",  Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-35, -3,   "3",  Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-25, 42,   "{",  Qt::darkBlue, 16.0));
  Texts.push_back(qucs::Text(-15, 47,   "Y",  Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-35, 17,   "0",  Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-35, 37,   "1",  Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-35, 57,   "2",  Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-35, 77,   "3",  Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(  5, 17, "X<Y",  Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(  5, -3, "X>Y",  Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(  5,-23, "X=Y",  Qt::darkBlue, 12.0));

  Ports.push_back(qucs::Port(-60,-50));  // X0
  Ports.push_back(qucs::Port(-60,-30));  // X1
  Ports.push_back(qucs::Port(-60,-10));  // X2
  Ports.push_back(qucs::Port(-60, 10));  // X3
  Ports.push_back(qucs::Port(-60, 30));  // Y0
  Ports.push_back(qucs::Port(-60, 50));  // Y1
  Ports.push_back(qucs::Port(-60, 70));  // Y2
  Ports.push_back(qucs::Port(-60, 90));  // Y3
  Ports.push_back(qucs::Port( 60, 30));  // L
  Ports.push_back(qucs::Port( 60, 10));  // G
  Ports.push_back(qucs::Port( 60,-10));  // E

  set_x1(-60);
  set_y1(-94);
  set_x2(60);
  set_y2(104);
}

QString comp_4bit::vhdlCode( int )
{
  QString s="";

  QString td = prop(1).Value;     // delay time
  if(!misc::VHDL_Delay(td, Name)) return td; // time has not VHDL format
  td += ";\n";

  QString X0    = port(0).getConnection()->Name;
  QString X1    = port(1).getConnection()->Name;
  QString X2    = port(2).getConnection()->Name;
  QString X3    = port(3).getConnection()->Name;
  QString Y0    = port(4).getConnection()->Name;
  QString Y1    = port(5).getConnection()->Name;
  QString Y2    = port(6).getConnection()->Name;
  QString Y3    = port(7).getConnection()->Name;
  QString L     = port(8).getConnection()->Name;
  QString G     = port(9).getConnection()->Name;
  QString E     = port(10).getConnection()->Name;
 
  s = "\n  "+Name+":process ("+X0+", "+X1+", "+X2+", "+X3+", "+Y0+", "+Y1+", "+Y2+", "+Y3+")\n"+
      "  variable P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11 : std_logic;\n"+
      "  begin\n"+
      "    P0  := not ("+X0+" xor "+Y0+");\n"+
      "    P1  := not ("+X1+" xor "+Y1+");\n"+
      "    P2  := not ("+X2+" xor "+Y2+");\n"+
      "    P3  := not ("+X3+" xor "+Y3+");\n"+
      "    P4  := (not "+X0+") and "+Y0+";\n"+
      "    P5  := (not "+X1+") and "+Y1+";\n"+
      "    P6  := (not "+X2+") and "+Y2+";\n"+
      "    P7  := (not "+X3+") and "+Y3+";\n"+
      "    P8  := "+X0+" and (not "+Y0+");\n"+
      "    P9  := "+X1+" and (not "+Y1+");\n"+
      "    P10 := "+X2+" and (not "+Y2+");\n"+
      "    P11 := "+X3+" and (not "+Y3+");\n"+
      "    "+L+" <= P7 or (P3 and P6) or (P3 and P2 and P5) or (P3 and P2 and P1 and P4)"+td+
      "    "+G+" <= P11 or (P3 and P10) or (P3 and P2 and P9) or (P3 and P2 and P1 and P8)"+td+
      "    "+E+" <= P3 and P2 and P1 and P0"+td+  
      "  end process;\n";
  return s;
}

QString comp_4bit::verilogCode( int )
{
  QString l="";

  QString td = prop(1).Value;        // delay time
  if(!misc::Verilog_Delay(td, Name)) return td; // time does not have VHDL format

  QString X0    = port(0).getConnection()->Name;
  QString X1    = port(1).getConnection()->Name;
  QString X2    = port(2).getConnection()->Name;
  QString X3    = port(3).getConnection()->Name;
  QString Y0    = port(4).getConnection()->Name;
  QString Y1    = port(5).getConnection()->Name;
  QString Y2    = port(6).getConnection()->Name;
  QString Y3    = port(7).getConnection()->Name;
  QString L     = port(8).getConnection()->Name;
  QString G     = port(9).getConnection()->Name;
  QString E     = port(10).getConnection()->Name;

  QString LR  = "L_reg"  + Name + L;
  QString GR  = "G_reg"  + Name + G;
  QString ER  = "E_reg"  + Name + E;

  l = "\n  // "+Name+" 4bit comparator\n"+
      "  assign  "+L+" = "+LR+";\n"+
      "  reg     "+LR+" = 0;\n"+
      "  assign  "+G+" = "+GR+";\n"+
      "  reg     "+GR+" = 0;\n"+
      "  assign  "+E+" = "+ER+";\n"+
      "  reg     "+ER+" = 0;\n"+
      "  reg     P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11;\n"+
      "  always @ ("+X0+" or "+X1+" or "+X2+" or "+X3+" or "+Y0+" or "+Y1+" or "+Y2+" or "+Y3+")\n"+
      "  begin\n"+
      "    P0  = ~("+X0+" ^ "+Y0+");\n"+
      "    P1  = ~("+X1+" ^ "+Y1+");\n"+
      "    P2  = ~("+X2+" ^ "+Y2+");\n"+
      "    P3  = ~("+X3+" ^ "+Y3+");\n"+
      "    P4  = (~"+X0+") && "+Y0+";\n"+
      "    P5  = (~"+X1+") && "+Y1+";\n"+
      "    P6  = (~"+X2+") && "+Y2+";\n"+
      "    P7  = (~"+X3+") && "+Y3+";\n"+
      "    P8  = "+X0+" && (~"+Y0+");\n"+
      "    P9  = "+X1+" && (~"+Y1+");\n"+
      "    P10 = "+X2+" && (~"+Y2+");\n"+
      "    P11 = "+X3+" && (~"+Y3+");\n"+
      "    "+LR+" <="+td+" P7 || (P3 && P6) || (P3 && P2 && P5) || (P3 && P2 && P1 && P4)"+";\n"+
      "    "+GR+" <="+td+" P11 || (P3 && P10) || (P3 && P2 && P9) || (P3 && P2 && P1 && P8)"+";\n"+
      "    "+ER+" <="+td+" P3 && P2 && P1 && P0"+";\n"+
      "  end\n";

  return l;
}
