/***************************************************************************
                                diode.cpp
                               -----------
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

#include "diode.h"


Diode::Diode()
{
  Description = QObject::tr("Diode");

  Props.append(new Property("Is", "1e-15 A", true,
	QObject::tr("Saturation current")));
  Props.append(new Property("N", "1", true,
	QObject::tr("Emission coefficient")));
  Props.append(new Property("Cj0", "10 fF", true,
	QObject::tr("Zero-bias junction capacitance")));
  Props.append(new Property("M", "0.5", false,
	QObject::tr("Grading coefficient")));
  Props.append(new Property("Vj", "0.7 V", false,
	QObject::tr("Junction potential")));
  Props.append(new Property("Fc", "0.5", false,
	QObject::tr("Forward-bias depletion capacitance coefficient")));
  Props.append(new Property("Cp", "0.0 fF", false,
	QObject::tr("Linear capacitance")));
  Props.append(new Property("Isr", "0.0", false,
	QObject::tr("Recombination current parameter")));
  Props.append(new Property("Nr", "2.0", false,
	QObject::tr("Emission coefficient for Isr")));
  Props.append(new Property("Rs", "0.0 Ohm", false,
	QObject::tr("Ohmic series resistance")));
  Props.append(new Property("Tt", "0.0 ps", false,
	QObject::tr("Transit time")));
  Props.append(new Property("Ikf", "0", false,
	QObject::tr("High-injection knee current (0=infinity)")));
  Props.append(new Property("Kf", "0.0", false,
	QObject::tr("Flicker noise coefficient")));
  Props.append(new Property("Af", "1.0", false,
	QObject::tr("Flicker noise exponent")));
  Props.append(new Property("Ffe", "1.0", false,
	QObject::tr("Flicker noise frequency exponent")));
  Props.append(new Property("Bv", "0", false,
	QObject::tr("Reverse breakdown voltage")));
  Props.append(new Property("Ibv", "1 mA", false,
	QObject::tr("Current at reverse breakdown voltage")));
  Props.append(new Property("Temp", "26.85", false,
	QObject::tr("Simulation temperature in degrees Celsius")));
  Props.append(new Property("Xti", "3.0", false,
	QObject::tr("Saturation current temperature exponent")));
  Props.append(new Property("Eg", "1.11", false,
	QObject::tr("Energy bandgap in eV")));
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
	QObject::tr("Temperature at which parameters were extracted")));
  Props.append(new Property("Area", "1.0", false,
	QObject::tr("Default area for diode")));
  Props.append(new Property("Symbol", "normal", false,
	QObject::tr("Schematic symbol")+" [normal, US, Schottky, Zener, Varactor]"));

  createSymbol();
  tx = x1+4;
  ty = y2+4;
  Model = "Diode";
  Name  = "D";
}

Component* Diode::newOne()
{
  return new Diode();
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

  Ports.append(new Port(-30, 0));
  Ports.append(new Port( 30, 0));

  x1 = -30; y1 = -11;
  x2 =  30; y2 =  11;
}
