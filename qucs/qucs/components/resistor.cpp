/***************************************************************************
                          resistor.cpp  -  description
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

#include "resistor.h"

Resistor::Resistor(bool european)
{
  Description = QObject::tr("resistor");

  createSymbol(european);

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -11;
  x2 =  30; y2 =  11;

  tx = x1+4;
  ty = y2+4;
  Model = "R";
  Name  = "R";

  Props.append(new Property("R", "50 Ohm", true,
		QObject::tr("ohmic resistance in Ohms")));

  // this must be the last property in the list !!!
  Props.append(new Property("Symbol", "europaen", false,
		QObject::tr("schematic symbol (european,US)")));
  if(!european)  Props.getLast()->Value = "US";
}

Resistor::~Resistor()
{
}


void Resistor::createSymbol(bool european)
{
  if(european) {
    Lines.append(new Line(-18, -9, 18, -9,QPen(QPen::darkBlue,2)));
    Lines.append(new Line( 18, -9, 18,  9,QPen(QPen::darkBlue,2)));
    Lines.append(new Line( 18,  9,-18,  9,QPen(QPen::darkBlue,2)));
    Lines.append(new Line(-18,  9,-18, -9,QPen(QPen::darkBlue,2)));
    Lines.append(new Line(-30,  0,-18,  0,QPen(QPen::darkBlue,2)));
    Lines.append(new Line( 18,  0, 30,  0,QPen(QPen::darkBlue,2)));
  }
  else {
    Lines.append(new Line(-30,  0,-18,  0,QPen(QPen::darkBlue,2)));
    Lines.append(new Line(-18,  0,-15, -7,QPen(QPen::darkBlue,2)));
    Lines.append(new Line(-15, -7, -9,  7,QPen(QPen::darkBlue,2)));
    Lines.append(new Line( -9,  7, -3, -7,QPen(QPen::darkBlue,2)));
    Lines.append(new Line( -3, -7,  3,  7,QPen(QPen::darkBlue,2)));
    Lines.append(new Line(  3,  7,  9, -7,QPen(QPen::darkBlue,2)));
    Lines.append(new Line(  9, -7, 15,  7,QPen(QPen::darkBlue,2)));
    Lines.append(new Line( 15,  7, 18,  0,QPen(QPen::darkBlue,2)));
    Lines.append(new Line( 18,  0, 30,  0,QPen(QPen::darkBlue,2)));
  }
}

Component* Resistor::newOne()
{
  Resistor* p = new Resistor();
  p->Props.getLast()->Value = Props.getLast()->Value;
  p->recreate();
  return p;
}

Component* Resistor::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Resistor");
  BitmapFile = "resistor";

  if(getNewOne)  return new Resistor();
  return 0;
}

Component* Resistor::info_us(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Resistor US");
  BitmapFile = "resistor_us";

  if(getNewOne) {
    Resistor* p = new Resistor();
    p->Props.getLast()->Value = "US";
    p->recreate();
    return p;
  }
  return 0;
}

void Resistor::recreate()
{
  Lines.clear();
  createSymbol(Props.getLast()->Value != "US");

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
