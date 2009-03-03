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
#include <stdlib.h>

#include "gatedDlatch.h"
#include "node.h"
#include "main.h"

gatedDlatch::gatedDlatch()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("gated D latch verilog device");

  Props.append (new Property ("TR_H", "6", false,
    QObject::tr ("cross coupled gate tranfer function high scaling factor")));
  Props.append (new Property ("TR_L", "5", false,
    QObject::tr ("cross coupled gate tranfer function low scaling factor")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("cross coupled gate delay")
    +" ("+QObject::tr ("s")+")"));

  createSymbol ();
  tx = x1 + 19;
  ty = y2 + 4;
  Model = "gatedDlatch";
  Name  = "Y";
}

Component * gatedDlatch::newOne()
{
  gatedDlatch * p = new gatedDlatch();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
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
  Lines.append(new Line(-30, -40, 30,-40,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30, -40, 30, 40,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,  40,-30, 40,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30, 40,-30, -40,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-50,-25,-30,-25,QPen(QPen::darkBlue,2))); // D
  Lines.append(new Line(-50, 25,-30, 25,QPen(QPen::darkBlue,2))); // C
  Lines.append(new Line( 40, 25, 50, 25,QPen(QPen::darkBlue,2))); // QB
  Lines.append(new Line( 30,-25, 50,-25,QPen(QPen::darkBlue,2))); // Q

  Arcs.append(new Arc( 30, 20,  10,  10, 0, 16*360, QPen(QPen::darkBlue,2)));

  Texts.append(new Text(-25,-35, "D", QPen::darkBlue, 12.0));
  Texts.append(new Text(-25, 14, "C", QPen::darkBlue, 12.0));
 
  Ports.append(new Port(-50,-25));  // D
  Ports.append(new Port(-50, 25));  // C
  Ports.append(new Port( 50, 25));  // QB
  Ports.append(new Port( 50,-25));  // Q

  x1 = -50; y1 = -44;
  x2 =  50; y2 =  44;
}

QString gatedDlatch::vhdlCode( int )
{
  QString s="";
  QString td=";\n";

  if(strtod(Props.at(2)->Value.latin1(), 0) != 0.0) { // delay time
      td = Props.at(2)->Value;
      if(!VHDL_Time(td, Name))
        return td;    // Time does not have VHDL format.
      td = " after " + td + ";\n";
    }

  QString D    = Ports.at(0)->Connection->Name;
  QString C    = Ports.at(1)->Connection->Name;
  QString QB   = Ports.at(2)->Connection->Name;
  QString Q    = Ports.at(3)->Connection->Name;

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
  QString td = "";
  if(strtod(Props.at(2)->Value.latin1(), 0) != 0.0) { // delay time
      td = Props.at(2)->Value;
      if(!Verilog_Time(td, Name))
        return td;    // Time does not have VHDL format.
      td = " #" + td ;
    }
  
  QString l = "";

  QString D    = Ports.at(0)->Connection->Name;
  QString C    = Ports.at(1)->Connection->Name;
  QString QB   = Ports.at(2)->Connection->Name;
  QString Q    = Ports.at(3)->Connection->Name;

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
