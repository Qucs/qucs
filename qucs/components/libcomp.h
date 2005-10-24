/***************************************************************************
                               libcomp.h
                              -----------
    begin                : Fri Jun 10 2005
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

#ifndef LIBCOMP_H
#define LIBCOMP_H

#include "component.h"


class LibComp : public Component  {
public:
  LibComp();
 ~LibComp();
  Component* newOne();
  QString NetList();
  bool outputSubNetlist(QTextStream*);
  void recreate(QucsDoc*);

private:
  void remakeSymbol();
  int  loadSymbol();

  int  loadSection(const QString&, QString&);
};

#endif
