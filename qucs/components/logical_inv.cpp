/***************************************************************************
                              logical_inv.cpp
                             -----------------
    begin                : Wed Sep 28 2005
    copyright            : (C) 2005 by Michael Margraf
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
#include "logical_inv.h"
#include "schematic.h"
#include "node.h"
#include "misc.h"


Logical_Inv::Logical_Inv()
{
  Type = isComponent;   // both analog and digital
  Description = QObject::tr("logical inverter");

  // the list order must be preserved !!!
  Props.push_back(qucs::Property("V", "1 V", false,
		QObject::tr("voltage of high level")));
  Props.push_back(qucs::Property("t", "0", false,
		QObject::tr("delay time")));
  Props.push_back(qucs::Property("TR", "10", false,
		QObject::tr("transfer function scaling factor")));

  // this must be the last property in the list !!!
  Props.push_back(qucs::Property("Symbol", "old", false,
		QObject::tr("schematic symbol")+" [old, DIN40900]"));

  createSymbol();
  set_qucs_text_position(x1()+4, y2()+4);
  Model = "Inv";
  Name  = "Y";
}

// -------------------------------------------------------
QString Logical_Inv::vhdlCode(int NumPorts)
{
  QString s = "  " + Ports.front().getConnection()->Name + " <= not " +
              Ports.back().getConnection()->Name;

  if(NumPorts <= 0) { // no truth table simulation ?
    QString td = prop(1).Value;
    if(!misc::VHDL_Delay(td, Name)) return td;
    s += td;
  }

  s += ";\n";
  return s;
}

// -------------------------------------------------------
QString Logical_Inv::verilogCode(int NumPorts)
{
  bool synthesize = true;
  qucs::Port &pp = port(0);
  QString s ("");

  if (synthesize) {
    s = "  assign";

    if(NumPorts <= 0) { // no truth table simulation ?
      QString td = prop(1).Value;
      if(!misc::Verilog_Delay(td, Name)) return td;
      s += td;
    }
    s += " ";
    s += pp.getConnection()->Name + " = ";  // output port
    pp = port(1);
    s += "~" + pp.getConnection()->Name;   // input port
    s += ";\n";
  }
  else {
    s = "  not";

    if(NumPorts <= 0) { // no truth table simulation ?
      QString td = prop(1).Value;
      if(!misc::Verilog_Delay(td, Name))
	return td;    // time has not VHDL format
      s += td;
    }
    s += " " + Name + " (" + pp.getConnection()->Name;  // output port
    pp = port(1);
    s += ", " + pp.getConnection()->Name; // first input port
    s += ");\n";
  }
  return s;
}

// -------------------------------------------------------
void Logical_Inv::createSymbol()
{
  int xr;

  if(Props.back().Value.at(0) == 'D') {  // DIN symbol
    Lines.push_back(qucs::Line( 15,-20, 15, 20,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line(-15,-20, 15,-20,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line(-15, 20, 15, 20,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line(-15,-20,-15, 20,QPen(Qt::darkBlue,2)));

    Texts.push_back(qucs::Text(-11,-17, "1", Qt::darkBlue, 15.0));
    xr =  15;
  }
  else {   // old symbol
    Lines.push_back(qucs::Line(-10,-20,-10,20, QPen(Qt::darkBlue,2)));
    Arcs.push_back(qucs::Arc(-30,-20, 40, 30,  0, 16*90,QPen(Qt::darkBlue,2)));
    Arcs.push_back(qucs::Arc(-30,-10, 40, 30,  0,-16*90,QPen(Qt::darkBlue,2)));
    Lines.push_back(qucs::Line( 10,-5, 10, 5,QPen(Qt::darkBlue,2)));
    xr =  10;
  }

  Ellips.push_back(qucs::Area(xr,-4, 8, 8,
                QPen(Qt::darkBlue,0), QBrush(Qt::darkBlue)));

  Lines.push_back(qucs::Line( xr, 0, 30, 0, QPen(Qt::darkBlue,2)));
  Lines.push_back(qucs::Line(-30, 0,-xr, 0, QPen(Qt::darkBlue,2)));
  Ports.push_back(qucs::Port( 30, 0));
  Ports.push_back(qucs::Port(-30, 0));

  set_x1(-30); set_y1(-23);
  set_x2(30); set_y2(23);
}

// -------------------------------------------------------
Component* Logical_Inv::newOne()
{
  Logical_Inv* p = new Logical_Inv();
  p->Props.back().Value = Props.back().Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* Logical_Inv::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Inverter");
  BitmapFile = (char *) "inverter";

  if(getNewOne)  return new Logical_Inv();
  return 0;
}
