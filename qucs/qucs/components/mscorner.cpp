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
#include "globals.h"
#include "module.h"

namespace{

class MScorner : public Component  {
private:
  MScorner(MScorner const& m) : Component(m) {};
public:
  MScorner();
  ~MScorner();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
private:
  Element* clone() const{ return new MScorner(*this); }
}d0;
static Dispatcher<Symbol>::INSTALL p(&symbol_dispatcher, "MCORN", &d0);
static Module::INSTALL pp("RF", &d0);

MScorner::MScorner()
{
  setLabel("microstrip corner");
  setTypeName("MCORN");

  Lines.append(new Line(-30,  0,-18,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  0, 18,  0, 30,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-18, -8,  8, -8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-18,  8, -8,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(-18, -8,-18,  8,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -8,  8, -8, 18,QPen(Qt::darkBlue,2)));
  Lines.append(new Line(  8, -8,  8, 18,QPen(Qt::darkBlue,2)));
  Lines.append(new Line( -8, 18,  8, 18,QPen(Qt::darkBlue,2)));

  Ports.append(new ComponentPort(-30, 0));
  Ports.append(new ComponentPort(  0,30));

  x1 = -30; y1 =-11;
  x2 =  11; y2 = 30;

  tx = x2+4;
  ty = y1+4;
  Model = "MCORN";
  Name  = "MS";

  Props.append(new Property("Subst", "Subst1", true,
		QObject::tr("substrate")));
  Props.append(new Property("W", "1 mm", true,
		QObject::tr("width of line")));
}

MScorner::~MScorner()
{
}

Component* MScorner::newOne()
{
  return new MScorner();
}

Element* MScorner::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Microstrip Corner");
  BitmapFile = (char *) "mscorner";

  if(getNewOne)  return new MScorner();
  return 0;
}
}
