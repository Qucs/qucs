/***************************************************************************
                          tr_sim.cpp  -  description
                             -------------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "tr_sim.h"
#include "../main.h"


TR_Sim::TR_Sim()
{
  Description = QObject::tr("transient simulation");

  QString  s = Description;
  s.replace(" ","\n");
  QFontMetrics  metrics(QucsSettings.largeFont);
  QRect r = metrics.boundingRect(0,0,0,0, Qt::AlignAuto, s);
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

  tx = x1+4;
  ty = y2+4;
  Sign  = ".TR";
  Model = ".TR";
  Name  = "TR";

  Props.append(new Property("Start", "0", true,
			QObject::tr("start time in seconds")));
  Props.append(new Property("Stop", "1 ms", true,
			QObject::tr("stop time in seconds")));
  Props.append(new Property("Step", "10 us", true,
			QObject::tr("time step in seconds")));
}

TR_Sim::~TR_Sim()
{
}

TR_Sim* TR_Sim::newOne()
{
  return new TR_Sim();
}
