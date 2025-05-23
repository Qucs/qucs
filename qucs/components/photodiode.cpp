/*
 * photodiode.cpp - device implementations for photodiode module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include "photodiode.h"

photodiode::photodiode()
{
  Description = QObject::tr("Photodiode verilog device");

  Props.push_back(qucs::Property("N", "1.35", false,
    QObject::tr("photodiode emission coefficient")));
  Props.push_back(qucs::Property("Rseries", "1e-3", false,
    QObject::tr("series lead resistance")
    +" ("+QObject::tr("Ohm")+")"));
  Props.push_back(qucs::Property("Is", "0.34e-12", false,
    QObject::tr("diode dark current")
    +" ("+QObject::tr("A")+")"));
  Props.push_back(qucs::Property("Bv", "60", false,
    QObject::tr("reverse breakdown voltage")
    +" ("+QObject::tr("V")+")"));
  Props.push_back(qucs::Property("Ibv", "1e-3", false,
    QObject::tr("current at reverse breakdown voltage")
    +" ("+QObject::tr("A")+")"));
  Props.push_back(qucs::Property("Vj", "0.7", false,
    QObject::tr("junction potential")
    +" ("+QObject::tr("V")+")"));
  Props.push_back(qucs::Property("Cj0", "60e-12", false,
    QObject::tr("zero-bias junction capacitance")
    +" ("+QObject::tr("F")+")"));
  Props.push_back(qucs::Property("M", "0.5", false,
    QObject::tr("grading coefficient")));
  Props.push_back(qucs::Property("Area", "1.0", false,
    QObject::tr("diode relative area")));
  Props.push_back(qucs::Property("Tnom", "26.85", false,
    QObject::tr("parameter measurement temperature")
    +" ("+QObject::tr("Celsius")+")"));
  Props.push_back(qucs::Property("Fc", "0.5", false,
    QObject::tr("forward-bias depletion capacitance coefficient")));
  Props.push_back(qucs::Property("Tt", "10e-9", false,
    QObject::tr("transit time")
    +" ("+QObject::tr("s")+")"));
  Props.push_back(qucs::Property("Xti", "3.0", false,
    QObject::tr("saturation current temperature exponent")));
  Props.push_back(qucs::Property("Eg", "1.16", false,
    QObject::tr("energy gap")
    +" ("+QObject::tr("eV")+")"));
  Props.push_back(qucs::Property("Responsivity", "0.5", false,
    QObject::tr("responsivity")
    +" ("+QObject::tr("A/W")+")"));
  Props.push_back(qucs::Property("Rsh", "5e8", false,
    QObject::tr("shunt resistance")
    +" ("+QObject::tr("Ohm")+")"));
  Props.push_back(qucs::Property("QEpercent", "80", false,
    QObject::tr("quantum efficiency")
    +" ("+QObject::tr("%")+")"));
  Props.push_back(qucs::Property("Lambda", "900", false,
    QObject::tr("light wavelength")
    +" ("+QObject::tr("nm")+")"));
  Props.push_back(qucs::Property("LEVEL", "1", false,
    QObject::tr("responsivity calculator selector")));
  Props.push_back(qucs::Property("Kf", "1e-12", false,
    QObject::tr("flicker noise coefficient")));
  Props.push_back(qucs::Property("Af", "1.0", false,
    QObject::tr("flicker noise exponent")));
  Props.push_back(qucs::Property("Ffe", "1.0", false,
    QObject::tr("flicker noise frequency exponent")));
  Props.push_back(qucs::Property("Temp", "26.85", false,
    QObject::tr("simulation temperature")));

  createSymbol ();
  set_qucs_text_position(x2()+4, y1()+4);
  Model = "photodiode";
  Name  = "PD";
}

Component * photodiode::newOne()
{
  photodiode * p = new photodiode();
  p->Props.front().Value = Props.front().Value; 
  p->recreate(0); 
  return p;
}

Element * photodiode::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Photodiode");
  BitmapFile = (char *) "photodiode";

  if(getNewOne) return new photodiode();
  return 0;
}

void photodiode::createSymbol()
{
  Arcs.push_back(qucs::Arc(-12,-12, 24, 24, 0, 16*360,QPen(Qt::red,2)));
  Lines.push_back(qucs::Line(-30,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -6, -9, -6,  9,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  6, -9,  6,  9,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -6,  0,  6, -9,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -6,  0,  6,  9,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0, 12,  0,  30,QPen(Qt::green,2)));

  Ports.push_back(qucs::Port( 30, 0));
  Ports.push_back(qucs::Port(-30, 0));
  Ports.push_back(qucs::Port( 0, 30));

  set_x1(-30);
  set_y1(-20);
  set_x2(30);
  set_y2(30);
}
