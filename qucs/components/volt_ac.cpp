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
  Description = QObject::tr("ideal ac voltage source");

  Arcs.push_back(qucs::Arc(-12,-12, 24, 24,     0, 16*360,QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc( -3, -7,  7,  7,16*270, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc( -3,  0,  7,  7, 16*90, 16*180,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 18,  5, 18, 11,QPen(Qt::red,1)));
  Lines.push_back(qucs::Line( 21,  8, 15,  8,QPen(Qt::red,1)));
  Lines.push_back(qucs::Line(-18,  5,-18, 11,QPen(Qt::black,1)));

  Ports.push_back(qucs::Port( 30,  0));
  Ports.push_back(qucs::Port(-30,  0));

  set_x1(-30);
  set_y1(-14);
  set_x2(30);
  set_y2(14);

  set_qucs_text_position(x1()+4, y2()+4);
  Model = "Vac";
  Name  = "V";

  Props.push_back(qucs::Property("U", "1 V", true,
		QObject::tr("peak voltage in Volts")));
  Props.push_back(qucs::Property("f", "1 GHz", false,
		QObject::tr("frequency in Hertz")));
  Props.push_back(qucs::Property("Phase", "0", false,
		QObject::tr("initial phase in degrees")));
  Props.push_back(qucs::Property("Theta", "0", false,
		QObject::tr("damping factor (transient simulation only)")));

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
  Name = QObject::tr("ac Voltage Source");
  BitmapFile = (char *) "ac_voltage";

  if(getNewOne)  return new Volt_ac();
  return 0;
}
