/***************************************************************************
                          capacitor.cpp  -  description
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

#include "capacitor.h"


Capacitor::Capacitor()
{
  Description = QObject::tr("capacitor");

  Props.push_back(qucs::Property("C", "1 pF", true,
		QObject::tr("capacitance in Farad")));
  Props.push_back(qucs::Property("V", "", false,
		QObject::tr("initial voltage for transient simulation")));
  Props.push_back(qucs::Property("Symbol", "neutral", false,
	QObject::tr("schematic symbol")+" [neutral, polar]"));

  createSymbol();
  set_qucs_text_position(x1+4, y2+4);
  Model = "C";
  Name  = "C";
}

Component* Capacitor::newOne()
{
  return new Capacitor();
}

Element* Capacitor::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Capacitor");
  BitmapFile = (char *) "capacitor";

  if(getNewOne)  return new Capacitor();
  return 0;
}

void Capacitor::createSymbol()
{
  if(Props.back().Value.at(0) == 'n') {
    Lines.push_back(qucs::Line( -4,-11, -4, 11,QPen(Qt::darkBlue,4)));
    Lines.push_back(qucs::Line(  4,-11,  4, 11,QPen(Qt::darkBlue,4)));
  }
  else {
    Lines.push_back(qucs::Line(-11, -5,-11,-11,QPen(Qt::red,1)));
    Lines.push_back(qucs::Line(-14, -8, -8, -8,QPen(Qt::red,1)));
    Lines.push_back(qucs::Line( -4,-11, -4, 11,QPen(Qt::darkBlue,3)));
    Arcs.push_back(qucs::Arc(4,-12, 20, 24, 16*122, 16*116,QPen(Qt::darkBlue,3)));
  }

  Lines.push_back(qucs::Line(-30,  0, -4,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  4,  0, 30,  0,QPen(Qt::darkBlue,2)));

  Ports.push_back(qucs::Port(-30,  0));
  Ports.push_back(qucs::Port( 30,  0));

  x1 = -30; y1 = -13;
  x2 =  30; y2 =  13;
}
