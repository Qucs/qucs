/***************************************************************************
                          subcirport.cpp  -  description
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

#include "subcirport.h"


SubCirPort::SubCirPort()
{
  Description = "port of a subcircuit";

  Arcs.append(new Arc(-25, -6, 13, 13,  0, 16*360,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-14,  0,  0,  0,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(  0,  0));

  x1 = -27; y1 = -8;
  x2 =   0; y2 =  8;

  tx = x1+4;
  ty = y2+4;
  Sign  = "Port";
  Model = "";
  Name  = "P";

  Props.append(new Property("Num", "1", true));
}

SubCirPort::~SubCirPort()
{
}

SubCirPort* SubCirPort::newOne()
{
  return new SubCirPort();
}
