/***************************************************************************
                               logical_and.cpp
                              -----------------
    begin                : Sun Sep 25 2005
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

#include "logical_and.h"

Logical_AND::Logical_AND()
{
  Description = QObject::tr("logical AND");
  Model = "AND";

  createSymbol();
  tx = x1+4;
  ty = y2+4;
}

Logical_AND::~Logical_AND()
{
}

Component* Logical_AND::newOne()
{
  Logical_AND* p = new Logical_AND();
  p->Props.first()->Value = Props.first()->Value;
  p->Props.last()->Value = Props.last()->Value;
  p->recreate(0);
  return p;
}

GraphicItem* Logical_AND::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("n-port AND");
  BitmapFile = (char *) "and";

  if(getNewOne)  return new Logical_AND();
  return 0;
}
