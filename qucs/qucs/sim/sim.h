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
#include <components/component.h>
#include <iostream>

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
  void printItem(Element const*, stream_t&) const;
private:
  virtual void printInstance(Component const*, stream_t&) const = 0;
};

/*!
 * print an item
 * boldly ripped from gnucap.
 */
inline void NetLang::printItem(Element const* c, stream_t& s) const
{
  assert(c);
//  if (auto C=dynamic_cast<const Subcircuit*>(c)) {
//    still using obsolete code
//  }else
//  ...
  if (auto C=dynamic_cast<const Component*>(c)) {
    printInstance(C, s);
  }else{
    std::cerr << "incomplete\n";
  }
}


/*!
 * a dispatcher stub. hardly sophisticated, but functional
 */
template<class T>
class Dispatcher{
public:
  Dispatcher(std::string label, T const* what)
  {
    qDebug() << "dispatcher install" << label.c_str();
    assert(what);
    Stash[label] = what;
  }
  static T const* get(std::string const& s)
  {
    qDebug() << "dispatcher get" << s.c_str();
    return Stash[s];
  }
  static T const* get(QString const& s){
    return get(std::string(s));
  }
  static T const* get(char const* s){
    return get(std::string(s));
  }
private:
  static std::map<std::string, T const*> Stash;
};

#define QUCS_SIM_H__
#endif
// vim:ts=8:sw=2:noet
