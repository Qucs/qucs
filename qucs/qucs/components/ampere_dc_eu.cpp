/***************************************************************************
                               ampere_dc_eu.cpp
                             -------------------
    begin                : Wed 03 june 2015
    copyright            : (C) 2015 Xavier Galtier
    email                : none
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ampere_dc_eu.h"


Ampere_dc_EU::Ampere_dc_EU()
{
  Description = QObject::tr("ideal dc current source");

  Arcs.append(new Arc(-12,-12, 24, 24,  0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -7,  -17,  7,  -17,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  -17,  0, -21,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  -17,  0,  -13,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  0,  -12,  0,  12,QPen(Qt::darkBlue,2)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Model = "Idc";
  Name  = "I";

  Props.append(new Property("I", "1 mA", true,
		QObject::tr("current in Ampere")));

  rotate();  // fix historical flaw
}

Ampere_dc_EU::~Ampere_dc_EU()
{
}

Component* Ampere_dc_EU::newOne()
{
  return new Ampere_dc_EU();
}

Element* Ampere_dc_EU::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("dc Current Source");
  BitmapFile = (char *) "dc_current_eu";

  if(getNewOne)  return new Ampere_dc_EU();
  return 0;
}
