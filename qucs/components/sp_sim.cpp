/***************************************************************************
                          sp_sim.cpp  -  description
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

#include "sp_sim.h"


SP_Sim::SP_Sim()
{
  Description = "S parameter simulation";

  Lines.append(new Line(-53,-21, 53,-21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-53, 21, 53, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-53,-21,-53, 21,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 53,-21, 53, 21,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-49, 26, 57, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-53, 21,-49, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 53, 21, 57, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 57,-16, 57, 26,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 53,-21, 57,-16,QPen(QPen::darkBlue,2)));

  x1 = -57; y1 = -25;
  x2 =  61; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Sign  = ".SP";
  Model = ".SP";
  Name  = "SP";

  Props.append(new Property("Start", "1 GHz", true, "start frequency in Hertz"));
  Props.append(new Property("Stop", "10 GHz", true, "stop frequency in Hertz"));
  Props.append(new Property("Step", "1 GHz", true, "frequency steps in Hertz"));
}

SP_Sim::~SP_Sim()
{
}

SP_Sim* SP_Sim::newOne()
{
  return new SP_Sim();
}
