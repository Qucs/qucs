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
  Description = QObject::tr("junction field-effect transistor");

  Lines.append(new Line(-10,-15,-10, 15,QPen(QPen::darkBlue,3)));
  Lines.append(new Line(-30,  0,-10,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,-10,  0,-10,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,-10,  0,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, 10,  0, 10,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0, 10,  0, 30,QPen(QPen::darkBlue,2)));

  Lines.append(new Line( -4, 24,  4, 20,QPen(QPen::darkBlue,2)));

  // these two lines must be the last ones !
  Lines.append(new Line(-16, -5,-11,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-16,  5,-11,  0,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port(  0,-30));
  Ports.append(new Port(  0, 30));

  x1 = -30; y1 = -30;
  x2 =   4; y2 =  30;

  tx = x2+4;
  ty = y1+4;
  Model = "JFET";
  Name  = "T";

  // this must be the first property in the list !!!
  Props.append(new Property("Type", "nfet", true,
	QObject::tr("polarity")+" [nfet, pfet]"));
  Props.append(new Property("Vt0", "-2.0 V", true,
	QObject::tr("threshold voltage")));
  Props.append(new Property("Beta", "1e-4", true,
	QObject::tr("transconductance parameter")));
  Props.append(new Property("Lambda", "0.0", true,
	QObject::tr("channel-length modulation parameter")));
  Props.append(new Property("Rd", "0.0", false,
	QObject::tr("parasitic drain resistance")));
  Props.append(new Property("Rs", "0.0", false,
	QObject::tr("parasitic source resistance")));
  Props.append(new Property("Is", "1e-14", false,
	QObject::tr("gate-junction saturation current")));
  Props.append(new Property("N", "1.0", false,
	QObject::tr("gate-junction emission coefficient")));
  Props.append(new Property("Isr", "1e-14", false,
	QObject::tr("gate-junction recombination current parameter")));
  Props.append(new Property("Nr", "2.0", false,
	QObject::tr("Isr emission coefficient")));
  Props.append(new Property("Cgs", "0.0", false,
	QObject::tr("zero-bias gate-source junction capacitance")));
  Props.append(new Property("Cgd", "0.0", false,
	QObject::tr("zero-bias gate-drain junction capacitance")));
  Props.append(new Property("Pb", "1.0", false,
	QObject::tr("gate-junction potential")));
  Props.append(new Property("Fc", "0.5", false,
	QObject::tr("forward-bias junction capacitance coefficient")));
  Props.append(new Property("M", "0.5", false,
	QObject::tr("gate P-N grading coefficient")));
  Props.append(new Property("Kf", "0.0", false,
	QObject::tr("flicker noise coefficient")));
  Props.append(new Property("Af", "1.0", false,
	QObject::tr("flicker noise exponent")));
  Props.append(new Property("Ffe", "1.0", false,
	QObject::tr("flicker noise frequency exponent")));
  Props.append(new Property("Temp", "26.85", false,
	QObject::tr("simulation temperature in degree Celsius")));
  Props.append(new Property("Xti", "3.0", false,
	QObject::tr("saturation current temperature exponent")));
  Props.append(new Property("Vt0tc", "0.0", false,
	QObject::tr("Vt0 temperature coefficient")));
  Props.append(new Property("Betatce", "0.0", false,
	QObject::tr("Beta exponential temperature coefficient")));
  Props.append(new Property("Tnom", "26.85", false,
	QObject::tr("temperature at which parameters were extracted")));
  Props.append(new Property("Area", "1.0", false,
	QObject::tr("default area for JFET")));
}

JFET::~JFET()
{
}

Component* JFET::newOne()
{
  JFET* p = new JFET();
  p->Props.getFirst()->Value = Props.getFirst()->Value;
  p->recreate(0);
  return p;
}

Element* JFET::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("n-JFET");
  BitmapFile = "nfet";

  if(getNewOne)  return new JFET();
  return 0;
}

Element* JFET::info_p(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("p-JFET");
  BitmapFile = "pfet";

  if(getNewOne) {
    JFET* p = new JFET();
    p->Props.getFirst()->Value = "pfet";
    p->recreate(0);
    return p;
  }
  return 0;
}

// Makes the schematic symbol a n-type or a p-type transistor (according
// to the "Type" property).
void JFET::recreate(QucsDoc*)
{
  Line *pl2 = Lines.last();
  Line *pl1 = Lines.prev();

  if(Props.getFirst()->Value == "nfet") {
    pl1->x1 = -16;  pl1->y1 = -5;  pl1->x2 = -11; pl1->y2 = 0;
    pl2->x1 = -16;  pl2->y1 =  5;  pl2->x2 = -11; pl2->y2 = 0;
  }
  else {
    pl1->x1 = -18;  pl1->y1 = 0;  pl1->x2 = -13; pl1->y2 = -5;
    pl2->x1 = -18;  pl2->y1 = 0;  pl2->x2 = -13; pl2->y2 =  5;
  }

  if(mirroredX) {
    pl1->y1 = -pl1->y1;
    pl1->y2 = -pl1->y2;
    pl2->y1 = -pl2->y1;
    pl2->y2 = -pl2->y2;
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
  }
}
