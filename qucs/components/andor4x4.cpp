/***************************************************************************
                              andor4x4
                             ----------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * andor4x4.cpp - device implementations for andor4x4 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include "andor4x4.h"
#include "node.h"
#include "misc.h"

andor4x4::andor4x4()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("4x4 andor verilog device");

  Props.push_back(qucs::Property("TR", "6", false,
    QObject::tr ("transfer function high scaling factor")));
  Props.push_back(qucs::Property("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));

  createSymbol ();
  set_qucs_text_position(x1()+19, y2()+4);
  Model = "andor4x4";
  Name  = "Y";
}

Component * andor4x4::newOne()
{
  andor4x4 * p = new andor4x4();
  p->Props.front().Value = Props.front().Value; 
  p->recreate(0); 
  return p;
}

Element * andor4x4::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("4x4 AndOr");
  BitmapFile = (char *) "andor4x4";

  if(getNewOne) return new andor4x4();
  return 0;
}

void andor4x4::createSymbol()
{
  Lines.push_back(qucs::Line(-30, -60, 30,-60,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30, -60, 30, 140,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30,  140,-30,140,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30,  140,-30,-60,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-30, -10,  0,-10,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30,  40,  0, 40,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30,  90,  0, 90,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0, -60,  0,140,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-50,-50,-30,-50,QPen(Qt::darkBlue,2)));   //A11
  Lines.push_back(qucs::Line(-50,-40,-30,-40,QPen(Qt::darkBlue,2)));   //A12
  Lines.push_back(qucs::Line(-50,-30,-30,-30,QPen(Qt::darkBlue,2)));   //A13
  Lines.push_back(qucs::Line(-50,-20,-30,-20,QPen(Qt::darkBlue,2)));   //A14

  Lines.push_back(qucs::Line(-50,  0,-30,  0,QPen(Qt::darkBlue,2)));   //A21
  Lines.push_back(qucs::Line(-50, 10,-30, 10,QPen(Qt::darkBlue,2)));   //A22
  Lines.push_back(qucs::Line(-50, 20,-30, 20,QPen(Qt::darkBlue,2)));   //A23
  Lines.push_back(qucs::Line(-50, 30,-30, 30,QPen(Qt::darkBlue,2)));   //A24

  Lines.push_back(qucs::Line(-50, 50, -30, 50,QPen(Qt::darkBlue,2)));  //A31
  Lines.push_back(qucs::Line(-50, 60, -30, 60,QPen(Qt::darkBlue,2)));  //A32
  Lines.push_back(qucs::Line(-50, 70,-30,  70,QPen(Qt::darkBlue,2)));  //A33
  Lines.push_back(qucs::Line(-50, 80, -30, 80,QPen(Qt::darkBlue,2)));   //A34

  Lines.push_back(qucs::Line(-50, 100,-30,100,QPen(Qt::darkBlue,2)));  //A41
  Lines.push_back(qucs::Line(-50, 110,-30,110,QPen(Qt::darkBlue,2)));  //A42
  Lines.push_back(qucs::Line(-50, 120,-30,120,QPen(Qt::darkBlue,2)));  //A43
  Lines.push_back(qucs::Line(-50, 130,-30,130,QPen(Qt::darkBlue,2)));  //A44

  Lines.push_back(qucs::Line( 30,  40, 50, 40,QPen(Qt::darkBlue,2)));  //Y

  Texts.push_back(qucs::Text( -20, -60, "&", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text( -20, -10, "&", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text( -20,  40, "&", Qt::darkBlue, 12.0));
  Texts.push_back(qucs::Text( -20,  90, "&", Qt::darkBlue, 12.0));
 
  Lines.push_back(qucs::Line(  7, -45, 17, -40,QPen(Qt::darkBlue,2))); 
  Lines.push_back(qucs::Line(  7, -35, 17, -40,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  7, -30, 17, -35,QPen(Qt::darkBlue,2))); 
  Lines.push_back(qucs::Line( 22, -30, 22, -45,QPen(Qt::darkBlue,2)));  

  Ports.push_back(qucs::Port(-50,-50));  // A11
  Ports.push_back(qucs::Port(-50,-40));  // A12
  Ports.push_back(qucs::Port(-50,-30));  // A13
  Ports.push_back(qucs::Port(-50,-20));  // A14

  Ports.push_back(qucs::Port(-50,  0));  // A21
  Ports.push_back(qucs::Port(-50, 10));  // A22
  Ports.push_back(qucs::Port(-50, 20));  // A23
  Ports.push_back(qucs::Port(-50, 30));  // A24

  Ports.push_back(qucs::Port(-50, 50));  // A31
  Ports.push_back(qucs::Port(-50, 60));  // A32
  Ports.push_back(qucs::Port(-50, 70));  // A33
  Ports.push_back(qucs::Port(-50, 80));  // A34

  Ports.push_back(qucs::Port(-50,100));  // A41
  Ports.push_back(qucs::Port(-50,110));  // A42
  Ports.push_back(qucs::Port(-50,120));  // A43
  Ports.push_back(qucs::Port(-50,130));  // A44

  Ports.push_back(qucs::Port( 50, 40));  // Y

  set_x1(-50);
  set_y1(-64);
  set_x2(50);
  set_y2(144);
}

QString andor4x4::vhdlCode( int )
{
  QString s="";

  QString td = prop(1).Value;     // delay time
  if(!misc::VHDL_Delay(td, Name)) return td; // time has not VHDL format
  td += ";\n";

  QString a11 = port(0).getConnection()->Name;
  QString a12 = port(1).getConnection()->Name;
  QString a13 = port(2).getConnection()->Name;
  QString a14 = port(3).getConnection()->Name;
  QString a21 = port(4).getConnection()->Name;
  QString a22 = port(5).getConnection()->Name;
  QString a23 = port(6).getConnection()->Name;
  QString a24 = port(7).getConnection()->Name;
  QString a31 = port(8).getConnection()->Name;
  QString a32 = port(9).getConnection()->Name;
  QString a33 = port(10).getConnection()->Name;
  QString a34 = port(11).getConnection()->Name;
  QString a41 = port(12).getConnection()->Name;
  QString a42 = port(13).getConnection()->Name;
  QString a43 = port(14).getConnection()->Name;
  QString a44 = port(15).getConnection()->Name;

  QString y   = port(16).getConnection()->Name;

  s = "\n  " + Name + ":process (" +
      a11 + ", " +  a12 + ", " + a13 + ", " + a14 + ", " +
      a21 + ", " +  a22 + ", " + a23 + ", " + a24 + ", " +
      a31 + ", " +  a32 + ", " + a33 + ", " + a34 + ", " +
      a41 + ", " +  a42 + ", " + a43 + ", " + a44 + ")\n"  +
     "  begin\n    " +
     y + " <= " + "(" + a11 + " and " + a12 + " and " + a13 + " and " + a14 + ") or " + 
                  "(" + a21 + " and " + a22 + " and " + a23 + " and " + a24 + ") or\n" + 
         "                 (" + a31 + " and " + a32 + " and " + a33 + " and " + a34 + ") or " + 
                 "(" + a41 + " and " + a42 + " and " + a43 + " and " + a43 + ")" + td  +
     "  end process;\n";
  return s;
}

QString andor4x4::verilogCode( int )
{
  QString td = prop(1).Value;        // delay time
  if(!misc::Verilog_Delay(td, Name)) return td; // time does not have VHDL format
  
  QString a11 = port(0).getConnection()->Name;
  QString a12 = port(1).getConnection()->Name;
  QString a13 = port(2).getConnection()->Name;
  QString a14 = port(3).getConnection()->Name;
  QString a21 = port(4).getConnection()->Name;
  QString a22 = port(5).getConnection()->Name;
  QString a23 = port(6).getConnection()->Name;
  QString a24 = port(7).getConnection()->Name;
  QString a31 = port(8).getConnection()->Name;
  QString a32 = port(9).getConnection()->Name;
  QString a33 = port(10).getConnection()->Name;
  QString a34 = port(11).getConnection()->Name;
  QString a41 = port(12).getConnection()->Name;
  QString a42 = port(13).getConnection()->Name;
  QString a43 = port(14).getConnection()->Name;
  QString a44 = port(15).getConnection()->Name;

  QString y   = port(16).getConnection()->Name;

  QString l = "";
 
  QString v = "net_reg" + Name + y;
  
  l = "\n  // " + Name + " 4x4 andor\n" +
      "  assign  " + y + " = " + v + ";\n" +
      "  reg     " + v + " = 0;\n" +
      "  always @ (" + a11 + " or " + a12 + " or " + a13 + " or " + a14 + " or "
                     + a21 + " or " + a22 + " or " + a23 + " or " + a24 + " or "
		     + a31 + " or " + a32 + " or " + a33 + " or " + a34 + " or "
                     + a41 + " or " + a42 + " or " + a43 + " or " + a44 + ")\n  " +
      "  " + v + " <=" + td + " (" + a11 + " && " + a12 + " && " + a13 + " && " + a14 +")" + " || " +
                               "(" + a21 + " && " + a22 + " && " + a23 + " && " + a24 +")" + " ||\n" +
                 "                           (" + a31 + " && " + a32 + " && " + a33 + " && " + a34 +")" + " || " +
                               "(" + a41 + " && " + a42 + " && " + a43 + " && " + a44 +");\n" ;
  return l;
}

