/***************************************************************************
                          tr_sim.cpp  -  description
                             -------------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "tr_sim.h"


TR_Sim::TR_Sim()
{
  Description = QObject::tr("transient simulation");

  Lines.append(new Line(-45,-21, 45,-21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-45, 21, 45, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-45,-21,-45, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 45,-21, 45, 21,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-41, 26, 49, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-45, 21,-41, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 45, 21, 49, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 49,-16, 49, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 45,-21, 49,-16,QPen(QPen::darkBlue,2)));

  x1 = -49; y1 = -25;
  x2 =  53; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Sign  = ".TR";
  Model = ".TR";
  Name  = "TR";

  Props.append(new Property("Start", "0", true, QObject::tr("start time in seconds")));
  Props.append(new Property("Stop", "1 ms", true, QObject::tr("stop time in seconds")));
  Props.append(new Property("Step", "10 us", true, QObject::tr("time step in seconds")));
}

TR_Sim::~TR_Sim()
{
}

TR_Sim* TR_Sim::newOne()
{
  return new TR_Sim();
}
