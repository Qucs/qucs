/***************************************************************************
                                sp_sim.h
                               ----------
    begin                : Sat Aug 23 2003
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

#ifndef SP_SIM_H
#define SP_SIM_H

#include "command.h"


class SP_Sim : public Command  {
public:
  SP_Sim();
  ~SP_Sim();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  void recreate(Schematic*);
};

#endif
