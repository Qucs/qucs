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

#ifndef QUCS_SIMULATOR_H
#define QUCS_SIMULATOR_H

#include "object.h"
#include <assert.h>
#include <task_element.h>
#include <iostream>

#include <QTextStream>
#include <QDebug>
#include "object.h"
#include "language.h"
#include "data.h"

class DocumentFormat;
class Component;
class QucsData;

// simulator controller
struct SimCtrl{
  virtual void stateChange() = 0;
  virtual void message(int level, std::string msg){
    trace2("received message", level, msg);
  }
};

// simulatorDriver maybe?
class Simulator : public Object{
public:
  typedef enum {
    sst_killed = -2, // needed?
    sst_error = -1,
    sst_idle = 0,
    sst_running = 1
  } state_t;
protected:
  explicit Simulator();
public:
  virtual ~Simulator();
 Object::Type type() const override {return Object::Type::Simulator;}
  virtual Simulator* clone() const = 0;

  virtual NetLang const* netLang() const{return nullptr;}
  virtual DocumentFormat const* netLister() const{return nullptr;}
//  virtual SimOutputData const* results(){}

public:
  void attachDoc(QucsDoc*);
  QucsDoc const* doc() const {return _doc;}
  virtual void run(istream_t& cs, SimCtrl* ctx) = 0;
  virtual std::string errorString() const = 0;
  virtual void kill() = 0;

  int state() const{return _state;}
  void attachCtrl(SimCtrl*);
  void detachCtrl(SimCtrl const*);
  void message(QucsMsgType, std::string const&);

protected:
  void setState(state_t s){_state = s;}
  void notifyState(state_t);
  void releaseOutput(QucsData* d) {
    QucsData::attach(d, _data_p);
  }

public:
  void setOutput(QucsData** d) {
    assert(d);
    _data_p = d;
    // QucsData::attach(d, _data_p);
  }

private:
  virtual void init() = 0;

private:
  QucsDoc* _doc; // const?
  QucsData** _data_p;
  int _state;
  SimCtrl* _ctrl;
}; // Simulator




#endif
// vim:ts=8:sw=2:noet
