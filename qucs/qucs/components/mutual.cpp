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

  Arcs.append(new Arc(-16,-18,13,13, 16*270,16*180, QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16, -6,13,13, 16*270,16*180, QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16,  6,13,13, 16*270,16*180, QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4,-18,13,13,  16*90,16*180, QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4, -6,13,13,  16*90,16*180, QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4,  6,13,13,  16*90,16*180, QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-18,-10,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-30,-30,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10,-18, 10,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10,-30, 30,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 18,-10, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 30,-30, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10, 18, 10, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10, 30, 30, 30,QPen(QPen::darkBlue,2)));

  Texts.append(new Text(-21, -22, "1"));
  Texts.append(new Text( 15, -22, "2"));
  Arcs.append(new Arc(-14,-40, 28, 20, 16*30, 16*120,QPen(QPen::darkBlue,2)));


  Ports.append(new Port(-30,-30));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 30));

  x1 = -33; y1 = -43;
  x2 =  33; y2 =  34;

  tx = x1+4;
  ty = y2+4;
  Model = "MUT";
  Name  = "Tr";

  Props.append(new Property("L1", "1 mH", false,
		QObject::tr("inductance of coil 1")));
  Props.append(new Property("L2", "1 mH", false,
		QObject::tr("inductance of coil 2")));
  Props.append(new Property("k", "0.9", false,
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
  BitmapFile = "mutual";

  if(getNewOne)  return new Mutual();
  return 0;
}
