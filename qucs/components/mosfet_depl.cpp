/***************************************************************************
                       mosfet_depl.cpp  -  description
                             -------------------
    begin                : Fri Jun 4 2004
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "mosfet_depl.h"


MOSFET_depl::MOSFET_depl()
{
  Description = QObject::tr("depletion MOS field-effect transistor");

  Lines.append(new Line(-14,-13,-14, 13,QPen(QPen::darkBlue,3)));
  Lines.append(new Line(-30,  0,-14,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-16,-10, 16,QPen(QPen::darkBlue,3)));

  Lines.append(new Line(-10,-11,  0,-11,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,-11,  0,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 11,  0, 11,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,  0,  0, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,  0,  0,  0,QPen(QPen::darkBlue,2)));

  Lines.append(new Line( -9,  0, -4, -5,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -9,  0, -4,  5,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port(  0, 30));
  Ports.append(new Port(  0,-30));

  x1 = -30; y1 = -30;
  x2 =   4; y2 =  30;

  tx = x2+4;
  ty = y1+4;
  Model = "DMOSFET";
  Name  = "T";

//  Props.append(new Property("Type", "n", true,
//			QObject::tr("polarity (n,p)")));
}

MOSFET_depl::~MOSFET_depl()
{
}

Component* MOSFET_depl::newOne()
{
  return new MOSFET_depl();
}

Component* MOSFET_depl::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("depletion MOSFET");
  BitmapFile = "dmosfet";

  if(getNewOne)  return new MOSFET_depl();
  return 0;
}
