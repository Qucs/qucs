/***************************************************************************
                               opt_sim.cpp
                              -------------
    begin                : Sat Jul 22 2006
    copyright            : (C) 2006 by Michael Margraf
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

#include <qdir.h>
#include <qfile.h>

#include "opt_sim.h"
#include "main.h"


Optimize_Sim::Optimize_Sim()
{
  Description = QObject::tr("Optimization");

  Texts.append(new Text(0, 0, Description, QPen::darkBlue, QucsSettings.largeFontSize));

  x1 = -10; y1 = -9;
  x2 = x1+128; y2 = y1+41;

  tx = 0;
  ty = y2+1;
  Model = ".Opt";
  Name  = "Opt";

  Props.append(new Property("Sim", "", false, ""));
  Props.append(new Property("DE", "3|50|2|20|0.85|1|3|1e-6|10|100", false, ""));
}

Optimize_Sim::~Optimize_Sim()
{
}

Component* Optimize_Sim::newOne()
{
  return new Optimize_Sim();
}

Element* Optimize_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("optimization");
  BitmapFile = "optimize";

  if(getNewOne)  return new Optimize_Sim();
  return 0;
}

// -------------------------------------------------------
QString Optimize_Sim::NetList()
{
  QString s = "";
  createASCOFiles();
  s += "#\n";
  s += "# ASCO configuration file(s) created\n";
  s += "#\n";
  return s;
}

extern QDir QucsHomeDir;
 
// -----------------------------------------------------------
void Optimize_Sim::createASCOFiles()
{
  Property* pp;
  QFile afile(QucsHomeDir.filePath("asco.cfg"));
  if(afile.open(IO_WriteOnly)) {
    QTextStream stream(&afile);
    stream << "*\n";
    stream << "* ASCO configuration file for '" << Name << "'\n";
    stream << "*\n\n";

    stream << "#Optimization Flow#\n";
    stream << "Alter:no\n";
    stream << "MonteCarlo:no\n";
    stream << "AlterMC cost:0.00\n";
    stream << "ExecuteRF:no\n";
    stream << "#\n\n";

    stream << "#DE#\n";
    pp = Props.at(1);
    QString val;
    val = pp->Value.section('|',0,0);
    stream << "choice of method:" << val << "\n";
    val = pp->Value.section('|',1,1);
    stream << "maximum no. of iterations:" << val << "\n";
    val = pp->Value.section('|',2,2);
    stream << "Output refresh cycle:" << val << "\n";
    val = pp->Value.section('|',3,3);
    stream << "No. of parents NP:" << val << "\n";
    val= pp->Value.section('|',4,4);
    stream << "Constant F:" << val << "\n";
    val = pp->Value.section('|',5,5);
    stream << "Crossing Over factor CR:" << val << "\n";
    val = pp->Value.section('|',6,6);
    stream << "Seed for pseudo random number generator:" << val << "\n";
    val = pp->Value.section('|',7,7);
    stream << "Minimum Cost Variance:" << val << "\n";
    val = pp->Value.section('|',8,8);
    stream << "Cost objectives:" << val << "\n";
    val = pp->Value.section('|',9,9);
    stream << "Cost constraints:" << val << "\n";
    stream << "#\n\n";

    stream << "# Parameters #\n";
    int i=1;
    for(pp = Props.at(2); pp != 0; pp = Props.next(), i++) {
      if(pp->Name == "Var") {
	stream << "Parameter " << i << ":";
	val = pp->Value.section('|',0,0);
	stream << "#" << val << "#" << ":";
	val = pp->Value.section('|',2,2);
	stream << val << ":";
	val = pp->Value.section('|',3,3);
	stream << val << ":";
	val = pp->Value.section('|',4,4);
	stream << val << ":";
	val = pp->Value.section('|',5,5);
	stream << val << ":";
	val = pp->Value.section('|',1,1);
	stream << ((val == "yes") ? "OPT" : "---") << "\n";
      }
    }
    stream << "#\n\n";

    stream << "# Measurements #\n";
    for(pp = Props.at(2); pp != 0; pp = Props.next(), i++) {
      if(pp->Name == "Goal") {
	val = pp->Value.section('|',1,1);
	QString Type, Value;
	Value = pp->Value.section('|',2,2);
	if (val == "MIN" || val == "MAX" || val == "MON") {
	  Value = "---";
	}
	Type = val;
	val = pp->Value.section('|',0,0);
	stream << val <<  ":"
	       << "---" << ":"
	       << Type << ":" << Value << "\n";
      }
    }
    stream << "#\n\n";

    stream << "# Post Processing #\n";
    stream << "#\n\n";

    afile.close();
  }

  QDir ExtractDir(QucsHomeDir);
  if(!ExtractDir.cd("extract")) {
    if(!ExtractDir.mkdir("extract"))
      return;
    if(!ExtractDir.cd("extract"))
      return;
  }      

  for(pp = Props.at(2); pp != 0; pp = Props.next()) {
    if(pp->Name == "Goal") {
      QString VarName = pp->Value.section('|',0,0);
      QFile efile(ExtractDir.filePath(VarName));
      if(efile.open(IO_WriteOnly)) {
	QTextStream stream(&efile);
	stream << "# Info #\n";
	stream << "#\n\n";
	stream << "# Commands #\n";
	stream << "#\n\n";
	stream << "# Post Processing #\n";
	stream << "MEASURE_VAR:#SYMBOL#:SEARCH_FOR:'<indep " << VarName
	       << "':S_COL:01:P_LINE:01:P_COL:01:20" << "\n";
	stream << "#\n\n";
	efile.close();
      }
    }
  }
}
