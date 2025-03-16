/***************************************************************************
                        amplifier.cpp  -  description
                             -------------------
    begin                : Sat Oct 30 2004
    copyright            : (C) 2004 by Michael Margraf
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

#include "amplifier.h"


Amplifier::Amplifier()
{
  Description = QObject::tr("ideal amplifier");

  Lines.push_back(qucs::Line(-16,-20,-16, 20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-16,-20, 16,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-16, 20, 16,  0,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-30,  0,-16,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 16,  0, 30,  0,QPen(Qt::darkBlue,2)));

  Ports.push_back(qucs::Port(-30,  0));
  Ports.push_back(qucs::Port( 30,  0));

  x1 = -30; y1 = -23;
  x2 =  30; y2 =  23;

  set_qucs_text_position(x1+4, y2+4);
  Model = "Amp";
  Name  = "X";

  Props.push_back(qucs::Property("G", "10", true,
		QObject::tr("voltage gain")));
  Props.push_back(qucs::Property("Z1", "50 Ohm", false,
		QObject::tr("reference impedance of input port")));
  Props.push_back(qucs::Property("Z2", "50 Ohm", false,
		QObject::tr("reference impedance of output port")));
  Props.push_back(qucs::Property("NF", "0 dB", false,
		QObject::tr("noise figure")));
}

Amplifier::~Amplifier()
{
}

Component* Amplifier::newOne()
{
  return new Amplifier();
}

Element* Amplifier::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Amplifier");
  BitmapFile = (char *) "amplifier";

  if(getNewOne)  return new Amplifier();
  return 0;
}
