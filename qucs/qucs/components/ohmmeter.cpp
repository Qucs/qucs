/*
 * ohmmeter.cpp - Construction of the ohmmeter
 *		
 * Copyright (C) 2017, Alberto Silva, 1120196@isep.ipp.pt	
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

#include "ohmmeter.h"

Ohmmeter::Ohmmeter()
{
  Description = QObject::tr("ohmmeter");
  
  //Large Window
  Lines.append(new Line(-20,-31, 20,-31,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20,  9, 20,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20,-31,-20,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 20,-31, 20,  9,QPen(Qt::darkBlue,2)));

  //Small Window
  Lines.append(new Line(-16,-27, 16,-27,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-16, -9, 16, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-16,-27,-16, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 16,-27, 16, -9,QPen(Qt::darkBlue,2)));

  //Ports
  Lines.append(new Line(-10,  9,-10, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 10,  9, 10, 20,QPen(Qt::darkBlue,2)));
  Ports.append(new Port(-10, 20));
  Ports.append(new Port( 10, 20));

  //Underlined Zed
  QString tmp = QString::fromUtf8("\u005A");
  Texts.append(new Text(-6, -32, tmp,Qt::darkBlue,12));
  Lines.append(new Line(-6, -12, 4, -12,QPen(Qt::darkBlue,1)));

  x1 = -24; y1 = -35;
  x2 =  24; y2 =  20;

  tx = x2+4;
  ty = y1+4;
  Model = "Ohmmeter";
  Name  = "Pr";
}

Ohmmeter::~Ohmmeter()
{
}

Component* Ohmmeter::newOne()
{
  return new Ohmmeter();
}

Element* Ohmmeter::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Ohmmeter");
  BitmapFile = (char *) "ohmmeter";

  if(getNewOne)  return new Ohmmeter();
  return 0;
}
