/***************************************************************************
                               subcircuit.cpp
                              ----------------
    copyright            : (C) 2003 by Michael Margraf
                               2018 Felix Salfelder / QUCS team
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_LEGACY_SUBCKT_H
#define QUCS_LEGACY_SUBCKT_H


#include "../components/component.h"


// BUG: must derive from subckt_model (or so)
class Subcircuit : public MultiViewComponent  {
public:
  Subcircuit();
 ~Subcircuit() {};
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);

private: // not yet
public:
  QString getSubcircuitFile(SchematicModel const* scope) const;

protected:
  QString netlist() const;
  QString vhdlCode(int);
  QString verilogCode(int);
  void createSymbol();
  void remakeSymbol(int No);
  int  loadSymbol(const QString&);

private: // overrides
  void build();
  Element* proto(SchematicModel const* schem) const;
	  // obsolete.
  void tAC(QTextStream&, SchematicModel const*, QStringList&, int&, int, NetLang const&);
};

#endif
