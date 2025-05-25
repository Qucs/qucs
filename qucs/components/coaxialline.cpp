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

  Arcs.push_back(qucs::Arc(-20, -9, 8, 18,     0, 16*360,QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc( 11, -9, 8, 18,16*270, 16*180,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-30,  0,-16,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 19,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-16, -9, 16, -9,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-16,  9, 16,  9,QPen(Qt::darkBlue,2)));

  Ports.push_back(qucs::Port(-30, 0));
  Ports.push_back(qucs::Port( 30, 0));

  set_x1(-30);
  set_y1(-12);
  set_x2(30);
  set_y2(12);

  set_qucs_text_position(x1()+4, y2()+4);
  Model = "COAX";
  Name  = "Line";

  Props.push_back(qucs::Property("er", "2.29", true,
		QObject::tr("relative permittivity of dielectric")));
  Props.push_back(qucs::Property("rho", "0.022e-6", false,
		QObject::tr("specific resistance of conductor")));
  Props.push_back(qucs::Property("mur", "1", false,
		QObject::tr("relative permeability of conductor")));
  Props.push_back(qucs::Property("D", "2.95 mm", false,
		QObject::tr("inner diameter of shield")));
  Props.push_back(qucs::Property("d", "0.9 mm", false,
		QObject::tr("diameter of inner conductor")));
  Props.push_back(qucs::Property("L", "1500 mm", true,
		QObject::tr("mechanical length of the line")));
  Props.push_back(qucs::Property("tand", "4e-4", false,
		QObject::tr("loss tangent")));
  Props.push_back(qucs::Property("Temp", "26.85", false,
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
  BitmapFile = (char *) "coaxial";

  if(getNewOne)  return new CoaxialLine();
  return 0;
}
