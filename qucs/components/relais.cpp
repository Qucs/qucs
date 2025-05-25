/***************************************************************************
                                relais.cpp
                               ------------
    begin                : Sat Feb 25 2006
    copyright            : (C) 2006 by Michael Margraf
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

#include "relais.h"


Relais::Relais()
{
  Description = QObject::tr("relay");

  Lines.push_back(qucs::Line(-30,-30,-30, -8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30,  8,-30, 30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-45, -8,-15, -8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-45,  8,-15,  8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-45, -8,-45,  8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-15, -8,-15,  8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-45,  8,-15, -8,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-43, -3,-37, -3,QPen(Qt::red,1)));
  Lines.push_back(qucs::Line(-40, -6,-40,  0,QPen(Qt::red,1)));
  Lines.push_back(qucs::Line(-23,  3,-17,  3,QPen(Qt::black,1)));  

  Lines.push_back(qucs::Line(-15,  0, 35,  0,QPen(Qt::darkBlue,1,Qt::DotLine)));

  Lines.push_back(qucs::Line( 30,-30, 30,-18,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30, 15, 30, 30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30, 15, 45,-15,QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc( 27,-18, 5, 5, 0, 16*360,QPen(Qt::darkBlue,2)));
  Ellips.push_back(qucs::Area( 27, 12, 6, 6, QPen(Qt::darkBlue,2),
                         QBrush(Qt::darkBlue, Qt::SolidPattern)));

  Ports.push_back(qucs::Port(-30,-30));
  Ports.push_back(qucs::Port( 30,-30));
  Ports.push_back(qucs::Port( 30, 30));
  Ports.push_back(qucs::Port(-30, 30));

  set_x1(-48);
  set_y1(-30);
  set_x2(45);
  set_y2(30);

  set_qucs_text_position(x2()+4, y1()+4);
  Model = "Relais";
  Name  = "S";

  Props.push_back(qucs::Property("Vt", "0.5 V", false,
		QObject::tr("threshold voltage in Volts")));
  Props.push_back(qucs::Property("Vh", "0.1 V", false,
		QObject::tr("hysteresis voltage in Volts")));
  Props.push_back(qucs::Property("Ron", "0", false,
		QObject::tr("resistance of \"on\" state in Ohms")));
  Props.push_back(qucs::Property("Roff", "1e12", false,
		QObject::tr("resistance of \"off\" state in Ohms")));
  Props.push_back(qucs::Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));
}

Relais::~Relais()
{
}

Component* Relais::newOne()
{
  return new Relais();
}

Element* Relais::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Relay");
  BitmapFile = (char *) "relais";

  if(getNewOne)  return new Relais();
  return 0;
}
