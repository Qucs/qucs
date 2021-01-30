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

//#include <QTextStream>
//#include <QDebug>
#include "object.h"
#include "language.h"
#include "output.h"
/*--------------------------------------------------------------------------*/
class DocumentFormat;
class Component;
class CommonData;
/*--------------------------------------------------------------------------*/
// simulator controller
struct SimCtrl{
  virtual void stateChange() = 0;
  virtual void message(int level, std::string msg){
    trace2("received message", level, msg);
  }
};
/*--------------------------------------------------------------------------*/
// must be Element, so it fits into a cl
class Simulator : public Data {
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
  virtual Simulator* clone() const = 0;

  virtual NetLang const* netLang() const{return nullptr;}
  virtual DocumentFormat const* netLister() const{return nullptr;}
//  virtual SimOutputData const* results(){}
//
private: // Element
  void paint(ViewPainter*) const override{ untested(); }

public:
  void attachDoc(QucsDoc*);
  QucsDoc const* doc() const {return _doc;}
  virtual void run(istream_t& cs, SimCtrl* ctx) = 0; // really??
  virtual void join() = 0;
  virtual void do_it(istream_t& cs, SchematicModel const* ctx) = 0;
  virtual std::string errorString() const = 0;
  virtual void kill() = 0;

  int state() const{return _state;}
  void attachCtrl(SimCtrl*);
  void detachCtrl(SimCtrl const*);
  void message(QucsMsgType, std::string const&);

protected:
  void setState(state_t s){_state = s;}
  void notifyState(state_t);
  void releaseOutput(CommonData const* d) {
    attach(d);
  }

private:
  virtual void init() = 0;

protected:
  bool has_ctrl() const{return _ctrl;}

private:
  QucsDoc* _doc; // const?
  int _state;
  SimCtrl* _ctrl;
}; // Simulator
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet
