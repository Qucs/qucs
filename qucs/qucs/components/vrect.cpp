/***************************************************************************
                                vrect.cpp
                               -----------
    begin                : Sat Sep 18 2004
    copyright            : (C) 2004 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "component.h"
#include "module.h"
#include "globals.h"

namespace{

class vRect : public Component  {
public:
  vRect();
  ~vRect();
  Object* newOne() const;
  static Element* info(QString&, char* &, bool getNewOne=false);
}D;

Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "Vrect", &D);
Module::INSTALL pp("sources", &D);

vRect::vRect()
{
  Description = QObject::tr("ideal rectangle voltage source");

  Arcs.append(new Arc(-12,-12, 24, 24,     0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 18,  5, 18, 11,QPen(Qt::red,1)));
  Lines.append(new Line( 21,  8, 15,  8,QPen(Qt::red,1)));
  Lines.append(new Line(-18,  5,-18, 11,QPen(Qt::black,1)));

  Lines.append(new Line( -5, -7, -5, -5,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -5, -5,  5, -5,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  5, -5,  6,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -5,  0,  6,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -5,  0, -5,  5,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -5,  5,  5,  5,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  5,  5,  5,  7,QPen(Qt::darkBlue,2)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Model = "Vrect";
  Name  = "V";

  Props.append(new Property("U", "1 V", true,
		QObject::tr("voltage of high signal")));
  Props.append(new Property("TH", "1 ms", true,
		QObject::tr("duration of high pulses")));
  Props.append(new Property("TL", "1 ms", true,
		QObject::tr("duration of low pulses")));
  Props.append(new Property("Tr", "1 ns", false,
		QObject::tr("rise time of the leading edge")));
  Props.append(new Property("Tf", "1 ns", false,
		QObject::tr("fall time of the trailing edge")));
  Props.append(new Property("Td", "0 ns", false,
		QObject::tr("initial delay time")));

  rotate();  // fix historical flaw
}

vRect::~vRect()
{
}

Object* vRect::newOne() const
{
  return new vRect();
}

Element* vRect::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Rectangle Voltage");
  BitmapFile = (char *) "vrect";

  if(getNewOne)  return new vRect();
  return 0;
}

}
