/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "bjtsub.h"
//#include "node.h"
#include "globals.h"

namespace{

class BJT : public Basic_BJT  {
public:
  BJT();
 ~BJT() {};
private:
  BJT(BJT const& b) : Basic_BJT(b){ }
  Element* clone() const override{ return new BJT(*this); }
  static Element* info(QString&, char* &, bool getNewOne=false);
  static Element* info_pnp(QString&, char* &, bool getNewOne=false);

protected:
  void createSymbol();
  QString netlist() const;
}D;
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "_BJT", &D);

BJT::BJT() : Basic_BJT()
{
  // properties obtained from "Basic_BJT" in bjtsub.cpp
  Description = QObject::tr("bipolar junction transistor");
  createSymbol();
  tx = x2+4;
  ty = y1+4;
  Model = "_BJT";
}

// -------------------------------------------------------
#if 0
Component* BJT::newOne()
{
  BJT* p = new BJT();
  p->Props.getFirst()->Value = Props.getFirst()->Value;
  p->recreate(0);
  return p;
}

// -------------------------------------------------------
Element* BJT::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("npn transistor");
  BitmapFile = (char *) "npn";

  if(getNewOne)  return new BJT();
  return 0;
}

// -------------------------------------------------------
Element* BJT::info_pnp(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("pnp transistor");
  BitmapFile = (char *) "pnp";

  if(getNewOne) {
    BJT* p = new BJT();
    p->Props.getFirst()->Value = "pnp";
    p->recreate(0);
    return p;
  }
  return 0;
}
#endif

// -------------------------------------------------------
void BJT::createSymbol()
{
  Lines.append(new Line(-10,-15,-10, 15,QPen(Qt::darkBlue,3)));
  Lines.append(new Line(-30,  0,-10,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10, -5,  0,-15,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0,-15,  0,-30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-10,  5,  0, 15,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, 15,  0, 30,QPen(Qt::darkBlue,2)));

  if(Props.getFirst()->Value == "npn") {
    Lines.append(new Line( -6, 15,  0, 15,QPen(Qt::darkBlue,2)));
    Lines.append(new Line(  0,  9,  0, 15,QPen(Qt::darkBlue,2)));
  }
  else {
    Lines.append(new Line( -5, 10, -5, 16,QPen(Qt::darkBlue,2)));
    Lines.append(new Line( -5, 10,  1, 10,QPen(Qt::darkBlue,2)));
  }

  Ports.append(new ComponentPort(-30,  0));
  Ports.append(new ComponentPort(  0,-30));
  Ports.append(new ComponentPort(  0, 30));
  Ports.append(new ComponentPort(  0,-30));

  x1 = -30; y1 = -30;
  x2 =   4; y2 =  30;
}

// -------------------------------------------------------
QString BJT::netlist() const
{
#if 0
  QString s = "BJT:"+name();

  // output all node names
  foreach(Port *p1, Ports)
    s += " "+p1->Connection->name();   // node names
  s += " "+Ports.at(1)->Connection->name();  // connect substrate to collector

  // output all properties
  for(Property *p2 = Props.first(); p2 != 0; p2 = Props.next())
    s += " "+p2->name()+"=\""+p2->Value+"\"";

  return s + '\n';
#endif
  return QString("incompleteBJT");
}

}
