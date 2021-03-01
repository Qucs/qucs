/*$Id: l_dispatcher.h  al -*- C++ -*-
 * Copyright (C) 2006 Albert Davis
 *               2020 Felix Salfelder
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * dispatcher -- for dynamically loaded modules
 *
 * To address the issue of unknown static construction order:
 * Must not have a constructor or destructor, or any parts that do.
 * Also must not have any virtual functions, because the v-table has a constructor.
 * "construction" will be done on demand by check_init.
 *
 * DISPATCHER is suitable for static objects only!
 */
//testing=manual 2020.04.29
#ifndef L_DISPATCHER__H
#define L_DISPATCHER__H
#include "e_base.h"
// #include "u_opt.h"
// #include "ap.h"
#include "exception.h"
#include "settings.h" // not options.
/*--------------------------------------------------------------------------*/
class DISPATCHER_BASE {
private:
  std::map<std::string, CKT_BASE*> _map;
public:
  typedef std::map<std::string, CKT_BASE*>::const_iterator const_iterator;
private:
  DISPATCHER_BASE(DISPATCHER_BASE const&) {unreachable();}
public:
  explicit DISPATCHER_BASE() {}
  ~DISPATCHER_BASE();
public:
  const_iterator begin()const		{return _map.begin();}
  const_iterator end()const		{return _map.end();}
public:
  CKT_BASE* operator[](std::string s);
private:
  void      uninstall(CKT_BASE* p);
  void	    install(const std::string& s, CKT_BASE* p);
public:
  class INSTALL {
  private:
    const std::string _name;
    DISPATCHER_BASE* _d;
    CKT_BASE* _p;
  public:
    INSTALL(DISPATCHER_BASE* d, const std::string& name, CKT_BASE* p) :
      _name(name),
      _d(d),
      _p(p)
    {
      assert(_d);
      assert(p);
      _d->install(_name, p);
    }

    ~INSTALL() {
      _d->uninstall(_p);
    }
  };
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// thin wrapper controlling initialisation.
// only static instances allowed.
template <class TT>
class INTERFACE DISPATCHER {
public:
  typedef typename DISPATCHER_BASE::const_iterator const_iterator;
  typedef typename DISPATCHER_BASE::INSTALL INSTALL;
public:
  TT* operator[](std::string s);
/* QUCS  TT* operator[](CS& cmd); */
  TT* clone(std::string s);
public: // forward to BASE
  DISPATCHER_BASE* operator&() {
    check_init();
    return _base;
  }
  const_iterator begin()const {
    assert(_base);
    return _base->begin();
  }
  const_iterator end()const {
    assert(_base);
    return _base->end();
  }
private:
  void check_init();
  DISPATCHER_BASE* _base;
};
/*--------------------------------------------------------------------------*/
// not sure if this is needed, could be put wherever used.
// just drop it?
template <class TT>
class INTERFACE D_DISPATCHER : public DISPATCHER_BASE {
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
inline DISPATCHER_BASE::~DISPATCHER_BASE()
{ untested();
#if !defined(NDEBUG)
  for (typename std::map<std::string, CKT_BASE*>::iterator
      ii = _map.begin();  ii != _map.end();  ++ii) { untested();
    assert(!(ii->second));
  }
#endif
}
/*--------------------------------------------------------------------------*/
template<class TT>
void DISPATCHER<TT>::check_init()
{
  if (!_base) {
    _base = new DISPATCHER_BASE;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
template <class TT>
TT* DISPATCHER<TT>::operator[](std::string s)
{
  check_init(); // main.cc gets here first (??)
  CKT_BASE* rv = (*_base)[s];
  if (!rv && OPT::case_insensitive) {
    notstd::to_lower(&s);
    rv = (*_base)[s];
  }else{
  }
  TT* t=prechecked_cast<TT*>(rv);
  assert(t || !rv);
  return(t);
}
/*--------------------------------------------------------------------------*/
#if 0 // qucs
template <class TT>
TT* DISPATCHER<TT>::operator[](CS& cmd)
{
  unsigned here = cmd.cursor();
  std::string s;
  cmd >> s;
  //------------------------
  TT* p = (*this)[s];
  //------------------------
  if (!p) {
    cmd.reset(here);
  }else{
  }
  return p;
}
#endif
/*--------------------------------------------------------------------------*/
template <class TT>
TT* DISPATCHER<TT>::clone(std::string s)
{
  TT* proto = (*this)[s];
  if (proto) {
    TT* c=prechecked_cast<TT*>(proto->clone());
    assert(c);
    return c;
  }else{
    return NULL;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
