/***************************************************************************
                          ac_sim.cpp  -  description
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

#include "ac_sim.h"
#include "../main.h"


AC_Sim::AC_Sim()
{
  Description = QObject::tr("ac simulation");

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
  Model = ".AC";
  Name  = "AC";

  Props.append(new Property("Type", "lin", true,
			QObject::tr("sweep type")+" [lin, log]"));
  Props.append(new Property("Start", "1 GHz", true,
			QObject::tr("start frequency in Hertz")));
  Props.append(new Property("Stop", "10 GHz", true,
			QObject::tr("stop frequency in Hertz")));
  Props.append(new Property("Points", "19", true,
			QObject::tr("number of simulation steps")));
}

AC_Sim::~AC_Sim()
{
}

Component* AC_Sim::newOne()
{
  return new AC_Sim();
}

Component* AC_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("ac simulation");
  BitmapFile = "ac";

  if(getNewOne)  return new AC_Sim();
  return 0;
}
