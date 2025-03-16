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
  Description = QObject::tr("diode");

  Props.push_back(qucs::Property("Is", "1e-15 A", true,
	QObject::tr("saturation current")));
  Props.push_back(qucs::Property("N", "1", true,
	QObject::tr("emission coefficient")));
  Props.push_back(qucs::Property("Cj0", "10 fF", true,
	QObject::tr("zero-bias junction capacitance")));
  Props.push_back(qucs::Property("M", "0.5", false,
	QObject::tr("grading coefficient")));
  Props.push_back(qucs::Property("Vj", "0.7 V", false,
	QObject::tr("junction potential")));
  Props.push_back(qucs::Property("Fc", "0.5", false,
	QObject::tr("forward-bias depletion capacitance coefficient")));
  Props.push_back(qucs::Property("Cp", "0.0 fF", false,
	QObject::tr("linear capacitance")));
  Props.push_back(qucs::Property("Isr", "0.0", false,
	QObject::tr("recombination current parameter")));
  Props.push_back(qucs::Property("Nr", "2.0", false,
	QObject::tr("emission coefficient for Isr")));
  Props.push_back(qucs::Property("Rs", "0.0 Ohm", false,
	QObject::tr("ohmic series resistance")));
  Props.push_back(qucs::Property("Tt", "0.0 ps", false,
	QObject::tr("transit time")));
  Props.push_back(qucs::Property("Ikf", "0", false,
	QObject::tr("high-injection knee current (0=infinity)")));
  Props.push_back(qucs::Property("Kf", "0.0", false,
	QObject::tr("flicker noise coefficient")));
  Props.push_back(qucs::Property("Af", "1.0", false,
	QObject::tr("flicker noise exponent")));
  Props.push_back(qucs::Property("Ffe", "1.0", false,
	QObject::tr("flicker noise frequency exponent")));
  Props.push_back(qucs::Property("Bv", "0", false,
	QObject::tr("reverse breakdown voltage")));
  Props.push_back(qucs::Property("Ibv", "1 mA", false,
	QObject::tr("current at reverse breakdown voltage")));
  Props.push_back(qucs::Property("Temp", "26.85", false,
	QObject::tr("simulation temperature in degree Celsius")));
  Props.push_back(qucs::Property("Xti", "3.0", false,
	QObject::tr("saturation current temperature exponent")));
  Props.push_back(qucs::Property("Eg", "1.11", false,
	QObject::tr("energy bandgap in eV")));
  Props.push_back(qucs::Property("Tbv", "0.0", false,
	QObject::tr("Bv linear temperature coefficient")));
  Props.push_back(qucs::Property("Trs", "0.0", false,
	QObject::tr("Rs linear temperature coefficient")));
  Props.push_back(qucs::Property("Ttt1", "0.0", false,
	QObject::tr("Tt linear temperature coefficient")));
  Props.push_back(qucs::Property("Ttt2", "0.0", false,
	QObject::tr("Tt quadratic temperature coefficient")));
  Props.push_back(qucs::Property("Tm1", "0.0", false,
	QObject::tr("M linear temperature coefficient")));
  Props.push_back(qucs::Property("Tm2", "0.0", false,
	QObject::tr("M quadratic temperature coefficient")));
  Props.push_back(qucs::Property("Tnom", "26.85", false,
	QObject::tr("temperature at which parameters were extracted")));
  Props.push_back(qucs::Property("Area", "1.0", false,
	QObject::tr("default area for diode")));
  Props.push_back(qucs::Property("Symbol", "normal", false,
	QObject::tr("schematic symbol")+" [normal, US, Schottky, Zener, Varactor]"));

  createSymbol();
  set_qucs_text_position(x1+4, y2+4);
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
  if(Props.back().Value.at(0) == 'V') {
    Lines.push_back(qucs::Line(-30,  0, -9,  0,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line( -6,  0, 30,  0,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line( -9, -9, -9,  9,QPen(Qt::darkBlue,2)));
  }
  else if(Props.back().Value.at(0) == 'U') {
    Lines.push_back(qucs::Line(-30,  0, -6,  0,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line(  6,  0, 30,  0,QPen(Qt::darkBlue,2)));
  }
  else {
    Lines.push_back(qucs::Line(-30,  0, 30,  0,QPen(Qt::darkBlue,2)));
  }
  Lines.push_back(qucs::Line( -6, -9, -6,  9,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  6, -9,  6,  9,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -6,  0,  6, -9,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( -6,  0,  6,  9,QPen(Qt::darkBlue,2)));

  if(Props.back().Value.at(0) == 'S') {
    Lines.push_back(qucs::Line( -6, -9,-12,-12,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line( -6,  9,  0, 12,QPen(Qt::darkBlue,2)));
  }
  else if(Props.back().Value.at(0) == 'Z') {
    Lines.push_back(qucs::Line( -6, 9, -1, 9,QPen(Qt::darkBlue,2)));
  }

  Ports.push_back(qucs::Port(-30, 0));
  Ports.push_back(qucs::Port( 30, 0));

  x1 = -30; y1 = -11;
  x2 =  30; y2 =  11;
}
