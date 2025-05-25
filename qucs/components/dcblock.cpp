/***************************************************************************
                          dcblock.cpp  -  description
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

#include "dcblock.h"

dcBlock::dcBlock()
{
  Description = QObject::tr("dc block");

  Lines.push_back(qucs::Line(- 4,-11, -4, 11,QPen(Qt::darkBlue,4)));
  Lines.push_back(qucs::Line(  4,-11,  4, 11,QPen(Qt::darkBlue,4)));
  Lines.push_back(qucs::Line(-30,  0, -4,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  4,  0, 30,  0,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-23,-14, 23,-14,QPen(Qt::darkBlue,1)));
  Lines.push_back(qucs::Line(-23, 14, 23, 14,QPen(Qt::darkBlue,1)));
  Lines.push_back(qucs::Line(-23,-14,-23, 14,QPen(Qt::darkBlue,1)));
  Lines.push_back(qucs::Line( 23,-14, 23, 14,QPen(Qt::darkBlue,1)));

  Ports.push_back(qucs::Port(-30,  0));
  Ports.push_back(qucs::Port( 30,  0));

  set_x1(-30);
  set_y1(-16);
  set_x2(30);
  set_y2(17);

  set_qucs_text_position(x1()+4, y2()+4);
  Model = "DCBlock";
  Name  = "C";

  Props.push_back(qucs::Property("C", "1 uF", false,
	QObject::tr("for transient simulation: capacitance in Farad")));
}

dcBlock::~dcBlock()
{
}

Component* dcBlock::newOne()
{
  return new dcBlock();
}

Element* dcBlock::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("dc Block");
  BitmapFile = (char *) "dcblock";

  if(getNewOne)  return new dcBlock();
  return 0;
}
