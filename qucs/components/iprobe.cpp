/***************************************************************************
                          iprobe.cpp  -  description
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

#include "iprobe.h"


iProbe::iProbe()
{
  Description = QObject::tr("current probe");

  Lines.append(new Line(-30,  0, 30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -2, -3,  2,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -2,  3,  2,  0,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-20,-22, 20,-22,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-20,  6, 20,  6,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-20,-22, 20,  6,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 20,-22, 20,  6,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -11;
  x2 =  30; y2 =  11;

  tx = x1+4;
  ty = y2+4;
  Sign  = "IProbe";
  Model = "IProbe";
  Name  = "Pr";
}

iProbe::~iProbe()
{
}

iProbe* iProbe::newOne()
{
  return new iProbe();
}
