/***************************************************************************
                                sim.h
                              ----------
    begin                : yes
    copyright            : (C) 2015 by Felix Salfelder
    email                : felix@salfelder.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_SIM_H__

class NetLang;
class Component;

#include <assert.h>

#include <QTextStream>
#include <QDebug>
#define stream_t QTextStream

/*!
 * class to provide simulator duties
 */
class Simulator{
public:
  virtual ~Simulator(){}

  virtual NetLang const* netLang() const{return nullptr;}
//  virtual SimOutputData const* results(){}
};

/*!
 * class to provide language dependent functionality, such as netlisting
 * FIXME (later): don't use Qt types here. has nothing to do with GUI
 */
class NetLang{
public:
  virtual ~NetLang(){}

  virtual void printInstance(Component const*, stream_t&) const = 0;
};

/*!
 * a dispatcher stub. hardly sophisticated, but functional
 */
class SimulatorDispatcher{
public:
  SimulatorDispatcher(std::string label, Simulator const* what)
  {
    qDebug() << "dispatcher install" << label.c_str();
    assert(what);
    Simulators[label] = what;
  }
  static Simulator const* get(std::string const& s)
  {
    qDebug() << "dispatcher get" << s.c_str();
    return Simulators[s];
  }
private:
  static std::map<std::string, Simulator const*> Simulators;
};

#define QUCS_SIM_H__
#endif
// vim:ts=8:sw=2:noet
