/***************************************************************************
                          dcfeed.h  -  description
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

#ifndef DCFEED_H
#define DCFEED_H

#include "component.h"


class dcFeed : public Component  {
public:
  dcFeed();
  ~dcFeed();
  Object* newOne() const {return new dcFeed(*this);}
  static Element* info(QString&, char* &, bool getNewOne=false);
};

#endif
