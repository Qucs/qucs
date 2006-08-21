/***************************************************************************
                               opt_sim.cpp
                              -------------
    begin                : Sat Jul 22 2006
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

#include "opt_sim.h"
#include "main.h"


Optimize_Sim::Optimize_Sim()
{
  Description = QObject::tr("Optimization");

  Texts.append(new Text(0, 0, Description, QPen::darkBlue, QucsSettings.largeFontSize));

  x1 = -10; y1 = -9;
  x2 = x1+128; y2 = y1+41;

  tx = 0;
  ty = y2+1;
  Model = ".Opt";
  Name  = "Opt";

  Props.append(new Property("Sim", "", false, ""));
}

Optimize_Sim::~Optimize_Sim()
{
}

Component* Optimize_Sim::newOne()
{
  return new Optimize_Sim();
}

Element* Optimize_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("optimization");
  BitmapFile = "optimize";

  if(getNewOne)  return new Optimize_Sim();
  return 0;
}
