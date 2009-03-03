/***************************************************************************
                              mux8to1
                             ---------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/
/*
 * mux8to1.cpp - device implementations for mux8to1 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include <stdlib.h>

#include "mux8to1.h"
#include "node.h"
#include "main.h"

mux8to1::mux8to1()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("8to1 multiplexer verilog device");

  Props.append (new Property ("TR", "6", false,
    QObject::tr ("tranfer function high scaling factor")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));
 

  createSymbol ();
  tx = x1 + 19;
  ty = y2 + 4;
  Model = "mux8to1";
  Name  = "Y";
}

Component * mux8to1::newOne()
{
  mux8to1 * p = new mux8to1();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * mux8to1::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("8to1 Mux");
  BitmapFile = (char *) "mux8to1";

  if(getNewOne) return new mux8to1();
  return 0;
}

void mux8to1::createSymbol()
{
  Lines.append(new Line(-30, -80, 30,-80,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30, -80, 30, 190,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30, 190,-30, 190,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30, 190,-30, -80,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-50,-40,-40,-40,QPen(QPen::darkBlue,2)));  //EN
  Lines.append(new Line(-50,-20,-30, -20,QPen(QPen::darkBlue,2))); //A
  Lines.append(new Line(-50, 0, -30, 0,QPen(QPen::darkBlue,2)));   //B
  Lines.append(new Line(-50, 20, -30, 20,QPen(QPen::darkBlue,2))); //C

  Lines.append(new Line(-50, 40, -30, 40,QPen(QPen::darkBlue,2))); //D0
  Lines.append(new Line(-50, 60, -30, 60,QPen(QPen::darkBlue,2))); //D1
  Lines.append(new Line(-50, 80, -30, 80,QPen(QPen::darkBlue,2))); //D2
  Lines.append(new Line(-50, 100,-30,100,QPen(QPen::darkBlue,2))); //D3
  Lines.append(new Line(-50, 120,-30,120,QPen(QPen::darkBlue,2))); //D4
  Lines.append(new Line(-50, 140,-30,140,QPen(QPen::darkBlue,2))); //D5
  Lines.append(new Line(-50, 160,-30,160,QPen(QPen::darkBlue,2))); //D6
  Lines.append(new Line(-50, 180,-30,180,QPen(QPen::darkBlue,2))); //D7
  Lines.append(new Line( 30, 60, 50, 60,QPen(QPen::darkBlue,2)));  // Y

  Arcs.append(new Arc(  -40,   -45,  10,  10, 0, 16*360, QPen(QPen::darkBlue,2)));
 
  Texts.append(new Text(-17,-75, "MUX", QPen::darkBlue, 12.0));

  Texts.append(new Text(-25,-51, "En", QPen::darkBlue, 12.0));
  Texts.append(new Text(-14, -13, "G", QPen::darkBlue, 12.0));
  Texts.append(new Text(-1, -18, "}", QPen::darkBlue, 16.0));
  Texts.append(new Text( 12,-20, "0", QPen::darkBlue, 12.0));
  Texts.append(new Text( 12, -1, "7", QPen::darkBlue, 12.0));

  Texts.append(new Text( -25, -29, "0", QPen::darkBlue, 12.0));
  Texts.append(new Text( -25,   9, "2", QPen::darkBlue, 12.0));

  Texts.append(new Text( -25, 29, "0", QPen::darkBlue, 12.0));
  Texts.append(new Text( -25, 49, "1", QPen::darkBlue, 12.0));
  Texts.append(new Text( -25, 69, "2", QPen::darkBlue, 12.0));
  Texts.append(new Text( -25, 89, "3", QPen::darkBlue, 12.0));
  Texts.append(new Text( -25,109, "4", QPen::darkBlue, 12.0));
  Texts.append(new Text( -25,129, "5", QPen::darkBlue, 12.0));
  Texts.append(new Text( -25,149, "6", QPen::darkBlue, 12.0));
  Texts.append(new Text( -25,169, "7", QPen::darkBlue, 12.0));


  Lines.append(new Line(9, 0, 25, 0, QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-50,-40));  // En
  Ports.append(new Port(-50,-20));  // A
  Ports.append(new Port(-50,  0));  // B
  Ports.append(new Port(-50, 20));  // C
  Ports.append(new Port(-50, 40));  // D0
  Ports.append(new Port(-50, 60));  // D1
  Ports.append(new Port(-50, 80));  // D2
  Ports.append(new Port(-50,100));  // D3
  Ports.append(new Port(-50,120));  // D4
  Ports.append(new Port(-50,140));  // D5
  Ports.append(new Port(-50,160));  // D6
  Ports.append(new Port(-50,180));  // D7
  Ports.append(new Port( 50, 60 )); // Y

  x1 = -50; y1 = -84;
  x2 =  50; y2 =  194;
}

QString mux8to1::vhdlCode( int )
{
  QString s="";
  QString td=";\n";

  if(strtod(Props.at(1)->Value.latin1(), 0) != 0.0) { // delay time
      td = Props.at(1)->Value;
      if(!VHDL_Time(td, Name))
        return td;    // Time does not have VHDL format.
      td = " after " + td + ";\n";
    }

  QString En = Ports.at(0)->Connection->Name;
  QString A  = Ports.at(1)->Connection->Name;
  QString B  = Ports.at(2)->Connection->Name;
  QString C  = Ports.at(3)->Connection->Name;
  QString D0 = Ports.at(4)->Connection->Name;
  QString D1 = Ports.at(5)->Connection->Name;
  QString D2 = Ports.at(6)->Connection->Name;
  QString D3 = Ports.at(7)->Connection->Name;
  QString D4 = Ports.at(8)->Connection->Name;
  QString D5 = Ports.at(9)->Connection->Name;
  QString D6 = Ports.at(10)->Connection->Name;
  QString D7 = Ports.at(11)->Connection->Name;
  QString y  = Ports.at(12)->Connection->Name;

  s = "\n  " + Name + ":process (" + En + ", " +  A + ", " + B + ", " + C + ", " +
                              D0 + ", " +  D1 + ", " + D2 + ", " + D3 + ", " +
                              D4 + ", " +  D5 + ", " + D6 + ", " + D7 + ")\n"  +
     "  begin\n    " +
     y + " <= " +  "(not " + En + ") and ((" + D7 + " and "  + C + " and " + B   + " and " +  A + ") or\n" + 
         "                 (" + D6 + " and " + C + " and " + B + " and not "+ A + ") or\n" +
         "                 (" + D5 + " and "  + C + " and not "+ B + " and " + A + ") or\n" +
         "                 (" + D4 + " and "  + C + " and not " + B + " and not "+A + ") or\n" + 
         "                 (" + D3 + " and not "  + C + " and " + B   +  " and " +  A + ") or\n" + 
         "                 (" + D2 + " and not "  + C + " and " + B   +  " and not "+ A + ") or\n" +
         "                 (" + D1 + " and not "  + C + " and not "+ B   +  " and " +  A + ") or\n" +
         "                 (" + D0 + " and not "  + C + " and not " + B +  " and not "+A + "))" + td +
     "  end process;\n";
  return s;
}

QString mux8to1::verilogCode( int )
{
  QString td = " ";
  if(strtod(Props.at(1)->Value.latin1(), 0) != 0.0) { // delay time
      td = Props.at(1)->Value;
      if(!Verilog_Time(td, Name))
        return td;    // time has not VHDL format.
      td = " #" + td;
    }
  
  QString l = "";

  QString En = Ports.at(0)->Connection->Name;
  QString A  = Ports.at(1)->Connection->Name;
  QString B  = Ports.at(2)->Connection->Name;
  QString C  = Ports.at(3)->Connection->Name;
  QString D0 = Ports.at(4)->Connection->Name;
  QString D1 = Ports.at(5)->Connection->Name;
  QString D2 = Ports.at(6)->Connection->Name;
  QString D3 = Ports.at(7)->Connection->Name;
  QString D4 = Ports.at(8)->Connection->Name;
  QString D5 = Ports.at(9)->Connection->Name;
  QString D6 = Ports.at(10)->Connection->Name;
  QString D7 = Ports.at(11)->Connection->Name;
  QString y  = Ports.at(12)->Connection->Name;

  QString v = "net_reg" + Name + y;
  
  l = "\n  // " + Name + " 8to1 mux\n" +
      "  assign  " + y + " = " + v + ";\n" +
      "  reg     " + v + " = 0;\n" +
      "  always @ (" + En + " or " + A + " or " + B + " or " + C + " or " +
                       D0 + " or " + D1 + " or " + D2 + " or " + D3 +  " or " +
                       D4 + " or " + D5 + " or " + D6 + " or " + D7 + ")\n" +
      "    " + v + " <=" + td + " (" + D7 + " && "  + C + " && " + B + " && " + A  +")" + " ||\n" +
      "                         (" + D6 + " && "  + C + " && " + B + " && ~"+ A  +")" + " ||\n" +
      "                         (" + D5 + " && "  + C + " && ~"+ B + " && " + A  +")" + " ||\n" +
      "                         (" + D4 + " && "  + C + " && ~"+ B + " && ~"+ A  +")" + " ||\n" +
      "                         (" + D3 + " && ~" + C + " && " + B + " && " + A  +")" + " ||\n" +
      "                         (" + D2 + " && ~" + C + " && " + B + " && ~"+ A  +")" + " ||\n" +
      "                         (" + D1 + " && ~" + C + " && ~"+ B + " && " + A  +")" + " ||\n" +
      "                         (" + D0 + " && ~" + C + " && ~"+ B + " && ~"+ A  +")" + ";\n" ;

  return l;
}

