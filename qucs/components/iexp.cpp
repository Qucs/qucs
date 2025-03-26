/***************************************************************************
                                iexp.cpp
                               ----------
    begin                : Tue Mar 06 2007
    copyright            : (C) 2007 by Gunther Kraut
    email                : gn.kraut@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "iexp.h"


iExp::iExp()
{
  Description = QObject::tr("exponential current source");

  // normal current source symbol
  Arcs.push_back(qucs::Arc(-12,-12, 24, 24,  0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -7,  0,  7,  0,QPen(Qt::darkBlue,3)));
  Lines.push_back(qucs::Line(  6,  0,  0, -4,QPen(Qt::darkBlue,3)));
  Lines.push_back(qucs::Line(  6,  0,  0,  4,QPen(Qt::darkBlue,3)));

  // write 'Exp' beside current source symbol
  Lines.push_back(qucs::Line( 13,  7, 13, 10,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 13,  7, 21, 7,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 17,  7, 17, 10,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 21,  7, 21, 10,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 13,  13, 17, 17,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 17,  13, 13, 17,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 17,  20, 11, 20,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 17,  20, 17, 23,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 13,  20, 13, 23,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 17,  23, 13, 23,QPen(Qt::darkBlue,2)));

  Ports.push_back(qucs::Port( 30,  0));
  Ports.push_back(qucs::Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  20;

  set_qucs_text_position(x1+4, y2+8);
  Model = "Iexp";
  Name  = "I";

  Props.push_back(qucs::Property("I1", "0", true,
		QObject::tr("current before rising edge")));
  Props.push_back(qucs::Property("I2", "1 A", true,
		QObject::tr("maximum current of the pulse")));
  Props.push_back(qucs::Property("T1", "0", true,
		QObject::tr("start time of the exponentially rising edge")));
  Props.push_back(qucs::Property("T2", "1 ms", true,
		QObject::tr("start of exponential decay")));
  Props.push_back(qucs::Property("Tr", "1 ns", false,
		QObject::tr("time constant of the rising edge")));
  Props.push_back(qucs::Property("Tf", "1 ns", false,
		QObject::tr("time constant of the falling edge")));

  rotate();  // fix historical flaw
}

iExp::~iExp()
{
}

Component* iExp::newOne()
{
  return new iExp();
}

Element* iExp::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Exponential Current Pulse");
  BitmapFile = (char *) "iexp";

  if(getNewOne)  return new iExp();
  return 0;
}
