/***************************************************************************
                          msline.cpp  -  description
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

#include "msline.h"


MSline::MSline()
{
  Description = QObject::tr("microstrip line");

  Lines.append(new Line(-30,  0,-18,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 18,  0, 30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-13, -8, 23, -8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-23,  8, 13,  8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-13, -8,-23,  8,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 23, -8, 13,  8,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-22, -4,-26,  4,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));

  x1 = -30; y1 =-11;
  x2 =  30; y2 = 11;

  tx = x1+4;
  ty = y2+4;
  Sign  = "MLIN";
  Model = "MLIN";
  Name  = "MS";

  Props.append(new Property("Subst", "Subst1", true, QObject::tr("name of substrate definition")));
  Props.append(new Property("W", "1 mm", true, QObject::tr("width of the line")));
  Props.append(new Property("L", "10 mm", true, QObject::tr("length of the line")));
  Props.append(new Property("Model", "Kirschning", false, QObject::tr("microstrip model (Kirschning,Kobayashi,Yamashita)")));
}

MSline::~MSline()
{
}

MSline* MSline::newOne()
{
  return new MSline();
}
