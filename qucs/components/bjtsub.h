/***************************************************************************
                            bjtsub.h  -  description
                             -------------------
    begin                : Sat Jul 17 2004
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

#ifndef BJTSUB_H
#define BJTSUB_H

#include "component.h"


class BJTsub : public Component  {
public:
  BJTsub();
  ~BJTsub();
  Component* newOne();
  static Component* info(QString&, char* &, bool getNewOne=false);
  static Component* info_pnp(QString&, char* &, bool getNewOne=false);
  void recreate();
};

#endif
