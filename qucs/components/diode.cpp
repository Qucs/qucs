/***************************************************************************
                          diode.cpp  -  description
                             -------------------
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

#include "diode.h"


Diode::Diode()
{
  Description = QObject::tr("diode");

  Lines.append(new Line(-30,  0, 30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -6, -9, -6,  9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  6, -9,  6,  9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -6,  0,  6, -9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -6,  0,  6,  9,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));

  x1 = -30; y1 = -9;
  x2 =  30; y2 =  9;

  tx = x1+4;
  ty = y2+4;
  Model = "Diode";
  Name  = "D";

  Props.append(new Property("Is", "1e-15 A", true,
		QObject::tr("saturation current")));
  Props.append(new Property("N", "1", true,
		QObject::tr("emission coefficient")));
  Props.append(new Property("Cj0", "10 fF", true,
		QObject::tr("zero-bias junction capacitance")));
  Props.append(new Property("M", "0.5", true,
		QObject::tr("grading coefficient")));
  Props.append(new Property("Vj", "0.7 V", true,
		QObject::tr("junction potential")));
  Props.append(new Property("Rs", "0.0 Ohm", false,
		QObject::tr("ohmic series resistance")));
  Props.append(new Property("Tt", "0.0 ps", false,
		QObject::tr("transit time")));
}

Diode::~Diode()
{
}

Component* Diode::newOne()
{
  return new Diode();
}

Component* Diode::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Diode");
  BitmapFile = "diode";

  if(getNewOne)  return new Diode();
  return 0;
}
