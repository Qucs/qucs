/***************************************************************************
                               ampere_dc.cpp
                              ---------------
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

#include "ampere_dc.h"


Ampere_dc::Ampere_dc(bool european)
{
  Description = QObject::tr("ideal dc current source");

  Props.append(new Property("I", "1 mA", true,
		QObject::tr("current in Ampere")));

  // this must be the last property in the list !!!
  Props.append(new Property("Symbol", "european", false,
		QObject::tr("schematic symbol")+" [european, US]"));
  if(!european)  Props.getLast()->Value = "US";
  createSymbol();

  tx = x1+4;
  ty = y2+4;
  Model = "Idc";
  Name  = "I";


  rotate();  // fix historical flaw
}

Ampere_dc::~Ampere_dc()
{
}

Component* Ampere_dc::newOne()
{
  return new Ampere_dc(Props.getLast()->Value != "US");
}

Element* Ampere_dc::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("dc Current Source");
  BitmapFile = (char *) "dc_current_eu";

  if(getNewOne)  return new Ampere_dc();
  return 0;
}

// -------------------------------------------------------
Element* Ampere_dc::info_us(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("dc Current Source");
  BitmapFile = (char *) "dc_current";

  if(getNewOne)  return new Ampere_dc(false);
  return 0;
}
// -------------------------------------------------------
void Ampere_dc::createSymbol()
{
  if(Props.getLast()->Value == "US") {
  Arcs.append(new Arc(-12,-12, 24, 24,  0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -7,  0,  7,  0,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0, -4,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0,  4,QPen(Qt::darkBlue,3)));
  }
  else {
  Arcs.append(new Arc(-12,-12, 24, 24,  0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -7,  -17,  7,  -17,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  -17,  0, -21,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  -17,  0,  -13,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  0,  -12,  0,  12,QPen(Qt::darkBlue,2)));
  }

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;
}