/***************************************************************************
                               volt_noise.cpp
                              ----------------
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

#include "volt_noise.h"


Volt_noise::Volt_noise()
{
  Description = QObject::tr("noise voltage source");

  Arcs.push_back(qucs::Arc(-12,-12, 24, 24,     0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-12,  1,  1,-12,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10,  6,  6,-10,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -7, 10, 10, -7,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -2, 12, 12, -2,QPen(Qt::darkBlue,2)));

  Ports.push_back(qucs::Port( 30,  0));
  Ports.push_back(qucs::Port(-30,  0));

  x1 = -30; y1 = -15;
  x2 =  30; y2 =  15;

  set_qucs_text_position(x1+4, y2+4);
  Model = "Vnoise";
  Name  = "V";

  Props.push_back(qucs::Property("u", "1e-6", true,
		QObject::tr("voltage power spectral density in V^2/Hz")));
  Props.push_back(qucs::Property("e", "0", false,
		QObject::tr("frequency exponent")));
  Props.push_back(qucs::Property("c", "1", false,
		QObject::tr("frequency coefficient")));
  Props.push_back(qucs::Property("a", "0", false,
		QObject::tr("additive frequency term")));

  rotate();  // fix historical flaw
}

Volt_noise::~Volt_noise()
{
}

Component* Volt_noise::newOne()
{
  return new Volt_noise();
}

Element* Volt_noise::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Noise Voltage Source");
  BitmapFile = (char *) "noise_volt";

  if(getNewOne)  return new Volt_noise();
  return 0;
}
