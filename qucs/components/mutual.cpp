/***************************************************************************
                                mutual.cpp
                               ------------
    begin                : Sat Aug 20 2005
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

#include "mutual.h"


Mutual::Mutual()
{
  Description = QObject::tr("two mutual inductors");

  Arcs.push_back(qucs::Arc(-16,-18,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(-16, -6,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(-16,  6,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(  4,-18,12,12,  16*90,16*180, QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(  4, -6,12,12,  16*90,16*180, QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(  4,  6,12,12,  16*90,16*180, QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10,-18,-10,-30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10,-30,-30,-30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 10,-18, 10,-30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 10,-30, 30,-30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10, 18,-10, 30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10, 30,-30, 30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 10, 18, 10, 30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 10, 30, 30, 30,QPen(Qt::darkBlue,2)));

  Texts.push_back(qucs::Text(-21, -22, "1"));
  Texts.push_back(qucs::Text( 15, -22, "2"));
  Lines.push_back(qucs::Line(  0,-20,  0, 20,QPen(Qt::darkBlue,1,Qt::DashLine)));

  Ports.push_back(qucs::Port(-30,-30));
  Ports.push_back(qucs::Port( 30,-30));
  Ports.push_back(qucs::Port( 30, 30));
  Ports.push_back(qucs::Port(-30, 30));

  x1 = -33; y1 = -34;
  x2 =  33; y2 =  34;

  set_qucs_text_position(x1+4, y2+4);
  Model = "MUT";
  Name  = "Tr";

  Props.push_back(qucs::Property("L1", "1 mH", false,
		QObject::tr("inductance of coil 1")));
  Props.push_back(qucs::Property("L2", "1 mH", false,
		QObject::tr("inductance of coil 2")));
  Props.push_back(qucs::Property("k", "0.9", false,
		QObject::tr("coupling factor between coil 1 and 2")));
}

Mutual::~Mutual()
{
}

Component* Mutual::newOne()
{
  return new Mutual();
}

Element* Mutual::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Mutual Inductors");
  BitmapFile = (char *) "mutual";

  if(getNewOne)  return new Mutual();
  return 0;
}
