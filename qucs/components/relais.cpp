/***************************************************************************
                                relais.cpp
                               ------------
    begin                : Sat Feb 25 2006
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

#include "relais.h"


Relais::Relais()
{
  Description = QObject::tr("relais");

  Lines.append(new Line(-30,-30,-30, -8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  8,-30, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-45, -8,-15, -8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-45,  8,-15,  8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-45, -8,-45,  8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-15, -8,-15,  8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-45,  8,-15, -8,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-15,  0, 35,  0,QPen(QPen::darkBlue,1,Qt::DotLine)));

  Lines.append(new Line( 30,-30, 30,-18,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30, 15, 30, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30, 15, 45,-15,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( 27,-18, 6, 6, 0, 16*360,QPen(QPen::darkBlue,2)));
  Ellips.append(new Area( 27, 12, 6, 6, QPen(QPen::darkBlue,2),
                         QBrush(Qt::darkBlue, Qt::SolidPattern)));

  Ports.append(new Port(-30,-30));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 30));

  x1 = -48; y1 = -30;
  x2 =  45; y2 =  30;

  tx = x1-4;
  ty = y2+4;
  Model = "Relais";
  Name  = "S";

  Props.append(new Property("Von", "0.4 V", false,
		QObject::tr("voltage to switch on the relais")));
  Props.append(new Property("Voff", "0.6 V", false,
		QObject::tr("voltage to switch off the relais")));
  Props.append(new Property("Ron", "0", false,
		QObject::tr("resistance of \"on\" state in ohms")));
  Props.append(new Property("Roff", "1e12", false,
		QObject::tr("resistance of \"off\" state in ohms")));
}

Relais::~Relais()
{
}

Component* Relais::newOne()
{
  return new Relais();
}

Element* Relais::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Relais");
  BitmapFile = "relais";

  if(getNewOne)  return new Relais();
  return 0;
}
