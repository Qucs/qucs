/***************************************************************************
                          vccs.cpp  -  description
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

#include "vccs.h"


VCCS::VCCS()
{
  Description = "voltage controlled current source";

  Arcs.append(new Arc(0,-11, 23, 23,  0, 16*360,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11, -7, 11,  7,QPen(QPen::darkBlue,3)));
  Lines.append(new Line( 11,  6, 15,  1,QPen(QPen::darkBlue,3)));
  Lines.append(new Line( 11,  6,  7,  1,QPen(QPen::darkBlue,3)));

  Lines.append(new Line(-30,-30,-12,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30, 30,-12, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11,-30, 30,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11, 30, 30, 30,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-12,-30,-12,-23,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-12, 30,-12, 23,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11,-30, 11,-11,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 11, 30, 11, 11,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-12,-18,-12, 18,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-12, 18,-17,  9,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-12, 18, -8,  9,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-25,-27, 25,-27,QPen(QPen::darkGray,1)));
  Lines.append(new Line( 25,-27, 25, 27,QPen(QPen::darkGray,1)));
  Lines.append(new Line( 25, 27,-25, 27,QPen(QPen::darkGray,1)));
  Lines.append(new Line(-25, 27,-25,-27,QPen(QPen::darkGray,1)));

  Ports.append(new Port(-30,-30));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 30));

  x1 = -30; y1 = -30;
  x2 =  30; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Sign  = "VCCS";
  Model = "VCCS";
  Name  = "SRC";

  Props.append(new Property("G", "1 S", true, "forward transconductance"));
  Props.append(new Property("T", "0", true, "delay time"));
}

VCCS::~VCCS()
{
}

VCCS* VCCS::newOne()
{
  return new VCCS();
}
