/***************************************************************************
                               param_sweep.h
                              ---------------
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

#ifndef PARAM_SWEEP_H
#define PARAM_SWEEP_H

#include "command.h"


class Param_Sweep : public Command  {
public:
  Param_Sweep();
  ~Param_Sweep();
  Element* newOne()const {return new Param_Sweep(*this);}
  static Element* info(QString&, char* &, bool getNewOne=false);
  void recreate(Schematic*);
};

#endif
