/***************************************************************************
                          ampere_dc_eu.h  -  description
                             -------------------
    begin                : Wed 03 june 2015
    copyright            : (C) 2015 Xavier Galtier
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

#ifndef AMPERE_DC_EU_H
#define AMPERE_DC_EU_H

#include "component.h"


class Ampere_dc_EU : public Component  {
public:
  Ampere_dc_EU();
  ~Ampere_dc_EU();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
