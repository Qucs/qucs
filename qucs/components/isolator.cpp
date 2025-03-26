/***************************************************************************
                          isolator.cpp  -  description
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

#include "isolator.h"


Isolator::Isolator()
{
  Description = QObject::tr("isolator");

  Lines.push_back(qucs::Line( -8,  0,  8,  0,QPen(Qt::darkBlue,3)));
  Lines.push_back(qucs::Line(  8,  0,  0, -5,QPen(Qt::darkBlue,3)));
  Lines.push_back(qucs::Line(  8,  0,  0,  5,QPen(Qt::darkBlue,3)));

  Lines.push_back(qucs::Line(-14,-14, 14,-14,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-14, 14, 14, 14,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-14,-14,-14, 14,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 14,-14, 14, 14,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-30,  0,-14,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 14,  0, 30,  0,QPen(Qt::darkBlue,2)));

  Ports.push_back(qucs::Port(-30,  0));
  Ports.push_back(qucs::Port( 30,  0));

  x1 = -30; y1 = -17;
  x2 =  30; y2 =  17;

  set_qucs_text_position(x1+4, y2+4);
  Model = "Isolator";
  Name  = "X";

  Props.push_back(qucs::Property("Z1", "50 Ohm", false,
		QObject::tr("reference impedance of input port")));
  Props.push_back(qucs::Property("Z2", "50 Ohm", false,
		QObject::tr("reference impedance of output port")));
  Props.push_back(qucs::Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));
}

Isolator::~Isolator()
{
}

Component* Isolator::newOne()
{
  return new Isolator();
}

Element* Isolator::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Isolator");
  BitmapFile = (char *) "isolator";

  if(getNewOne)  return new Isolator();
  return 0;
}
