/***************************************************************************
                              ampere_noise.cpp
                             ------------------
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

#include "ampere_noise.h"


Ampere_noise::Ampere_noise()
{
  Description = QObject::tr("noise current source");

  Arcs.append(new Arc(-12,-12, 24, 24,  0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -7,  0,  7,  0,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0, -4,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0,  4,QPen(Qt::darkBlue,3)));

  Lines.append(new Line(-12,  1,  1,-12,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10,  6, -7,  3,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  3, -7,  6,-10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -7, 10, -2,  5,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  7, -4, 10, -7,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -1, 12, 12, -1,QPen(Qt::darkBlue,2)));

  Ports.append(new ComponentPort( 30,  0));
  Ports.append(new ComponentPort(-30,  0));

  x1 = -30; y1 = -15;
  x2 =  30; y2 =  15;

  tx = x1+4;
  ty = y2+4;
  Model = "Inoise";
  Name  = "I";

  Props.append(new Property("i", "1e-6", true,
		QObject::tr("current power spectral density in A^2/Hz")));
  Props.append(new Property("e", "0", false,
		QObject::tr("frequency exponent")));
  Props.append(new Property("c", "1", false,
		QObject::tr("frequency coefficient")));
  Props.append(new Property("a", "0", false,
		QObject::tr("additive frequency term")));

  set_rotated(1);  // fix historical flaw
}

Ampere_noise::~Ampere_noise()
{
}

Component* Ampere_noise::newOne()
{
  return new Ampere_noise();
}

Element* Ampere_noise::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Noise Current Source");
  BitmapFile = (char *) "noise_current";

  if(getNewOne)  return new Ampere_noise();
  return 0;
}
