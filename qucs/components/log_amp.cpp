/*
 * log_amp.cpp - device implementations for log_amp module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include "log_amp.h"

log_amp::log_amp()
{
  Description = QObject::tr ("Logarithmic Amplifier verilog device");

  Props.push_back(qucs::Property ("Kv", "1.0", false,
    QObject::tr ("scale factor")));
  Props.push_back(qucs::Property ("Dk", "0.3", false,
    QObject::tr ("scale factor error")
    +" ("+QObject::tr ("%")+")"));
  Props.push_back(qucs::Property ("Ib1", "5e-12", false,
    QObject::tr ("input I1 bias current")
    +" ("+QObject::tr ("A")+")"));
  Props.push_back(qucs::Property ("Ibr", "5e-12", false,
    QObject::tr ("input reference bias current")
    +" ("+QObject::tr ("A")+")"));
  Props.push_back(qucs::Property ("M", "5", false,
    QObject::tr ("number of decades")));
  Props.push_back(qucs::Property ("N", "0.1", false,
    QObject::tr ("conformity error")
    +" ("+QObject::tr ("%")+")"));
  Props.push_back(qucs::Property ("Vosout", "3e-3", false,
    QObject::tr ("output offset error")
    +" ("+QObject::tr ("V")+")"));
  Props.push_back(qucs::Property ("Rinp", "1e6", false,
    QObject::tr ("amplifier input resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.push_back(qucs::Property ("Fc", "1e3", false,
    QObject::tr ("amplifier 3dB frequency")
    +" ("+QObject::tr ("Hz")+")"));
  Props.push_back(qucs::Property ("Ro", "1e-3", false,
    QObject::tr ("amplifier output resistance")
    +" ("+QObject::tr ("Ohm")+")"));
  Props.push_back(qucs::Property ("Ntc", "0.002", false,
    QObject::tr ("conformity error temperature coefficient")
    +" ("+QObject::tr ("%/Celsius")+")"));
  Props.push_back(qucs::Property ("Vosouttc", "80e-6", false,
    QObject::tr ("offset temperature coefficient")
    +" ("+QObject::tr ("V/Celsius")+")"));
  Props.push_back(qucs::Property ("Dktc", "0.03", false,
    QObject::tr ("scale factor error temperature coefficient")
    +" ("+QObject::tr ("%/Celsius")+")"));
  Props.push_back(qucs::Property ("Ib1tc", "0.5e-12", false,
    QObject::tr ("input I1 bias current temperature coefficient")
    +" ("+QObject::tr ("A/Celsius")+")"));
  Props.push_back(qucs::Property ("Ibrtc", "0.5e-12", false,
    QObject::tr ("input reference bias current temperature coefficient")
    +" ("+QObject::tr ("A/Celsius")+")"));
  Props.push_back(qucs::Property ("Tnom", "26.85", false,
    QObject::tr ("parameter measurement temperature")
    +" ("+QObject::tr ("Celsius")+")"));
  Props.push_back(qucs::Property ("Temp", "26.85", false,
    QObject::tr ("simulation temperature")));

  createSymbol ();
  set_qucs_text_position(x2+4, y1+4);
  Model = "log_amp";
  Name  = "LA";
}

Component * log_amp::newOne()
{
  log_amp * p = new log_amp();
  p->Props.front().Value = Props.front().Value; 
  p->recreate(0); 
  return p;
}

Element * log_amp::info(QString& Name, char * &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Logarithmic Amplifier");
  BitmapFile = (char *) "log_amp";

  if(getNewOne) return new log_amp();
  return 0;
}

void log_amp::createSymbol()
{
  Lines.push_back(qucs::Line(-30,-20,-20,-20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30, 20,-20, 20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30,  0, 40,  0,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-20,-35,-20, 35,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-20,-35, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-20, 35, 30,  0,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-5, -10, -5, 10,QPen(Qt::red,2)));
  Lines.push_back(qucs::Line(-5,  10,  5,  10,QPen(Qt::red,2)));

  Texts.push_back(qucs::Text(-17, -26, QObject::tr("I"), Qt::black, 10.0, 1.0, 0.0));
  Texts.push_back(qucs::Text(-17,  14, QObject::tr("R"), Qt::black, 10.0, 1.0, 0.0));
  
  Ports.push_back(qucs::Port(-30,-20));
  Ports.push_back(qucs::Port(-30, 20));
  Ports.push_back(qucs::Port( 40,  0));

  x1 = -30; y1 = -38;
  x2 =  40; y2 =  38;
}
