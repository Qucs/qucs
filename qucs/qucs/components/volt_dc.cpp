/***************************************************************************
                               volt_dc.cpp
                              -------------
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

#include "volt_dc.h"


Volt_dc::Volt_dc(bool european)
{
  Description = QObject::tr("ideal dc voltage source");

  Props.append(new Property("U", "1 V", true,
		QObject::tr("voltage in Volts")));

  // this must be the last property in the list !!!
  Props.append(new Property("Symbol", "european", false,
		QObject::tr("schematic symbol")+" [european, US]"));
  if(!european)  Props.getLast()->Value = "US";
  createSymbol();
  tx = x1+4;
  ty = y2+4;
  Model = "Vdc";
  Name  = "V";

  rotate();  // fix historical flaw
}

Volt_dc::~Volt_dc()
{
}

Component* Volt_dc::newOne()
{
  return new Volt_dc(); //Props.getLast()->Value != "US");
}

Element* Volt_dc::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("dc Voltage Source");
  BitmapFile = (char *) "dc_voltage_eu";

  if(getNewOne)  return new Volt_dc();
  return 0;
}

// -------------------------------------------------------
Element* Volt_dc::info_us(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("dc Voltage Source");
  BitmapFile = (char *) "dc_voltage";

  if(getNewOne)  return new Volt_dc(false);
  return 0;
}

// -------------------------------------------------------
void Volt_dc::createSymbol()
{
  if(Props.getLast()->Value.at(0) == 'U') {
  Lines.append(new Line(  4,-13,  4, 13,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -4, -6, -4,  6,QPen(Qt::darkBlue,4)));
  Lines.append(new Line( 30,  0,  4,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -4,  0,-30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 11,  5, 11, 11,QPen(Qt::red,1)));
  Lines.append(new Line( 14,  8,  8,  8,QPen(Qt::red,1)));
  Lines.append(new Line(-11,  5,-11, 11,QPen(Qt::black,1)));
  }
  else {
  Arcs.append(new Arc(-12,-12, 24, 24,  0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -7,  0,  7,  0,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0, -4,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0,  4,QPen(Qt::darkBlue,3)));
  }

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;
}