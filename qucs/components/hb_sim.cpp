/***************************************************************************
                          hb_sim.cpp  -  description
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

#include "hb_sim.h"


HB_Sim::HB_Sim()
{
  Description = QObject::tr("Harmonic balance simulation");

  Lines.append(new Line(-75,-21, 75,-21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-75, 21, 75, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-75,-21,-75, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 75,-21, 75, 21,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-71, 26, 79, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-75, 21,-71, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 75, 21, 79, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 79,-16, 79, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 75,-21, 79,-16,QPen(QPen::darkBlue,2)));

  x1 = -79; y1 = -25;
  x2 =  83; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Sign  = ".HB";
  Model = ".HB";
  Name  = "HB";

  Props.append(new Property("f", "1 GHz", true, QObject::tr("frequency in Hertz")));
  Props.append(new Property("n", "4", true, QObject::tr("number of harmonics")));
}

HB_Sim::~HB_Sim()
{
}

HB_Sim* HB_Sim::newOne()
{
  return new HB_Sim();
}
