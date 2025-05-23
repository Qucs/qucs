/***************************************************************************
                                ac_sim.cpp
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
#include "ac_sim.h"
#include "qucs.h"


AC_Sim::AC_Sim()
{
  Description = QObject::tr("ac simulation");

  QString s = Description;
  int a = s.indexOf(" ");
  int b = s.lastIndexOf(" ");
  if (a != -1 && b != -1) {
    if (a > (int) s.length() - b)  b = a;
  }
  if (a < 8 || s.length() - b < 8) b = -1;
  if (b != -1) s[b] = '\n';

  Texts.push_back(qucs::Text(0, 0, s.left(b), Qt::darkBlue, QucsSettings.largeFontSize));
  if (b != -1)
    Texts.push_back(qucs::Text(0, 0, s.mid(b+1), Qt::darkBlue, QucsSettings.largeFontSize));

  set_x1(-10);
  set_y1(-9);
  set_x2(x1()+128);
  set_y2(y1()+41);

  set_qucs_text_position(0, y2()+1);
  Model = ".AC";
  Name  = "AC";

  // The index of the first 4 properties must not changed. Used in recreate().
  Props.push_back(qucs::Property("Type", "lin", true,
			QObject::tr("sweep type")+" [lin, log, list, const]"));
  Props.push_back(qucs::Property("Start", "1 GHz", true,
			QObject::tr("start frequency in Hertz")));
  Props.push_back(qucs::Property("Stop", "10 GHz", true,
			QObject::tr("stop frequency in Hertz")));
  Props.push_back(qucs::Property("Points", "19", true,
			QObject::tr("number of simulation steps")));
  Props.push_back(qucs::Property("Noise", "no", false,
			QObject::tr("calculate noise voltages")+
			" [yes, no]"));
}

AC_Sim::~AC_Sim()
{
}

Component* AC_Sim::newOne()
{
  return new AC_Sim();
}

Element* AC_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("ac simulation");
  BitmapFile = (char *) "ac";

  if(getNewOne)  return new AC_Sim();
  return 0;
}

void AC_Sim::recreate(Schematic*)
{
  qucs::Property &pp = Props.front();
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
