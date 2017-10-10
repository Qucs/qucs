/***************************************************************************
                                vprobe.cpp
                               ------------
    begin                : Sat Feb 18 2006
    copyright            : (C) 2006 by Michael Margraf
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

#include "vprobe.h"


vProbe::vProbe()
{
  Description = QObject::tr("voltage probe");

  Props.append(new Property("Ri", "0 Ohm", false,
		QObject::tr("Internal resistance, (0 : disable)")));

  Lines.append(new Line(-20,-31, 20,-31,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20,  9, 20,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20,-31,-20,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 20,-31, 20,  9,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-16,-27, 16,-27,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-16, -9, 16, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-16,-27,-16, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 16,-27, 16, -9,QPen(Qt::darkBlue,2)));

  Arcs.append(new Arc(-20,-23, 39, 39, 16*50, 16*80,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-11,-24, -2, -9,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-10,  0,-10,  6,QPen(Qt::red,2)));
  Lines.append(new Line(-13,  3, -7,  3,QPen(Qt::red,2)));
  Lines.append(new Line(  7,  3, 13,  3,QPen(Qt::black,2)));
  
  Lines.append(new Line(-10,  9,-10, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 10,  9, 10, 20,QPen(Qt::darkBlue,2)));
  Ports.append(new Port(-10, 20));
  Ports.append(new Port( 10, 20));

  x1 = -24; y1 = -35;
  x2 =  24; y2 =  20;

  tx = x2+4;
  ty = y1+4;
  Model = "VProbe";
  Name  = "Pr";
}

vProbe::~vProbe()
{
}

Component* vProbe::newOne()
{
  return new vProbe();
}

Element* vProbe::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Voltage Probe");
  BitmapFile = (char *) "vprobe";

  if(getNewOne)  return new vProbe();
  return 0;
}
