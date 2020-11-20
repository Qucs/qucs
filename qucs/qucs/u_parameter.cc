/*$Id: u_parameter.cc,v 26.119 2009/09/09 13:27:53 al Exp $ -*- C++ -*-
 * Copyright (C) 2005 Albert Davis
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
 * A class for parameterized values
 * Used for .param statements
 * and passing arguments to models and subcircuits
 */
//testing=script,sparse 2006.07.14
//#include "l_stlextra.h"
#include "u_parameter.h"
//#include "u_lang.h"
/*--------------------------------------------------------------------------*/
void PARAM_LIST::parse(CS& cmd)
{
  (cmd >> "real |integer "); // ignore type
  size_t here = cmd.cursor();
  for (;;) {
    if (!(cmd.more() && (cmd.is_alpha() || cmd.match1('_')))) {
      break;
    }else{
    }
    std::string Name;
    PARAMETER<double> Value;
    cmd >> Name >> '=' >> Value;
    if (cmd.stuck(&here)) {untested();
      break;
    }else{
    }
//    if (OPT::case_insensitive) {
//      notstd::to_lower(&Name);
//    }else{
//    }
    _pl[Name] = Value;
  }
  cmd.check(bDANGER, "syntax error");
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::print(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(0);
//  for (const_iterator i = _pl.begin(); i != _pl.end(); ++i) {
//    if (i->second.has_hard_value()) {
//      print_pair(o, lang, i->first, i->second);
//    }else{
//    }
//  }
}
/*--------------------------------------------------------------------------*/
bool PARAM_LIST::is_printable(int i)const
{
  //BUG// ugly linear search
  int i_try = 0;
  for (const_iterator ii = _pl.begin(); ii != _pl.end(); ++ii) {
    if (i_try++ == i) {
      return ii->second.has_hard_value();
    }else{
    }
  }
  return false;
}
/*--------------------------------------------------------------------------*/
std::string PARAM_LIST::name(int i)const
{
  //BUG// ugly linear search
  int i_try = 0;
  for (const_iterator ii = _pl.begin(); ii != _pl.end(); ++ii) {
    if (i_try++ == i) {
      return ii->first;
    }else{
    }
  }
  return "";
}
/*--------------------------------------------------------------------------*/
std::string PARAM_LIST::value(int i)const
{
  //BUG// ugly linear search
  int i_try = 0;
  for (const_iterator ii = _pl.begin(); ii != _pl.end(); ++ii) {
    if (i_try++ == i) {
      return ii->second.string();
    }else{
    }
  }
  return "";
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::eval_copy(PARAM_LIST& p, const CARD_LIST* scope)
{
  assert(!_try_again);
  _try_again = p._try_again;

  for (iterator i = p._pl.begin(); i != p._pl.end(); ++i) {
    if (i->second.has_hard_value()) {
      if (_pl[i->first].has_hard_value()) {untested();
	_pl[i->first] = i->second.e_val(_pl[i->first], scope);
      }else{
	_pl[i->first] = i->second.e_val(NOT_INPUT, scope);
      }
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
const PARAMETER<double>& PARAM_LIST::deep_lookup(std::string Name)const
{
//  if (OPT::case_insensitive) {
//    notstd::to_lower(&Name);
//  }else{
//  }
  PARAMETER<double> & rv = _pl[Name];
  if (rv.has_hard_value()) {
    // found a value, return it
    return rv;
  }else if (_try_again) {
    // didn't find one, look in enclosing scope
    return _try_again->deep_lookup(Name);
  }else{
    // no enclosing scope to look in
    // really didn't find it, give up
    // return garbage value (NOT_INPUT)
    return rv;
  }
}
/*--------------------------------------------------------------------------*/
void PARAM_LIST::set(std::string Name, const std::string& Value)
{
//  if (OPT::case_insensitive) {
//    notstd::to_lower(&Name);
//  }else{
//  }
  trace2("PL::set", Name, Value);
  _pl[Name] = Value;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
bool Get(CS& cmd, const std::string& key, PARAMETER<bool>* val)
{
  if (cmd.umatch(key + ' ')) {
    if (cmd.skip1b('=')) {
      cmd >> *val;
    }else{
      *val = true;
    }
    return true;
  }else if (cmd.umatch("no" + key)) {
    *val = false;
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
bool Get(CS& cmd, const std::string& key, PARAMETER<int>* val)
{
  if (cmd.umatch(key + " {=}")) {
    *val = int(cmd.ctof());
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
