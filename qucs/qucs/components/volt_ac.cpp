/***************************************************************************
                               volt_ac.cpp
                              -------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "volt_ac.h"


Volt_ac::Volt_ac()
{
  Description = QObject::tr("Ideal AC voltage source");

  Arcs.append(new Arc(-12,-12, 24, 24,     0, 16*360,QPen(Qt::darkBlue,2)));
  Arcs.append(new Arc( -3, -7,  7,  7,16*270, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.append(new Arc( -3,  0,  7,  7, 16*90, 16*180,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 18,  5, 18, 11,QPen(Qt::red,1)));
  Lines.append(new Line( 21,  8, 15,  8,QPen(Qt::red,1)));
  Lines.append(new Line(-18,  5,-18, 11,QPen(Qt::black,1)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Model = "Vac";
  Name  = "V";

  Props.append(new Property("U", "1 V", true,
		QObject::tr("Peak voltage in volts")));
  Props.append(new Property("f", "1 GHz", false,
		QObject::tr("Frequency in hertz")));
  Props.append(new Property("Phase", "0", false,
		QObject::tr("Initial phase in degrees")));
  Props.append(new Property("Theta", "0", false,
		QObject::tr("Damping factor (transient simulation only)")));

  rotate();  // fix historical flaw
}

Volt_ac::~Volt_ac()
{
}

Component* Volt_ac::newOne()
{
  return new Volt_ac();
}

Element* Volt_ac::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("AC Voltage Source");
  BitmapFile = (char *) "ac_voltage";

  if(getNewOne)  return new Volt_ac();
  return 0;
}
