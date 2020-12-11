/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2020 Felix Salfelder
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
#include "globals.h"
#include "module.h"

namespace{
class Ampere_ac : public Component  {
private:
	Ampere_ac(Ampere_ac const&) = default;
public:
  Ampere_ac();
  ~Ampere_ac() {}
private:
  Element* clone() const override{
	  return new Ampere_ac(*this);
  }

  static Element* info(QString&, char* &, bool getNewOne=false);
}D;
Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "Iac", &D);
Module::INSTALL pp("sources", &D);

Ampere_ac::Ampere_ac() : Component()
{
  Description = QObject::tr("ideal ac current source");

  Arcs.append(new Arc(-12,-12, 24, 24,  0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -7,  0,  7,  0,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0, -4,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  6,  0,  0,  4,QPen(Qt::darkBlue,3)));
  Arcs.append(new Arc( 12,  5,  6,  6,16*270, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.append(new Arc( 12, 11,  6,  6, 16*90, 16*180,QPen(Qt::darkBlue,2)));

  Ports.append(new ComponentPort( 30,  0));
  Ports.append(new ComponentPort(-30,  0));

  x1 = -30; y1 = -14; // 16???
  x2 =  30; y2 =  16;

  tx = x1+4;
  ty = y2+4;
  Model = "Iac";
  Name  = "I";

  Props.append(new Property("I", "1 mA", true,
		QObject::tr("peak current in Ampere")));
  Props.append(new Property("f", "1 GHz", false,
		QObject::tr("frequency in Hertz")));
  Props.append(new Property("Phase", "0", false,
		QObject::tr("initial phase in degrees")));
  Props.append(new Property("Theta", "0", false,
		QObject::tr("damping factor (transient simulation only)")));

  set_rotated(1);  // fix historical flaw ??
}

#if 0
Element* Ampere_ac::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("ac Current Source");
  BitmapFile = (char *) "ac_current";

  if(getNewOne)  return new Ampere_ac();
  return 0;
}
#endif

}
