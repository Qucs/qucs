/***************************************************************************
                                eqndefined.h
                               --------------
    begin                : Thu Apr 19 2007
    copyright            : (C) 2007 by Stefan Jahn
    email                : stefan@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef EQNDEFINED_H
#define EQNDEFINED_H

#include "component.h"


class EqnDefined : public MultiViewComponent  {
public:
  EqnDefined();
 ~EqnDefined() {};
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

protected:
  QString netlist() const;
  void createSymbol();
};

#endif
