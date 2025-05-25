/***************************************************************************
    copyright            : (C) 2008 by Michael Margraf
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

#include "tunneldiode.h"

TunnelDiode::TunnelDiode()
{
  Description = QObject::tr("resonance tunnel diode");

  Props.push_back(qucs::Property("Ip", "4 mA", true,
	QObject::tr("peak current")));
  Props.push_back(qucs::Property("Iv", "0.6 mA", true,
	QObject::tr("valley current")));
  Props.push_back(qucs::Property("Vv", "0.8 V", true,
	QObject::tr("valley voltage")));
  Props.push_back(qucs::Property("Wr", "2.7e-20", false,
	QObject::tr("resonance energy in Ws")));
  Props.push_back(qucs::Property("eta", "1e-20", false,
	QObject::tr("Fermi energy in Ws")));
  Props.push_back(qucs::Property("dW", "4.5e-21", false,
	QObject::tr("resonance width in Ws")));
  Props.push_back(qucs::Property("Tmax", "0.95", false,
	QObject::tr("maximum of transmission")));
  Props.push_back(qucs::Property("de", "0.9", false,
	QObject::tr("fitting factor for electron density")));
  Props.push_back(qucs::Property("dv", "2.0", false,
	QObject::tr("fitting factor for voltage drop")));
  Props.push_back(qucs::Property("nv", "16", false,
	QObject::tr("fitting factor for diode current")));

  Props.push_back(qucs::Property("Cj0", "80 fF", false,
	QObject::tr("zero-bias depletion capacitance")));
  Props.push_back(qucs::Property("M", "0.5", false,
	QObject::tr("grading coefficient")));
  Props.push_back(qucs::Property("Vj", "0.5 V", false,
	QObject::tr("junction potential")));
  Props.push_back(qucs::Property("te", "0.6 ps", false,
	QObject::tr("life-time of electrons")));

  Props.push_back(qucs::Property("Temp", "26.85", false,
	QObject::tr("simulation temperature in degree Celsius")));
  Props.push_back(qucs::Property("Area", "1.0", false,
	QObject::tr("default area for diode")));

  Lines.push_back(qucs::Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 12,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-12, -9,-12,  9,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(  0, -9,  0,  9,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 12, -9, 12,  9,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-12, -9, 12,  9,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-12,  9, 12, -9,QPen(Qt::darkBlue,2)));

  Ports.push_back(qucs::Port(-30, 0));
  Ports.push_back(qucs::Port( 30, 0));

  set_x1(-30);
  set_y1(-11);
  set_x2(30);
  set_y2(11);

  set_qucs_text_position(x1()+4, y2()+4);
  Model = "RTD";
  Name  = "D";
}

// -------------------------------------------------------------------
Component* TunnelDiode::newOne()
{
  return new TunnelDiode();
}

Element* TunnelDiode::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Tunnel Diode");
  BitmapFile = (char *) "tunneldiode";

  if(getNewOne)  return new TunnelDiode();
  return 0;
}
