/***************************************************************************
                          msgap.cpp  -  description
                             -------------------
    begin                : Sat Jul 17 2004
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

#include "msgap.h"


MSgap::MSgap()
{
  Description = QObject::tr("microstrip gap");

  Lines.push_back(qucs::Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-13, -8,  0, -8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 10, -8, 23, -8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-23,  8,-10,  8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0,  8, 13,  8,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-13, -8,-23,  8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 23, -8, 13,  8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0, -8,-10,  8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 10, -8,  0,  8,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-22, -4,-26,  4,QPen(Qt::darkBlue,2)));

  Ports.push_back(qucs::Port(-30, 0));
  Ports.push_back(qucs::Port( 30, 0));

  set_x1(-30);
  set_y1(-11);
  set_x2(30);
  set_y2(11);

  set_qucs_text_position(x1()+4, y2()+4);
  Model = "MGAP";
  Name  = "MS";

  Props.push_back(qucs::Property("Subst", "Subst1", true,
	QObject::tr("name of substrate definition")));
  Props.push_back(qucs::Property("W1", "1 mm", true,
	QObject::tr("width of the line 1")));
  Props.push_back(qucs::Property("W2", "1 mm", true,
	QObject::tr("width of the line 2")));
  Props.push_back(qucs::Property("S", "1 mm", true,
	QObject::tr("spacing between the microstrip ends")));
  Props.push_back(qucs::Property("MSModel", "Hammerstad", false,
	QObject::tr("quasi-static microstrip model")+
	" [Hammerstad, Wheeler, Schneider]"));
  Props.push_back(qucs::Property("MSDispModel", "Kirschning", false,
	QObject::tr("microstrip dispersion model")+" [Kirschning, Kobayashi, "
	"Yamashita, Hammerstad, Getsinger, Schneider, Pramanick]"));
}

MSgap::~MSgap()
{
}

Component* MSgap::newOne()
{
  return new MSgap();
}

Element* MSgap::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Microstrip Gap");
  BitmapFile = (char *) "msgap";

  if(getNewOne)  return new MSgap();
  return 0;
}
