/***************************************************************************
                             qucsator.cc
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

#include "sim.h"
#include "../components/component.h"
#include <QString>

namespace {
// qucslang language implementation
class QucsLang : public NetLang
{
  virtual void printInstance(Component const*, QTextStream&) const;
};

/*!
 * print Components in qucs language
 */
void QucsLang::printInstance(Component const* c, QTextStream& s) const
{
  // use callback hack (for now)
  s << c->getNetlist();
}

static QucsLang qucslang;

// qucsator simulator backend
class Qucsator : public Simulator
{
  NetLang const* netLang() const{return &qucslang;}
};

static Qucsator Q;
static SimulatorDispatcher p("qucsator", &Q);
}

// vim:ts=8:sw=2:noet
