/***************************************************************************
                          volt_ac.cpp  -  description
                             -------------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
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

  Arcs.append(new Arc(-12,-12, 25, 25,     0, 16*360,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( -3, -7,  8,  8,16*270, 16*180,QPen(QPen::darkBlue,1)));
  Arcs.append(new Arc( -3,  0,  8,  8, 16*90, 16*180,QPen(QPen::darkBlue,1)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  5, 18, 11,QPen(QPen::red,1)));
  Lines.append(new Line( 21,  8, 15,  8,QPen(QPen::red,1)));
  Lines.append(new Line(-18,  5,-18, 11,QPen(QPen::black,1)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Sign  = "Vac";
  Model = "Vac";
  Name  = "V";

  Props.append(new Property("U", "1 V", true, QObject::tr("rms voltage in Volts")));
  Props.append(new Property("f", "1 GHz", true, QObject::tr("frequency in Hertz")));
}

Volt_ac::~Volt_ac()
{
}

Volt_ac* Volt_ac::newOne()
{
  return new Volt_ac();
}
