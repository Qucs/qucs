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
#include "language.h"

/*!
 * class to provide simulator duties
 */
class Simulator : public Object{
public:
  virtual ~Simulator(){}

  virtual NetLang const* netLang() const{return nullptr;}
//  virtual SimOutputData const* results(){}
};

//obsolete?
class NetLang : public DocumentLanguage {
public:
  virtual ~NetLang(){}
};


#define QUCS_SIM_H__
#endif
// vim:ts=8:sw=2:noet
