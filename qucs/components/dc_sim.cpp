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
#include "main.h"


DC_Sim::DC_Sim()
{
  Description = QObject::tr("dc simulation");

  QFontMetrics  metrics(QucsSettings.largeFont);
  QSize r = metrics.size(0, Description);
  int xb = (r.width()  >> 1) + 6;
  int yb = (r.height() >> 1) + 4;

  Lines.append(new Line(-xb,-yb, xb,-yb,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-xb, yb, xb, yb,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-xb,-yb,-xb, yb,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( xb,-yb, xb, yb,QPen(QPen::darkBlue,2)));

  Lines.append(new Line(-xb+5, yb+5, xb+4, yb+5,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-xb,   yb,  -xb+5, yb+5,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( xb,   yb,   xb+4, yb+5,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( xb+4,-yb+5, xb+4, yb+5,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( xb,  -yb,   xb+4,-yb+5,QPen(QPen::darkBlue,2)));

  x1 = -xb-4; y1 = -yb-4;
  x2 =  xb+8; y2 =  yb+9;

  tx = x1+8;
  ty = y2+4;
  Model = ".DC";
  Name  = "DC";

  Props.append(new Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));
  Props.append(new Property("reltol", "0.001", false,
		QObject::tr("relative tolerance for convergence")));
  Props.append(new Property("abstol", "1 pA", false,
		QObject::tr("absolute tolerance for currents")));
  Props.append(new Property("vntol", "1 uV", false,
		QObject::tr("absolute tolerance for voltages")));
  Props.append(new Property("saveOPs", "no", false,
		QObject::tr("put operating points into dataset (yes,no)")));
  Props.append(new Property("MaxIter", "150", false,
		QObject::tr("maximum number of iterations until error")));
}

DC_Sim::~DC_Sim()
{
}

Component* DC_Sim::newOne()
{
  return new DC_Sim();
}

Component* DC_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("dc simulation");
  BitmapFile = "dc";

  if(getNewOne)  return new DC_Sim();
  return 0;
}
