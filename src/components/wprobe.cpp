/*
 * wprobe.cpp - Construction of the Wattmeter Probe
 *
 * Copyright (C) 2015, Pedro Macedo
 *		 2017, Alberto Silva, 1120196@isep.ipp.pt
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//Taking vprobe as a basis
#include "wprobe.h"

wProbe::wProbe()
{
  Description = QObject::tr("power probe");

//Large box
  Lines.append(new Line(-20,-33, 20,-33,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20, 14, 20, 14,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20,-33,-20, 14,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 20,-33, 20, 14,QPen(Qt::darkBlue,2)));

//Small box
  Lines.append(new Line(-16,-28, 16,-28,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-16, -10, 16, -10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-16,-28,-16, -10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 16,-28, 16, -10,QPen(Qt::darkBlue,2)));

//Letter W
  Lines.append(new Line(-9,-22, -2, -11,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(0, -17, -2, -11,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(0, -17, 2, -11,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(9,-22, 2, -11,QPen(Qt::darkBlue,3)));

//+ and - signs
  Lines.append(new Line(-12,  5,-12,  11,QPen(Qt::red,2)));
  Lines.append(new Line(-15,  8, -9,  8,QPen(Qt::red,2)));
  Lines.append(new Line(  9,  8, 15,  8,QPen(Qt::darkBlue,2)));

//Current Entries
  Ports.append(new ComponentPort(-30,  0));
  Ports.append(new ComponentPort( 30,  0));

//Voltage Entries
  Lines.append(new Line(-10,  14,-10, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 10,  14, 10, 20,QPen(Qt::darkBlue,2)));
  Ports.append(new ComponentPort(-10, 20));
  Ports.append(new ComponentPort( 10, 20));

//Letter V
  Lines.append(new Line(-3,  7 ,0,  13,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 0,  13, 3,  7,QPen(Qt::darkBlue,2)));

//Letter A
  Lines.append(new Line(  11,  -9, 15,0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  7,  0, 11, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  14, -3, 8, -3,QPen(Qt::darkBlue,2)));

//Arrow
  Lines.append(new Line(-30,  0,-20,  0,QPen(Qt::darkBlue,1)));
  Lines.append(new Line( 30,  0, 20,  0,QPen(Qt::darkBlue,1)));
  Lines.append(new Line(-20,  0, 20,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  4,  0, -4, -3,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  4,  0, -4,  4,QPen(Qt::darkBlue,2)));

  x1 = -24; y1 = -35;
  x2 =  24; y2 =  20;

  tx = x2+4;
  ty = y1+4;
  Model = "WProbe";
  Name  = "Pr";
}

wProbe::~wProbe()
{
}

Component* wProbe::newOne()
{
  return new wProbe();
}

Element* wProbe::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Power Probe");
  BitmapFile = (char *) "wprobe";

  if(getNewOne)  return new wProbe();
  return 0;
}
