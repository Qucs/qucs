/***************************************************************************
                          capacitor.cpp  -  description
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

#include "globals.h"
#include "component.h"
#include "module.h"

namespace {

class Capacitor : public MultiViewComponent  {
public:
  Capacitor();
 ~Capacitor() {};
  Object* newOne() const {return new Capacitor(*this);}
  static Element* info(QString&, char* &, bool getNewOne=false);

protected:
  void createSymbol();
};


Capacitor::Capacitor()
{
info(Name, bitmap_file);

  Description = QObject::tr("capacitor");

  Props.append(new Property("C", "1 pF", true,
		QObject::tr("capacitance in Farad")));
  Props.append(new Property("V", "", false,
		QObject::tr("initial voltage for transient simulation")));
  Props.append(new Property("Symbol", "neutral", false,
	QObject::tr("schematic symbol")+" [neutral, polar]"));

  createSymbol();
  tx = x1+4;
  ty = y2+4;
  Model = "C";
  Name  = "C";
}

Capacitor C;

static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "C", &C);
// the toolbox item
static Module::INSTALL pp("lumped", &C);

Element* Capacitor::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Capacitor");
  BitmapFile = (char *) "capacitor";

  if(getNewOne)  return new Capacitor();
  return 0;
}

void Capacitor::createSymbol()
{
  if(Props.getLast()->Value.at(0) == 'n') {
    Lines.append(new Line( -4,-11, -4, 11,QPen(Qt::darkBlue,4)));
    Lines.append(new Line(  4,-11,  4, 11,QPen(Qt::darkBlue,4)));
  }
  else {
    Lines.append(new Line(-11, -5,-11,-11,QPen(Qt::red,1)));
    Lines.append(new Line(-14, -8, -8, -8,QPen(Qt::red,1)));
    Lines.append(new Line( -4,-11, -4, 11,QPen(Qt::darkBlue,3)));
    Arcs.append(new Arc(4,-12, 20, 24, 16*122, 16*116,QPen(Qt::darkBlue,3)));
  }

  Lines.append(new Line(-30,  0, -4,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  4,  0, 30,  0,QPen(Qt::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -13;
  x2 =  30; y2 =  13;
}

}
