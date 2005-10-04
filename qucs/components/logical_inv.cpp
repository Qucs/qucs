/***************************************************************************
                              logical_inv.cpp
                             -----------------
    begin                : Wed Sep 28 2005
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

#include "logical_inv.h"

Logical_Inv::Logical_Inv()
{
  Description = QObject::tr("logical inverter");

  x1 = -30; y1 = -23;
  x2 =  30; y2 =  23;

  tx = x1+4;
  ty = y2+4;
  Model = "Inv";
  Name  = "Y";

  Props.append(new Property("V", "1 V", false,
		QObject::tr("voltage of high level")));
  Props.append(new Property("t", "0", false,
		QObject::tr("delay time")));

  // this must be the last property in the list !!!
  Props.append(new Property("Symbol", "old", false,
		QObject::tr("schematic symbol")+" [old, DIN40900]"));

  createSymbol();
}


void Logical_Inv::createSymbol()
{
  int xr;

  if(Props.getLast()->Value.at(0) == 'D') {  // DIN symbol
    Lines.append(new Line( 15,-20, 15, 20,QPen(QPen::darkBlue,2)));
    Lines.append(new Line(-15,-20, 15,-20,QPen(QPen::darkBlue,2)));
    Lines.append(new Line(-15, 20, 15, 20,QPen(QPen::darkBlue,2)));
    Lines.append(new Line(-15,-20,-15, 20,QPen(QPen::darkBlue,2)));

    Texts.append(new Text(-11,-17, "1", QPen::darkBlue, 15.0));
    xr =  15;
  }
  else {   // old symbol
    Lines.append(new Line(-10,-20,-10,20, QPen(QPen::darkBlue,2)));
    Arcs.append(new Arc(-30,-20, 40, 30,  0, 16*90,QPen(QPen::darkBlue,2)));
    Arcs.append(new Arc(-30,-10, 40, 30,  0,-16*90,QPen(QPen::darkBlue,2)));
    Lines.append(new Line( 10,-5, 10, 5,QPen(QPen::darkBlue,2)));
    xr =  10;
  }

  Ellips.append(new Area(xr,-4, 8, 8,
                QPen(QPen::darkBlue,0), QBrush(QPen::darkBlue)));

  Lines.append(new Line( xr, 0, 30, 0, QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30, 0,-xr, 0, QPen(QPen::darkBlue,2)));
  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));
}

Component* Logical_Inv::newOne()
{
  Logical_Inv* p = new Logical_Inv();
  p->Props.getLast()->Value = Props.getLast()->Value;
  p->recreate();
  return p;
}

Element* Logical_Inv::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Inverter");
  BitmapFile = "inverter";

  if(getNewOne)  return new Logical_Inv();
  return 0;
}

void Logical_Inv::recreate()
{
  Ellips.clear();
  Ports.clear();
  Lines.clear();
  Texts.clear();
  Arcs.clear();
  createSymbol();

  Line *p1;
  bool mmir = mirroredX;
  int  tmp, rrot = rotated;
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


  rotated = rrot;  // restore properties (were changed by rotate/mirror)
  mirroredX = mmir;
}
