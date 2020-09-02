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

// namespace {

// BUG: move to .cpp
// BUG: must derive from subckt_model (or so)
// Subcircuit is used as "subcircuit instance"..
// but a subcircuit instance is not a Component...
class Subcircuit : public Component  {
public:
  Subcircuit();
 ~Subcircuit() {};
private:
  Subcircuit(Subcircuit const&x);
  Component* newOne();

public: // obsolete.
  static Element* info(QString&, char* &, bool getNewOne=false);

private:
  Element* clone() const override{ untested(); return new Subcircuit(*this); }
public:
  QString getSubcircuitFile(SchematicModel const* scope) const;

protected:
  QString netlist() const;
  QString vhdlCode(int);
  QString verilogCode(int);
  void createSymbol(); // SchematicModel const& scope);
  void remakeSymbol(int No);
  int  loadSymbol(const QString&);

private: // Symbol
// unsigned numPorts() const override;
  bool portExists(unsigned) const override;
  QString portName(unsigned) const override;

  // void setParameter(QString const& name, QString const& value);
  void setParameter(unsigned i, std::string const& value) override;

private: // overrides
  void build();

  // create full prototype element, bit of a hack
  Symbol const* proto(SchematicModel const* schem) const override;
  
  // obsolete?
  void tAC(QTextStream&, SchematicModel const*, QStringList&, int&, int, NetLang const&);
};

// } // namespace
#endif
