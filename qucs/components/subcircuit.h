/***************************************************************************
                               subcircuit.h
                              --------------
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

#ifndef SUBCIRCUIT_H
#define SUBCIRCUIT_H

#include "component.h"


class Subcircuit : public Component  {
public:
  Subcircuit();
  ~Subcircuit();
  Component* newOne();
  QString NetList();
  QString VHDL_Code(int);
  void recreate(Schematic*);

private:
  void remakeSymbol(int No);
  int  loadSymbol(const QString&);
};

#endif
