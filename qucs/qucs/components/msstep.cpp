/***************************************************************************
                          msstep.cpp  -  description
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

#include "msstep.h"


MSstep::MSstep()
{
  Description = "microstrip impedance step";

  Lines.append(new Line(-30,  0,-18,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18,-12,  0,-12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18, 12,  0, 12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18,-12,-18, 12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0, -7, 18, -7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,  7, 18,  7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18, -7, 18,  7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,-12,  0, -7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,  7,  0, 12,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-22, -4,-26,  4,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));

  x1 = -30; y1 =-13;
  x2 =  30; y2 = 13;

  tx = x1+4;
  ty = y2+4;
  Sign  = "MSTEP";
  Model = "MSTEP";
  Name  = "MS";

  Props.append(new Property("Subst", "Subst1", true, "substrate"));
  Props.append(new Property("W1", "2 mm", true, "width 1 of the line"));
  Props.append(new Property("W2", "1 mm", true, "width 2 of the line"));
}

MSstep::~MSstep()
{
}

MSstep* MSstep::newOne()
{
  return new MSstep();
}
