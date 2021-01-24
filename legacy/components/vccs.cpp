/***************************************************************************
                          vccs.cpp  -  description
                             -------------------
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

class VCCS : public Component  {
public:
  VCCS();
  ~VCCS();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
}D;

Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "VCCS", &D);
Module::INSTALL pp("sources", &D);


VCCS::VCCS()
{
  info(Name, bitmap_file);
  Description = QObject::tr("voltage controlled current source");

  Arcs.append(new Arc(0,-11, 22, 22,  0, 16*360,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 11, -7, 11,  7,QPen(Qt::darkBlue,3)));
  Lines.append(new Line( 11,  6, 15,  1,QPen(Qt::darkBlue,3)));
  Lines.append(new Line( 11,  6,  7,  1,QPen(Qt::darkBlue,3)));

  Lines.append(new Line(-30,-30,-12,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30, 30,-12, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 11,-30, 30,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 11, 30, 30, 30,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-12,-30,-12,-23,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-12, 30,-12, 23,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 11,-30, 11,-11,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 11, 30, 11, 11,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-12,-18,-12, 18,QPen(Qt::darkBlue,1)));
  Lines.append(new Line(-12, 18,-17,  9,QPen(Qt::darkBlue,1)));
  Lines.append(new Line(-12, 18, -7,  9,QPen(Qt::darkBlue,1)));

  Lines.append(new Line(-25,-27, 25,-27,QPen(Qt::darkGray,1)));
  Lines.append(new Line( 25,-27, 25, 27,QPen(Qt::darkGray,1)));
  Lines.append(new Line( 25, 27,-25, 27,QPen(Qt::darkGray,1)));
  Lines.append(new Line(-25, 27,-25,-27,QPen(Qt::darkGray,1)));

  Ports.append(new ComponentPort(-30,-30));
  Ports.append(new ComponentPort( 30,-30));
  Ports.append(new ComponentPort( 30, 30));
  Ports.append(new ComponentPort(-30, 30));

  x1 = -30; y1 = -30;
  x2 =  30; y2 =  30;

  tx = x1+4;
  ty = y2+4;
  Model = "VCCS";
  Name  = "SRC";

  Props.append(new Property("G", "1 S", true,
		QObject::tr("forward transconductance")));
  Props.append(new Property("T", "0", false, QObject::tr("delay time")));
}

VCCS::~VCCS()
{
}

Component* VCCS::newOne()
{
  return new VCCS();
}

Element* VCCS::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Voltage Controlled Current Source");
  BitmapFile = (char *) "vccs";

  if(getNewOne)  return new VCCS();
  return 0;
}

}
