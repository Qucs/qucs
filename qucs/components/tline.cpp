/***************************************************************************
                                tline.cpp
                               -----------
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

#include "tline.h"


TLine::TLine()
{
  Description = QObject::tr("ideal transmission line");

  Lines.push_back(qucs::Line(-30,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-28,  7, 28,  7,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-28, 14,-21,  7,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-21, 14,-14,  7,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-14, 14, -7,  7,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -7, 14,  0,  7,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0, 14,  7,  7,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  7, 14, 14,  7,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 14, 14, 21,  7,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 21, 14, 28,  7,QPen(Qt::darkBlue,2)));

  Ports.push_back(qucs::Port(-30, 0));
  Ports.push_back(qucs::Port( 30, 0));

  set_x1(-30);
  set_y1(-4);
  set_x2(30);
  set_y2(16);

  set_qucs_text_position(x2()+4, y1()+4);
  Model = "TLIN";
  Name  = "Line";

  Props.push_back(qucs::Property("Z", "50 Ohm", true,
		QObject::tr("characteristic impedance")));
  Props.push_back(qucs::Property("L", "1 mm", true,
		QObject::tr("electrical length of the line")));
  Props.push_back(qucs::Property("Alpha", "0 dB", false,
		QObject::tr("attenuation factor per length in 1/m")));
  Props.push_back(qucs::Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));
}

TLine::~TLine()
{
}

Component* TLine::newOne()
{
  return new TLine();
}

Element* TLine::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Transmission Line");
  BitmapFile = (char *) "tline";

  if(getNewOne)  return new TLine();
  return 0;
}
