/***************************************************************************
                             gatedDlatch
                            -------------
    begin                : January 2009
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * gatedDlatch.cpp - device implementations for gatedDlatch module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "gatedDlatch.h"
#include "node.h"
#include "misc.h"

gatedDlatch::gatedDlatch()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("gated D latch verilog device");

  Props.push_back(qucs::Property("TR_H", "6", false,
    QObject::tr ("cross coupled gate transfer function high scaling factor")));
  Props.push_back(qucs::Property("TR_L", "5", false,
    QObject::tr ("cross coupled gate transfer function low scaling factor")));
  Props.push_back(qucs::Property("Delay", "1 ns", false,
    QObject::tr ("cross coupled gate delay")
    +" ("+QObject::tr ("s")+")"));

  createSymbol ();
  set_qucs_text_position(x1() + 19, y2() + 4);
  Model = "gatedDlatch";
  Name  = "Y";
}

Component * gatedDlatch::newOne()
{
  gatedDlatch * p = new gatedDlatch();
  p->Props.front().Value = Props.front().Value; 
  p->recreate(0); 
  return p;
}

Element * gatedDlatch::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Gated D-Latch");
  BitmapFile = (char *) "gatedDlatch";

  if(getNewOne) return new gatedDlatch();
  return 0;
}

void gatedDlatch::createSymbol()
{
  Lines.push_back(qucs::Line(-30, -40, 30,-40,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30, -40, 30, 40,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30,  40,-30, 40,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30, 40,-30, -40,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-50,-20,-30,-20,QPen(Qt::darkBlue,2))); // D
  Lines.push_back(qucs::Line(-50, 20,-30, 20,QPen(Qt::darkBlue,2))); // C
  Lines.push_back(qucs::Line( 40, 20, 50, 20,QPen(Qt::darkBlue,2))); // QB
  Lines.push_back(qucs::Line( 30,-20, 50,-20,QPen(Qt::darkBlue,2))); // Q

  Arcs.push_back(qucs::Arc( 30, 15, 10, 10, 0, 16*360, QPen(Qt::darkBlue,2)));

  Texts.push_back(qucs::Text(-25,-32, "D", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-25,  7, "C", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text( 11,-32, "Q", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text( 11,  7, "Q", Qt::darkBlue, 12.0));
  Texts.back().over=true;

  Ports.push_back(qucs::Port(-50,-20));  // D
  Ports.push_back(qucs::Port(-50, 20));  // C
  Ports.push_back(qucs::Port( 50, 20));  // QB
  Ports.push_back(qucs::Port( 50,-20));  // Q

  set_x1(-50);
  set_y1(-44);
  set_x2(50);
  set_y2(44);
}

QString gatedDlatch::vhdlCode( int )
{
  QString s="";

  QString td = prop(2).Value;     // delay time
  if(!misc::VHDL_Delay(td, Name)) return td; // time has not VHDL format
  td += ";\n";

  QString D    = port(0).getConnection()->Name;
  QString C    = port(1).getConnection()->Name;
  QString QB   = port(2).getConnection()->Name;
  QString Q    = port(3).getConnection()->Name;

  s = "\n  "+Name+":process ("+D+", "+C+")\n"+
      "  begin\n" +
      "    if ("+C+" = '1') then\n"+
      "      "+Q+" <= "+D+td+
      "      "+QB+" <= not "+D+td+
      "    end if;\n"+
      "  end process;\n";
  return s;
}

QString gatedDlatch::verilogCode( int )
{
  QString td = prop(2).Value;        // delay time
  if(!misc::Verilog_Delay(td, Name)) return td; // time does not have VHDL format
  
  QString l = "";

  QString D    = port(0).getConnection()->Name;
  QString C    = port(1).getConnection()->Name;
  QString QB   = port(2).getConnection()->Name;
  QString Q    = port(3).getConnection()->Name;

  QString QR   = "Q_reg"  + Name + Q;
  QString QBR  = "QB_reg"  + Name + QB;

  l = "\n  // "+Name+" gated d latch\n"+
      "  assign  "+Q+" = "+QR+";\n"+
      "  reg     "+QR+" = 0;\n"+
      "  assign  "+QB+" = "+QBR+";\n"+
      "  reg     "+QBR+" = 0;\n"+
      "  always @ ("+D+" or "+C+")\n"+
      "  begin\n"+
      "    if ("+C+" == 1)\n"+
      "    begin\n"+
      "      " +QR+" <="+td+" "+D+";\n"+
      "      " +QBR+" <="+td+" ~"+D+";\n" +
      "    end\n"+
      "  end\n";

  return l;
}
