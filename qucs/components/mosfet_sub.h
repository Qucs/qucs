/***************************************************************************
                        mosfet_sub.h  -  description
                             -------------------
    begin                : Fri Jun 4 2004
    copyright            : (C) 2003 by Michael Margraf
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

#ifndef MOSFET_SUB_H
#define MOSFET_SUB_H

#include "component.h"


class MOSFET_sub : public Component  {
public:
  MOSFET_sub();
  ~MOSFET_sub();
  Component* newOne();
  static Component* info(QString&, char* &, bool getNewOne=false);
  static Component* info_p(QString&, char* &, bool getNewOne=false);
  static Component* info_depl(QString&, char* &, bool getNewOne=false);
  void recreate();
};

#endif
