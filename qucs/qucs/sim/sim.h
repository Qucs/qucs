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

#include "object.h"
#include <assert.h>
#include <command.h>
#include <components/component.h>
#include <iostream>

#include <QTextStream>
#include <QDebug>
#include "object.h"
#define stream_t QTextStream

/*!
 * class to provide simulator duties
 */
class Simulator : public Object{
public:
  virtual ~Simulator(){}

  virtual NetLang const* netLang() const{return nullptr;}
//  virtual SimOutputData const* results(){}
};

/*!
 * class to provide language dependent functionality, such as netlisting
 * FIXME (later): don't use Qt types here. has nothing to do with GUI
 */
class NetLang : public Object{
public:
  virtual ~NetLang(){}
  virtual void printItem(Element const*, stream_t&) const;
private: //called by printItem
  virtual void printPainting(Painting const*, stream_t&) const {incomplete();}
  virtual void printDiagram(Symbol const*, stream_t&) const {incomplete();}
  virtual void printSymbol(Symbol const*, stream_t&) const {incomplete();}
  virtual void printCommand(Command const*, stream_t&) const {incomplete();}
};

inline void NetLang::printItem(Element const* c, stream_t& s) const
{
  assert(c);
  if (auto C=dynamic_cast<const Command*>(c)) {
    printCommand(C, s);
  }else if (auto C=dynamic_cast<const Symbol*>(c)) {
    printSymbol(C, s);
  }else{
    incomplete();
  }
}


#define QUCS_SIM_H__
#endif
// vim:ts=8:sw=2:noet
