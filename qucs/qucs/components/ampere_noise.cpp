/***************************************************************************
                          ampere_noise.cpp  -  description
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

#include "ampere_noise.h"


Ampere_noise::Ampere_noise()
{
  Description = QObject::tr("noise current source");

  Arcs.append(new Arc(-12,-12, 25, 25,  0, 16*360,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -7,  0,  7,  0,QPen(QPen::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0, -4,QPen(QPen::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0,  4,QPen(QPen::darkBlue,3)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Sign  = "Inoise";
  Model = "Inoise";
  Name  = "I";

  Props.append(new Property("i", "1e-6", true, QObject::tr("equivalent current density in A/sqrt(Hz)")));
  Props.append(new Property("e", "0", true, QObject::tr("frequency exponent")));
  Props.append(new Property("c", "1", true, QObject::tr("frequency coefficient")));
  Props.append(new Property("a", "0", true, QObject::tr("additive frequency term")));
}

Ampere_noise::~Ampere_noise()
{
}

Ampere_noise* Ampere_noise::newOne()
{
  return new Ampere_noise();
}
