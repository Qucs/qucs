/***************************************************************************
                          mosfet.cpp  -  description
                             -------------------
    begin                : Fri Jun 4 2004
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

#include "mosfet.h"


MOSFET::MOSFET()
{
  Description = QObject::tr("MOS field-effect transistor");

  Lines.append(new Line(-14,-13,-14, 13,QPen(QPen::darkBlue,3)));
  Lines.append(new Line(-30,  0,-14,  0,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-10,-16,-10, -7,QPen(QPen::darkBlue,3)));
  Lines.append(new Line(-10, -4,-10,  4,QPen(QPen::darkBlue,3)));
  Lines.append(new Line(-10,  7,-10, 16,QPen(QPen::darkBlue,3)));

  Lines.append(new Line(-10,-11,  0,-11,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,-11,  0,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 11,  0, 11,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,  0,  0, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,  0,  0,  0,QPen(QPen::darkBlue,2)));

  Lines.append(new Line( -9,  0, -4, -5,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -9,  0, -4,  5,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port(  0, 30));
  Ports.append(new Port(  0,-30));

  x1 = -30; y1 = -30;
  x2 =   4; y2 =  30;

  tx = x2+4;
  ty = y1+4;
  Model = "MOSFET";
  Name  = "T";

  Props.append(new Property("Type", "n", true,
			QObject::tr("polarity (n,p)")));
}

MOSFET::~MOSFET()
{
}

Component* MOSFET::newOne()
{
  MOSFET* p = new MOSFET();
  p->Props.getFirst()->Value = Props.getFirst()->Value;
  p->recreate();
  return p;
}

Component* MOSFET::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("n-MOSFET");
  BitmapFile = "nmosfet";

  if(getNewOne)  return new MOSFET();
  return 0;
}

Component* MOSFET::info_p(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("p-MOSFET");
  BitmapFile = "pmosfet";

  if(getNewOne) {
    MOSFET* p = new MOSFET();
    p->Props.getFirst()->Value = "p";
    p->recreate();
    return p;
  }
  return 0;
}

// Makes the schematic symbol a n-type, p-type or depletion MOSFET (according
// to the "Type" property).
void MOSFET::recreate()
{
  Line *pl2 = Lines.last();
  Line *pl1 = Lines.prev();

  if(Props.getFirst()->Value == "n") {
    pl1->x1 = -9;  pl1->y1 = 0;  pl1->x2 = -4; pl1->y2 = -5;
    pl2->x1 = -9;  pl2->y1 = 0;  pl2->x2 = -4; pl2->y2 =  5;
  }
  else {
    pl1->x1 = -1;  pl1->y1 = 0;  pl1->x2 = -6; pl1->y2 = -5;
    pl2->x1 = -1;  pl2->y1 = 0;  pl2->x2 = -6; pl2->y2 =  5;
  }

  if(mirroredX) {
    pl1->y1 = -pl1->y1;
    pl1->y2 = -pl1->y2;
    pl2->y1 = -pl2->y1;
    pl2->y2 = -pl2->y2;
  }

  int tmp;
  for(int z=0; z<rotated; z++) {
    tmp = -pl1->x1;
    pl1->x1 = pl1->y1;
    pl1->y1 = tmp;
    tmp = -pl1->x2;
    pl1->x2 = pl1->y2;
    pl1->y2 = tmp;

    tmp = -pl2->x1;
    pl2->x1 = pl2->y1;
    pl2->y1 = tmp;
    tmp = -pl2->x2;
    pl2->x2 = pl2->y2;
    pl2->y2 = tmp;
  }
}
