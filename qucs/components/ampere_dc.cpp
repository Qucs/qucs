/***************************************************************************
                               ampere_dc.cpp
                              ---------------
    copyright            : (C) 2003 by Michael Margraf
                               2019 Felix Salfelder / QUCS team
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "globals.h"
#include "module.h"
#include "component.h"


namespace{

class Ampere_dc : public Component  {
public:
  Ampere_dc();
  ~Ampere_dc();
  Component* newOne() {
	  return new Ampere_dc(*this);
  }
  static Element* info(QString&, char* &, bool getNewOne=false);
}D;
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "Idc", &D);
static Module::INSTALL pp("sources", &D);


Ampere_dc::Ampere_dc()
{
  info(Name, bitmap_file);
  Description = QObject::tr("ideal dc current source");

  Arcs.append(new Arc(-12,-12, 24, 24,  0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -7,  0,  7,  0,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0, -4,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0,  4,QPen(Qt::darkBlue,3)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Model = "Idc";
  Name  = "I";

  Props.append(new Property("I", "1 mA", true,
		QObject::tr("current in Ampere")));

  rotate();  // fix historical flaw
}

Ampere_dc::~Ampere_dc()
{
}

Element* Ampere_dc::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("dc Current Source");
  BitmapFile = (char *) "dc_current";

  if(getNewOne)  return new Ampere_dc();
  return 0;
}

}
