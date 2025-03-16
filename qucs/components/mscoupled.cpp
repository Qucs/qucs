/***************************************************************************
                          mscoupled.cpp  -  description
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

#include "mscoupled.h"


MScoupled::MScoupled()
{
  Description = QObject::tr("coupled microstrip line");

  Lines.push_back(qucs::Line(-30,-12,-16,-12,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30,-30,-30,-12,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 20,-12, 30,-12,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30,-30, 30,-12,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-11,-20, 25,-20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-21, -4, 15, -4,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-11,-20,-21, -4,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 25,-20, 15, -4,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-30, 12,-20, 12,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30, 30,-30, 12,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 16, 12, 30, 12,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30, 30, 30, 12,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-15,  4, 21,  4,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-25, 20, 11, 20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-15,  4,-25, 20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 21,  4, 11, 20,QPen(Qt::darkBlue,2)));

  Ports.push_back(qucs::Port(-30,-30));
  Ports.push_back(qucs::Port( 30,-30));
  Ports.push_back(qucs::Port( 30, 30));
  Ports.push_back(qucs::Port(-30, 30));

  x1 = -30; y1 =-33;
  x2 =  30; y2 = 33;

  set_qucs_text_position(x2+4, y1+4);
  Model = "MCOUPLED";
  Name  = "MS";

  Props.push_back(qucs::Property("Subst", "Subst1", true,
	QObject::tr("name of substrate definition")));
  Props.push_back(qucs::Property("W", "1 mm", true,
	QObject::tr("width of the line")));
  Props.push_back(qucs::Property("L", "10 mm", true,
	QObject::tr("length of the line")));
  Props.push_back(qucs::Property("S", "1 mm", true,
	QObject::tr("spacing between the lines")));
  Props.push_back(qucs::Property("Model", "Kirschning", false,
	QObject::tr("microstrip model")+" [Kirschning, Hammerstad]"));
  Props.push_back(qucs::Property("DispModel", "Kirschning", false,
	QObject::tr("microstrip dispersion model")+
	" [Kirschning, Getsinger]"));
  Props.push_back(qucs::Property("Temp", "26.85", false,
	QObject::tr("simulation temperature in degree Celsius")));
}

MScoupled::~MScoupled()
{
}

Component* MScoupled::newOne()
{
  return new MScoupled();
}

Element* MScoupled::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Coupled Microstrip Line");
  BitmapFile = (char *) "mscoupled";

  if(getNewOne)  return new MScoupled();
  return 0;
}
