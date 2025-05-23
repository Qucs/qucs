/***************************************************************************
                          dc_sim.cpp  -  description
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
#include "dc_sim.h"
#include "qucs.h"


DC_Sim::DC_Sim()
{
  Description = QObject::tr("dc simulation");

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
  Model = ".DC";
  Name  = "DC";

  Props.push_back(qucs::Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));
  Props.push_back(qucs::Property("reltol", "0.001", false,
		QObject::tr("relative tolerance for convergence")));
  Props.push_back(qucs::Property("abstol", "1 pA", false,
		QObject::tr("absolute tolerance for currents")));
  Props.push_back(qucs::Property("vntol", "1 uV", false,
		QObject::tr("absolute tolerance for voltages")));
  Props.push_back(qucs::Property("saveOPs", "no", false,
		QObject::tr("put operating points into dataset")+
		" [yes, no]"));
  Props.push_back(qucs::Property("MaxIter", "150", false,
		QObject::tr("maximum number of iterations until error")));
  Props.push_back(qucs::Property("saveAll", "no", false,
	QObject::tr("save subcircuit nodes into dataset")+
	" [yes, no]"));
  Props.push_back(qucs::Property("convHelper", "none", false,
	QObject::tr("preferred convergence algorithm")+
	" [none, gMinStepping, SteepestDescent, LineSearch, Attenuation, SourceStepping]"));
  Props.push_back(qucs::Property("Solver", "CroutLU", false,
	QObject::tr("method for solving the circuit matrix")+
	" [CroutLU, DoolittleLU, HouseholderQR, HouseholderLQ, GolubSVD]"));
}

DC_Sim::~DC_Sim()
{
}

Component* DC_Sim::newOne()
{
  return new DC_Sim();
}

Element* DC_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("dc simulation");
  BitmapFile = (char *) "dc";

  if(getNewOne)  return new DC_Sim();
  return 0;
}
