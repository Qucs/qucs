/***************************************************************************
                                ohmmeter.cpp
                               -----------
    begin                : Fri Sep 25 2016
    copyright            : (C) 2016 by Nelson Martins
    email                : macedo.pp@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef OHMMETER_H
#define OHMMETER_H

#include "component.h"


class Ohmmeter : public Component  {
public:
  Ohmmeter();
  ~Ohmmeter();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
