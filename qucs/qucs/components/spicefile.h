/***************************************************************************
                                 spicefile.h
                               ---------------
    begin                : Tue Dez 28 2004
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

#ifndef SPICEFILE_H
#define SPICEFILE_H

#include "component.h"


class SpiceFile : public Component  {
public:
  SpiceFile();
  ~SpiceFile();
  Component* newOne();
  static Component* info(QString&, char* &, bool getNewOne=false);
  void recreate();
  QString NetList();
  bool convertSpiceNetlist(QTextStream*, QString&);

  bool withSim;
};

#endif
