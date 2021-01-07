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

#include "component.h"
#include "module.h"
#include "globals.h"

namespace{

class TLine : public Component  {
  TLine(TLine const& t) : Component(t) {}
public:
  TLine();
  ~TLine();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
private:
  Element* clone() const{return new TLine(*this); }
}d0;
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "TLIN", &d0);
static Module::INSTALL pp("lumped", &d0);

TLine::TLine() : Component()
{
  setLabel("ideal transmission line");
  setTypeName("TLIN");

  Lines.append(new Line(-30,  0, 30,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-28,  7, 28,  7,QPen(Qt::darkBlue,2)));

  Lines.append(new Line(-28, 14,-21,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-21, 14,-14,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-14, 14, -7,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -7, 14,  0,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, 14,  7,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  7, 14, 14,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 14, 14, 21,  7,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( 21, 14, 28,  7,QPen(Qt::darkBlue,2)));

  Ports.append(new ComponentPort(-30, 0));
  Ports.append(new ComponentPort( 30, 0));

  x1 = -30; y1 = -4;
  x2 =  30; y2 = 16;

  tx = x1+4;
  ty = y2+4;
  Model = "TLIN";
  setLabel("Line");

  Props.append(new Property("Z", "50 Ohm", true,
		QObject::tr("characteristic impedance")));
  Props.append(new Property("L", "1 mm", true,
		QObject::tr("electrical length of the line")));
  Props.append(new Property("Alpha", "0 dB", false,
		QObject::tr("attenuation factor per length in 1/m")));
  Props.append(new Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));
}

TLine::~TLine()
{
}

Component* TLine::newOne()
{
  return new TLine();
}

#if 0
Element* TLine::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Transmission Line");
  BitmapFile = (char *) "tline";

  if(getNewOne)  return new TLine();
  return 0;
}
#endif

}
