/***************************************************************************
                               source_ac.cpp
                              ---------------
    copyright            : (C) 2003 by Michael Margraf
                               2018 Felix Salfelder / QUCS
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qucs_globals.h"
#include "module.h"
#include "component.h"

namespace{

class Source_ac : public Component  {
public:
  Source_ac();
  ~Source_ac();
  Component* newOne(){
	  return new Source_ac(*this);
  }
  static Element* info(QString&, char* &, bool getNewOne=false);
}D;
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "Pac", &D);
static Module::INSTALL pp("probes", &D);


Source_ac::Source_ac()
{
  info(Name, bitmap_file);
  Description = QObject::tr("ac power source");

  Lines.append(new Line(-22,-11, 22,-11,QPen(Qt::darkGray,0)));
  Lines.append(new Line(-22, 11, 22, 11,QPen(Qt::darkGray,0)));
  Lines.append(new Line(-22,-11,-22, 11,QPen(Qt::darkGray,0)));
  Lines.append(new Line( 22,-11, 22, 11,QPen(Qt::darkGray,0)));

  Arcs.append(new Arc(-19, -9, 18, 18,     0, 16*360,QPen(Qt::darkBlue,2)));
  Arcs.append(new Arc(-13, -6,  6,  6,16*270, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.append(new Arc(-13,  0,  6,  6, 16*90, 16*180,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  0,-19,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 19,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -1,  0,  3,  0,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(  3, -5, 19, -5,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  3,  5, 19,  5,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  3, -5,  3,  5,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 19, -5, 19,  5,QPen(Qt::darkBlue,2)));

  Lines.append(new Line( 25,  5, 25, 11,QPen(Qt::red,1)));
  Lines.append(new Line( 28,  8, 22,  8,QPen(Qt::red,1)));
  Lines.append(new Line(-25,  5,-25, 11,QPen(Qt::black,1)));

  Ports.append(new ComponentPort( 30,  0));
  Ports.append(new ComponentPort(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Model = "Pac";
  Name  = "P";

  // This property must be the first one !
  Props.append(new Property("Num", "1", true,
		QObject::tr("number of the port")));
  Props.append(new Property("Z", "50 Ohm", true,
		QObject::tr("port impedance")));
  Props.append(new Property("P", "0 dBm", false,
		QObject::tr("(available) ac power in Watts")));
  Props.append(new Property("f", "1 GHz", false,
		QObject::tr("frequency in Hertz")));
  Props.append(new Property("Temp", "26.85", false,
	QObject::tr("simulation temperature in degree Celsius")));

  set_rotated(1);  // fix historical flaw ??
}

Source_ac::~Source_ac()
{
}

Element* Source_ac::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Power Source");
  BitmapFile = (char *) "source";

  if(getNewOne)  return new Source_ac();
  return 0;
}

}
