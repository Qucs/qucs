/***************************************************************************
                        mosfet_depl.h  -  description
                             -------------------
    begin                : Fri Jun 4 2004
    copyright            : (C) 2003 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MOSFET_DEPL_H
#define MOSFET_DEPL_H

#include "component.h"


class MOSFET_depl : public Component  {
public:
  MOSFET_depl();
  ~MOSFET_depl();
  Component* newOne();
  static Component* info(QString&, char* &, bool getNewOne=false);
};

#endif
