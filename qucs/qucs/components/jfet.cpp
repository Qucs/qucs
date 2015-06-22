/***************************************************************************
                                jfet.cpp
                               ----------
    begin                : Fri Jun 4 2004
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

#include "jfet.h"


JFET::JFET()
{
  Description = QObject::tr("Junction field-effect transistor");

  // this must be the first property in the list !!!
  Props.append(new Property("Type", "nfet", true,
	QObject::tr("Polarity")+" [nfet, pfet]"));
  Props.append(new Property("Vt0", "-2.0 V", true,
	QObject::tr("Threshold voltage")));
  Props.append(new Property("Beta", "1e-4", true,
	QObject::tr("Transconductance parameter")));
  Props.append(new Property("Lambda", "0.0", true,
	QObject::tr("Channel-length modulation parameter")));
  Props.append(new Property("Rd", "0.0", false,
	QObject::tr("Parasitic drain resistance")));
  Props.append(new Property("Rs", "0.0", false,
	QObject::tr("Parasitic source resistance")));
  Props.append(new Property("Is", "1e-14", false,
	QObject::tr("Gate-junction saturation current")));
  Props.append(new Property("N", "1.0", false,
	QObject::tr("Gate-junction emission coefficient")));
  Props.append(new Property("Isr", "1e-14", false,
	QObject::tr("Gate-junction recombination current parameter")));
  Props.append(new Property("Nr", "2.0", false,
	QObject::tr("Isr emission coefficient")));
  Props.append(new Property("Cgs", "0.0", false,
	QObject::tr("Zero-bias gate-source junction capacitance")));
  Props.append(new Property("Cgd", "0.0", false,
	QObject::tr("Zero-bias gate-drain junction capacitance")));
  Props.append(new Property("Pb", "1.0", false,
	QObject::tr("Gate-junction potential")));
  Props.append(new Property("Fc", "0.5", false,
	QObject::tr("Forward-bias junction capacitance coefficient")));
  Props.append(new Property("M", "0.5", false,
	QObject::tr("Gate P-N grading coefficient")));
  Props.append(new Property("Kf", "0.0", false,
	QObject::tr("Flicker noise coefficient")));
  Props.append(new Property("Af", "1.0", false,
	QObject::tr("Flicker noise exponent")));
  Props.append(new Property("Ffe", "1.0", false,
	QObject::tr("Flicker noise frequency exponent")));
  Props.append(new Property("Temp", "26.85", false,
	QObject::tr("Simulation temperature in degrees Celsius")));
  Props.append(new Property("Xti", "3.0", false,
	QObject::tr("Saturation current temperature exponent")));
  Props.append(new Property("Vt0tc", "0.0", false,
	QObject::tr("Vt0 temperature coefficient")));
  Props.append(new Property("Betatce", "0.0", false,
	QObject::tr("Beta exponential temperature coefficient")));
  Props.append(new Property("Tnom", "26.85", false,
	QObject::tr("Temperature at which parameters were extracted")));
  Props.append(new Property("Area", "1.0", false,
	QObject::tr("Default area for JFET")));

  createSymbol();
  tx = x2+4;
  ty = y1+4;
  Model = "JFET";
  Name  = "T";
}

// -------------------------------------------------------
Component* JFET::newOne()
{
  JFET* p = new JFET();
  p->Props.getFirst()->Value = Props.getFirst()->Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* JFET::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("N-JFET");
  BitmapFile = (char *) "nfet";

  if(getNewOne)  return new JFET();
  return 0;
}

// -------------------------------------------------------
Element* JFET::info_p(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("P-JFET");
  BitmapFile = (char *) "pfet";

  if(getNewOne) {
    JFET* p = new JFET();
    p->Props.getFirst()->Value = "pfet";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
void JFET::createSymbol()
{
  Lines.append(new Line(-10,-15,-10, 15,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-30,  0,-10,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10,-10,  0,-10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0,-10,  0,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10, 10,  0, 10,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, 10,  0, 30,QPen(Qt::darkBlue,2)));

  Lines.append(new Line( -4, 24,  4, 20,QPen(Qt::darkBlue,2)));

  if(Props.getFirst()->Value == "nfet") {
    Lines.append(new Line(-16, -5,-11,  0,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-16,  5,-11,  0,QPen(Qt::darkBlue,2)));
  }
  else {
    Lines.append(new Line(-18,  0,-13, -5,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(-18,  0,-13,  5,QPen(Qt::darkBlue,2)));
  }

  Ports.append(new Port(-30,  0));
  Ports.append(new Port(  0,-30));
  Ports.append(new Port(  0, 30));

  x1 = -30; y1 = -30;
  x2 =   4; y2 =  30;
}
