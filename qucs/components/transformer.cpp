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

  Arcs.push_back(qucs::Arc(-16,-18,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(-16, -6,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(-16,  6,12,12, 16*270,16*180, QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(  4,-18,12,12,  16*90,16*180, QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(  4, -6,12,12,  16*90,16*180, QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc(  4,  6,12,12,  16*90,16*180, QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10,-18,-10,-30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10,-30,-30,-30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 10,-18, 10,-30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 10,-30, 30,-30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10, 18,-10, 30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10, 30,-30, 30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 10, 18, 10, 30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 10, 30, 30, 30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -1,-20, -1, 20,QPen(Qt::darkBlue,1)));
  Lines.push_back(qucs::Line(  1,-20,  1, 20,QPen(Qt::darkBlue,1)));

  Texts.push_back(qucs::Text(-21, -18,"T"));
  Arcs.push_back(qucs::Arc(-21,-24,  5,  5,  0, 16*360,QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc( 15,-24,  5,  5,  0, 16*360,QPen(Qt::darkBlue,2)));


  Ports.push_back(qucs::Port(-30,-30));
  Ports.push_back(qucs::Port( 30,-30));
  Ports.push_back(qucs::Port( 30, 30));
  Ports.push_back(qucs::Port(-30, 30));

  set_x1(-33);
  set_y1(-34);
  set_x2(33);
  set_y2(34);

  set_qucs_text_position(x1()+4, y2()+4);
  Model = "Tr";
  Name  = "Tr";

  Props.push_back(qucs::Property("T", "1", true,
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
  BitmapFile = (char *) "transformer";

  if(getNewOne)  return new Transformer();
  return 0;
}
