/***************************************************************************
    copyright            : (C) 2005 by Michael Margraf
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "digi_sim.h"
#include "qucs_app.h"
#include "property.h"
#include "../legacy/obsolete_paintings.h"
#include "task_element.h"

namespace {

using namespace qucs;

class Digi_Sim : public TaskElement {
public:
  Digi_Sim();
 ~Digi_Sim();
  Element* clone() const {return new Digi_Sim(*this);}
  static Element* info(QString&, char* &, bool getNewOne=false);
};

Digi_Sim::Digi_Sim()
{
  // Type = isDigitalComponent;
  // Description = QObject::tr("digital simulation");
  //
  /// QList<Line *>     Lines;
  /// QList<struct Arc *>      Arcs;
  /// QList<Area *>     Rects;
  /// QList<Area *>     Ellips;
  /// QList<Port *>     Ports;
  QList<Text *>     Texts;
  QList<Property *>     Props;

  QString  s = "incomplete";
  int a = s.indexOf(" ");
  if (a != -1) s[a] = '\n';  // break line before the word "simulation"

  Texts.append(new Text(0, 0, s.left(a), Qt::darkBlue, QucsSettings.largeFontSize));
  if (a != -1)
    Texts.append(new Text(0, 0, s.mid(a+1), Qt::darkBlue, QucsSettings.largeFontSize));

  x1 = -10; y1 = -9;
  x2 = x1+120; y2 = y1+59;

  tx = 0;
  ty = y2+1;
  setTypeName("Digi");

  // Property list must keeps its order !
  Props.append(new Property("Type", "TruthTable", true,
	QObject::tr("type of simulation")+" [TruthTable, TimeList]"));
  Props.append(new Property("time", "10 ns", false,
	QObject::tr("duration of TimeList simulation")));
  Props.append(new Property("Model", "VHDL", false,
	QObject::tr("netlist format")+" [VHDL, Verilog]"));
}

// -------------------------------------------------------
Digi_Sim::~Digi_Sim()
{
}

// -------------------------------------------------------
Element* Digi_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("digital simulation");
  BitmapFile = (char *) "digi";

  if(getNewOne)  return new Digi_Sim();
  return 0;
}

}
