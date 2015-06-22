/***************************************************************************
                                etr_sim.cpp
                               ------------
    begin                : Wed Dec 04 2013
    copyright            : (C) 2013 by Richard Crozier
    email                : richard.crozier@yahoo.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "etr_sim.h"
#include "main.h"


ETR_Sim::ETR_Sim()
{
  Description = QObject::tr("Externally driven transient simulation");

  QString s = Description;
  int a = 17;
  s[a] = '\n';

  Texts.append(new Text(0, 0, s.left(a), Qt::darkBlue, QucsSettings.largeFontSize));
  if (a != -1)
    Texts.append(new Text(0, 0, s.mid(a+1), Qt::darkBlue, QucsSettings.largeFontSize));

  x1 = -10; y1 = -9;
  x2 = x1+130; y2 = y1+59;

  tx = 0;
  ty = y2+1;
  Model = ".ETR";
  Name  = "ETR";

  Props.append(new Property("IntegrationMethod", "Trapezoidal", false,
	QObject::tr("Integration method")+
	" [Euler, Trapezoidal, Gear, AdamsMoulton]"));
  Props.append(new Property("Order", "2", false,
	QObject::tr("Order of integration method")+" (1-6)"));
  Props.append(new Property("InitialStep", "1 ns", false,
	QObject::tr("Initial step size in seconds")));
  Props.append(new Property("MinStep", "1e-16", false,
	QObject::tr("Minimum step size in seconds")));
  Props.append(new Property("MaxIter", "150", false,
	QObject::tr("Maximum number of iterations until error")));
  Props.append(new Property("reltol", "0.001", false,
	QObject::tr("Relative tolerance for convergence")));
  Props.append(new Property("abstol", "1 pA", false,
	QObject::tr("Absolute tolerance for currents")));
  Props.append(new Property("vntol", "1 uV", false,
	QObject::tr("Absolute tolerance for voltages")));
  Props.append(new Property("Temp", "26.85", false,
	QObject::tr("Simulation temperature in degrees Celsius")));
  Props.append(new Property("LTEreltol", "1e-3", false,
	QObject::tr("Relative tolerance of local truncation error")));
  Props.append(new Property("LTEabstol", "1e-6", false,
	QObject::tr("Absolute tolerance of local truncation error")));
  Props.append(new Property("LTEfactor", "1", false,
	QObject::tr("Overestimation of local truncation error")));
  Props.append(new Property("Solver", "CroutLU", false,
	QObject::tr("Method for solving the circuit matrix")+
	" [CroutLU, DoolittleLU, HouseholderQR, HouseholderLQ, GolubSVD]"));
  Props.append(new Property("relaxTSR", "no", false,
	QObject::tr("Relax time step raster")+" [no, yes]"));
  Props.append(new Property("initialDC", "yes", false,
	QObject::tr("Perform an initial DC analysis")+" [yes, no]"));
  Props.append(new Property("MaxStep", "0", false,
	QObject::tr("Maximum step size in seconds")));
}

ETR_Sim::~ETR_Sim()
{
}

Component* ETR_Sim::newOne()
{
  return new ETR_Sim();
}

Element* ETR_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("External transient simulation");
  BitmapFile = (char *) "etran";

  if(getNewOne)  return new ETR_Sim();
  return 0;
}

