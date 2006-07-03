/***************************************************************************
                              param_sweep.cpp
                             -----------------
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

#include "param_sweep.h"
#include "main.h"


Param_Sweep::Param_Sweep()
{
  Description = QObject::tr("Parameter sweep");

  QString  s = Description;
  int a = s.findRev(" ");
  if (a != -1) s[a] = '\n';    // break line

  Texts.append(new Text(0, 0, s.left(a), QPen::darkBlue, QucsSettings.largeFontSize));
  if (a != -1)
    Texts.append(new Text(0, 0, s.mid(a+1), QPen::darkBlue, QucsSettings.largeFontSize));

  x1 = -10; y1 = -9;
  x2 = x1+104; y2 = y1+59;

  tx = 0;
  ty = y2+1;
  Model = ".SW";
  Name  = "SW";

  // The index of the first 6 properties must not changed. Used in recreate().
  Props.append(new Property("Sim", "", true,
		QObject::tr("simulation to perform parameter sweep on")));
  Props.append(new Property("Type", "lin", true,
		QObject::tr("sweep type")+" [lin, log, list, const]"));
  Props.append(new Property("Param", "R1", true,
		QObject::tr("parameter to sweep")));
  Props.append(new Property("Start", "5 Ohm", true,
		QObject::tr("start value for sweep")));
  Props.append(new Property("Stop", "50 Ohm", true,
		QObject::tr("stop value for sweep")));
  Props.append(new Property("Points", "20", true,
		QObject::tr("number of simulation steps")));
}

Param_Sweep::~Param_Sweep()
{
}

Component* Param_Sweep::newOne()
{
  return new Param_Sweep();
}

Element* Param_Sweep::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Parameter sweep");
  BitmapFile = "sweep";

  if(getNewOne)  return new Param_Sweep();
  return 0;
}

void Param_Sweep::recreate(Schematic*)
{
  if((Props.at(1)->Value == "list") || (Props.at(1)->Value == "const")) {
    // Call them "Symbol" to omit them in the netlist.
    Props.at(3)->Name = "Symbol";
    Props.next()->Name = "Symbol";
    Props.next()->Name = "Values";
  }
  else {
    Props.at(3)->Name = "Start";
    Props.next()->Name = "Stop";
    Props.next()->Name = "Points";
  }
}

