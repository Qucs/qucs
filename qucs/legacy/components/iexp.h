/***************************************************************************
                         iexp.h  -  description
                        ------------------------
    begin                : Tue Mar 06 2007
    copyright            : (C) 2007 by Gunther Kraut
    email                : gn.kraut@t-online.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IEXP_H
#define IEXP_H

#include "component.h"


class iExp : public Component  {
public:
  iExp();
  ~iExp();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
