/***************************************************************************
                          param_sweep.cpp  -  description
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

#include "param_sweep.h"


Param_Sweep::Param_Sweep()
{
  Description = "Parameter sweep";

  Lines.append(new Line(-46,-22, 46,-22,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-46, 22, 46, 22,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-46,-22,-46, 22,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 46,-22, 46, 22,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-42, 27, 50, 27,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-46, 22,-42, 27,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 46, 22, 50, 27,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 50,-17, 50, 27,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 46,-22, 50,-17,QPen(QPen::darkBlue,2)));

  x1 = -50; y1 = -26;
  x2 =  54; y2 =  31;

  tx = x1+4;
  ty = y2+4;
  Sign  = ".SW";
  Model = ".SW";
  Name  = "SW";

  Props.append(new Property("Param", "R1", true, "parameter to sweep"));
  Props.append(new Property("Start", "5 Ohm", true, "start value for sweep"));
  Props.append(new Property("Stop", "50 Ohm", true, "stop value for sweep"));
  Props.append(new Property("Step", "5 Ohm", true, "step size for sweep"));
}

Param_Sweep::~Param_Sweep()
{
}

Param_Sweep* Param_Sweep::newOne()
{
  return new Param_Sweep();
}
