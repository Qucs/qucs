/***************************************************************************
                        wprobe.cpp  -  description
                             -------------------
    begin                : Sat Oct 30 2004
    copyright            : (C) 2015 by Pedro Macedo
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

#ifndef WPROBE_H
#define WPROBE_H

#include "component.h"


class wProbe : public Component  {
public:
  wProbe();
  ~wProbe();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
