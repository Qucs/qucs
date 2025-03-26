/***************************************************************************
                                ecvs.cpp
                               -----------
    copyright            : (C) 2013 by Richard Crozier
    email                : richard dot crozier at yahoo dot co dot uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "ecvs.h"
#include "schematic.h"

ecvs::ecvs()
{
  Description = QObject::tr("externally controlled voltage source");


//  Arcs.push_back(qucs::Arc( -3, -7,  7,  7,16*270, 16*180,QPen(Qt::darkBlue,2)));
//  Arcs.push_back(qucs::Arc( -3,  0,  7,  7, 16*90, 16*180,QPen(Qt::darkBlue,2)));

  // Circle in middle
  Arcs.push_back(qucs::Arc(-12,-12, 24, 24,     0, 16*360,QPen(Qt::darkBlue,2)));
  // The 'E' symbol in middle of circle
  // horizontal lines (actually drawn vertically here)
  Lines.push_back(qucs::Line(4, -3, 4, 3,QPen(Qt::darkGreen,2)));
  Lines.push_back(qucs::Line(0, -3, 0, 2,QPen(Qt::darkGreen,2)));
  Lines.push_back(qucs::Line(-4, -3, -4, 3,QPen(Qt::darkGreen,2)));
  // Vertical Line
  Lines.push_back(qucs::Line(4, -3, -4, -3,QPen(Qt::darkGreen,2)));

  // Wires at top and bottom
  Lines.push_back(qucs::Line(-30,  0,-12,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 30,  0, 12,  0,QPen(Qt::darkBlue,2)));
  // positive symbol
  Lines.push_back(qucs::Line( 18,  5, 18, 11,QPen(Qt::red,1)));
  Lines.push_back(qucs::Line( 21,  8, 15,  8,QPen(Qt::red,1)));
  // negative symbol
  Lines.push_back(qucs::Line(-18,  5,-18, 11,QPen(Qt::black,1)));

//  Lines.push_back(qucs::Line( -6,-17, -6,-21,QPen(Qt::darkBlue,1)));
//  Lines.push_back(qucs::Line( -8,-17, -8,-21,QPen(Qt::darkBlue,1)));
//  Lines.push_back(qucs::Line(-10,-17,-10,-21,QPen(Qt::darkBlue,1)));
//  Lines.push_back(qucs::Line( -3,-15, -3,-23,QPen(Qt::darkBlue,2)));
//  Lines.push_back(qucs::Line(-13,-15,-13,-23,QPen(Qt::darkBlue,2)));
//  Lines.push_back(qucs::Line( -3,-23,-13,-23,QPen(Qt::darkBlue,2)));
//  Lines.push_back(qucs::Line( -3,-15,-13,-15,QPen(Qt::darkBlue,2)));

  Ports.push_back(qucs::Port( 30,  0));
  Ports.push_back(qucs::Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  set_qucs_text_position(x1+4, y2+4);
  Model = "ECVS";
  Name  = "ECVS";

  Props.push_back(qucs::Property("U", "0 V", true,
		QObject::tr("voltage in Volts")));
//  Props.push_back(qucs::Property("Interpolator", "linear", false,
//		QObject::tr("interpolation type")+" [hold, linear, cubic]"));

  rotate();  // fix historical flaw
}

ecvs::~ecvs()
{
}

Component* ecvs::newOne()
{
  return new ecvs();
}

Element* ecvs::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Externally Controlled Voltage Source");
  BitmapFile = (char *) "ecvs";

  if(getNewOne)  return new ecvs();
  return 0;
}


// -------------------------------------------------------
QString ecvs::netlist()
{
  QString s = Model+":"+Name;

  // output all node names
  for(auto p1 = Ports.begin(); p1 != Ports.end(); ++p1)
    s += " "+p1->getConnection()->Name;   // node names

  // output all properties
  for(auto p2 = Props.begin(); p2 != Props.end(); ++p2)
    s += " "+p2->Name+"=\""+p2->Value+"\"";

  return s + "\n";
}

