/***************************************************************************
                             hpribin4bit
                            -------------
    begin                : January 2009
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/

/*
 * hpribin4bit.cpp - device implementations for hpribin4bit module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include <stdlib.h>

#include "hpribin4bit.h"
#include "node.h"
#include "main.h"

hpribin4bit::hpribin4bit()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("4bit highest priority encoder (binary form) verilog device");

  Props.append (new Property ("TR", "6", false,
    QObject::tr ("transfer function scaling factor")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));

  createSymbol ();
  tx = x1 + 19;
  ty = y2 + 4;
  Model = "hpribin4bit";
  Name  = "Y";
}

Component * hpribin4bit::newOne()
{
  hpribin4bit * p = new hpribin4bit();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * hpribin4bit::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("4Bit HPRI-Bin");
  BitmapFile = (char *) "hpribin4bit";

  if(getNewOne) return new hpribin4bit();
  return 0;
}

void hpribin4bit::createSymbol()
{
  Lines.append(new Line(-40, -50, 40,-50,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 40, -50, 40, 60,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 40,  60,-40, 60,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-40,  60,-40, -50,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-60,-10,-40,-10,QPen(QPen::darkBlue,2)));  // A
  Lines.append(new Line(-60, 10,-40, 10,QPen(QPen::darkBlue,2)));  // B
  Lines.append(new Line(-60, 30,-40, 30,QPen(QPen::darkBlue,2)));  // C
  Lines.append(new Line(-60, 50,-40, 50,QPen(QPen::darkBlue,2)));  // D
  Lines.append(new Line( 40, 30, 60, 30,QPen(QPen::darkBlue,2)));  // V
  Lines.append(new Line( 40, 10, 60, 10,QPen(QPen::darkBlue,2)));  // Y
  Lines.append(new Line( 40,-10, 60,-10,QPen(QPen::darkBlue,2)));  // X

  Texts.append(new Text(-35,-45, "HPRI/BIN", QPen::darkBlue, 12.0));

  Texts.append(new Text(-35,-23,  "0",  QPen::darkBlue, 12.0));
  Texts.append(new Text(-35, -3,  "1",  QPen::darkBlue, 12.0)); 
  Texts.append(new Text(-35, 17,  "2",  QPen::darkBlue, 12.0));
  Texts.append(new Text(-35, 37,  "3",  QPen::darkBlue, 12.0));

  Texts.append(new Text( 25, 17,  "V",  QPen::darkBlue, 12.0)); 
  Texts.append(new Text( 25, -3,  "Y",  QPen::darkBlue, 12.0));
  Texts.append(new Text( 25,-23,  "X",  QPen::darkBlue, 12.0));

  Ports.append(new Port(-60,-10));  // A
  Ports.append(new Port(-60, 10));  // B
  Ports.append(new Port(-60, 30));  // C
  Ports.append(new Port(-60, 50));  // D
  Ports.append(new Port( 60, 30));  // V
  Ports.append(new Port( 60, 10));  // Y
  Ports.append(new Port( 60,-10));  // X

  x1 = -60; y1 = -54;
  x2 =  60; y2 =  64;
}

QString hpribin4bit::vhdlCode( int )
{
  QString s="";
  QString td=";\n";

  if(strtod(Props.at(1)->Value.latin1(), 0) != 0.0) { // delay time
    td = Props.at(1)->Value;
    if(!VHDL_Time(td, Name))
      return td;    // Time does not have VHDL format.
    td = " after " + td + ";\n";
  }

  QString A    = Ports.at(0)->Connection->Name;
  QString B    = Ports.at(1)->Connection->Name;
  QString C    = Ports.at(2)->Connection->Name;
  QString D    = Ports.at(3)->Connection->Name;
  QString V    = Ports.at(4)->Connection->Name;
  QString Y    = Ports.at(5)->Connection->Name;
  QString X    = Ports.at(6)->Connection->Name;

  s = "\n  "+Name+":process ("+A+", "+B+", "+C+", "+D+")\n"+
      "  begin\n" +
      "    "+X+" <= "+D+" or "+C+td+ 
      "    "+Y+" <= "+D+" or ((not "+C+") and "+B+")"+td+
      "    "+V+" <= "+D+" or "+C+" or "+B+" or "+A+td+ 
      "  end process;\n";
  return s;
}

QString hpribin4bit::verilogCode( int )
{
  QString t = "";

  if(strtod(Props.at(1)->Value.latin1(), 0) != 0.0) { // delay time
    t = Props.at(1)->Value;
    if(!Verilog_Time(t, Name))
      return t;    // Time does not have VHDL format.
    t = " #" + t ;
  }
  
  QString l = "";

  QString A    = Ports.at(0)->Connection->Name;
  QString B    = Ports.at(1)->Connection->Name;
  QString C    = Ports.at(2)->Connection->Name;
  QString D    = Ports.at(3)->Connection->Name;
  QString V    = Ports.at(4)->Connection->Name;
  QString Y    = Ports.at(5)->Connection->Name;
  QString X    = Ports.at(6)->Connection->Name;

  QString VR  = "V_reg"  + Name + V;
  QString YR  = "Y_reg"  + Name + Y;
  QString XR  = "X_reg"  + Name + X;

  l = "\n  // "+Name+" 4bit hpribin\n"+
      "  assign  "+V+" = "+VR+";\n"+
      "  reg     "+VR+" = 0;\n"+
      "  assign  "+Y+" = "+YR+";\n"+
      "  reg     "+YR+" = 0;\n"+
      "  assign  "+X+" = "+XR+";\n"+
      "  reg     "+XR+" = 0;\n"+
      "  always @ ("+A+" or "+B+" or "+C+" or "+D+")\n"+
      "  begin\n" +
      "    " +XR+" <="+t+" "+D+" || "+C+";\n"+
      "    " +YR+" <="+t+" "+D+" || ((~"+C+") && "+B+");\n" +
      "    " +VR+" <="+t+" "+D+" || "+C+" || "+B+" || "+A+";\n"+     
      "  end\n";

  return l;
}
