/***************************************************************************
                                 hb_sim.h
                                ----------
    begin                : Mon Oct 3 2005
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

#ifndef DIGI_SIM_H
#define DIGI_SIM_H

#include "command.h"


class Digi_Sim : public Command {
public:
  Digi_Sim();
 ~Digi_Sim();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
