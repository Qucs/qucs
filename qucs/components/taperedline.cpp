/*
* taperedline.cpp - Exponentially tapered line implementation
*
* copyright (C) 2015 Andres Martinez-Mera <andresmartinezmera@gmail.com>
*
* This is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2, or (at your option)
* any later version.
*
* This software is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this package; see the file COPYING.  If not, write to
* the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
* Boston, MA 02110-1301, USA.
*
*
*/
#include "taperedline.h"


taperedline::taperedline()
{
  Description = QObject::tr("Exponential Tapered line");

  //Lines connection device and ports
  Lines.push_back(qucs::Line(-30,  0,-17,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-17,  -6,-17, 6,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(18,  -12,18, 12,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-17,  6, 18, 12,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-17,  -6,18, -12,QPen(Qt::darkBlue,2)));


  Ports.push_back(qucs::Port(-30, 0));
  Ports.push_back(qucs::Port( 30, 0));

  x1 = -30; y1 =-16;
  x2 =  30; y2 = 14;

  set_qucs_text_position(x1+4, y2+4);
  Model = "TAPEREDLINE";
  Name  = "Line";

  Props.push_back(qucs::Property("Z1", "50 Ohm", true,
		QObject::tr("Characteristic impedance at port 1")));
  Props.push_back(qucs::Property("Z2", "100 Ohm", true,
		QObject::tr("Characteristic impedance at port 2")));
  Props.push_back(qucs::Property("L", "75 mm", true,
		QObject::tr("Line length")));
  Props.push_back(qucs::Property("Weighting", "Exponential", true,
		QObject::tr("Taper weighting")+
		" [Exponential, Linear, Triangular, Klopfenstein]"));
  Props.push_back(qucs::Property("Gamma_max", "0.1", false,
		QObject::tr("Maximum ripple (Klopfenstein taper only) ")));
  Props.push_back(qucs::Property("Alpha", "0 dB", false,
		QObject::tr("attenuation factor per length in 1/m")));
  Props.push_back(qucs::Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));
}
taperedline::~taperedline()
{
}

Component* taperedline::newOne()
{
  return new taperedline();
}

Element* taperedline::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Tapered line");
  BitmapFile = (char *) "taperedline";

  if(getNewOne)  return new taperedline();
  return 0;
}
