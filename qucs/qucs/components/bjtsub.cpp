/***************************************************************************
                           bjtsub.cpp  -  description
                             -------------------
    begin                : Sat Jul 17 2004
    copyright            : (C) 2004 by Michael Margraf
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

#include "bjtsub.h"


BJTsub::BJTsub()
{
  Description = QObject::tr("npn bipolar junction transistor with substrate");

  Lines.append(new Line(-10,-15,-10, 15,QPen(QPen::darkBlue,3)));
  Lines.append(new Line(-30,  0,-10,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, -5,  0,-15,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,-15,  0,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,  5,  0, 15,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0, 15,  0, 30,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(  9,  0, 30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  9, -7,  9,  7,QPen(QPen::darkBlue,3)));

  Lines.append(new Line( -6, 15,  0, 15,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,  9,  0, 15,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port(  0, 30));
  Ports.append(new Port( 30,  0));
  Ports.append(new Port(  0,-30));

  x1 = -30; y1 = -30;
  x2 =  30; y2 =  30;

  tx = x2+4;
  ty = y1+4;
  Model = "BJTsub";
  Name  = "T";

  // this must be the first property in the list  !!!
  Props.append(new Property("Type", "npn", true,
			QObject::tr("polarity (npn,pnp)")));
}

BJTsub::~BJTsub()
{
}

Component* BJTsub::newOne()
{
  BJTsub* p = new BJTsub();
  p->Props.getFirst()->Value = Props.getFirst()->Value;
  p->recreate();
  return p;
}

Component* BJTsub::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("npn transistor");
  BitmapFile = "npnsub";

  if(getNewOne)  return new BJTsub();
  return 0;
}

Component* BJTsub::info_pnp(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("pnp transistor");
  BitmapFile = "pnpsub";

  if(getNewOne) {
    BJTsub* p = new BJTsub();
    p->Props.getFirst()->Value = "pnp";
    p->recreate();
    return p;
  }
  return 0;
}

// Makes the schematic symbol a npn or a pnp transistor (according
// to the "Type" property).
void BJTsub::recreate()
{
  Line *pl2 = Lines.last();
  Line *pl1 = Lines.prev();

  if(Props.getFirst()->Value == "npn") {
    pl1->x1 = -6;  pl1->y1 = 15;  pl1->x2 = 0; pl1->y2 = 15;
    pl2->x1 = 0;  pl2->y1 = 9;  pl2->x2 = 0; pl2->y2 = 15;
  }
  else {
    pl1->x1 = -5;  pl1->y1 = 10;  pl1->x2 = -5; pl1->y2 = 16;
    pl2->x1 = -5;  pl2->y1 = 10;  pl2->x2 = 1; pl2->y2 = 10;
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
