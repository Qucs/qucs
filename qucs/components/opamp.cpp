/***************************************************************************
                          opamp.cpp  -  description
                             -------------------
    begin                : Sun Oct 31 2004
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

#include "opamp.h"


OpAmp::OpAmp()
{
  Description = QObject::tr("operational amplifier");

  Lines.push_back(qucs::Line(-30,-20,-20,-20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30, 20,-20, 20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30,  0, 40,  0,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-20,-35,-20, 35,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-20,-35, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-20, 35, 30,  0,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-16, 19, -9, 19,QPen(Qt::black,1)));
  Lines.push_back(qucs::Line(-16,-19, -9,-19,QPen(Qt::red,1)));
  Lines.push_back(qucs::Line(-13,-22,-13,-15,QPen(Qt::red,1)));

  Ports.push_back(qucs::Port(-30, 20));
  Ports.push_back(qucs::Port(-30,-20));
  Ports.push_back(qucs::Port( 40,  0));

  set_x1(-30);
  set_y1(-38);
  set_x2(30);
  set_y2(38);

  set_qucs_text_position(x1()+4, y2()+4);
  Model = "OpAmp";
  Name  = "OP";

  Props.push_back(qucs::Property("G", "1e6", true,
		QObject::tr("voltage gain")));
  Props.push_back(qucs::Property("Umax", "15 V", false,
	QObject::tr("absolute value of maximum and minimum output voltage")));
}

OpAmp::~OpAmp()
{
}

Component* OpAmp::newOne()
{
  return new OpAmp();
}

Element* OpAmp::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("OpAmp");
  BitmapFile = (char *) "opamp";

  if(getNewOne)  return new OpAmp();
  return 0;
}
