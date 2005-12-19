/***************************************************************************
                               bjtsub.cpp
                              ------------
    begin                : Sat Jul 17 2004
    copyright            : (C) 2004 by Michael Margraf
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

#include "bjtsub.h"


BJTsub::BJTsub()
{
  Description = QObject::tr("bipolar junction transistor with substrate");

  Lines.append(new Line(-10,-15,-10, 15,QPen(QPen::darkBlue,3)));
  Lines.append(new Line(-30,  0,-10,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10, -5,  0,-15,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,-15,  0,-30,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-10,  5,  0, 15,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0, 15,  0, 30,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(  9,  0, 30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  9, -7,  9,  7,QPen(QPen::darkBlue,3)));

  Lines.append(new Line( -6, 15,  0, 15,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(  0,  9,  0, 15,QPen(QPen::darkBlue,2)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port(  0,-30));
  Ports.append(new Port(  0, 30));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -30;
  x2 =  30; y2 =  30;

  tx = x2+4;
  ty = y1+4;
  Model = "BJT";
  Name  = "T";

  // this must be the first property in the list  !!!
  Props.append(new Property("Type", "npn", true,
	QObject::tr("polarity")+" [npn, pnp]"));
  Props.append(new Property("Is", "1e-16", true,
	QObject::tr("saturation current")));
  Props.append(new Property("Nf", "1", true,
	QObject::tr("forward emission coefficient")));
  Props.append(new Property("Nr", "1", false,
	QObject::tr("reverse emission coefficient")));
  Props.append(new Property("Ikf", "0", false,
	QObject::tr("high current corner for forward beta")));
  Props.append(new Property("Ikr", "0", false,
	QObject::tr("high current corner for reverse beta")));
  Props.append(new Property("Vaf", "0", true,
	QObject::tr("forward early voltage")));
  Props.append(new Property("Var", "0", false,
	QObject::tr("reverse early voltage")));
  Props.append(new Property("Ise", "0", false,
	QObject::tr("base-emitter leakage saturation current")));
  Props.append(new Property("Ne", "1.5", false,
	QObject::tr("base-emitter leakage emission coefficient")));
  Props.append(new Property("Isc", "0", false,
	QObject::tr("base-collector leakage saturation current")));
  Props.append(new Property("Nc", "2", false,
	QObject::tr("base-collector leakage emission coefficient")));
  Props.append(new Property("Bf", "100", true,
	QObject::tr("forward beta")));
  Props.append(new Property("Br", "1", false,
	QObject::tr("reverse beta")));
  Props.append(new Property("Rbm", "0", false,
	QObject::tr("minimum base resistance for high currents")));
  Props.append(new Property("Irb", "0", false,
	QObject::tr("current for base resistance midpoint")));
  Props.append(new Property("Rc", "0", false,
	QObject::tr("collector ohmic resistance")));
  Props.append(new Property("Re", "0", false,
	QObject::tr("emitter ohmic resistance")));
  Props.append(new Property("Rb", "0", false,
	QObject::tr("zero-bias base resistance (may be high-current dependent)")));
  Props.append(new Property("Cje", "0", false,
	QObject::tr("base-emitter zero-bias depletion capacitance")));
  Props.append(new Property("Vje", "0.75", false,
	QObject::tr("base-emitter junction built-in potential")));
  Props.append(new Property("Mje", "0.33", false,
	QObject::tr("base-emitter junction exponential factor")));
  Props.append(new Property("Cjc", "0", false,
	QObject::tr("base-collector zero-bias depletion capacitance")));
  Props.append(new Property("Vjc", "0.75", false,
	QObject::tr("base-collector junction built-in potential")));
  Props.append(new Property("Mjc", "0.33", false,
	QObject::tr("base-collector junction exponential factor")));
  Props.append(new Property("Xcjc", "1.0", false,
	QObject::tr("fraction of Cjc that goes to internal base pin")));
  Props.append(new Property("Cjs", "0", false,
	QObject::tr("zero-bias collector-substrate capacitance")));
  Props.append(new Property("Vjs", "0.75", false,
	QObject::tr("substrate junction built-in potential")));
  Props.append(new Property("Mjs", "0", false,
	QObject::tr("substrate junction exponential factor")));
  Props.append(new Property("Fc", "0.5", false,
	QObject::tr("forward-bias depletion capacitance coefficient")));
  Props.append(new Property("Tf", "0.0", false,
	QObject::tr("ideal forward transit time")));
  Props.append(new Property("Xtf", "0.0", false,
	QObject::tr("coefficient of bias-dependence for Tf")));
  Props.append(new Property("Vtf", "0.0", false,
	QObject::tr("voltage dependence of Tf on base-collector voltage")));
  Props.append(new Property("Itf", "0.0", false,
	QObject::tr("high-current effect on Tf")));
  Props.append(new Property("Tr", "0.0", false,
	QObject::tr("ideal reverse transit time")));
  Props.append(new Property("Temp", "26.85", false,
	QObject::tr("simulation temperature in degree Celsius")));
  Props.append(new Property("Kf", "0.0", false,
	QObject::tr("flicker noise coefficient")));
  Props.append(new Property("Af", "1.0", false,
	QObject::tr("flicker noise exponent")));
  Props.append(new Property("Ffe", "1.0", false,
	QObject::tr("flicker noise frequency exponent")));
  Props.append(new Property("Kb", "0.0", false,
	QObject::tr("burst noise coefficient")));
  Props.append(new Property("Ab", "1.0", false,
	QObject::tr("burst noise exponent")));
  Props.append(new Property("Fb", "1.0", false,
	QObject::tr("burst noise corner frequency in Hertz")));
  Props.append(new Property("Ptf", "0.0", false,
	QObject::tr("excess phase in degrees")));
  Props.append(new Property("Xtb", "0.0", false,
	QObject::tr("temperature exponent for forward- and reverse beta")));
  Props.append(new Property("Xti", "3.0", false,
	QObject::tr("saturation current temperature exponent")));
  Props.append(new Property("Eg", "1.11", false,
	QObject::tr("energy bandgap in eV")));
  Props.append(new Property("Tnom", "26.85", false,
	QObject::tr("temperature at which parameters were extracted")));
  Props.append(new Property("Area", "1.0", false,
	QObject::tr("default area for bipolar transistor")));
}

BJTsub::~BJTsub()
{
}

Component* BJTsub::newOne()
{
  BJTsub* p = new BJTsub();
  p->Props.getFirst()->Value = Props.getFirst()->Value;
  p->recreate(0);
  return p;
}

Element* BJTsub::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("npn transistor");
  BitmapFile = "npnsub";

  if(getNewOne)  return new BJTsub();
  return 0;
}

Element* BJTsub::info_pnp(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("pnp transistor");
  BitmapFile = "pnpsub";

  if(getNewOne) {
    BJTsub* p = new BJTsub();
    p->Props.getFirst()->Value = "pnp";
    p->recreate(0);
    return p;
  }
  return 0;
}

// Makes the schematic symbol a npn or a pnp transistor (according
// to the "Type" property).
void BJTsub::recreate(QucsDoc*)
{
  Line *pl2 = Lines.last();
  Line *pl1 = Lines.prev();

  if(Props.getFirst()->Value == "npn") {
    pl1->x1 = -6;  pl1->y1 = 15;  pl1->x2 = 0; pl1->y2 = 15;
    pl2->x1 = 0;  pl2->y1 = 9;  pl2->x2 = 0; pl2->y2 = 15;
  }
  else {
    pl1->x1 = -5;  pl1->y1 = 10;  pl1->x2 = -5; pl1->y2 = 16;
    pl2->x1 = -5;  pl2->y1 = 10;  pl2->x2 = 1; pl2->y2 = 10;
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
