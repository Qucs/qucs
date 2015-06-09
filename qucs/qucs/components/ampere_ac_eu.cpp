/***************************************************************************
                               ampere_ac_eu.cpp
                             -------------------
    begin                : Wed 03 june 2015
    copyright            : (C) 2015 Xavier Galtier
    email                : none
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ampere_ac_eu.h"


Ampere_ac_EU::Ampere_ac_EU()
{
  Description = QObject::tr("ideal ac current source");

  Arcs.append(new Arc(-12,-12, 24, 24,  0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -7,  -17,  7,  -17,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  -17,  0, -21,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  -17,  0,  -13,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  0,  -12,  0,  12,QPen(Qt::darkBlue,2)));
  Arcs.append(new Arc( 12,  5,  6,  6,16*270, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.append(new Arc( 12, 11,  6,  6, 16*90, 16*180,QPen(Qt::darkBlue,2)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  16;

  tx = x1+4;
  ty = y2+4;
  Model = "Iac";
  Name  = "I";

  Props.append(new Property("I", "1 mA", true,
		QObject::tr("peak current in Ampere")));
  Props.append(new Property("f", "1 GHz", false,
		QObject::tr("frequency in Hertz")));
  Props.append(new Property("Phase", "0", false,
		QObject::tr("initial phase in degrees")));
  Props.append(new Property("Theta", "0", false,
		QObject::tr("damping factor (transient simulation only)")));

  rotate();  // fix historical flaw
}

Ampere_ac_EU::~Ampere_ac_EU()
{
}

Component* Ampere_ac_EU::newOne()
{
  return new Ampere_ac_EU();
}

Element* Ampere_ac_EU::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("ac Current Source");
  BitmapFile = (char *) "ac_current_eu";

  if(getNewOne)  return new Ampere_ac_EU();
  return 0;
}
