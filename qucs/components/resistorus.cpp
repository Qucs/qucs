/***************************************************************************
                          resistorus.cpp  -  description
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

#include "resistorus.h"

ResistorUS::ResistorUS()
{
  Description = QObject::tr("resistor (US symbol)");

  Lines.append(new Line(-30,  0,-18,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-18,  0,-15, -7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-15, -7, -9,  7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -9,  7, -3, -7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -3, -7,  3,  7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  3,  7,  9, -7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  9, -7, 15,  7,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 15,  7, 18,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -9;
  x2 =  30; y2 =  9;

  tx = x1+4;
  ty = y2+4;
  Sign  = "Rus";
  Model = "R";
  Name  = "R";

  Props.append(new Property("R", "50 Ohm", true, QObject::tr("ohmic resistance in Ohms")));
//  Props.append(new Property("Noise", "yes", false, QObject::tr("thermal noise yes or no")));
}

ResistorUS::~ResistorUS()
{
}

ResistorUS* ResistorUS::newOne()
{
  return new ResistorUS();
}
