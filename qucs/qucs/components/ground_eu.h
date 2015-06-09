/***************************************************************************
                                 ground_eu.h
                                ----------
    begin                : Wed 03 june 2015
    copyright            : (C) 2015 by Xavier Galtier
    email                : none
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GROUND_EU_H
#define GROUND_EU_H

#include "component.h"


class Ground_EU : public Component  {
public:
  Ground_EU();
 ~Ground_EU();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

protected:
  QString netlist();
};

#endif
