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

  Props.push_back(qucs::Property ("R_pot", "1e4", false,
    QObject::tr("nominal device resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.push_back(qucs::Property ("Rotation", "120", false,
    QObject::tr("shaft/wiper arm rotation")
    +" ("+QObject::tr ("degrees")+")"));
  Props.push_back(qucs::Property ("Taper_Coeff", "0", false,
    QObject::tr("resistive law taper coefficient")));
  Props.push_back(qucs::Property ("LEVEL", "1", false,
    QObject::tr("device type selector")+" [1, 2, 3]"));
  Props.push_back(qucs::Property ("Max_Rotation", "240.0", false,
    QObject::tr("maximum shaft/wiper rotation")
    +" ("+QObject::tr ("degrees")+")"));
  Props.push_back(qucs::Property ("Conformity", "0.2", false,
    QObject::tr("conformity error")
    +" ("+QObject::tr ("%")+")"));
  Props.push_back(qucs::Property ("Linearity", "0.2", false,
    QObject::tr("linearity error")
    +" ("+QObject::tr ("%")+")"));
  Props.push_back(qucs::Property ("Contact_Res", "1", false,
    QObject::tr("wiper arm contact resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.push_back(qucs::Property ("Temp_Coeff", "100", false,
    QObject::tr("resistance temperature coefficient")
    +" ("+QObject::tr ("PPM/Celsius")+")"));
  Props.push_back(qucs::Property ("Tnom", "26.85", false,
    QObject::tr("parameter measurement temperature")
    +" ("+QObject::tr ("Celsius")+")"));
  Props.push_back(qucs::Property ("Temp", "26.85", false,
    QObject::tr ("simulation temperature")));

  createSymbol ();
  set_qucs_text_position(x1+8, y2+4);
  Model = "potentiometer";
  Name  = "POT";
}

Component * potentiometer::newOne()
{
  potentiometer * p = new potentiometer();
  p->Props.front().Value = Props.front().Value; 
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
  Lines.push_back(qucs::Line(-30,-13,-30, 10,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30, 10, 30, 10,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30, 10, 30,-13,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30,-13,-30,-13,QPen(Qt::darkBlue,2)));

  // resistor
  Lines.push_back(qucs::Line(-40,  0, -25, 0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-25,  0, -20,-5,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-20, -5, -15, 0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-15,  0, -10,-5,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10, -5, -5,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -5,  0,  0, -5,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0, -5,  5,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  5,  0, 10, -5,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 10, -5, 15,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 15,  0, 20, -5,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 20, -5, 25,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 25,  0, 40,  0,QPen(Qt::darkBlue,2)));

  // arrow
  Lines.push_back(qucs::Line( -4, -9,  0, -5,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  4, -9,  0, -5,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0, -5,  0,-20,QPen(Qt::darkBlue,2)));

  Texts.push_back(qucs::Text(-23,   0, QObject::tr("B"), Qt::black, 6.0, 1.0, 0.0));
  Texts.push_back(qucs::Text( 18,   0, QObject::tr("T"), Qt::black, 6.0, 1.0, 0.0));

  Ports.push_back(qucs::Port(-40,   0)); // B
  Ports.push_back(qucs::Port(  0, -20)); // M
  Ports.push_back(qucs::Port( 40,   0)); // T

  x1 = -40; y1 = -20;
  x2 =  40; y2 =  15;
}
