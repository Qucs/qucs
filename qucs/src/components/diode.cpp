/***************************************************************************
                                diode.cpp
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

class Diode : public MultiViewComponent  {
private:
  Diode(Diode const& p) : MultiViewComponent(p) {}
public:
  Diode();
 ~Diode() {};
  Element* clone() const{ return new Diode(*this); }
  static Element* info(QString&, char* &, bool getNewOne=false);

protected:
  void createSymbol();
}D;
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "Diode", &D);
static Module::INSTALL pp("nonlinear", &D);


Diode::Diode() : MultiViewComponent()
{
  info(Name, bitmap_file);
  Description = QObject::tr("diode");

  Props.append(new Property("Is", "1e-15 A", true,
	QObject::tr("saturation current")));
  Props.append(new Property("N", "1", true,
	QObject::tr("emission coefficient")));
  Props.append(new Property("Cj0", "10 fF", true,
	QObject::tr("zero-bias junction capacitance")));
  Props.append(new Property("M", "0.5", false,
	QObject::tr("grading coefficient")));
  Props.append(new Property("Vj", "0.7 V", false,
	QObject::tr("junction potential")));
  Props.append(new Property("Fc", "0.5", false,
	QObject::tr("forward-bias depletion capacitance coefficient")));
  Props.append(new Property("Cp", "0.0 fF", false,
	QObject::tr("linear capacitance")));
  Props.append(new Property("Isr", "0.0", false,
	QObject::tr("recombination current parameter")));
  Props.append(new Property("Nr", "2.0", false,
	QObject::tr("emission coefficient for Isr")));
  Props.append(new Property("Rs", "0.0 Ohm", false,
	QObject::tr("ohmic series resistance")));
  Props.append(new Property("Tt", "0.0 ps", false,
	QObject::tr("transit time")));
  Props.append(new Property("Ikf", "0", false,
	QObject::tr("high-injection knee current (0=infinity)")));
  Props.append(new Property("Kf", "0.0", false,
	QObject::tr("flicker noise coefficient")));
  Props.append(new Property("Af", "1.0", false,
	QObject::tr("flicker noise exponent")));
  Props.append(new Property("Ffe", "1.0", false,
	QObject::tr("flicker noise frequency exponent")));
  Props.append(new Property("Bv", "0", false,
	QObject::tr("reverse breakdown voltage")));
  Props.append(new Property("Ibv", "1 mA", false,
	QObject::tr("current at reverse breakdown voltage")));
  Props.append(new Property("Temp", "26.85", false,
	QObject::tr("simulation temperature in degree Celsius")));
  Props.append(new Property("Xti", "3.0", false,
	QObject::tr("saturation current temperature exponent")));
  Props.append(new Property("Eg", "1.11", false,
	QObject::tr("energy bandgap in eV")));
  Props.append(new Property("Tbv", "0.0", false,
	QObject::tr("Bv linear temperature coefficient")));
  Props.append(new Property("Trs", "0.0", false,
	QObject::tr("Rs linear temperature coefficient")));
  Props.append(new Property("Ttt1", "0.0", false,
	QObject::tr("Tt linear temperature coefficient")));
  Props.append(new Property("Ttt2", "0.0", false,
	QObject::tr("Tt quadratic temperature coefficient")));
  Props.append(new Property("Tm1", "0.0", false,
	QObject::tr("M linear temperature coefficient")));
  Props.append(new Property("Tm2", "0.0", false,
	QObject::tr("M quadratic temperature coefficient")));
  Props.append(new Property("Tnom", "26.85", false,
	QObject::tr("temperature at which parameters were extracted")));
  Props.append(new Property("Area", "1.0", false,
	QObject::tr("default area for diode")));
  Props.append(new Property("Symbol", "normal", false,
	QObject::tr("schematic symbol")+" [normal, US, Schottky, Zener, Varactor]"));

  createSymbol();
  tx = x1+4;
  ty = y2+4;
  Model = "Diode";
  Name  = "D";
}

Element* Diode::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Diode");
  BitmapFile = (char *) "diode";

  if(getNewOne)  return new Diode();
  return 0;
}

// -------------------------------------------------------
void Diode::createSymbol()
{
  if(Props.getLast()->Value.at(0) == 'V') {
    Lines.append(new Line(-30,  0, -9,  0,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( -6,  0, 30,  0,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( -9, -9, -9,  9,QPen(Qt::darkBlue,2)));
  }
  else if(Props.getLast()->Value.at(0) == 'U') {
    Lines.append(new Line(-30,  0, -6,  0,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  6,  0, 30,  0,QPen(Qt::darkBlue,2)));
  }
  else {
    Lines.append(new Line(-30,  0, 30,  0,QPen(Qt::darkBlue,2)));
  }
  Lines.append(new Line( -6, -9, -6,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  6, -9,  6,  9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -6,  0,  6, -9,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -6,  0,  6,  9,QPen(Qt::darkBlue,2)));

  if(Props.getLast()->Value.at(0) == 'S') {
    Lines.append(new Line( -6, -9,-12,-12,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( -6,  9,  0, 12,QPen(Qt::darkBlue,2)));
  }
  else if(Props.getLast()->Value.at(0) == 'Z') {
    Lines.append(new Line( -6, 9, -1, 9,QPen(Qt::darkBlue,2)));
  }

  Ports.append(new ComponentPort(-30, 0));
  Ports.append(new ComponentPort( 30, 0));

  x1 = -30; y1 = -11;
  x2 =  30; y2 =  11;
}
}
