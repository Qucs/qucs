/***************************************************************************
                         vpulse.h  -  description
                             -------------------
    begin                : Sat Sep 18 2004
    copyright            : (C) 2004 by Michael Margraf
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

#ifndef VPULSE_H
#define VPULSE_H

#include "component.h"


class vPulse : public Component  {
public:
  vPulse();
  ~vPulse();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
