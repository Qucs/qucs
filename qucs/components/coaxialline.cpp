/***************************************************************************
                              coaxialline.cpp
                             -----------------
    begin                : Sun Jan 29 2006
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

#include "coaxialline.h"


CoaxialLine::CoaxialLine()
{
  Description = QObject::tr("coaxial transmission line");

  Arcs.append(new Arc(-20, -9, 9, 19,     0, 16*360,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( 11, -9, 9, 19,16*270, 16*180,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-30,  0,-16,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-16, -9, 16, -9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-16,  9, 16,  9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-11,  0, 18,  0,QPen(QPen::darkBlue,1,Qt::DotLine)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));

  x1 = -30; y1 =-12;
  x2 =  30; y2 = 12;

  tx = x1+4;
  ty = y2+4;
  Model = "COAX";
  Name  = "Line";

  Props.append(new Property("er", "1", true,
		QObject::tr("relative permittivity of dielectrica")));
  Props.append(new Property("rho", "0.022e-6", false,
		QObject::tr("specific resistance of conductor")));
  Props.append(new Property("mu", "1", false,
		QObject::tr("relative permeability of conductor")));
  Props.append(new Property("D", "5 mm", false,
		QObject::tr("inner diameter of shield")));
  Props.append(new Property("d", "1 mm", false,
		QObject::tr("diameter of inner conductor")));
  Props.append(new Property("L", "1 m", true,
		QObject::tr("mechanical length of the line")));
  Props.append(new Property("tand", "2e-4", false,
		QObject::tr("loss tangent")));
  Props.append(new Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));
}

CoaxialLine::~CoaxialLine()
{
}

Component* CoaxialLine::newOne()
{
  return new CoaxialLine();
}

Element* CoaxialLine::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Coaxial Line");
  BitmapFile = "coaxial";

  if(getNewOne)  return new CoaxialLine();
  return 0;
}
