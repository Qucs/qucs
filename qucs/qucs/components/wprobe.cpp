/*
 * wprobe.cpp - File to draw the wattmeter
 *
 * Copyright (C) [2015], [Pedro Macedo], [macedo.pp@gmail.com]
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

#include "wprobe.h"


wProbe::wProbe()
{
  Description = QObject::tr("power probe");

//Caixa grande
  Lines.append(new Line(-20,-33, 20,-33,QPen(Qt::darkGreen,2)));
  Lines.append(new Line(-20, 14, 20, 14,QPen(Qt::darkGreen,2)));
  Lines.append(new Line(-20,-33,-20, 14,QPen(Qt::darkGreen,2)));
  Lines.append(new Line( 20,-33, 20, 14,QPen(Qt::darkGreen,2)));

//Caixa pequena
  Lines.append(new Line(-16,-28, 16,-28,QPen(Qt::darkYellow,2)));
  Lines.append(new Line(-16, -10, 16, -10,QPen(Qt::darkYellow,2)));
  Lines.append(new Line(-16,-28,-16, -10,QPen(Qt::darkYellow,2)));
  Lines.append(new Line( 16,-28, 16, -10,QPen(Qt::darkYellow,2)));

//Conatrução do W
  Lines.append(new Line(-9,-22, -2, -11,QPen(Qt::darkGreen,3)));
  Lines.append(new Line(0, -17, -2, -11,QPen(Qt::darkGreen,3)));
  Lines.append(new Line(0, -17, 2, -11,QPen(Qt::darkGreen,3)));
  Lines.append(new Line(9,-22, 2, -11,QPen(Qt::darkGreen,3)));

//+ e -
  Lines.append(new Line(-12,  5,-12,  11,QPen(Qt::red,2)));
  Lines.append(new Line(-15,  8, -9,  8,QPen(Qt::red,2)));
  Lines.append(new Line(  9,  8, 15,  8,QPen(Qt::black,2)));

//Portas do I
  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

//Portas do V
  Lines.append(new Line(-10,  14,-10, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 10,  14, 10, 20,QPen(Qt::darkBlue,2)));
  Ports.append(new Port(-10, 20));
  Ports.append(new Port( 10, 20));

//Construção do V
  Lines.append(new Line(-3,  7 ,0,  13,QPen(Qt::darkGreen,2)));
  Lines.append(new Line( 0,  13, 3,  7,QPen(Qt::darkGreen,2)));

//Construção do A
  Lines.append(new Line(  11,  -9, 15,0,QPen(Qt::darkGreen,2)));
  Lines.append(new Line(  7,  0, 11, -9,QPen(Qt::darkGreen,2)));
  Lines.append(new Line(  14, -3, 8, -3,QPen(Qt::darkGreen,2)));

//Seta
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
