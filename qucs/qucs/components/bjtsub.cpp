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


Basic_BJT::Basic_BJT()
{
  // this must be the first property in the list  !!!
  Props.append(new Property("Type", "npn", true,
	QObject::tr("Polarity")+" [npn, pnp]"));
  Props.append(new Property("Is", "1e-16", true,
	QObject::tr("Saturation current")));
  Props.append(new Property("Nf", "1", true,
	QObject::tr("Forward emission coefficient")));
  Props.append(new Property("Nr", "1", false,
	QObject::tr("Reverse emission coefficient")));
  Props.append(new Property("Ikf", "0", false,
	QObject::tr("High current corner for forward beta")));
  Props.append(new Property("Ikr", "0", false,
	QObject::tr("High current corner for reverse beta")));
  Props.append(new Property("Vaf", "0", true,
	QObject::tr("Forward early voltage")));
  Props.append(new Property("Var", "0", false,
	QObject::tr("Reverse early voltage")));
  Props.append(new Property("Ise", "0", false,
	QObject::tr("Base-emitter leakage saturation current")));
  Props.append(new Property("Ne", "1.5", false,
	QObject::tr("Base-emitter leakage emission coefficient")));
  Props.append(new Property("Isc", "0", false,
	QObject::tr("Base-collector leakage saturation current")));
  Props.append(new Property("Nc", "2", false,
	QObject::tr("Base-collector leakage emission coefficient")));
  Props.append(new Property("Bf", "100", true,
	QObject::tr("Forward beta")));
  Props.append(new Property("Br", "1", false,
	QObject::tr("Reverse beta")));
  Props.append(new Property("Rbm", "0", false,
	QObject::tr("Minimum base resistance for high currents")));
  Props.append(new Property("Irb", "0", false,
	QObject::tr("Current for base resistance midpoint")));
  Props.append(new Property("Rc", "0", false,
	QObject::tr("Collector ohmic resistance")));
  Props.append(new Property("Re", "0", false,
	QObject::tr("Emitter ohmic resistance")));
  Props.append(new Property("Rb", "0", false,
	QObject::tr("Zero-bias base resistance (may be high-current dependent)")));
  Props.append(new Property("Cje", "0", false,
	QObject::tr("Base-emitter zero-bias depletion capacitance")));
  Props.append(new Property("Vje", "0.75", false,
	QObject::tr("Base-emitter junction built-in potential")));
  Props.append(new Property("Mje", "0.33", false,
	QObject::tr("Base-emitter junction exponential factor")));
  Props.append(new Property("Cjc", "0", false,
	QObject::tr("Base-collector zero-bias depletion capacitance")));
  Props.append(new Property("Vjc", "0.75", false,
	QObject::tr("Base-collector junction built-in potential")));
  Props.append(new Property("Mjc", "0.33", false,
	QObject::tr("Base-collector junction exponential factor")));
  Props.append(new Property("Xcjc", "1.0", false,
	QObject::tr("Fraction of Cjc that goes to internal base pin")));
  Props.append(new Property("Cjs", "0", false,
	QObject::tr("Zero-bias collector-substrate capacitance")));
  Props.append(new Property("Vjs", "0.75", false,
	QObject::tr("Substrate junction built-in potential")));
  Props.append(new Property("Mjs", "0", false,
	QObject::tr("Substrate junction exponential factor")));
  Props.append(new Property("Fc", "0.5", false,
	QObject::tr("Forward-bias depletion capacitance coefficient")));
  Props.append(new Property("Tf", "0.0", false,
	QObject::tr("Ideal forward transit time")));
  Props.append(new Property("Xtf", "0.0", false,
	QObject::tr("Coefficient of bias-dependence for Tf")));
  Props.append(new Property("Vtf", "0.0", false,
	QObject::tr("Voltage dependence of Tf on base-collector voltage")));
  Props.append(new Property("Itf", "0.0", false,
	QObject::tr("High-current effect on Tf")));
  Props.append(new Property("Tr", "0.0", false,
	QObject::tr("Ideal reverse transit time")));
  Props.append(new Property("Temp", "26.85", false,
	QObject::tr("Simulation temperature in degrees Celsius")));
  Props.append(new Property("Kf", "0.0", false,
	QObject::tr("Flicker noise coefficient")));
  Props.append(new Property("Af", "1.0", false,
	QObject::tr("Flicker noise exponent")));
  Props.append(new Property("Ffe", "1.0", false,
	QObject::tr("Flicker noise frequency exponent")));
  Props.append(new Property("Kb", "0.0", false,
	QObject::tr("Burst noise coefficient")));
  Props.append(new Property("Ab", "1.0", false,
	QObject::tr("Burst noise exponent")));
  Props.append(new Property("Fb", "1.0", false,
	QObject::tr("Burst noise corner frequency in hertz")));
  Props.append(new Property("Ptf", "0.0", false,
	QObject::tr("Excess phase in degrees")));
  Props.append(new Property("Xtb", "0.0", false,
	QObject::tr("Temperature exponent for forward- and reverse beta")));
  Props.append(new Property("Xti", "3.0", false,
	QObject::tr("Saturation current temperature exponent")));
  Props.append(new Property("Eg", "1.11", false,
	QObject::tr("Energy bandgap in eV")));
  Props.append(new Property("Tnom", "26.85", false,
	QObject::tr("Temperature at which parameters were extracted")));
  Props.append(new Property("Area", "1.0", false,
	QObject::tr("Default area for bipolar transistor")));

  Name  = "T";
}

BJTsub::BJTsub()
{
  Description = QObject::tr("Bipolar junction transistor with substrate");
  createSymbol();
  tx = x2+4;
  ty = y1+4;
  Model = "BJT";
}

// -------------------------------------------------------
Component* BJTsub::newOne()
{
  BJTsub* p = new BJTsub();
  p->Props.getFirst()->Value = Props.getFirst()->Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* BJTsub::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("NPN transistor");
  BitmapFile = (char *) "npnsub";

  if(getNewOne)  return new BJTsub();
  return 0;
}

// -------------------------------------------------------
Element* BJTsub::info_pnp(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("PNP transistor");
  BitmapFile = (char *) "pnpsub";

  if(getNewOne) {
    BJTsub* p = new BJTsub();
    p->Props.getFirst()->Value = "pnp";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
void BJTsub::createSymbol()
{
  Lines.append(new Line(-10,-15,-10, 15,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-30,  0,-10,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10, -5,  0,-15,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0,-15,  0,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10,  5,  0, 15,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, 15,  0, 30,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(  9,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  9, -7,  9,  7,QPen(Qt::darkBlue,3)));

  if(Props.getFirst()->Value == "npn") {
    Lines.append(new Line( -6, 15,  0, 15,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  0,  9,  0, 15,QPen(Qt::darkBlue,2)));
  }
  else {
    Lines.append(new Line( -5, 10, -5, 16,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( -5, 10,  1, 10,QPen(Qt::darkBlue,2)));
  }

  Ports.append(new Port(-30,  0));
  Ports.append(new Port(  0,-30));
  Ports.append(new Port(  0, 30));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -30;
  x2 =  30; y2 =  30;
}
