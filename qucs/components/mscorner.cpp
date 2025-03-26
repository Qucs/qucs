/***************************************************************************
                          mscorner.cpp  -  description
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

#include "mscorner.h"


MScorner::MScorner()
{
  Description = QObject::tr("microstrip corner");

  Lines.push_back(qucs::Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0, 18,  0, 30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-18, -8,  8, -8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-18,  8, -8,  8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-18, -8,-18,  8,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -8,  8, -8, 18,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  8, -8,  8, 18,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -8, 18,  8, 18,QPen(Qt::darkBlue,2)));

  Ports.push_back(qucs::Port(-30, 0));
  Ports.push_back(qucs::Port(  0,30));

  x1 = -30; y1 =-11;
  x2 =  11; y2 = 30;

  set_qucs_text_position(x2+4, y1+4);
  Model = "MCORN";
  Name  = "MS";

  Props.push_back(qucs::Property("Subst", "Subst1", true,
		QObject::tr("substrate")));
  Props.push_back(qucs::Property("W", "1 mm", true,
		QObject::tr("width of line")));
}

MScorner::~MScorner()
{
}

Component* MScorner::newOne()
{
  return new MScorner();
}

Element* MScorner::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Microstrip Corner");
  BitmapFile = (char *) "mscorner";

  if(getNewOne)  return new MScorner();
  return 0;
}
