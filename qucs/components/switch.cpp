/***************************************************************************
                                switch.cpp
                               ------------
    begin                : Sat Feb 25 2006
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
#include "switch.h"
#include "node.h"
#include "schematic.h"


Switch::Switch()
{
  Description = QObject::tr("switch (time controlled)");

  Props.push_back(qucs::Property("init", "off", false,
		QObject::tr("initial state")+" [on, off]"));
  Props.push_back(qucs::Property("time", "1 ms", false,
		QObject::tr("time when state changes (semicolon separated list possible, even numbered lists are repeated)")));
  Props.push_back(qucs::Property("Ron", "0", false,
		QObject::tr("resistance of \"on\" state in ohms")));
  Props.push_back(qucs::Property("Roff", "1e12", false,
		QObject::tr("resistance of \"off\" state in ohms")));
  Props.push_back(qucs::Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));
  Props.push_back(qucs::Property("MaxDuration", "1e-6", false,
		QObject::tr("Max possible switch transition time (transition time 1/100 smallest value in 'time', or this number)")));
  Props.push_back(qucs::Property("Transition", "spline", false,
		QObject::tr("Resistance transition shape")+" [abrupt, linear, spline]"));

  createSymbol();
  set_qucs_text_position(x1()+4, y2()+4);
  Model = "Switch";
  Name  = "S";
}

// -------------------------------------------------------
Component* Switch::newOne()
{
  Switch *p = new Switch();
  p->Props.front().Value = Props.front().Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* Switch::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Switch");
  BitmapFile = (char *) "switch";

  if(getNewOne)  return new Switch();
  return 0;
}

// -------------------------------------------------------
QString Switch::netlist()
{
  QString s = Model+":"+Name;

  // output all node names
  s += " "+port(0).getConnection()->Name;
  s += " "+port(1).getConnection()->Name;

  // output all properties
  s += " "+prop(0).Name+"=\""+prop(0).Value+"\"";
  s += " "+prop(1).Name+"=\"["+prop(1).Value+"]\"";
  for(int i = 2; i < int(Props.size()); ++i)
    s += " "+prop(i).Name+"=\""+prop(i).Value+"\"";

  return s + '\n';
}

// -------------------------------------------------------
void Switch::createSymbol()
{
  if(Props.front().Value != "on") {
    Lines.push_back(qucs::Line(-15,  0, 15,-15,QPen(Qt::darkBlue,2)));
    set_y1(-17);
  }
  else {
    Lines.push_back(qucs::Line(-15,  0, 16,-5,QPen(Qt::darkBlue,2)));
    set_y1(-7);
  }

  Lines.push_back(qucs::Line(-30,  0,-15,  0,QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line( 17,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Arcs.push_back(qucs::Arc( 12, -3, 5, 5, 0, 16*360,QPen(Qt::darkBlue,2)));
  Ellips.push_back(qucs::Area(-18, -3, 6, 6, QPen(Qt::darkBlue,2),
                QBrush(Qt::darkBlue, Qt::SolidPattern)));

  Ports.push_back(qucs::Port(-30,  0));
  Ports.push_back(qucs::Port( 30,  0));

  set_x1(-30);
  set_x2(30);
  set_y2(7);
}
