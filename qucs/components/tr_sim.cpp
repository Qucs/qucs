/***************************************************************************
                                tr_sim.cpp
                               ------------
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
#include "qucs.h"


TR_Sim::TR_Sim()
{
  Description = QObject::tr("transient simulation");

  QString  s = Description;
  int a = s.indexOf(" ");
  if (a != -1) s[a] = '\n';

  Texts.push_back(qucs::Text(0, 0, s.left(a), Qt::darkBlue, QucsSettings.largeFontSize));
  if (a != -1)
    Texts.push_back(qucs::Text(0, 0, s.mid(a+1), Qt::darkBlue, QucsSettings.largeFontSize));

  x1 = -10; y1 = -9;
  x2 = x1+104; y2 = y1+59;

  set_qucs_text_position(0, y2+1);
  Model = ".TR";
  Name  = "TR";

  // The index of the first 4 properties must not changed. Used in recreate().
  Props.push_back(qucs::Property("Type", "lin", true,
	QObject::tr("sweep type")+" [lin, log, list, const]"));
  Props.push_back(qucs::Property("Start", "0", true,
	QObject::tr("start time in seconds")));
  Props.push_back(qucs::Property("Stop", "1 ms", true,
	QObject::tr("stop time in seconds")));
  Props.push_back(qucs::Property("Points", "11", false,
	QObject::tr("number of simulation time steps")));
  Props.push_back(qucs::Property("IntegrationMethod", "Trapezoidal", false,
	QObject::tr("integration method")+
	" [Euler, Trapezoidal, Gear, AdamsMoulton]"));
  Props.push_back(qucs::Property("Order", "2", false,
	QObject::tr("order of integration method")+" (1-6)"));
  Props.push_back(qucs::Property("InitialStep", "1 ns", false,
	QObject::tr("initial step size in seconds")));
  Props.push_back(qucs::Property("MinStep", "1e-16", false,
	QObject::tr("minimum step size in seconds")));
  Props.push_back(qucs::Property("MaxIter", "150", false,
	QObject::tr("maximum number of iterations until error")));
  Props.push_back(qucs::Property("reltol", "0.001", false,
	QObject::tr("relative tolerance for convergence")));
  Props.push_back(qucs::Property("abstol", "1 pA", false,
	QObject::tr("absolute tolerance for currents")));
  Props.push_back(qucs::Property("vntol", "1 uV", false,
	QObject::tr("absolute tolerance for voltages")));
  Props.push_back(qucs::Property("Temp", "26.85", false,
	QObject::tr("simulation temperature in degree Celsius")));
  Props.push_back(qucs::Property("LTEreltol", "1e-3", false,
	QObject::tr("relative tolerance of local truncation error")));
  Props.push_back(qucs::Property("LTEabstol", "1e-6", false,
	QObject::tr("absolute tolerance of local truncation error")));
  Props.push_back(qucs::Property("LTEfactor", "1", false,
	QObject::tr("overestimation of local truncation error")));
  Props.push_back(qucs::Property("Solver", "CroutLU", false,
	QObject::tr("method for solving the circuit matrix")+
	" [CroutLU, DoolittleLU, HouseholderQR, HouseholderLQ, GolubSVD]"));
  Props.push_back(qucs::Property("relaxTSR", "no", false,
	QObject::tr("relax time step raster")+" [no, yes]"));
  Props.push_back(qucs::Property("initialDC", "yes", false,
	QObject::tr("perform an initial DC analysis")+" [yes, no]"));
  Props.push_back(qucs::Property("MaxStep", "0", false,
	QObject::tr("maximum step size in seconds")));
}

TR_Sim::~TR_Sim()
{
}

Component* TR_Sim::newOne()
{
  return new TR_Sim();
}

Element* TR_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Transient simulation");
  BitmapFile = (char *) "tran";

  if(getNewOne)  return new TR_Sim();
  return 0;
}

void TR_Sim::recreate(Schematic*)
{
  qucs::Property &pp = prop(0);
  if((pp.Value == "list") || (pp.Value == "const")) {
    // Call them "Symbol" to omit them in the netlist.
    prop(1).Name = "Symbol";
    prop(1).display = false;
    prop(2).Name = "Symbol";
    prop(2).display = false;
    prop(3).Name = "Values";
  }
  else {
    prop(1).Name = "Start";
    prop(2).Name = "Stop";
    prop(3).Name = "Points";
  }
}
