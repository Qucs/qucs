/***************************************************************************
                              mux4to1
                             ---------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/
/*
 * mux4to1.cpp - device implementations for mux4to1 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include <stdlib.h>

#include "mux4to1.h"
#include "node.h"
#include "main.h"

mux4to1::mux4to1()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("4to1 multiplexer verilog device");

  Props.append (new Property ("TR", "6", false,
    QObject::tr ("tranfer function high scaling factor")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));
 
  createSymbol ();
  tx = x1 + 19;
  ty = y2 + 4;
  Model = "mux4to1";
  Name  = "Y";
}

Component * mux4to1::newOne()
{
  mux4to1 * p = new mux4to1();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * mux4to1::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("4to1 Mux");
  BitmapFile = (char *) "mux4to1";

  if(getNewOne) return new mux4to1();
  return 0;
}

void mux4to1::createSymbol()
{
  Lines.append(new Line(-30, -80, 30,-80,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30, -80, 30, 100,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,  100,-30, 100,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30, 100,-30, -80,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-50,-40,-40,-40,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-50,-20,-30, -20,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-50, 0, -30, 0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-50, 30, -30, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-50, 50,-30, 50,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-50, 70, -30, 70,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-50, 90, -30, 90,QPen(QPen::darkBlue,2)));

  Lines.append(new Line( 30, 10, 50, 10,QPen(QPen::darkBlue,2)));

  Arcs.append(new Arc(  -40,   -45,  10,  10, 0, 16*360, QPen(QPen::darkBlue,2)));
 
  Texts.append(new Text(-17,-75, "MUX", QPen::darkBlue, 12.0));

  Texts.append(new Text(-25,-51, "En", QPen::darkBlue, 12.0));
  Texts.append(new Text(-14, -23, "G", QPen::darkBlue, 12.0));
  Texts.append(new Text(-1, -28, "}", QPen::darkBlue, 16.0));
  Texts.append(new Text( 12,-28, "0", QPen::darkBlue, 12.0));
  Texts.append(new Text( 12, -9, "3", QPen::darkBlue, 12.0));

  Texts.append(new Text( -25, -29, "0", QPen::darkBlue, 12.0));
  Texts.append(new Text( -25,  -9, "1", QPen::darkBlue, 12.0));

  Texts.append(new Text( -25, 19, "0", QPen::darkBlue, 12.0));
  Texts.append(new Text( -25, 39, "1", QPen::darkBlue, 12.0));
  Texts.append(new Text( -25, 59, "2", QPen::darkBlue, 12.0));
  Texts.append(new Text( -25, 79, "3", QPen::darkBlue, 12.0));

  Lines.append(new Line(9, -8, 25, -8, QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-50,-40));  // En
  Ports.append(new Port(-50, -20)); // A
  Ports.append(new Port(-50,  0));  // B
  Ports.append(new Port(-50, 30));  // D0
  Ports.append(new Port(-50, 50));  // D1
  Ports.append(new Port(-50, 70));  // D2
  Ports.append(new Port(-50, 90));  // D3
  Ports.append(new Port( 50, 10 ));  // Y

  x1 = -50; y1 = -84;
  x2 =  50; y2 =  104;
}

QString mux4to1::vhdlCode( int )
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
  QString D0 = Ports.at(3)->Connection->Name;
  QString D1 = Ports.at(4)->Connection->Name;
  QString D2 = Ports.at(5)->Connection->Name;
  QString D3 = Ports.at(6)->Connection->Name;
  QString y  = Ports.at(7)->Connection->Name;

  s = "\n  " + Name + ":process (" + En + ", " +  A + ", " + B + ", " +
                              D0 + ", " +  D1 + ", " + D2 + ", " + D3 + ")\n"  +
     "  begin\n    " +
     y + " <= " +  "(not " + En + ") and ((" + D3 + " and " + B + " and " +  A + ") or\n" + 
         "                 (" + D2 + " and " + B + " and not " + A + ") or\n" +
         "                 (" + D1 + " and not " + B + " and " + A + ") or\n" +
         "                 (" + D0 + " and not " + B + " and not " + A + ")" + ")" + td +
     "  end process;\n";
  return s;
}

QString mux4to1::verilogCode( int )
{
  QString td = " ";
  if(strtod(Props.at(1)->Value.latin1(), 0) != 0.0) { // delay time
      td = Props.at(1)->Value;
      if(!Verilog_Time(td, Name))
        return td;    // time has not VHDL format.
      td = " #" + td ;
    }
  
  QString l = "";

  QString En = Ports.at(0)->Connection->Name;
  QString A  = Ports.at(1)->Connection->Name;
  QString B  = Ports.at(2)->Connection->Name;
  QString D0 = Ports.at(3)->Connection->Name;
  QString D1 = Ports.at(4)->Connection->Name;
  QString D2 = Ports.at(5)->Connection->Name;
  QString D3 = Ports.at(6)->Connection->Name;
  QString y  = Ports.at(7)->Connection->Name;

  QString v = "net_reg" + Name + y;
  
  l = "\n  // " + Name + " 4to1 mux\n" +
      "  assign  " + y + " = " + v + ";\n" +
      "  reg     " + v + " = 0;\n" +
      "  always @ (" + En + " or " + A + " or " + B + " or " 
                     + D0 + " or " + D1 +  " or " + D2 + " or " + D0 + ")\n" +
      "    " + v + " <=" + td + " (" + D3 + " && " + B + " && " + A  +")" + " ||\n" +
      "                         (" + D2 + " && " + B + " && ~"+ A  +")" + " ||\n" +
      "                         (" + D1 + " && ~"+ B + " && " + A  +")" + " ||\n" +
      "                         (" + D0 + " && ~"+ B + " && ~"+ A  +");\n";

  return l;
}

