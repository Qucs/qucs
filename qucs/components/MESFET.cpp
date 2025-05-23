/*
 * MESFET.cpp - device implementations for MESFET module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include "MESFET.h"

MESFET::MESFET()
{
  Description = QObject::tr("MESFET verilog device");

  Props.push_back(qucs::Property("LEVEL", "1", false,
    QObject::tr("model selector")));
  Props.push_back(qucs::Property("Vto", "-1.8", false,
    QObject::tr("pinch-off voltage")
    +" ("+QObject::tr("V")+")"));
  Props.push_back(qucs::Property("Beta", "3e-3", false,
    QObject::tr("transconductance parameter")
    +" ("+QObject::tr("A/(V*V)")+")"));
  Props.push_back(qucs::Property("Alpha", "2.25", false,
    QObject::tr("saturation voltage parameter")
    +" ("+QObject::tr("1/V")+")"));
  Props.push_back(qucs::Property("Lambda", "0.05", false,
    QObject::tr("channel length modulation parameter")
    +" ("+QObject::tr("1/V")+")"));
  Props.push_back(qucs::Property("B", "0.3", false,
    QObject::tr("doping profile parameter")
    +" ("+QObject::tr("1/V")+")"));
  Props.push_back(qucs::Property("Qp", "2.1", false,
    QObject::tr("power law exponent parameter")));
  Props.push_back(qucs::Property("Delta", "0.1", false,
    QObject::tr("power feedback parameter")
    +" ("+QObject::tr("1/W")+")"));
  Props.push_back(qucs::Property("Vmax", "0.5", false,
    QObject::tr("maximum junction voltage limit before capacitance limiting")
    +" ("+QObject::tr("V")+")"));
  Props.push_back(qucs::Property("Vdelta1", "0.3", false,
    QObject::tr("capacitance saturation transition voltage")
    +" ("+QObject::tr("V")+")"));
  Props.push_back(qucs::Property("Vdelta2", "0.2", false,
    QObject::tr("capacitance threshold transition voltage")
    +" ("+QObject::tr("V")+")"));
  Props.push_back(qucs::Property("Gamma", "0.015", false,
    QObject::tr("dc drain pull coefficient")));
  Props.push_back(qucs::Property("Nsc", "1", false,
    QObject::tr("subthreshold conductance parameter")));
  Props.push_back(qucs::Property("Is", "1e-14", false,
    QObject::tr("diode saturation current")
    +" ("+QObject::tr("I")+")"));
  Props.push_back(qucs::Property("N", "1", false,
    QObject::tr("diode emission coefficient")));
  Props.push_back(qucs::Property("Vbi", "1.0", false,
    QObject::tr("built-in gate potential")
    +" ("+QObject::tr("V")+")"));
  Props.push_back(qucs::Property("Bv", "60", false,
    QObject::tr("gate-drain junction reverse bias breakdown voltage")
    +" ("+QObject::tr("V")+")"));
  Props.push_back(qucs::Property("Xti", "3.0", false,
    QObject::tr("diode saturation current temperature coefficient")));
  Props.push_back(qucs::Property("Fc", "0.5", false,
    QObject::tr("forward-bias depletion capacitance coefficient")));
  Props.push_back(qucs::Property("Tau", "1e-9", false,
    QObject::tr("transit time under gate")
    +" ("+QObject::tr("s")+")"));
  Props.push_back(qucs::Property("Rin", "1e-3", false,
    QObject::tr("channel resistance")
    +" ("+QObject::tr("Ohm")+")"));
  Props.push_back(qucs::Property("Area", "1", false,
    QObject::tr("area factor")));
  Props.push_back(qucs::Property("Eg", "1.11", false,
    QObject::tr("energy gap")
    +" ("+QObject::tr("eV")+")"));
  Props.push_back(qucs::Property("M", "0.5", false,
    QObject::tr("grading coefficient")));
  Props.push_back(qucs::Property("Cgd", "0.2e-12", false,
    QObject::tr("zero bias gate-drain junction capacitance")
    +" ("+QObject::tr("F")+")"));
  Props.push_back(qucs::Property("Cgs", "1e-12", false,
    QObject::tr("zero bias gate-source junction capacitance")
    +" ("+QObject::tr("F")+")"));
  Props.push_back(qucs::Property("Cds", "1e-12", false,
    QObject::tr("zero bias drain-source junction capacitance")
    +" ("+QObject::tr("F")+")"));
  Props.push_back(qucs::Property("Betatc", "0", false,
    QObject::tr("Beta temperature coefficient")
    +" ("+QObject::tr("%/Celsius")+")"));
  Props.push_back(qucs::Property("Alphatc", "0", false,
    QObject::tr("Alpha temperature coefficient")
    +" ("+QObject::tr("%/Celsius")+")"));
  Props.push_back(qucs::Property("Gammatc", "0", false,
    QObject::tr("Gamma temperature coefficient")
    +" ("+QObject::tr("%/Celsius")+")"));
  Props.push_back(qucs::Property("Ng", "2.65", false,
    QObject::tr("Subthreshold slope gate parameter")));
  Props.push_back(qucs::Property("Nd", "-0.19", false,
    QObject::tr("subthreshold drain pull parameter")));
  Props.push_back(qucs::Property("ILEVELS", "3", false,
    QObject::tr("gate-source current equation selector")));
  Props.push_back(qucs::Property("ILEVELD", "3", false,
    QObject::tr("gate-drain current equation selector")));
  Props.push_back(qucs::Property("QLEVELS", "2", false,
    QObject::tr("gate-source charge equation selector")));
  Props.push_back(qucs::Property("QLEVELD", "2", false,
    QObject::tr("gate-drain charge equation selector")));
  Props.push_back(qucs::Property("QLEVELDS", "2", false,
    QObject::tr("drain-source charge equation selector")));
  Props.push_back(qucs::Property("Vtotc", "0", false,
    QObject::tr("Vto temperature coefficient")));
  Props.push_back(qucs::Property("Rg", "5.1", false,
    QObject::tr("gate resistance")
    +" ("+QObject::tr("Ohms")+")"));
  Props.push_back(qucs::Property("Rd", "1.3", false,
    QObject::tr("drain resistance")
    +" ("+QObject::tr("Ohms")+")"));
  Props.push_back(qucs::Property("Rs", "1.3", false,
    QObject::tr("source resistance")
    +" ("+QObject::tr("Ohms")+")"));
  Props.push_back(qucs::Property("Rgtc", "0", false,
    QObject::tr("gate resistance temperature coefficient")
    +" ("+QObject::tr("1/Celsius")+")"));
  Props.push_back(qucs::Property("Rdtc", "0", false,
    QObject::tr("drain resistance temperature coefficient")
    +" ("+QObject::tr("1/Celsius")+")"));
  Props.push_back(qucs::Property("Rstc", "0", false,
    QObject::tr("source resistance temperature coefficient")
    +" ("+QObject::tr("1/Celsius")+")"));
  Props.push_back(qucs::Property("Ibv", "1e-3", false,
    QObject::tr("gate reverse breakdown currrent")
    +" ("+QObject::tr("A")+")"));
  Props.push_back(qucs::Property("Rf", "10", false,
    QObject::tr("forward bias slope resistance")
    +" ("+QObject::tr("Ohms")+")"));
  Props.push_back(qucs::Property("R1", "10", false,
    QObject::tr("breakdown slope resistance")
    +" ("+QObject::tr("Ohms")+")"));
  Props.push_back(qucs::Property("Af", "1", false,
    QObject::tr("flicker noise exponent")));
  Props.push_back(qucs::Property("Kf", "0", false,
    QObject::tr("flicker noise coefficient")));
  Props.push_back(qucs::Property("Gdsnoi", "1", false,
    QObject::tr("shot noise coefficient")));
  Props.push_back(qucs::Property("Tnom", "26.85", false,
    QObject::tr("parameter measurement temperature")
    +" ("+QObject::tr("Celsius")+")"));
  Props.push_back(qucs::Property("Temp", "26.85", false,
    QObject::tr("simulation temperature")));

  createSymbol ();
  set_qucs_text_position(x2()+4, y1()+4);
  Model = "MESFET";
  Name  = "T";
}

Component * MESFET::newOne()
{
  MESFET * p = new MESFET();
  p->Props.front().Value = Props.front().Value; 
  p->recreate(0); 
  return p;
}

Element * MESFET::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("MESFET");
  BitmapFile = (char *) "MESFET";

  if(getNewOne) return new MESFET();
  return 0;
}

void MESFET::createSymbol()
{
  // put in here symbol drawing code and terminal definitions
  Lines.push_back(qucs::Line(-10,-15,-10, 15,QPen(Qt::darkBlue,3)));
  Lines.push_back(qucs::Line(-30,  0,-10,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10,-10,  0,-10,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0,-10,  0,-30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10, 10,  0, 10,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0, 10,  0, 30,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line( -4, 24,  4, 20,QPen(Qt::darkBlue,2)));

  Ports.push_back(qucs::Port(  0,-30));
  Ports.push_back(qucs::Port(-30,  0));
  Ports.push_back(qucs::Port(  0, 30));

  set_x1(-30);
  set_y1(-30);
  set_x2(4);
  set_y2(30);
}
