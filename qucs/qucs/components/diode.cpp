/***************************************************************************
                          diode.cpp  -  description
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
  Sign  = "Diode";
  Model = "Diode";
  Name  = "D";

  Props.append(new Property("Is", "1e-15 A", true, QObject::tr("saturation current")));
}

Diode::~Diode()
{
}

Diode* Diode::newOne()
{
  return new Diode();
}
