/***************************************************************************
                            binarytogrey4bit
                          -------------------
    begin                : December 2008
    copyright            : (C) 2008 by Mike Brinson
    email                : mbrin72043@yahoo.co.uk
 ***************************************************************************/
/*
 * binarytogrey4bit.cpp - device implementations for binarytogrey4bit module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include <stdlib.h>

#include "binarytogrey4bit.h"
#include "node.h"
#include "main.h"

binarytogrey4bit::binarytogrey4bit()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("4bit binary to grey converter verilog device");

  Props.append (new Property ("TR", "6", false,
    QObject::tr ("tranfer function scaling factor")));
  Props.append (new Property ("Delay", "1 ns", false,
    QObject::tr ("output delay")
    +" ("+QObject::tr ("s")+")"));

  createSymbol ();
  tx = x1 + 19;
  ty = y2 + 4;
  Model = "binarytogrey4bit";
  Name  = "Y";
}

Component * binarytogrey4bit::newOne()
{
  binarytogrey4bit * p = new binarytogrey4bit();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * binarytogrey4bit::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("4Bit Bin2Grey");
  BitmapFile = (char *) "binarytogrey4bit";

  if(getNewOne) return new binarytogrey4bit();
  return 0;
}

void binarytogrey4bit::createSymbol()
{
  Lines.append(new Line(-30, -60, 30,-60,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30, -60, 30, 40,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,  40,-30, 40,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  40,-30, -60,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-50,-30,-30,-30,QPen(QPen::darkBlue,2)));  // B0
  Lines.append(new Line(-50,-10,-30,-10,QPen(QPen::darkBlue,2)));  // B1
  Lines.append(new Line(-50, 10,-30, 10,QPen(QPen::darkBlue,2)));  // B2
  Lines.append(new Line(-50, 30,-30, 30,QPen(QPen::darkBlue,2)));  // B3

  Lines.append(new Line( 30, 30, 50, 30,QPen(QPen::darkBlue,2)));  // G3
  Lines.append(new Line( 30, 10, 50, 10,QPen(QPen::darkBlue,2)));  // G2
  Lines.append(new Line( 30,-10, 50,-10,QPen(QPen::darkBlue,2)));  // G1
  Lines.append(new Line( 30,-30, 50,-30,QPen(QPen::darkBlue,2)));  // G0
 
  Texts.append(new Text(-14,-58, "B/G", QPen::darkBlue, 12.0));
 
  Texts.append(new Text(-25,-41, "0", QPen::darkBlue, 12.0));
  Texts.append(new Text(-25,-21, "1", QPen::darkBlue, 12.0));
  Texts.append(new Text(-25, -1, "2", QPen::darkBlue, 12.0));
  Texts.append(new Text(-25, 19, "3", QPen::darkBlue, 12.0));


  Texts.append(new Text( 15,-41, "0", QPen::darkBlue, 12.0));
  Texts.append(new Text( 15,-21, "1", QPen::darkBlue, 12.0));
  Texts.append(new Text( 15, -1, "2", QPen::darkBlue, 12.0));
  Texts.append(new Text( 15, 19, "3", QPen::darkBlue, 12.0));

  Ports.append(new Port(-50,-30));  // B0
  Ports.append(new Port(-50,-10));  // B1
  Ports.append(new Port(-50, 10));  // B2
  Ports.append(new Port(-50, 30));  // B3

  Ports.append(new Port( 50, 30));  // G3
  Ports.append(new Port( 50, 10));  // G2
  Ports.append(new Port( 50,-10));  // G1
  Ports.append(new Port( 50,-30));  // G0

  x1 = -50; y1 = -64;
  x2 =  50; y2 =  44;
}

QString binarytogrey4bit::vhdlCode( int )
{
  QString s="";
  QString td=";\n";

  if(strtod(Props.at(1)->Value.latin1(), 0) != 0.0) { // delay time
      td = Props.at(1)->Value;
      if(!VHDL_Time(td, Name))
        return td;    // Time does not have VHDL format.
      td = " after " + td + ";\n";
    }

  QString B0 = Ports.at(0)->Connection->Name;
  QString B1 = Ports.at(1)->Connection->Name;
  QString B2 = Ports.at(2)->Connection->Name;
  QString B3 = Ports.at(3)->Connection->Name;
  QString G3 = Ports.at(4)->Connection->Name;
  QString G2 = Ports.at(5)->Connection->Name;
  QString G1 = Ports.at(6)->Connection->Name;
  QString G0 = Ports.at(7)->Connection->Name;
 
 
  s = "\n  "+Name + ":process ("+B0+", "+B1+", "+B2+", "+B3+")\n" +
      "  begin\n"+
      "    "+G0+" <= "+B0+" xor "+B1+td+
      "    "+G1+" <= "+B1+" xor "+B2+td+
      "    "+G2+" <= "+B2+" xor "+B3+td+
      "    "+G3+" <= "+B3+td+
      "  end process;\n";
  return s;
}

QString binarytogrey4bit::verilogCode( int )
{
  QString td = "";
  if(strtod(Props.at(1)->Value.latin1(), 0) != 0.0) { // delay time
      td = Props.at(1)->Value;
      if(!Verilog_Time(td, Name))
        return td;    // Time does not have VHDL format
      td = " #" + td ;
    }
  
  QString B0 = Ports.at(0)->Connection->Name;
  QString B1 = Ports.at(1)->Connection->Name;
  QString B2 = Ports.at(2)->Connection->Name;
  QString B3 = Ports.at(3)->Connection->Name;
  QString G3 = Ports.at(4)->Connection->Name;
  QString G2 = Ports.at(5)->Connection->Name;
  QString G1 = Ports.at(6)->Connection->Name;
  QString G0 = Ports.at(7)->Connection->Name; 
 
  QString l = "";
 
  QString G0R = "net_reg" + Name + G0;
  QString G1R = "net_reg" + Name + G1;
  QString G2R = "net_reg" + Name + G2;
  QString G3R = "net_reg" + Name + G3;
  
  l = "\n  // " + Name + " 4bit binary to grey\n" +
      "  assign  " + G0 + " = " + G0R + ";\n" +
      "  reg     " + G0R + " = 0;\n" +
      "  assign  " + G1 + " = " + G1R + ";\n" +
      "  reg     " + G1R + " = 0;\n" +
      "  assign  " + G2 + " = " + G2R + ";\n" +
      "  reg     " + G2R + " = 0;\n" +
      "  assign  " + G3 + " = " + G3R + ";\n" +
      "  reg     " + G3R + " = 0;\n" +
      "  always @ ("+B0+" or "+B1+" or "+B2+" or "+B3+")\n" +
      "  begin\n"+
      "    "+G0R+" <="+td+" "+B0+" ^ "+B1+";\n"+
      "    "+G1R+" <="+td+" "+B1+" ^ "+B2+";\n"+
      "    "+G2R+" <="+td+" "+B2+" ^ "+B3+";\n"+
      "    "+G3R+" <="+td+" "+B3+";\n"+
      "  end\n";
  return l;
}
