/*
 * pad4bit.cpp - device implementations for pad4bit module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */
#include <stdlib.h>

#include "pad4bit.h"
#include "node.h"
#include "main.h"

pad4bit::pad4bit()
{
  Type = isComponent; // Analogue and digital component.
  Description = QObject::tr ("4bit pattern generator verilog device");

  Props.append (new Property ("Number", "0", false,
    QObject::tr ("pad output value")));

  createSymbol ();
  tx = x1 + 4;
  ty = y2 + 4;
  Model = "pad4bit";
  Name  = "Y";
}

Component * pad4bit::newOne()
{
  pad4bit * p = new pad4bit();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * pad4bit::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("4Bit Pattern");
  BitmapFile = (char *) "pad4bit";

  if(getNewOne) return new pad4bit();
  return 0;
}

void pad4bit::createSymbol()
{
  Lines.append(new Line(-60, -50, 30,-50,QPen(QPen::darkGreen,2)));
  Lines.append(new Line( 30, -50, 30, 50,QPen(QPen::darkGreen,2)));
  Lines.append(new Line( 30,  50,-60, 50,QPen(QPen::darkGreen,2)));
  Lines.append(new Line(-60,  50,-60,-50,QPen(QPen::darkGreen,2)));

  Lines.append(new Line( 40,-30, 30,-30,QPen(QPen::darkGreen,2)));  // A
  Lines.append(new Line( 40,-10, 30,-10,QPen(QPen::darkGreen,2)));  // B
  Lines.append(new Line( 40, 10, 30, 10,QPen(QPen::darkGreen,2))); // C
  Lines.append(new Line( 40, 30, 30, 30,QPen(QPen::darkGreen,2))); // D
 
  Texts.append(new Text(-58,-46, " 0   1   2    3", QPen::darkGreen, 12.0));
  Texts.append(new Text(-58,-21, " 4   5   6    7", QPen::darkGreen, 12.0));
  Texts.append(new Text(-58, 4, " 8   9  10 11", QPen::darkGreen, 12.0));
  Texts.append(new Text(-58, 24, "12 13 14 15", QPen::darkGreen, 12.0));
 
  Ports.append(new Port(40, 30));  // D
  Ports.append(new Port(40, 10));  // C
  Ports.append(new Port(40,-10));  // B
  Ports.append(new Port(40,-30));  // A

  x1 = -64; y1 = -54;
  x2 =  40; y2 =  54;
}

QString pad4bit::vhdlCode( int )
{
  QString v = "";
  QString s1, s2, s3, s ="";

  QString A    = Ports.at(0)->Connection->Name;
  QString B    = Ports.at(1)->Connection->Name;
  QString C    = Ports.at(2)->Connection->Name;
  QString D    = Ports.at(3)->Connection->Name;

  v = Props.at(0)->Value; 
  s1 = "\n  "+Name+":process\n"+"  begin\n";
  if (v == "0") s2 = "    "+A+" <= '0'; "+B+" <= '0'; "+C+" <= '0'; "+D+" <= '0';\n";
  if (v == "1") s2 = "    "+A+" <= '0'; "+B+" <= '0'; "+C+" <= '0'; "+D+" <= '1';\n";
  if (v == "2") s2 = "    "+A+" <= '0'; "+B+" <= '0'; "+C+" <= '1'; "+D+" <= '0';\n";
  if (v == "3") s2 = "    "+A+" <= '0'; "+B+" <= '0'; "+C+" <= '1'; "+D+" <= '1';\n";
  if (v == "4") s2 = "    "+A+" <= '0'; "+B+" <= '1'; "+C+" <= '0'; "+D+" <= '0';\n";
  if (v == "5") s2 = "    "+A+" <= '0'; "+B+" <= '1'; "+C+" <= '0'; "+D+" <= '1';\n";
  if (v == "6") s2 = "    "+A+" <= '0'; "+B+" <= '1'; "+C+" <= '1'; "+D+" <= '0';\n";
  if (v == "7") s2 = "    "+A+" <= '0'; "+B+" <= '1'; "+C+" <= '1'; "+D+" <= '1';\n";
  if (v == "8") s2 = "    "+A+" <= '1'; "+B+" <= '0'; "+C+" <= '0'; "+D+" <= '0';\n";
  if (v == "9") s2 = "    "+A+" <= '1'; "+B+" <= '0'; "+C+" <= '0'; "+D+" <= '1';\n";
  if (v == "10") s2 = "   "+A+" <= '1'; "+B+" <= '0'; "+C+" <= '1'; "+D+" <= '0';\n";
  if (v == "11") s2 = "   "+A+" <= '1'; "+B+" <= '0'; "+C+" <= '1'; "+D+" <= '1';\n";
  if (v == "12") s2 = "   "+A+" <= '1'; "+B+" <= '1'; "+C+" <= '0'; "+D+" <= '0';\n";
  if (v == "13") s2 = "   "+A+" <= '1'; "+B+" <= '1'; "+C+" <= '0'; "+D+" <= '1';\n";
  if (v == "14") s2 = "   "+A+" <= '1'; "+B+" <= '1'; "+C+" <= '1'; "+D+" <= '0';\n";
  if (v == "15") s2 = "   "+A+" <= '1'; "+B+" <= '1'; "+C+" <= '1'; "+D+" <= '1';\n";

  s3 =     "  end process;\n";
  s = s1+s2+s3;
  return s;
}

QString pad4bit::verilogCode( int )
{
  QString v = "";
  v = Props.at(0)->Value;

  QString l = "";
  QString l1, l2, l3, s ="";

  QString A    = Ports.at(0)->Connection->Name;
  QString B    = Ports.at(1)->Connection->Name;
  QString C    = Ports.at(2)->Connection->Name;
  QString D    = Ports.at(3)->Connection->Name;

  QString AR  = "A_reg"  + Name + A;
  QString BR  = "B_reg"  + Name + B;
  QString CR  = "C_reg"  + Name + C;
  QString DR  = "C_reg"  + Name + D;
  

  l1 = "\n  // "+Name+" 4bit pattern generator\n"+
      "  assign  "+A+" = "+AR+";\n"+
      "  reg     "+AR+" = 0;\n"+
      "  assign  "+B+" = "+BR+";\n"+
      "  reg     "+BR+" = 0;\n"+
      "  assign  "+C+" = "+CR+";\n"+
      "  reg     "+CR+" = 0;\n"+
      "  assign  "+D+" = "+DR+";\n"+
      "  reg     "+DR+" = 0;\n"+
      "  initial\n";
  if (v == "0" ) l2 = "  begin\n    "+AR+" = 0; "+BR+" = 0; "+CR+" = 0; "+DR+" = 0;\n";
  if (v == "1" ) l2 = "  begin\n    "+AR+" = 0; "+BR+" = 0; "+CR+" = 0; "+DR+" = 1;\n";
  if (v == "2" ) l2 = "  begin\n    "+AR+" = 0; "+BR+" = 0; "+CR+" = 1; "+DR+" = 0;\n";
  if (v == "3" ) l2 = "  begin\n    "+AR+" = 0; "+BR+" = 0; "+CR+" = 1; "+DR+" = 1;\n";
  if (v == "4" ) l2 = "  begin\n    "+AR+" = 0; "+BR+" = 1; "+CR+" = 0; "+DR+" = 0;\n";
  if (v == "5" ) l2 = "  begin\n    "+AR+" = 0; "+BR+" = 1; "+CR+" = 0; "+DR+" = 1;\n";
  if (v == "6" ) l2 = "  begin\n    "+AR+" = 0; "+BR+" = 1; "+CR+" = 1; "+DR+" = 0;\n";
  if (v == "7" ) l2 = "  begin\n    "+AR+" = 0; "+BR+" = 1; "+CR+" = 1; "+DR+" = 1;\n";
  if (v == "8" ) l2 = "  begin\n    "+AR+" = 1; "+BR+" = 0; "+CR+" = 0; "+DR+" = 0;\n";
  if (v == "9" ) l2 = "  begin\n    "+AR+" = 1; "+BR+" = 0; "+CR+" = 0; "+DR+" = 1;\n";
  if (v == "10" ) l2 = "  begin\n    "+AR+" = 1; "+BR+" = 0; "+CR+" = 1; "+DR+" = 0;\n";
  if (v == "11" ) l2 = "  begin\n    "+AR+" = 1; "+BR+" = 0; "+CR+" = 1; "+DR+" = 1;\n";
  if (v == "12" ) l2 = "  begin\n    "+AR+" = 1; "+BR+" = 1; "+CR+" = 0; "+DR+" = 0;\n";
  if (v == "13" ) l2 = "  begin\n    "+AR+" = 1; "+BR+" = 1; "+CR+" = 0; "+DR+" = 1;\n";
  if (v == "14" ) l2 = "  begin\n    "+AR+" = 1; "+BR+" = 1; "+CR+" = 1; "+DR+" = 0;\n";
  if (v == "15" ) l2 = "  begin\n    "+AR+" = 1; "+BR+" = 1; "+CR+" = 1; "+DR+" = 1;\n";
  l3 =      "  end\n";
  l = l1+l2+l3;
  return l;
}
