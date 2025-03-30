/***************************************************************************
                               component.h
                              -------------
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

#ifndef LEGACY_COMPONENT_H
#define LEGACY_COMPONENT_H

#include "../component.h"
#include "trace.h"
#include <list>

#include <QTextStream>



class MultiViewComponent : public Component {
public:
  MultiViewComponent() {};
  virtual ~MultiViewComponent() {};

  void recreate(Schematic*);

protected:
  virtual void createSymbol() {};
};


// BUG wrong place.
class GateComponent : public MultiViewComponent {
public:
  GateComponent();
  QString netlist();
  QString vhdlCode(int);
  QString verilogCode(int);

  bool param_is_printable(int i)const override;

protected:
  void createSymbol();
};

// prototype of independent function
std::shared_ptr<Component> getComponentFromName(QString& Line, Schematic* p=NULL);

#endif
