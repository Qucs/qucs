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
  Props.push_back(qucs::Property("Type", "npn", true,
	QObject::tr("polarity")+" [npn, pnp]"));
  Props.push_back(qucs::Property("Is", "1e-16", true,
	QObject::tr("saturation current")));
  Props.push_back(qucs::Property("Nf", "1", true,
	QObject::tr("forward emission coefficient")));
  Props.push_back(qucs::Property("Nr", "1", false,
	QObject::tr("reverse emission coefficient")));
  Props.push_back(qucs::Property("Ikf", "0", false,
	QObject::tr("high current corner for forward beta")));
  Props.push_back(qucs::Property("Ikr", "0", false,
	QObject::tr("high current corner for reverse beta")));
  Props.push_back(qucs::Property("Vaf", "0", true,
	QObject::tr("forward early voltage")));
  Props.push_back(qucs::Property("Var", "0", false,
	QObject::tr("reverse early voltage")));
  Props.push_back(qucs::Property("Ise", "0", false,
	QObject::tr("base-emitter leakage saturation current")));
  Props.push_back(qucs::Property("Ne", "1.5", false,
	QObject::tr("base-emitter leakage emission coefficient")));
  Props.push_back(qucs::Property("Isc", "0", false,
	QObject::tr("base-collector leakage saturation current")));
  Props.push_back(qucs::Property("Nc", "2", false,
	QObject::tr("base-collector leakage emission coefficient")));
  Props.push_back(qucs::Property("Bf", "100", true,
	QObject::tr("forward beta")));
  Props.push_back(qucs::Property("Br", "1", false,
	QObject::tr("reverse beta")));
  Props.push_back(qucs::Property("Rbm", "0", false,
	QObject::tr("minimum base resistance for high currents")));
  Props.push_back(qucs::Property("Irb", "0", false,
	QObject::tr("current for base resistance midpoint")));
  Props.push_back(qucs::Property("Rc", "0", false,
	QObject::tr("collector ohmic resistance")));
  Props.push_back(qucs::Property("Re", "0", false,
	QObject::tr("emitter ohmic resistance")));
  Props.push_back(qucs::Property("Rb", "0", false,
	QObject::tr("zero-bias base resistance (may be high-current dependent)")));
  Props.push_back(qucs::Property("Cje", "0", false,
	QObject::tr("base-emitter zero-bias depletion capacitance")));
  Props.push_back(qucs::Property("Vje", "0.75", false,
	QObject::tr("base-emitter junction built-in potential")));
  Props.push_back(qucs::Property("Mje", "0.33", false,
	QObject::tr("base-emitter junction exponential factor")));
  Props.push_back(qucs::Property("Cjc", "0", false,
	QObject::tr("base-collector zero-bias depletion capacitance")));
  Props.push_back(qucs::Property("Vjc", "0.75", false,
	QObject::tr("base-collector junction built-in potential")));
  Props.push_back(qucs::Property("Mjc", "0.33", false,
	QObject::tr("base-collector junction exponential factor")));
  Props.push_back(qucs::Property("Xcjc", "1.0", false,
	QObject::tr("fraction of Cjc that goes to internal base pin")));
  Props.push_back(qucs::Property("Cjs", "0", false,
	QObject::tr("zero-bias collector-substrate capacitance")));
  Props.push_back(qucs::Property("Vjs", "0.75", false,
	QObject::tr("substrate junction built-in potential")));
  Props.push_back(qucs::Property("Mjs", "0", false,
	QObject::tr("substrate junction exponential factor")));
  Props.push_back(qucs::Property("Fc", "0.5", false,
	QObject::tr("forward-bias depletion capacitance coefficient")));
  Props.push_back(qucs::Property("Tf", "0.0", false,
	QObject::tr("ideal forward transit time")));
  Props.push_back(qucs::Property("Xtf", "0.0", false,
	QObject::tr("coefficient of bias-dependence for Tf")));
  Props.push_back(qucs::Property("Vtf", "0.0", false,
	QObject::tr("voltage dependence of Tf on base-collector voltage")));
  Props.push_back(qucs::Property("Itf", "0.0", false,
	QObject::tr("high-current effect on Tf")));
  Props.push_back(qucs::Property("Tr", "0.0", false,
	QObject::tr("ideal reverse transit time")));
  Props.push_back(qucs::Property("Temp", "26.85", false,
	QObject::tr("simulation temperature in degree Celsius")));
  Props.push_back(qucs::Property("Kf", "0.0", false,
	QObject::tr("flicker noise coefficient")));
  Props.push_back(qucs::Property("Af", "1.0", false,
	QObject::tr("flicker noise exponent")));
  Props.push_back(qucs::Property("Ffe", "1.0", false,
	QObject::tr("flicker noise frequency exponent")));
  Props.push_back(qucs::Property("Kb", "0.0", false,
	QObject::tr("burst noise coefficient")));
  Props.push_back(qucs::Property("Ab", "1.0", false,
	QObject::tr("burst noise exponent")));
  Props.push_back(qucs::Property("Fb", "1.0", false,
	QObject::tr("burst noise corner frequency in Hertz")));
  Props.push_back(qucs::Property("Ptf", "0.0", false,
	QObject::tr("excess phase in degrees")));
  Props.push_back(qucs::Property("Xtb", "0.0", false,
	QObject::tr("temperature exponent for forward- and reverse beta")));
  Props.push_back(qucs::Property("Xti", "3.0", false,
	QObject::tr("saturation current temperature exponent")));
  Props.push_back(qucs::Property("Eg", "1.11", false,
	QObject::tr("energy bandgap in eV")));
  Props.push_back(qucs::Property("Tnom", "26.85", false,
	QObject::tr("temperature at which parameters were extracted")));
  Props.push_back(qucs::Property("Area", "1.0", false,
	QObject::tr("default area for bipolar transistor")));

  Name  = "T";
}

BJTsub::BJTsub()
{
  Description = QObject::tr("bipolar junction transistor with substrate");
  createSymbol();
  set_qucs_text_position(x2()+4, y1()+4);
  Model = "BJT";
}

// -------------------------------------------------------
Component* BJTsub::newOne()
{
  BJTsub* p = new BJTsub();
  p->Props.front().Value = Props.front().Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* BJTsub::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("npn transistor");
  BitmapFile = (char *) "npnsub";

  if(getNewOne)  return new BJTsub();
  return 0;
}

// -------------------------------------------------------
Element* BJTsub::info_pnp(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("pnp transistor");
  BitmapFile = (char *) "pnpsub";

  if(getNewOne) {
    BJTsub* p = new BJTsub();
    p->Props.front().Value = "pnp";
    p->recreate(0);
    return p;
  }
  return 0;
}

// -------------------------------------------------------
void BJTsub::createSymbol()
{
  Lines.push_back(qucs::Line(-10,-15,-10, 15,QPen(Qt::darkBlue,3)));
  Lines.push_back(qucs::Line(-30,  0,-10,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10, -5,  0,-15,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0,-15,  0,-30,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-10,  5,  0, 15,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0, 15,  0, 30,QPen(Qt::darkBlue,2)));

  Lines.push_back(qucs::Line(  9,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  9, -7,  9,  7,QPen(Qt::darkBlue,3)));

  if(Props.front().Value == "npn") {
    Lines.push_back(qucs::Line( -6, 15,  0, 15,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line(  0,  9,  0, 15,QPen(Qt::darkBlue,2)));
  }
  else {
    Lines.push_back(qucs::Line( -5, 10, -5, 16,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line( -5, 10,  1, 10,QPen(Qt::darkBlue,2)));
  }

  Ports.push_back(qucs::Port(-30,  0));
  Ports.push_back(qucs::Port(  0,-30));
  Ports.push_back(qucs::Port(  0, 30));
  Ports.push_back(qucs::Port( 30,  0));

  set_x1(-30);
  set_y1(-30);
  set_x2(30);
  set_y2(30);
}
