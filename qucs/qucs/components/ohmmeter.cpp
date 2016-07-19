/***************************************************************************
                                ohmmeter.cpp
                               -----------
    begin                : Fri Sep 25 2016
    copyright            : (C) 2016 by Nelson Martins
    email                : macedo.pp@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ohmmeter.h"

Ohmmeter::Ohmmeter()
{
  Description = QObject::tr("ohmmeter");
  

  //box
  Lines.append(new Line(-20,-31, 20,-31,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20,  9, 20,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20,-31,-20,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 20,-31, 20,  9,QPen(Qt::darkBlue,2)));

  //small window
  Lines.append(new Line(-16,-27, 16,-27,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-16, -9, 16, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-16,-27,-16, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 16,-27, 16, -9,QPen(Qt::darkBlue,2)));

  //port of V
  Lines.append(new Line(-10,  9,-10, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 10,  9, 10, 20,QPen(Qt::darkBlue,2)));
  Ports.append(new Port(-10, 20));
  Ports.append(new Port( 10, 20));

  

  //inside small window
  QString tmp = QString::fromUtf8("\u2126");
  Texts.append(new Text(-8, -30, tmp,Qt::darkBlue,15));

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
  BitmapFile = (char *) "ohm";

  if(getNewOne)  return new Ohmmeter();
  return 0;
}
