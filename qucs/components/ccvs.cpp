/***************************************************************************
                          ccvs.cpp  -  description
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

#include "ccvs.h"


CCVS::CCVS()
{
  Description = QObject::tr("current controlled voltage source");

  Arcs.push_back(qucs::Arc(0,-11, 22, 22,  0, 16*360,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-30,-30,-12,-30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30, 30,-12, 30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 11,-30, 30,-30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 11, 30, 30, 30,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-12,-30,-12, 30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 11,-30, 11,-11,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 11, 30, 11, 11,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-12, 20,-17, 11,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-12, 20, -8, 11,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line( 19,-21, 19,-15,QPen(Qt::red,1)));
  Lines.push_back(qucs::Line( 16,-18, 22,-18,QPen(Qt::red,1)));
  Lines.push_back(qucs::Line( 16, 18, 22, 18,QPen(Qt::black,1)));

  Lines.push_back(qucs::Line(-25,-27, 25,-27,QPen(Qt::darkGray,1)));
  Lines.push_back(qucs::Line( 25,-27, 25, 27,QPen(Qt::darkGray,1)));
  Lines.push_back(qucs::Line( 25, 27,-25, 27,QPen(Qt::darkGray,1)));
  Lines.push_back(qucs::Line(-25, 27,-25,-27,QPen(Qt::darkGray,1)));

  Ports.push_back(qucs::Port(-30,-30));
  Ports.push_back(qucs::Port( 30,-30));
  Ports.push_back(qucs::Port( 30, 30));
  Ports.push_back(qucs::Port(-30, 30));

  set_x1(-30);
  set_y1(-30);
  set_x2(30);
  set_y2(30);

  set_qucs_text_position(x1()+4, y2()+4);
  Model = "CCVS";
  Name  = "SRC";

  Props.push_back(qucs::Property("G", "1 Ohm", true,
		QObject::tr("forward transfer factor")));
  Props.push_back(qucs::Property("T", "0", false, QObject::tr("delay time")));
}

CCVS::~CCVS()
{
}

Component* CCVS::newOne()
{
  return new CCVS();
}

Element* CCVS::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Current Controlled Voltage Source");
  BitmapFile = (char *) "ccvs";

  if(getNewOne)  return new CCVS();
  return 0;
}
