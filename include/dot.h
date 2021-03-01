/*$Id: d_dot.h 2014/11/23$ -*- C++ -*-
 * Copyright (C) 2007 Albert Davis
 *               2020 Felix Salfelder
 * Author: Albert Davis <aldavis@gnu.org>
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
 */
#ifndef QUCS_DOT_H
#define QUCS_DOT_H
//#include "e_card.h"
#include "element.h"
#define DEV_DOT Dot
/*--------------------------------------------------------------------------*/
namespace qucs {
/*--------------------------------------------------------------------------*/
class DEV_DOT : public Element {
private:
  std::string	_s;
  explicit DEV_DOT(const DEV_DOT& p)
    :Element(p), _s(p._s), _scope(p._scope), _c(p._c) {}//{set_constant(true);}
public:
  explicit	DEV_DOT()		:Element() {}//{set_constant(true);}
private: // override virtual
  std::string   value_name()const	{untested();return "";}
  char		id_letter()const	{untested();return '\0';}
  std::string	dev_type()const		{untested();return "dotcard";}
public: // override
  DEV_DOT* clone()const override {return new DEV_DOT(*this);}
  ElementList* scope() override;
public:
  void set(Command const* c) {_c = c;}
  void set(const std::string& S) {_s = S;}
  void set_scope(ElementList* s){_scope = s;} // yikes.
  const std::string& s()const {return _s;}
  Command const* c() const {return _c;}
private:
  ElementList* _scope;
  Command const* _c{nullptr};
};
/*--------------------------------------------------------------------------*/
inline ElementList* DEV_DOT::scope()
{
  if(_scope){
    return _scope;
  }else{ untested();
    return Element::scope();
  }
}
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
using qucs::Dot; // transition
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
