/***************************************************************************
                                noise_ii.h
                               ------------
    begin                : Sat Aug 20 2005
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

#ifndef NOISE_II_H
#define NOISE_II_H

#include "component.h"


class Noise_ii : public Component  {
public:
  Noise_ii();
 ~Noise_ii();
  Component* newOne() {
	  return new Noise_ii(*this);
  }
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
