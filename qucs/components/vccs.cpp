/***************************************************************************
                          vccs.cpp  -  description
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

#include "vccs.h"


VCCS::VCCS()
{
  Description = QObject::tr("voltage controlled current source");

  Arcs.push_back(qucs::Arc(0,-11, 22, 22,  0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 11, -7, 11,  7,QPen(Qt::darkBlue,3)));
  Lines.push_back(qucs::Line( 11,  6, 15,  1,QPen(Qt::darkBlue,3)));
  Lines.push_back(qucs::Line( 11,  6,  7,  1,QPen(Qt::darkBlue,3)));

  Lines.push_back(qucs::Line(-30,-30,-12,-30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30, 30,-12, 30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 11,-30, 30,-30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 11, 30, 30, 30,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-12,-30,-12,-23,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-12, 30,-12, 23,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 11,-30, 11,-11,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 11, 30, 11, 11,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(-12,-18,-12, 18,QPen(Qt::darkBlue,1)));
  Lines.push_back(qucs::Line(-12, 18,-17,  9,QPen(Qt::darkBlue,1)));
  Lines.push_back(qucs::Line(-12, 18, -7,  9,QPen(Qt::darkBlue,1)));

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
  Model = "VCCS";
  Name  = "SRC";

  Props.push_back(qucs::Property("G", "1 S", true,
		QObject::tr("forward transconductance")));
  Props.push_back(qucs::Property("T", "0", false, QObject::tr("delay time")));
}

VCCS::~VCCS()
{
}

Component* VCCS::newOne()
{
  return new VCCS();
}

Element* VCCS::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Voltage Controlled Current Source");
  BitmapFile = (char *) "vccs";

  if(getNewOne)  return new VCCS();
  return 0;
}
