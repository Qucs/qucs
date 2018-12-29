/***************************************************************************
                               logical_or.h
                              --------------
    begin                : Sun Sep 25 2005
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

#ifndef LOGICAL_OR_H
#define LOGICAL_OR_H

#include "component.h"


class Logical_OR : public GateComponent  {
private:
  Logical_OR(Logical_OR const& o)
  {
 	 Props.getFirst()->Value = o.Props.getFirst()->Value;
 	 Props.getLast()->Value = o.Props.getLast()->Value;
 	 recreate(0);
 	 return p;
  }

public:
  Logical_OR();
 ~Logical_OR();
  Element* newOne() const{
	  return new Logical_OR(*this);
  }
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
