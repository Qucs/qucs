/***************************************************************************
                              dmux2to4
                             ----------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * dmux2to4.cpp - device implementations for dmux2to4 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "dmux2to4.h"
#include "node.h"
#include "misc.h"

dmux2to4::dmux2to4()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("2to4 demultiplexer verilog device");

  Props.push_back(qucs::Property("TR", "6", false,
    QObject::tr ("transfer function high scaling factor")));
  Props.push_back(qucs::Property("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));

  createSymbol ();
  set_qucs_text_position(x1()+19, y2()+4);
  Model = "dmux2to4";
  Name  = "Y";
}

Component * dmux2to4::newOne()
{
  dmux2to4 * p = new dmux2to4();
  p->Props.front().Value = Props.front().Value; 
  p->recreate(0); 
  return p;
}

Element * dmux2to4::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("2to4 Demux");
  BitmapFile = (char *) "dmux2to4";

  if(getNewOne) return new dmux2to4();
  return 0;
}

void dmux2to4::createSymbol()
{
  Lines.push_back(qucs::Line(-30, -90, 30,-90,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30, -90, 30, 20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30,  20,-30, 20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30,  20,-30,-90,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-50,-50,-40,-50,QPen(Qt::darkBlue,2)));  // EN
  Lines.push_back(qucs::Line(-50,-30,-30,-30,QPen(Qt::darkBlue,2)));  // A
  Lines.push_back(qucs::Line(-50,-10,-30,-10,QPen(Qt::darkBlue,2)));  // B
  Lines.push_back(qucs::Line( 30, 10, 50, 10,QPen(Qt::darkBlue,2)));  // Y3
  Lines.push_back(qucs::Line( 30,-10, 50,-10,QPen(Qt::darkBlue,2)));  // Y2
  Lines.push_back(qucs::Line( 30,-30, 50,-30,QPen(Qt::darkBlue,2)));  // Y1
  Lines.push_back(qucs::Line( 30,-50, 50,-50,QPen(Qt::darkBlue,2)));  // Y0

  Arcs.push_back(qucs::Arc( -40, -55, 10, 10, 0, 16*360, QPen(Qt::darkBlue,2)));
 
  Texts.push_back(qucs::Text(-25,-85, "DMUX", Qt::darkBlue, 12.0));

  Texts.push_back(qucs::Text(-28,-63, "En",Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-20,-33, "G", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-8, -38, "}", Qt::darkBlue, 16.0));
  Texts.push_back(qucs::Text( 2, -40, "0", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text( 2, -20, "3", Qt::darkBlue, 12.0));

  Texts.push_back(qucs::Text(-28,-43, "0", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text(-28,-23, "1", Qt::darkBlue, 12.0));

  Texts.push_back(qucs::Text( 15,-63, "0", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text( 15,-43, "1", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text( 15,-23, "2", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text( 15, -3, "3", Qt::darkBlue, 12.0));

  Lines.push_back(qucs::Line(0, -18, 12, -18, QPen(Qt::darkBlue,2)));

  Ports.push_back(qucs::Port(-50,-50));  // En
  Ports.push_back(qucs::Port(-50,-30));  // A
  Ports.push_back(qucs::Port(-50,-10));  // B
  Ports.push_back(qucs::Port( 50, 10));  // Y3
  Ports.push_back(qucs::Port( 50,-10));  // Y2
  Ports.push_back(qucs::Port( 50,-30));  // Y1
  Ports.push_back(qucs::Port( 50,-50));  // Y0

  set_x1(-50);
  set_y1(-94);
  set_x2(50);
  set_y2(24);
}

QString dmux2to4::vhdlCode( int )
{
  QString s="";

  QString td = prop(1).Value;     // delay time
  if(!misc::VHDL_Delay(td, Name)) return td; // time has not VHDL format
  td += ";\n";

  QString En = port(0).getConnection()->Name;
  QString A  = port(1).getConnection()->Name;
  QString B  = port(2).getConnection()->Name;
  QString Y3 = port(3).getConnection()->Name;
  QString Y2 = port(4).getConnection()->Name;
  QString Y1 = port(5).getConnection()->Name;
  QString Y0 = port(6).getConnection()->Name;

  s = "\n  "+Name+":process ("+En+", "+A+", "+B+")\n"+
      "  begin\n" +
      "    "+Y0+" <= "+"(not "+En+") and (not "+B+") and (not "+A+")"+td+ 
      "    "+Y1+" <= "+"(not "+En+") and (not "+B+") and "+A+td+
      "    "+Y2+" <= "+"(not "+En+") and "+B+" and (not "+A+")"+td+
      "    "+Y3+" <= "+"(not "+En+") and "+B+" and "+A+td+  
      "  end process;\n" ;
  return s;
}

QString dmux2to4::verilogCode( int )
{
  QString td = prop(1).Value;        // delay time
  if(!misc::Verilog_Delay(td, Name)) return td; // time does not have VHDL format

  QString l = "";

  QString En = port(0).getConnection()->Name;
  QString A  = port(1).getConnection()->Name;
  QString B  = port(2).getConnection()->Name;
  QString Y3 = port(3).getConnection()->Name;
  QString Y2 = port(4).getConnection()->Name;
  QString Y1 = port(5).getConnection()->Name;
  QString Y0 = port(6).getConnection()->Name;

  QString Y3R = "net_reg" + Name + Y3;
  QString Y2R = "net_reg" + Name + Y2;
  QString Y1R = "net_reg" + Name + Y1;
  QString Y0R = "net_reg" + Name + Y0;
  
  l = "\n  // " + Name + " 2to4 demux\n" +
      "  assign  " + Y0 + " = " + Y0R + ";\n" +
      "  reg     " + Y0R + " = 0;\n" +
      "  assign  " + Y1 + " = " + Y1R + ";\n" +
      "  reg     " + Y1R + " = 0;\n" +
      "  assign  " + Y2 + " = " + Y2R + ";\n" +
      "  reg     " + Y2R + " = 0;\n" +
      "  assign  " + Y3 + " = " + Y3R + ";\n" +
      "  reg     " + Y3R + " = 0;\n" +

      "  always @ ("+En+" or "+A+" or "+B+")\n" +
      "  begin\n"+
      "    "+Y0R+" <="+td+" (~"+En+") && (~"+B+") && (~"+A+")"+";\n" +
      "    "+Y1R+" <="+td+" (~"+En+") && (~"+B+") && ("+A+")"+";\n" +
      "    "+Y2R+" <="+td+" (~"+En+") && ("+B+") && (~"+A+")"+";\n" +
      "    "+Y3R+" <="+td+" (~"+En+") && ("+B+") && ("+A+")"+";\n" +
      "  end\n" ;

  return l;
}
