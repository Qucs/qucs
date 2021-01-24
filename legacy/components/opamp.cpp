/***************************************************************************
    copyright            : (C) 2004 by Michael Margraf
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
#include "qucs_globals.h"
#include "module.h"
namespace{


class OpAmp : public Component  {
public:
  OpAmp();
  ~OpAmp();
  Symbol* clone() const{return new OpAmp(*this);}
  static Element* info(QString&, char* &, bool getNewOne=false);
}d0;
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "OpAmp", &d0);
static Module::INSTALL pp("lumped", &d0);

OpAmp::OpAmp() : Component()
{
  Description = QObject::tr("operational amplifier");

  Lines.append(new Line(-30,-20,-20,-20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-30, 20,-20, 20,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 40,  0,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-20,-35,-20, 35,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20,-35, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20, 35, 30,  0,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-16, 19, -9, 19,QPen(Qt::black,1)));
  Lines.append(new Line(-16,-19, -9,-19,QPen(Qt::red,1)));
  Lines.append(new Line(-13,-22,-13,-15,QPen(Qt::red,1)));

  Ports.append(new ComponentPort(-30, 20));
  Ports.append(new ComponentPort(-30,-20));
  Ports.append(new ComponentPort( 40,  0));

  x1 = -30; y1 = -38;
  x2 =  30; y2 =  38;

  tx = x1+4;
  ty = y2+4;
  Model = "OpAmp";
  Name  = "OP";

  Props.append(new Property("G", "1e6", true,
		QObject::tr("voltage gain")));
  Props.append(new Property("Umax", "15 V", false,
	QObject::tr("absolute value of maximum and minimum output voltage")));
}

OpAmp::~OpAmp()
{
}

#if 0
Element* OpAmp::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("OpAmp");
  BitmapFile = (char *) "opamp";

  if(getNewOne)  return new OpAmp();
  return 0;
}
#endif

}
