/*
 * potentiometer.cpp - device implementations for potentiometer module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include "potentiometer.h"

potentiometer::potentiometer()
{
  Description = QObject::tr ("Potentiometer verilog device");

  Props.append (new Property ("R_pot", "1e4", false,
    QObject::tr ("nominal device resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.append (new Property ("Rotation", "120", false,
    QObject::tr ("shaft/wiper arm rotation")
    +" ("+QObject::tr ("degrees")+")"));
  Props.append (new Property ("Taper_Coeff", "0", false,
    QObject::tr ("resistive law taper coefficient")));
  Props.append (new Property ("LEVEL", "1", false,
    QObject::tr ("device type selector")+" [1, 2, 3]"));
  Props.append (new Property ("Max_Rotation", "240.0", false,
    QObject::tr ("maximum shaft/wiper rotation")
    +" ("+QObject::tr ("degrees")+")"));
  Props.append (new Property ("Conformity", "0.2", false,
    QObject::tr ("conformity error")
    +" ("+QObject::tr ("%")+")"));
  Props.append (new Property ("Linearity", "0.2", false,
    QObject::tr ("linearity error")
    +" ("+QObject::tr ("%")+")"));
  Props.append (new Property ("Contact_Res", "1", false,
    QObject::tr ("wiper arm contact resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.append (new Property ("Temp_Coeff", "100", false,
    QObject::tr ("resistance temperature coefficient")
    +" ("+QObject::tr ("PPM/Celsius")+")"));
  Props.append (new Property ("Tnom", "26.85", false,
    QObject::tr ("parameter measurement temperature")
    +" ("+QObject::tr ("Celsius")+")"));
  Props.append (new Property ("Temp", "26.85", false,
    QObject::tr ("simulation temperature")));

  createSymbol ();
  tx = x1 + 8;
  ty = y2 + 4;
  Model = "potentiometer";
  Name  = "POT";
}

Component * potentiometer::newOne()
{
  potentiometer * p = new potentiometer();
  p->Props.getFirst()->Value = Props.getFirst()->Value; 
  p->recreate(0); 
  return p;
}

Element * potentiometer::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Potentiometer");
  BitmapFile = (char *) "potentiometer";

  if(getNewOne) return new potentiometer();
  return 0;
}

void potentiometer::createSymbol()
{
  // frame
  Lines.append(new Line(-30,-13,-30, 10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30, 10, 30, 10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30, 10, 30,-13,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,-13,-30,-13,QPen(Qt::darkBlue,2)));

  // resistor
  Lines.append(new Line(-40,  0, -25, 0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-25,  0, -20,-5,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20, -5, -15, 0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-15,  0, -10,-5,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10, -5, -5,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -5,  0,  0, -5,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, -5,  5,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  5,  0, 10, -5,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 10, -5, 15,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 15,  0, 20, -5,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 20, -5, 25,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 25,  0, 40,  0,QPen(Qt::darkBlue,2)));

  // arrow
  Lines.append(new Line( -4, -9,  0, -5,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  4, -9,  0, -5,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, -5,  0,-20,QPen(Qt::darkBlue,2)));

  Texts.append(new Text(-23,   0, QObject::tr("B"), Qt::black, 6.0, 1.0, 0.0));
  Texts.append(new Text( 18,   0, QObject::tr("T"), Qt::black, 6.0, 1.0, 0.0));

  Ports.append(new ComponentPort(-40,   0)); // B
  Ports.append(new ComponentPort(  0, -20)); // M
  Ports.append(new ComponentPort( 40,   0)); // T

  x1 = -40; y1 = -20;
  x2 =  40; y2 =  15;
}
