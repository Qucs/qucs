/***************************************************************************
                                noise_iv.h
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

#ifndef NOISE_IV_H
#define NOISE_IV_H

#include "component.h"


class Noise_iv : public Component  {
public:
  Noise_iv();
 ~Noise_iv();
  Component* newOne() {
    return new Noise_iv(*this);
  }
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
