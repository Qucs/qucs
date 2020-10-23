/***************************************************************************
    copyright            : (C) 2015, 2020 by Felix Salfelder
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

#include "object.h"
#include <assert.h>
#include <task_element.h>
#include <components/component.h>
#include <iostream>

#include <QTextStream>
#include <QDebug>
#include "object.h"
#include "language.h"

class DocumentFormat;
class NetLang;
class Component;
class QucsData;


// simulatorDriver maybe?
class Simulator : public Object{
protected:
  explicit Simulator() : _doc(nullptr) {
  }
public:
  virtual ~Simulator();
  virtual Simulator* clone() const = 0;

  virtual NetLang const* netLang() const{return nullptr;}
  virtual DocumentFormat const* netLister() const{return nullptr;}
//  virtual SimOutputData const* results(){}

public:
  void attachDoc(QucsDoc*);
  QucsDoc const* doc() const {return _doc;}
  virtual void run() = 0;

private:
  virtual void init() = 0;

protected:
  QucsData* data() {return _data;}

private:
  QucsDoc* _doc;
  QucsData* _data;
};


//obsolete?
class NetLang : public DocumentLanguage {
public:
  virtual ~NetLang(){}
};


#define QUCS_SIM_H__
#endif
// vim:ts=8:sw=2:noet
