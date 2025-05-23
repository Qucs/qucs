/***************************************************************************
                          msmbend.cpp  -  description
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

#include "msmbend.h"


MSmbend::MSmbend()
{
  Description = QObject::tr("microstrip mitered bend");

  Lines.push_back(qucs::Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0, 18,  0, 30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-18, -8, -8, -8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -8, -8,  8,  8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-18,  8, -8,  8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-18, -8,-18,  8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -8,  8, -8, 18,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  8,  8,  8, 18,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -8, 18,  8, 18,QPen(Qt::darkBlue,2)));

  Ports.push_back(qucs::Port(-30, 0));
  Ports.push_back(qucs::Port(  0,30));

  set_x1(-30);
  set_y1(-11);
  set_x2(11);
  set_y2(30);

  set_qucs_text_position(x2()+4, y1()+4);
  Model = "MMBEND";
  Name  = "MS";

  Props.push_back(qucs::Property("Subst", "Subst1", true,
		QObject::tr("substrate")));
  Props.push_back(qucs::Property("W", "1 mm", true,
		QObject::tr("width of line")));
}

MSmbend::~MSmbend()
{
}

Component* MSmbend::newOne()
{
  return new MSmbend();
}

Element* MSmbend::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Microstrip Mitered Bend");
  BitmapFile = (char *) "msmbend";

  if(getNewOne)  return new MSmbend();
  return 0;
}
