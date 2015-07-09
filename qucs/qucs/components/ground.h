/***************************************************************************
                                 ground.h
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

#ifndef GROUND_H
#define GROUND_H

#include "component.h"


class Ground : public MultiViewComponent  {
public:
  Ground(bool european=true);
 ~Ground();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
  static Element* info_us(QString&, char* &, bool getNewOne=false);

protected:
  QString netlist();
  void createSymbol();
};

#endif
