/***************************************************************************
                          circulator.cpp  -  description
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

#include "circulator.h"


Circulator::Circulator()
{
  Description = "circulator";

  Arcs.append(new Arc(-14,-14, 29, 29,  0,16*360,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-30,  0,-14,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 30,  0, 14,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0, 14,  0, 30,QPen(QPen::darkBlue,2)));

  Arcs.append(new Arc( -8, -6, 17, 17,16*20,16*150,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  8,  0,  9, -7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  8,  0,  2, -1,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));
  Ports.append(new Port(  0, 30));

  x1 = -30; y1 = -16;
  x2 =  30; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Sign  = "Circulator";
  Model = "Circulator";
  Name  = "X";
}

Circulator::~Circulator()
{
}

Circulator* Circulator::newOne()
{
  return new Circulator();
}
