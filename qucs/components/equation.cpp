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


Equation::Equation()
{
  Description = "equation";

  Lines.append(new Line(-24, -6,-24,  6,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-24,  6, 24,  6,QPen(QPen::darkBlue,2)));
  Texts.append(new Text(-20,  2,"Equation"));

  x1 = -27; y1 = -9;
  x2 =  27; y2 =  9;

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
