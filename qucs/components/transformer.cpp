/***************************************************************************
                          transformer.cpp  -  description
                             -------------------
    begin                : Sat Aug 23 2003
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

#include "transformer.h"


Transformer::Transformer()
{
  Description = QObject::tr("ideal transformer");

  Arcs.append(new Arc(-16,-18,13,13, 16*270,16*180, QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16, -6,13,13, 16*270,16*180, QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(-16,  6,13,13, 16*270,16*180, QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4,-18,13,13,  16*90,16*180, QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4, -6,13,13,  16*90,16*180, QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc(  4,  6,13,13,  16*90,16*180, QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-18,-10,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-30,-30,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10,-18, 10,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10,-30, 30,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 18,-10, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 30,-30, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10, 18, 10, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 10, 30, 30, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -1,-20, -1, 20,QPen(QPen::darkBlue,1)));
  Lines.append(new Line(  1,-20,  1, 20,QPen(QPen::darkBlue,1)));

  Texts.append(new Text(-21, -18,"T"));
  Arcs.append(new Arc(-21,-24,  6,  6,  0, 16*360,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( 15,-24,  6,  6,  0, 16*360,QPen(QPen::darkBlue,2)));


  Ports.append(new Port(-30,-30));
  Ports.append(new Port( 30,-30));
  Ports.append(new Port( 30, 30));
  Ports.append(new Port(-30, 30));

  x1 = -33; y1 = -33;
  x2 =  33; y2 =  33;

  tx = x1+4;
  ty = y2+4;
  Model = "Tr";
  Name  = "Tr";

  Props.append(new Property("T", "1", true,
		QObject::tr("voltage transformation ratio")));
}

Transformer::~Transformer()
{
}

Component* Transformer::newOne()
{
  return new Transformer();
}

Element* Transformer::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Transformer");
  BitmapFile = "transformer";

  if(getNewOne)  return new Transformer();
  return 0;
}
