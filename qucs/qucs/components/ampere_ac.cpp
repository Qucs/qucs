/***************************************************************************
                          ampere_ac.cpp  -  description
                             -------------------
    begin                : Sun May 23 2004
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

#include "ampere_ac.h"


Ampere_ac::Ampere_ac()
{
  Description = QObject::tr("ideal ac current source");

  Arcs.append(new Arc(-12,-12, 25, 25,  0, 16*360,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -7,  0,  7,  0,QPen(QPen::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0, -4,QPen(QPen::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0,  4,QPen(QPen::darkBlue,3)));
  Arcs.append(new Arc( 12,  5,  7,  7,16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( 12, 11,  7,  7, 16*90, 16*180,QPen(QPen::darkBlue,2)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  16;

  tx = x1+4;
  ty = y2+4;
  Model = "Iac";
  Name  = "I";

  Props.append(new Property("I", "1 mA", true,
		QObject::tr("current in Ampere")));
  Props.append(new Property("f", "1 GHz", false,
		QObject::tr("frequency in Hertz")));
  Props.append(new Property("Phase", "0", false,
		QObject::tr("initial phase in degrees")));
}

Ampere_ac::~Ampere_ac()
{
}

Component* Ampere_ac::newOne()
{
  return new Ampere_ac();
}

Component* Ampere_ac::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("ac Current Source");
  BitmapFile = "ac_current";

  if(getNewOne)  return new Ampere_ac();
  return 0;
}
