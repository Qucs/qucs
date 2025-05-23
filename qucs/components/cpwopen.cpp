/***************************************************************************
                               cpwopen.cpp
                             ---------------
    begin                : Sun May 01 2005
    copyright            : (C) 2005 by Michael Margraf
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

#include "cpwopen.h"


CPWopen::CPWopen()
{
  Description = QObject::tr("coplanar open");

  Lines.push_back(qucs::Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-13, -8,  0, -8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-23,  8,-10,  8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-13, -8,-23,  8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0, -8,-10,  8,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-25,-13, 11,-13,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-25, 13, -5, 13,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 11,-13, -5, 13,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-24,-21,-16,-13,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-16,-21, -8,-13,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -8,-21,  0,-13,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0,-21,  8,-13,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  8,-21, 15,-14,QPen(Qt::darkBlue,2)));
  
  Lines.push_back(qucs::Line( 10,-11, 15, -6,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  7, -6, 15,  2,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  4, -1, 15, 10,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  1,  4, 15, 18,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-25, 18,-22, 21,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-22, 13,-14, 21,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-14, 13, -6, 21,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -6, 13,  2, 21,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -2,  9, 10, 21,QPen(Qt::darkBlue,2)));

  Ports.push_back(qucs::Port(-30, 0));

  set_x1(-30);
  set_y1(-24);
  set_x2(17);
  set_y2(24);

  set_qucs_text_position(x1()+4, y2()+4);
  Model = "COPEN";
  Name  = "CL";

  Props.push_back(qucs::Property("Subst", "Subst1", true,
		QObject::tr("name of substrate definition")));
  Props.push_back(qucs::Property("W", "1 mm", true,
		QObject::tr("width of the line")));
  Props.push_back(qucs::Property("S", "1 mm", true,
		QObject::tr("width of a gap")));
  Props.push_back(qucs::Property("G", "5 mm", true,
		QObject::tr("width of gap at end of line")));
  Props.push_back(qucs::Property("Backside", "Air", false,
		QObject::tr("material at the backside of the substrate")+
		" [Metal, Air]"));
}

CPWopen::~CPWopen()
{
}

Component* CPWopen::newOne()
{
  return new CPWopen();
}

Element* CPWopen::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Coplanar Open");
  BitmapFile = (char *) "cpwopen";

  if(getNewOne)  return new CPWopen();
  return 0;
}
