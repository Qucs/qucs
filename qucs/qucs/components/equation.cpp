/***************************************************************************
                          equation.cpp  -  description
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

#include "equation.h"
#include "../main.h"


Equation::Equation()
{
  Description = QObject::tr("equation");

  QFontMetrics  metrics(QucsSettings.font);
  QRect r = metrics.boundingRect(0,0,0,0, Qt::AlignAuto, Description);
  int xb = r.width()  >> 1;
  int yb = r.height() >> 1;

  Lines.append(new Line(-xb, -yb, -xb,  yb,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-xb,  yb,  xb,  yb,QPen(QPen::darkBlue,2)));
  Texts.append(new Text(-xb+4,  yb-4, QObject::tr("Equation")));

  x1 = -xb-3; y1 = -yb-3;
  x2 =  xb+3; y2 =  yb+3;

  tx = x1+4;
  ty = y2+4;
  Sign  = "Eqn";
  Model = "Eqn";
  Name  = "Eqn";

  Props.append(new Property("y", "1", true));
}

Equation::~Equation()
{
}

Equation* Equation::newOne()
{
  return new Equation();
}
