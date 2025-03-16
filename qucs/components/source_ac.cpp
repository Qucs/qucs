/***************************************************************************
                               source_ac.cpp
                              ---------------
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

#include "source_ac.h"


Source_ac::Source_ac()
{
  Description = QObject::tr("ac power source");

  Lines.push_back(qucs::Line(-22,-11, 22,-11,QPen(Qt::darkGray,0)));
  Lines.push_back(qucs::Line(-22, 11, 22, 11,QPen(Qt::darkGray,0)));
  Lines.push_back(qucs::Line(-22,-11,-22, 11,QPen(Qt::darkGray,0)));
  Lines.push_back(qucs::Line( 22,-11, 22, 11,QPen(Qt::darkGray,0)));

  Arcs.push_back(qucs::Arc(-19, -9, 18, 18,     0, 16*360,QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(-13, -6,  6,  6,16*270, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(-13,  0,  6,  6, 16*90, 16*180,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30,  0,-19,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30,  0, 19,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -1,  0,  3,  0,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(  3, -5, 19, -5,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  3,  5, 19,  5,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  3, -5,  3,  5,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 19, -5, 19,  5,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line( 25,  5, 25, 11,QPen(Qt::red,1)));
  Lines.push_back(qucs::Line( 28,  8, 22,  8,QPen(Qt::red,1)));
  Lines.push_back(qucs::Line(-25,  5,-25, 11,QPen(Qt::black,1)));

  Ports.push_back(qucs::Port( 30,  0));
  Ports.push_back(qucs::Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  set_qucs_text_position(x1+4, y2+4);
  Model = "Pac";
  Name  = "P";

  // This property must be the first one !
  Props.push_back(qucs::Property("Num", "1", true,
		QObject::tr("number of the port")));
  Props.push_back(qucs::Property("Z", "50 Ohm", true,
		QObject::tr("port impedance")));
  Props.push_back(qucs::Property("P", "0 dBm", false,
		QObject::tr("(available) ac power in Watts")));
  Props.push_back(qucs::Property("f", "1 GHz", false,
		QObject::tr("frequency in Hertz")));
  Props.push_back(qucs::Property("Temp", "26.85", false,
	QObject::tr("simulation temperature in degree Celsius")));

  rotate();  // fix historical flaw
}

Source_ac::~Source_ac()
{
}

Component* Source_ac::newOne()
{
  return new Source_ac();
}

Element* Source_ac::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Power Source");
  BitmapFile = (char *) "source";

  if(getNewOne)  return new Source_ac();
  return 0;
}
