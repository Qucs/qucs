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
#include "module.h"
#include "globals.h"

namespace{

class iProbe : public Component  {
  iProbe(iProbe const& p) : Component(p) {}
public:
  iProbe();
  ~iProbe();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
private:
  Element* clone() const override{ return new iProbe(*this);}
}d0;
Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "IProbe", &d0);
Module::INSTALL pp("sources", &d0);


iProbe::iProbe() : Component()
{
  setLabel("current probe");
  setTypeName("IProbe");

  Lines.append(new Line(-30,  0,-20,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 30,  0, 20,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20,  0, 20,  0,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  4,  0, -4, -4,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(  4,  0, -4,  4,QPen(Qt::darkBlue,3)));

  Lines.append(new Line(-20,-31, 20,-31,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20,  9, 20,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-20,-31,-20,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 20,-31, 20,  9,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-16,-27, 16,-27,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-16, -9, 16, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-16,-27,-16, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 16,-27, 16, -9,QPen(Qt::darkBlue,2)));

  Arcs.append(new Arc(-20,-23, 39, 39, 16*50, 16*80,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-11,-24, -2, -9,QPen(Qt::darkBlue,2)));

  Ports.append(new ComponentPort(-30,  0));
  Ports.append(new ComponentPort( 30,  0));

  x1 = -30; y1 = -34;
  x2 =  30; y2 =  12;

  tx = x1+4;
  ty = y2+4;
  Model = "IProbe";
  setLabel("Pr");
}

iProbe::~iProbe()
{
}

Component* iProbe::newOne()
{
  return new iProbe();
}

#if 0
Element* iProbe::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Current Probe");
  BitmapFile = (char *) "iprobe";

  if(getNewOne)  return new iProbe();
  return 0;
}
#endif

}
