/***************************************************************************
                          mosfet.cpp  -  description
                             -------------------
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

#include "mosfet.h"


MOSFET::MOSFET()
{
  Description = QObject::tr("MOS field-effect transistor");

  Lines.append(new Line(-14,-13,-14, 13,QPen(QPen::darkBlue,3)));
  Lines.append(new Line(-30,  0,-14,  0,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-10,-11,  0,-11,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,-11,  0,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 11,  0, 11,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,  0,  0, 30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,  0,  0,  0,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-10,-16,-10, -7,QPen(QPen::darkBlue,3)));
  Lines.append(new Line(-10,  7,-10, 16,QPen(QPen::darkBlue,3)));

  // These three lines must be the last.
  Lines.append(new Line(-10, -4,-10,  4,QPen(QPen::darkBlue,3)));
  Lines.append(new Line( -9,  0, -4, -5,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( -9,  0, -4,  5,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port(  0,-30));
  Ports.append(new Port(  0, 30));

  x1 = -30; y1 = -30;
  x2 =   4; y2 =  30;

  tx = x2+4;
  ty = y1+4;
  Model = "_MOSFET";
  Name  = "T";

  // these must be the first properties in the list !!!
  Props.append(new Property("Type", "nfet", false,
	QObject::tr("polarity (nfet,pfet)")));
  Props.append(new Property("Vt0", "1.0 V", true,
	QObject::tr("zero-bias threshold voltage")));

  Props.append(new Property("Kp", "2e-5", true,
	QObject::tr("transconductance coefficient in A/m^2")));
  Props.append(new Property("Gamma", "0.0", false,
	QObject::tr("bulk threshold in sqrt(V)")));
  Props.append(new Property("Phi", "0.6 V", false,
	QObject::tr("surface potential")));
  Props.append(new Property("Lambda", "0.0", true,
	QObject::tr("channel-length modulation parameter")));
  Props.append(new Property("Rd", "0.0 Ohm", false,
	QObject::tr("drain ohmic resistance")));
  Props.append(new Property("Rs", "0.0 Ohm", false,
	QObject::tr("source ohmic resistance")));
  Props.append(new Property("Rg", "0.0 Ohm", false,
	QObject::tr("gate ohmic resistance")));
  Props.append(new Property("Is", "1e-14 A", false,
	QObject::tr("bulk junction saturation current")));
  Props.append(new Property("N", "1.0", false,
	QObject::tr("bulk junction emission coefficient")));
  Props.append(new Property("W", "1 um", false,
	QObject::tr("channel width")));
  Props.append(new Property("L", "1 um", false,
	QObject::tr("channel length")));
  Props.append(new Property("Ld", "0.0", false,
	QObject::tr("lateral diffusion length")));
  Props.append(new Property("Tox", "0.1 um", false,
	QObject::tr("oxide thickness")));
  Props.append(new Property("Cgso", "0.0", false,
	QObject::tr("gate-source overlap capacitance per meter of "
		    "channel width in F/m")));
  Props.append(new Property("Cgdo", "0.0", false,
	QObject::tr("gate-drain overlap capacitance per meter of "
		    "channel width in F/m")));
  Props.append(new Property("Cgbo", "0.0", false,
	QObject::tr("gate-bulk overlap capacitance per meter of "
		    "channel length in F/m")));
  Props.append(new Property("Cbd", "0.0 F", false,
	QObject::tr("zero-bias bulk-drain junction capacitance")));
  Props.append(new Property("Cbs", "0.0 F", false,
	QObject::tr("zero-bias bulk-source junction capacitance")));
  Props.append(new Property("Pb", "0.8 V", false,
	QObject::tr("bulk junction potential")));
  Props.append(new Property("Mj", "0.5", false,
	QObject::tr("bulk junction bottom grading coefficient")));
  Props.append(new Property("Fc", "0.5", false,
	QObject::tr("bulk junction forward-bias depletion capacitance "
		    "coefficient")));
  Props.append(new Property("Cjsw", "0.0", false,
	QObject::tr("zero-bias bulk junction periphery capacitance per meter "
		    "of junction perimeter in F/m")));
  Props.append(new Property("Mjsw", "0.33", false,
	QObject::tr("bulk junction periphery grading coefficient")));
  Props.append(new Property("Tt", "0.0 ps", false,
	QObject::tr("bulk transit time")));
  Props.append(new Property("Nsub", "0.0", false,
	QObject::tr("substrate bulk doping density in 1/cm^3")));
  Props.append(new Property("Nss", "0.0", false,
	QObject::tr("surface state density in 1/cm^2")));
  Props.append(new Property("Tpg", "1", false,
	QObject::tr("gate material type: 0 = alumina; -1 = same as bulk; "
		    "1 = opposite to bulk")));
  Props.append(new Property("Uo", "600.0", false,
	QObject::tr("surface mobility in cm^2/Vs")));
  Props.append(new Property("Rsh", "0.0", false,
	QObject::tr("drain and source diffusion sheet resistance in "
		    "Ohms/square")));
  Props.append(new Property("Nrd", "1", false,
	QObject::tr("number of equivalent drain squares")));
  Props.append(new Property("Nrs", "1", false,
	QObject::tr("number of equivalent source squares")));
  Props.append(new Property("Cj", "0.0", false,
	QObject::tr("zero-bias bulk junction bottom capacitance per square "
		    "meter of junction area in F/m^2")));
  Props.append(new Property("Js", "0.0", false,
	QObject::tr("bulk junction saturation current per square "
		    "meter of junction area in A/m^2")));
  Props.append(new Property("Ad", "0.0", false,
	QObject::tr("drain diffusion area in m^2")));
  Props.append(new Property("As", "0.0", false,
	QObject::tr("source diffusion area in m^2")));
  Props.append(new Property("Pd", "0.0 m", false,
	QObject::tr("drain junction perimeter")));
  Props.append(new Property("Ps", "0.0 m", false,
	QObject::tr("source junction perimeter")));
  Props.append(new Property("Kf", "0.0", false,
	QObject::tr("flicker noise coefficient")));
  Props.append(new Property("Af", "1.0", false,
	QObject::tr("flicker noise exponent")));
  Props.append(new Property("Ffe", "1.0", false,
	QObject::tr("flicker noise frequency exponent")));
  Props.append(new Property("Temp", "26.85", false,
	QObject::tr("simulation temperature in degree Celsius")));
}

MOSFET::~MOSFET()
{
}

Component* MOSFET::newOne()
{
  MOSFET* p = new MOSFET();
  p->Props.first()->Value = Props.first()->Value;
  p->Props.next()->Value = Props.next()->Value;
  p->recreate();
  return p;
}

Component* MOSFET::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("n-MOSFET");
  BitmapFile = "nmosfet";

  if(getNewOne)  return new MOSFET();
  return 0;
}

Component* MOSFET::info_p(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("p-MOSFET");
  BitmapFile = "pmosfet";

  if(getNewOne) {
    MOSFET* p = new MOSFET();
    p->Props.getFirst()->Value = "pfet";
    p->recreate();
    return p;
  }
  return 0;
}

Component* MOSFET::info_depl(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("depletion MOSFET");
  BitmapFile = "dmosfet";

  if(getNewOne) {
    MOSFET* p = new MOSFET();
    p->Props.first();
    p->Props.next()->Value = "-1.0 V";
    p->recreate();
    return p;
  }
  return 0;
}

// Makes the schematic symbol a n-type, p-type or depletion MOSFET (according
// to the "Type" and "Vt0" properties).
void MOSFET::recreate()
{
  Line *pl2 = Lines.last();
  Line *pl1 = Lines.prev();
  Line *pl3 = Lines.prev();

  if(Props.first()->Value == "nfet") {
    pl1->x1 = -9;  pl1->y1 = 0;  pl1->x2 = -4; pl1->y2 = -5;
    pl2->x1 = -9;  pl2->y1 = 0;  pl2->x2 = -4; pl2->y2 =  5;
  }
  else {
    pl1->x1 = -1;  pl1->y1 = 0;  pl1->x2 = -6; pl1->y2 = -5;
    pl2->x1 = -1;  pl2->y1 = 0;  pl2->x2 = -6; pl2->y2 =  5;
  }

  // depletion or enhancement MOSFET ?
  if(Props.next()->Value.stripWhiteSpace().at(0) == '-') {
    pl3->x1 = -10;  pl3->y1 = -8;  pl3->x2 = -10; pl3->y2 = 8;
  }
  else {
    pl3->x1 = -10;  pl3->y1 = -4;  pl3->x2 = -10; pl3->y2 = 4;
  }

  if(mirroredX) {
    pl1->y1 = -pl1->y1;
    pl1->y2 = -pl1->y2;
    pl2->y1 = -pl2->y1;
    pl2->y2 = -pl2->y2;
    pl3->y1 = -pl3->y1;
    pl3->y2 = -pl3->y2;
  }

  int tmp;
  for(int z=0; z<rotated; z++) {
    tmp = -pl1->x1;
    pl1->x1 = pl1->y1;
    pl1->y1 = tmp;
    tmp = -pl1->x2;
    pl1->x2 = pl1->y2;
    pl1->y2 = tmp;

    tmp = -pl2->x1;
    pl2->x1 = pl2->y1;
    pl2->y1 = tmp;
    tmp = -pl2->x2;
    pl2->x2 = pl2->y2;
    pl2->y2 = tmp;

    tmp = -pl3->x1;
    pl3->x1 = pl3->y1;
    pl3->y1 = tmp;
    tmp = -pl3->x2;
    pl3->x2 = pl3->y2;
    pl3->y2 = tmp;
  }
}
