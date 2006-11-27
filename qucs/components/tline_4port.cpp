/***************************************************************************
                              tline_4port.cpp
                             -----------------
    begin                : Nov 12 2006
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

#include "tline_4port.h"


TLine_4Port::TLine_4Port()
{
  Description = QObject::tr("ideal 4-port transmission line");

  Arcs.append(new Arc(-25,-36, 18, 38,16*230, 16*68,QPen(QPen::darkBlue,1)));
  Arcs.append(new Arc(-25, -2, 18, 38, 16*62, 16*68,QPen(QPen::darkBlue,1)));

  Arcs.append(new Arc(-17,-36, 18, 38,16*242, 16*56,QPen(QPen::darkBlue,1)));
  Arcs.append(new Arc(-17, -2, 18, 38, 16*62, 16*56,QPen(QPen::darkBlue,1)));

  Arcs.append(new Arc( -9,-36, 18, 38,16*242, 16*56,QPen(QPen::darkBlue,1)));
  Arcs.append(new Arc( -9, -2, 18, 38, 16*62, 16*56,QPen(QPen::darkBlue,1)));

  Arcs.append(new Arc( -1,-36, 18, 38,16*242, 16*56,QPen(QPen::darkBlue,1)));
  Arcs.append(new Arc( -1, -2, 18, 38, 16*62, 16*56,QPen(QPen::darkBlue,1)));

  Arcs.append(new Arc(  7,-36, 18, 38,16*242, 16*68,QPen(QPen::darkBlue,1)));
  Arcs.append(new Arc(  7, -2, 18, 38, 16*50, 16*68,QPen(QPen::darkBlue,1)));

  Arcs.append(new Arc(-40,-10, 20, 33, 16*32, 16*58,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-40,-23, 20, 33,16*270, 16*58,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( 20,-10, 20, 33, 16*90, 16*58,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( 20,-23, 20, 33,16*212, 16*58,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,-10));
  Ports.append(new Port( 30,-10));
  Ports.append(new Port( 30, 10));
  Ports.append(new Port(-30, 10));

  x1 = -30; y1 =-12;
  x2 =  30; y2 = 12;

  tx = x1+4;
  ty = y2+4;
  Model = "TLIN4P";
  Name  = "Line";

  Props.append(new Property("Z", "50 Ohm", true,
		QObject::tr("characteristic impedance")));
  Props.append(new Property("L", "1 mm", true,
		QObject::tr("electrical length of the line")));
  Props.append(new Property("Alpha", "0 dB", false,
		QObject::tr("attenuation factor per length in 1/m")));
  Props.append(new Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));
}

TLine_4Port::~TLine_4Port()
{
}

Component* TLine_4Port::newOne()
{
  return new TLine_4Port();
}

Element* TLine_4Port::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("4-Port Transmission Line");
  BitmapFile = "tline_4port";

  if(getNewOne)  return new TLine_4Port();
  return 0;
}
