/***************************************************************************
                          subcircuit.cpp  -  description
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

#include "subcircuit.h"


Subcircuit::Subcircuit(int No)
{
  Description = QObject::tr("subcircuit");

  Model = "Sub";
  Name  = "SUB";

  Props.append(new Property("Type", "test.sch", true,
		QObject::tr("name of qucs schematic file")));
  Props.append(new Property("Symbol", QString::number(No), false,
		QObject::tr("number of ports")));

  recreate();
}

Subcircuit::~Subcircuit()
{
}

Component* Subcircuit::newOne()
{
  return new Subcircuit(Props.getLast()->Value.toInt());
}

/*Component* Subcircuit::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Subcircuit");
  BitmapFile = "";  // has no bitmap

  if(getNewOne)  return new Subcircuit(Props.getLast()->Value.toInt());
  return 0;
}*/

// Makes the schematic symbol subcircuit with the correct number
// of ports.
void Subcircuit::recreate()
{
  Lines.clear();
  Texts.clear();
  Ports.clear();

  int No = Props.getLast()->Value.toInt();

  int h = 30*((No-1)/2) + 15;
  Lines.append(new Line(-15, -h, 15, -h,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 15, -h, 15,  h,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-15,  h, 15,  h,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-15, -h,-15,  h,QPen(QPen::darkBlue,2)));
  Texts.append(new Text( -7,  0,"sub"));

  int i=0, y = 15-h;
  while(i<No) {
    i++;
    Lines.append(new Line(-30,  y,-15,  y,QPen(QPen::darkBlue,2)));
    Ports.append(new Port(-30,  y));
    Texts.append(new Text(-25,y-3,QString::number(i)));

    if(i == No) break;
    i++;
    Lines.append(new Line( 15,  y, 30,  y,QPen(QPen::darkBlue,2)));
    Ports.append(new Port( 30,  y));
    Texts.append(new Text( 20,y-3,QString::number(i)));
    y += 60;
  }

  x1 = -30; y1 = -h-2;
  x2 =  30; y2 =  h+2;

  tx = x1+4;
  ty = y2+4;


  Line *p1;
  bool mmir = mirroredX;
  int  tmp, ttx = tx, tty = ty, rrot = rotated;
  if(mmir)  // mirror all lines
    for(p1 = Lines.first(); p1 != 0; p1 = Lines.next()) {
      p1->y1 = -p1->y1;
      p1->y2 = -p1->y2;
    }

  for(int z=0; z<rrot; z++)    // rotate all lines
    for(p1 = Lines.first(); p1 != 0; p1 = Lines.next()) {
      tmp = -p1->x1;
      p1->x1 = p1->y1;
      p1->y1 = tmp;
      tmp = -p1->x2;
      p1->x2 = p1->y2;
      p1->y2 = tmp;
    }


  tx = ttx; ty = tty;  // restore properties (were changed by rotate/mirror)
  rotated = rrot;
  mirroredX = mmir;
}
