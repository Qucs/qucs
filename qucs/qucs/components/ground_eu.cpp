/***************************************************************************
                                ground_eu.cpp
                               ------------
    begin                : Wed 03 June 2015
    copyright            : (C) 2015 by Xavier Galtier
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

#include "ground_eu.h"
#include "node.h"


Ground_EU::Ground_EU()
{
  Type = isComponent;   // both analog and digital
  Description = QObject::tr("ground (reference potential)");

//  Lines.append(new Line(  0,  0,  0, 10,QPen(Qt::darkBlue,2)));
//  Lines.append(new Line(-11, 10, 11, 10,QPen(Qt::darkBlue,3)));
//  Lines.append(new Line( -7, 16,  7, 16,QPen(Qt::darkBlue,3)));
//  Lines.append(new Line( -3, 22,  3, 22,QPen(Qt::darkBlue,3)));

  Lines.append(new Line(  0,  0,  0, 10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20, 10, 12, 10,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(12, 10, 4, 18,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(4, 10, -4, 18,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-4, 10, -12, 18,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-12, 10, -20, 18,QPen(Qt::darkBlue,3)));

  Ports.append(new Port(  0,  0));

  x1 = -12; y1 =  0;
  x2 =  12; y2 = 25;

  tx = 0;
  ty = 0;
  Model = "GND";
  Name  = "";
}

Ground_EU::~Ground_EU()
{
}

Component* Ground_EU::newOne()
{
  return new Ground_EU();
}

// -------------------------------------------------------
Element* Ground_EU::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Ground_eu");
  BitmapFile = (char *) "ground_eu";

  if(getNewOne)  return new Ground_EU();
  return 0;
}

// -------------------------------------------------------
QString Ground_EU::netlist()
{
  return QString("");
}
