/***************************************************************************
                          ac_sim.cpp  -  description
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

#include "ac_sim.h"
#include "main.h"


AC_Sim::AC_Sim()
{
  Description = QObject::tr("ac simulation");

  QString s = Description;
  int a = s.find(" ");
  int b = s.findRev(" ");
  if (a != -1 && b != -1) {
    if (a > (int) s.length() - b)  b = a;
  }
  if (a < 8 || s.length() - b < 8) b = -1;
  if (b != -1) s[b] = '\n';

  Texts.append(new Text(0, 0, s.left(b)));
  if (b != -1) Texts.append(new Text(0, 0, s.mid(b+1)));

  x1 = -10; y1 = -9;
  x2 = x1+128; y2 = y1+41;

  tx = 0;
  ty = y2+1;
  Model = ".AC";
  Name  = "AC";

  // The index of the first 4 properties must not changed. Used in recreate().
  Props.append(new Property("Type", "lin", true,
			QObject::tr("sweep type")+" [lin, log, list, const]"));
  Props.append(new Property("Start", "1 GHz", true,
			QObject::tr("start frequency in Hertz")));
  Props.append(new Property("Stop", "10 GHz", true,
			QObject::tr("stop frequency in Hertz")));
  Props.append(new Property("Points", "19", true,
			QObject::tr("number of simulation steps")));
}

AC_Sim::~AC_Sim()
{
}

Component* AC_Sim::newOne()
{
  return new AC_Sim();
}

Component* AC_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("ac simulation");
  BitmapFile = "ac";

  if(getNewOne)  return new AC_Sim();
  return 0;
}

void AC_Sim::recreate()
{
  if((Props.getFirst()->Value == "list") ||
     (Props.getFirst()->Value == "const")) {
    // Call them "Symbol" to omit them in the netlist.
    Props.at(1)->Name = "Symbol";
    Props.next()->Name = "Symbol";
    Props.next()->Name = "Values";
  }
  else {
    Props.at(1)->Name = "Start";
    Props.next()->Name = "Stop";
    Props.next()->Name = "Points";
  }
}
