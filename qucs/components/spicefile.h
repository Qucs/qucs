/***************************************************************************
                                 spicefile.h
                                -------------
    begin                : Tue Dec 28 2004
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


class SpiceFile : public MultiViewComponent  {
public:
  SpiceFile();
 ~SpiceFile() {};
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  QString NetList();

  bool withSim;

protected:
  void createSymbol();
};

#endif
