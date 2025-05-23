/***************************************************************************
                          msline.cpp  -  description
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

#include "msline.h"


MSline::MSline()
{
  Description = QObject::tr("microstrip line");

  Lines.push_back(qucs::Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-13, -8, 23, -8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-23,  8, 13,  8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-13, -8,-23,  8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 23, -8, 13,  8,QPen(Qt::darkBlue,2)));

  Ports.push_back(qucs::Port(-30, 0));
  Ports.push_back(qucs::Port( 30, 0));

  set_x1(-30);
  set_y1(-11);
  set_x2(30);
  set_y2(11);

  set_qucs_text_position(x1()+4, y2()+4);
  Model = "MLIN";
  Name  = "MS";

  Props.push_back(qucs::Property("Subst", "Subst1", true,
	QObject::tr("name of substrate definition")));
  Props.push_back(qucs::Property("W", "1 mm", true,
	QObject::tr("width of the line")));
  Props.push_back(qucs::Property("L", "10 mm", true,
	QObject::tr("length of the line")));
  Props.push_back(qucs::Property("Model", "Hammerstad", false,
	QObject::tr("quasi-static microstrip model")+
		    " [Hammerstad, Wheeler, Schneider]"));
  Props.push_back(qucs::Property("DispModel", "Kirschning", false,
	QObject::tr("microstrip dispersion model")+" [Kirschning, Kobayashi, "
	"Yamashita, Hammerstad, Getsinger, Schneider, Pramanick]"));
  Props.push_back(qucs::Property("Temp", "26.85", false,
	QObject::tr("simulation temperature in degree Celsius")));
}

MSline::~MSline()
{
}

Component* MSline::newOne()
{
  return new MSline();
}

Element* MSline::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Microstrip Line");
  BitmapFile = (char *) "msline";

  if(getNewOne)  return new MSline();
  return 0;
}
