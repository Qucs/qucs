/***************************************************************************
                          volt_dc.cpp  -  description
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

#include "volt_dc.h"


Volt_dc::Volt_dc()
{
  Description = "ideal dc voltage source";

  Lines.append(new Line(  4,-13,  4, 13,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -4, -6, -4,  6,QPen(QPen::darkBlue,4)));
  Lines.append(new Line( 30,  0,  4,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -4,  0,-30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10,  5, 10, 11,QPen(QPen::red,1)));
  Lines.append(new Line( 13,  8,  7,  8,QPen(QPen::red,1)));
  Lines.append(new Line( -9,  5, -9, 11,QPen(QPen::black,1)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Sign  = "Vdc";
  Model = "Vdc";
  Name  = "V";

  Props.append(new Property("U", "1 V", true, "voltage in Volts"));
}

Volt_dc::~Volt_dc()
{
}

Volt_dc* Volt_dc::newOne()
{
  return new Volt_dc();
}
