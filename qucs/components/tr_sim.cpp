/***************************************************************************
                          tr_sim.cpp  -  description
                             -------------------
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

#include "tr_sim.h"
#include "main.h"


TR_Sim::TR_Sim()
{
  Description = QObject::tr("transient simulation");

  QString  s = Description;
  s.replace(" ","\n");
  QFontMetrics  metrics(QucsSettings.largeFont);
  QSize r = metrics.size(0, s);
  int xb = (r.width()  >> 1) + 6;
  int yb = (r.height() >> 1) + 4;

  Lines.append(new Line(-xb,-yb, xb,-yb,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-xb, yb, xb, yb,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-xb,-yb,-xb, yb,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( xb,-yb, xb, yb,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-xb+5, yb+5, xb+4, yb+5,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-xb,   yb,  -xb+5, yb+5,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( xb,   yb,   xb+4, yb+5,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( xb+4,-yb+5, xb+4, yb+5,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( xb,  -yb,   xb+4,-yb+5,QPen(QPen::darkBlue,2)));

  x1 = -xb-4; y1 = -yb-4;
  x2 =  xb+8; y2 =  yb+9;

  tx = x1+8;
  ty = y2+4;
  Model = ".TR";
  Name  = "TR";

  Props.append(new Property("Type", "lin", true,
	QObject::tr("sweep type")+" [lin, log]"));
  Props.append(new Property("Start", "0", true,
	QObject::tr("start time in seconds")));
  Props.append(new Property("Stop", "1 ms", true,
	QObject::tr("stop time in seconds")));
  Props.append(new Property("Points", "10", false,
	QObject::tr("number of simulation time steps")));
  Props.append(new Property("IntegrationMethod", "Trapezoidal", false,
	QObject::tr("integration method")+
	" [Euler, Trapezoidal, Gear, AdamsMoulton]"));
  Props.append(new Property("Order", "2", false,
	QObject::tr("order of integration method")+" (1-6)"));
  Props.append(new Property("InitialStep", "1 ns", false,
	QObject::tr("initial step size in seconds")));
  Props.append(new Property("MinStep", "1e-16", false,
	QObject::tr("minimum step size in seconds")));
  Props.append(new Property("MaxIter", "150", false,
	QObject::tr("maximum number of iterations until error")));
  Props.append(new Property("reltol", "0.001", false,
	QObject::tr("relative tolerance for convergence")));
  Props.append(new Property("abstol", "1 pA", false,
	QObject::tr("absolute tolerance for currents")));
  Props.append(new Property("vntol", "1 uV", false,
	QObject::tr("absolute tolerance for voltages")));
  Props.append(new Property("Temp", "26.85", false,
	QObject::tr("simulation temperature in degree Celsius")));
}

TR_Sim::~TR_Sim()
{
}

Component* TR_Sim::newOne()
{
  return new TR_Sim();
}

Component* TR_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Transient simulation");
  BitmapFile = "tran";

  if(getNewOne)  return new TR_Sim();
  return 0;
}
