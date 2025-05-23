/***************************************************************************
                               resistor.cpp
                              --------------
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
#include "resistor.h"
#include "node.h"
#include "schematic.h"


Resistor::Resistor(bool european)
{
  Description = QObject::tr("resistor");

  Props.push_back(qucs::Property("R", "50 Ohm", true,
	QObject::tr("ohmic resistance in Ohms")));
  Props.push_back(qucs::Property("Temp", "26.85", false,
	QObject::tr("simulation temperature in degree Celsius")));
  Props.push_back(qucs::Property("Tc1", "0.0", false,
	QObject::tr("first order temperature coefficient")));
  Props.push_back(qucs::Property("Tc2", "0.0", false,
	QObject::tr("second order temperature coefficient")));
  Props.push_back(qucs::Property("Tnom", "26.85", false,
	QObject::tr("temperature at which parameters were extracted")));

  // this must be the last property in the list !!!
  Props.push_back(qucs::Property("Symbol", "european", false,
		QObject::tr("schematic symbol")+" [european, US]"));
  if(!european)  Props.back().Value = "US";

  createSymbol();
  set_qucs_text_position(x1()+4, y2()+4);
  Model = "R";
  Name  = "R";
}

// -------------------------------------------------------
Component* Resistor::newOne()
{
  return new Resistor(Props.back().Value != "US");
}

// -------------------------------------------------------
void Resistor::createSymbol()
{
  if(Props.back().Value != "US") {
    Lines.push_back(qucs::Line(-18, -9, 18, -9,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line( 18, -9, 18,  9,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line( 18,  9,-18,  9,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line(-18,  9,-18, -9,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));
  }
  else {
    Lines.push_back(qucs::Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line(-18,  0,-15, -7,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line(-15, -7, -9,  7,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line( -9,  7, -3, -7,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line( -3, -7,  3,  7,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line(  3,  7,  9, -7,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line(  9, -7, 15,  7,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line( 15,  7, 18,  0,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line( 18,  0, 30,  0,QPen(Qt::darkBlue,2)));
  }

  Ports.push_back(qucs::Port(-30,  0));
  Ports.push_back(qucs::Port( 30,  0));

  set_x1(-30);
  set_y1(-11);
  set_x2(30);
  set_y2(11);
}

// -------------------------------------------------------
Element* Resistor::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Resistor");
  BitmapFile = (char *) "resistor";

  if(getNewOne)  return new Resistor();
  return 0;
}

// -------------------------------------------------------
Element* Resistor::info_us(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Resistor US");
  BitmapFile = (char *) "resistor_us";

  if(getNewOne)  return new Resistor(false);
  return 0;
}
