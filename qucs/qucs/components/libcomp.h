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


class LibComp : public MultiViewComponent  {
public:
  LibComp();
 ~LibComp() {};
  Component* newOne();

  bool createSubNetlist(QTextStream*);
  bool createSubNetlist_Verilog(QTextStream*);
  bool createSubNetlist_VHDL(QTextStream*);

protected:
  QString netlist();
  QString vhdlCode(int);
  QString verilogCode(int);
  void createSymbol();

private:
  int  loadSymbol();
  int  loadSection(const QString&, QString&);
  QString createType();
};

#endif
