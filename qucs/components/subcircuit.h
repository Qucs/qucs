/***************************************************************************
                          subcircuit.h  -  description
                             -------------------
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
//  static Component* info(QString&, char* &, bool getNewOne=false);
  void recreate();

private:
  void remakeSymbol(int No);
  int  loadSymbol(const QString&);

  void performModification();
  int  analyseLine(const QString&);
  bool getIntegers(const QString&, int *i1=0, int *i2=0, int *i3=0,
		   int *i4=0, int *i5=0, int *i6=0);
  bool Subcircuit::getPen(const QString&, QPen&, int);
};

#endif
