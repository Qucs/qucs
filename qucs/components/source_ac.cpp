/***************************************************************************
                          source_ac.cpp  -  description
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

#include "source_ac.h"


Source_ac::Source_ac()
{
  Description = QObject::QObject::tr("ac power source");

  Lines.append(new Line(-22,-11, 22,-11,QPen(QPen::darkGray,1)));
  Lines.append(new Line(-22, 10, 22, 10,QPen(QPen::darkGray,1)));
  Lines.append(new Line(-22,-11,-22, 10,QPen(QPen::darkGray,1)));
  Lines.append(new Line( 22,-11, 22, 10,QPen(QPen::darkGray,1)));

  Arcs.append(new Arc(-20, -9, 18, 18,     0, 16*360,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-14, -6,  6,  6,16*270, 16*180,QPen(QPen::darkBlue,1)));
  Arcs.append(new Arc(-14, -1,  6,  6, 16*90, 16*180,QPen(QPen::darkBlue,1)));
  Lines.append(new Line(-30,  0,-20,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,  0, 19,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -3,  0,  3,  0,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(  3, -5, 19, -5,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  3,  5, 19,  5,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  3, -5,  3,  5,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 19, -5, 19,  5,QPen(QPen::darkBlue,2)));

  Lines.append(new Line( 25,  5, 25, 11,QPen(QPen::red,1)));
  Lines.append(new Line( 28,  8, 22,  8,QPen(QPen::red,1)));
  Lines.append(new Line(-25,  5,-25, 11,QPen(QPen::black,1)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Sign  = "Pac";
  Model = "Pac";
  Name  = "P";

  Props.append(new Property("Num", "1", true, QObject::QObject::tr("number of the port")));
  Props.append(new Property("Z", "50 Ohm", true, QObject::QObject::tr("port impedance")));
  Props.append(new Property("P", "0 dBm", true, QObject::QObject::tr("ac power in Watts")));
  Props.append(new Property("f", "1 GHz", true, QObject::QObject::tr("frequency in Hertz")));
}

Source_ac::~Source_ac()
{
}

Source_ac* Source_ac::newOne()
{
  return new Source_ac();
}
