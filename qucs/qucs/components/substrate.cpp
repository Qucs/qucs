/***************************************************************************
                          substrate.cpp  -  description
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

#include "substrate.h"


Substrate::Substrate()
{
  Description = "substrate definition";

  Lines.append(new Line(-30,-16, 30,-16,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,-12, 30,-12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30, 16, 30, 16,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30, 12, 30, 12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,-16,-30, 16,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,-16, 30, 16,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-30,-16, 16,-40,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,-16, 80,-40,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,-12, 80,-36,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30, 12, 80,-16,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30, 16, 80,-12,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 16,-40, 80,-40,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 80,-40, 80,-12,QPen(QPen::darkBlue,2)));
  
  Lines.append(new Line(-30,  0,-18,-12,QPen(QPen::darkBlue,1)));
  Lines.append(new Line(-22, 12,  2,-12,QPen(QPen::darkBlue,1)));
  Lines.append(new Line( -2, 12, 22,-12,QPen(QPen::darkBlue,1)));
  Lines.append(new Line( 18, 12, 30,  0,QPen(QPen::darkBlue,1)));

  Lines.append(new Line( 30,  1, 37,  8,QPen(QPen::darkBlue,1)));
  Lines.append(new Line( 37,-15, 52,  0,QPen(QPen::darkBlue,1)));
  Lines.append(new Line( 52,-22, 66, -8,QPen(QPen::darkBlue,1)));
  Lines.append(new Line( 66,-30, 80,-16,QPen(QPen::darkBlue,1)));

  x1 = -30; y1 =-40;
  x2 =  80; y2 = 16;

  tx = x1+4;
  ty = y2+4;
  Sign  = "SUBST";
  Model = "SUBST";
  Name  = "Subst";

  Props.append(new Property("er", "9.8", true, "permittivity"));
  Props.append(new Property("h", "1 mm", true, "thickness in meters"));
  Props.append(new Property("t", "35 um", true, "thickness of metalization"));
  Props.append(new Property("tand", "0", true, "loss tangent"));
}

Substrate::~Substrate()
{
}

Substrate* Substrate::newOne()
{
  return new Substrate();
}
