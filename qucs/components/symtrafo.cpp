/***************************************************************************
                          symtrafo.cpp  -  description
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

#include "symtrafo.h"


symTrafo::symTrafo()
{
  Description = QObject::tr("ideal symmetrical transformer");

  Arcs.append(new Arc(-16,-58, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16,-46, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16,-34, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16, 46, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16, 34, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16, 22, 13, 13, 16*270, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4,-18, 13, 13,  16*90, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4, -6, 13, 13,  16*90, 16*180,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4,  5, 13, 13,  16*90, 16*180,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-58,-10,-70,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-70,-30,-70,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10,-18, 10,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10,-30, 30,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 58,-10, 70,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 70,-30, 70,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10, 17, 10, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10, 30, 30, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-10,-30,-10,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-22,-10,-10,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 10,-30, 10,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 10,-10, 22,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -1,-57, -1, 57,QPen(QPen::darkBlue,1))); // core lines
  Lines.append(new Line(  1,-57,  1, 57,QPen(QPen::darkBlue,1)));

  Texts.append(new Text(-23,-47,"T1"));
  Texts.append(new Text(-23, 32,"T2"));

  Arcs.append(new Arc(-21,-64,  6,  6,  0, 16*360,QPen(QPen::darkBlue,2))); // mark the turn direction
  Arcs.append(new Arc(-21, 15,  6,  6,  0, 16*360,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( 15,-24,  6,  6,  0, 16*360,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,-70));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 70));
  Ports.append(new Port(-30, 10));
  Ports.append(new Port(-30,-10));

  x1 = -33; y1 = -73;
  x2 =  33; y2 =  73;

  tx = x1+4;
  ty = y2+4;
  Sign  = "sTr";
  Model = "sTr";
  Name  = "Tr";

  Props.append(new Property("T1", "1", true, QObject::tr("voltage transformation ratio of coil 1")));
  Props.append(new Property("T2", "1", true, QObject::tr("voltage transformation ratio of coil 2")));
}

symTrafo::~symTrafo()
{
}

symTrafo* symTrafo::newOne()
{
  return new symTrafo();
}
