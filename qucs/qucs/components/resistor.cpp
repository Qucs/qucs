/***************************************************************************
                          resistor.cpp  -  description
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

#include "resistor.h"


Resistor::Resistor()
{
  Description = "resistor";

  Lines.append(new Line(-19, -9, 19, -9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18, -9, 18,  9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 19,  9,-19,  9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18,  9,-18, -9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  0,-18,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -11;
  x2 =  30; y2 =  11;

  tx = x1+4;
  ty = y2+4;
  Sign  = "R";
  Model = "R";
  Name  = "R";

  Props.append(new Property("R", "50 Ohm", true, "ohmic resistance in Ohms"));
}

Resistor::~Resistor()
{
}

Resistor* Resistor::newOne()
{
  return new Resistor();
}
