/***************************************************************************
                          phaseshifter.cpp  -  description
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

#include "phaseshifter.h"


Phaseshifter::Phaseshifter()
{
  Description = QObject::tr("phase shifter");

  Lines.push_back(qucs::Line(-14,-14, 14,-14,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-14, 14, 14, 14,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-14,-14,-14, 14,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 14,-14, 14, 14,QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc( -9, -9, 17, 17, 0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10, 10, 10,-10,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-30,  0,-14,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 14,  0, 30,  0,QPen(Qt::darkBlue,2)));

  Ports.push_back(qucs::Port(-30,  0));
  Ports.push_back(qucs::Port( 30,  0));

  x1 = -30; y1 = -17;
  x2 =  30; y2 =  17;

  set_qucs_text_position(x1+4, y2+4);
  Model = "PShift";
  Name  = "X";

  Props.push_back(qucs::Property("phi", "90", true,
		QObject::tr("phase shift in degree")));
  Props.push_back(qucs::Property("Zref", "50 Ohm", false,
		QObject::tr("reference impedance")));
}

Phaseshifter::~Phaseshifter()
{
}

Component* Phaseshifter::newOne()
{
  return new Phaseshifter();
}

Element* Phaseshifter::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Phase Shifter");
  BitmapFile = (char *) "pshifter";

  if(getNewOne)  return new Phaseshifter();
  return 0;
}
