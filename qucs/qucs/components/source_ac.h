/***************************************************************************
                          source_ac.h  -  description
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

#ifndef SOURCE_AC_H
#define SOURCE_AC_H

#include "component.h"


class Source_ac : public Component  {
public:
  Source_ac();
  ~Source_ac();
  Component* newOne(){
	  return new Source_ac(*this);
  }
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
