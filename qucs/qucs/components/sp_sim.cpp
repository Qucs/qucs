/***************************************************************************
                                sp_sim.cpp
                               ------------
    copyright            : (C) 2003 by Michael Margraf
                               2018 Felix Salfelder / QUCS
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "qucs.h"
#include "globals.h"
#include "command.h"
#include "module.h"

namespace{

class SP_Sim : public CmdElement  {
public:
  SP_Sim();
  ~SP_Sim();
  Element* clone() const {return new SP_Sim(*this);}
  static Element* info(QString&, char* &, bool getNewOne=false);
  void recreate(SchematicDoc*);
}D;
Dispatcher<CmdElement>::INSTALL d(&command_dispatcher, "SP", &D);
Module::INSTALL pp("simulations", &D);

SP_Sim::SP_Sim()
{
  Description = QObject::tr("S parameter simulation");

  QString s = Description;
  int a = s.indexOf(" ");
  int b = s.lastIndexOf(" ");
  if (a != -1 && b != -1) {
    if (a > (int) s.length() - b)  b = a;
  }
  if (b != -1) s[b] = '\n';

  Texts.append(new Text(0, 0, s.left(b), Qt::darkBlue, QucsSettings.largeFontSize));
  if (b != -1)
    Texts.append(new Text(0, 0, s.mid(b+1), Qt::darkBlue, QucsSettings.largeFontSize));

  x1 = -10; y1 = -9;
  x2 = x1+121; y2 = y1+59;

  tx = 0;
  ty = y2+1;
  setName("SP");

  // The index of the first 4 properties must not changed. Used in recreate().
  Props.append(new Property("Type", "lin", true,
	QObject::tr("sweep type")+" [lin, log, list, const]"));
  Props.append(new Property("Start", "1 GHz", true,
	QObject::tr("start frequency in Hertz")));
  Props.append(new Property("Stop", "10 GHz", true,
	QObject::tr("stop frequency in Hertz")));
  Props.append(new Property("Points", "19", true,
	QObject::tr("number of simulation steps")));
  Props.append(new Property("Noise", "no", false,
	QObject::tr("calculate noise parameters")+
	" [yes, no]"));
  Props.append(new Property("NoiseIP", "1", false,
	QObject::tr("input port for noise figure")));
  Props.append(new Property("NoiseOP", "2", false,
	QObject::tr("output port for noise figure")));
  Props.append(new Property("saveCVs", "no", false,
	QObject::tr("put characteristic values into dataset")+
	" [yes, no]"));
  Props.append(new Property("saveAll", "no", false,
	QObject::tr("save subcircuit characteristic values into dataset")+
	" [yes, no]"));
}

SP_Sim::~SP_Sim()
{
}

#if 0
Element* SP_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("S-parameter simulation");
  BitmapFile = (char *) "sparameter";

  if(getNewOne)  return new SP_Sim();
  return 0;
}
#endif

void SP_Sim::recreate(SchematicDoc*)
{
  Property *pp = Props.first();
  if((pp->Value == "list") || (pp->Value == "const")) {
    // Call them "Symbol" to omit them in the netlist.
    pp = Props.next();
    pp->Name = "Symbol";
    pp->display = false;
    pp = Props.next();
    pp->Name = "Symbol";
    pp->display = false;
    Props.next()->Name = "Values";
  }
  else {
    Props.next()->Name = "Start";
    Props.next()->Name = "Stop";
    Props.next()->Name = "Points";
  }
}

}
