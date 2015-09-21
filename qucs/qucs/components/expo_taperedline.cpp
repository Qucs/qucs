/*
* expo_taperedline.cpp - Exponentially tapered line implementation
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
#include "expo_taperedline.h"


expo_taperedline::expo_taperedline()
{
  Description = QObject::tr("Exponential Tapered line");

  //Lines connection device and ports
  Lines.append(new Line(-30,  0,-17,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-17,  -2,-17, 2,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(18,  -15,18, 15,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-17,  2,-6, 4,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-17,  -2,-6, -4,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-6,  4,0, 8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-6,  -4,0, -8,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(0,  8, 2, 10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(0,  -8, 2, -10,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(2,  10, 6, 12,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(2,  -10, 6, -12,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(6,  12, 18, 15,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(6,  -12, 18, -15,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));

  x1 = -30; y1 =-16;
  x2 =  30; y2 = 14;

  tx = x1+4;
  ty = y2+4;
  Model = "expo_taperedline";
  Name  = "Line";

  Props.append(new Property("Z", "50 Ohm", true,
		QObject::tr("Characteristic impedance")));
  Props.append(new Property("L", "1 mm", true,
		QObject::tr("Length of the line")));
  Props.append(new Property("a", "0.1", true,
		QObject::tr("Exponential coefficient")));
  Props.append(new Property("Alpha", "0 dB", false,
		QObject::tr("attenuation factor per length in 1/m")));
  Props.append(new Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));
}

expo_taperedline::~expo_taperedline()
{
}

Component* expo_taperedline::newOne()
{
  return new expo_taperedline();
}

Element* expo_taperedline::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Exponential tapered line");
  BitmapFile = (char *) "expo_taperedline";

  if(getNewOne)  return new expo_taperedline();
  return 0;
}
