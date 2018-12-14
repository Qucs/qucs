/***************************************************************************
                          msvia.h  -  description
                             -------------------
    begin                : Sat Oct 30 2004
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

#ifndef MSVIA_H
#define MSVIA_H

#include "component.h"


class MSvia : public Component  {
public:
  MSvia();
  ~MSvia();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

protected:
  QString netlist() const;
};

#endif
