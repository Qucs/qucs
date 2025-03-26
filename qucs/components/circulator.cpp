/***************************************************************************
                          circulator.cpp  -  description
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
#include "circulator.h"
#include "qucs.h"


Circulator::Circulator()
{
  Description = QObject::tr("circulator");

  Arcs.push_back(qucs::Arc(-14,-14, 28, 28,  0,16*360,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30,  0,-14,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30,  0, 14,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0, 14,  0, 30,QPen(Qt::darkBlue,2)));

  Arcs.push_back(qucs::Arc( -8, -6, 16, 16,16*20,16*150,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  8,  0,  9, -7,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  8,  0,  2, -1,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-22, -4,-26,  4,QPen(Qt::darkBlue,2)));   // marks port 1

  Ports.push_back(qucs::Port(-30,  0));
  Ports.push_back(qucs::Port( 30,  0));
  Ports.push_back(qucs::Port(  0, 30));

  x1 = -30; y1 = -16;
  x2 =  30; y2 =  30;

  set_qucs_text_position(x1+4,
  y1 - QucsSettings.font.pointSize()-4); // yikes.
  Model = "Circulator";
  Name  = "X";

  Props.push_back(qucs::Property("Z1", "50 Ohm", false,
		QObject::tr("reference impedance of port 1")));
  Props.push_back(qucs::Property("Z2", "50 Ohm", false,
		QObject::tr("reference impedance of port 2")));
  Props.push_back(qucs::Property("Z3", "50 Ohm", false,
		QObject::tr("reference impedance of port 3")));
}

Circulator::~Circulator()
{
}

Component* Circulator::newOne()
{
  return new Circulator();
}

Element* Circulator::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Circulator");
  BitmapFile = (char *) "circulator";

  if(getNewOne)  return new Circulator();
  return 0;
}
