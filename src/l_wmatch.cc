/*$Id: l_wmatch.cc,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
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
 * wmatch: string match with wild cards
 * s1 may have wild cards: ? any character matches; * any repeated 0 or more
 * returns true or false
 * normally not case sensitive,
 *	but \ before any letter in s1 forces exact match
 * recursive
 */
//testing=script 2006.07.13
#include <cctype>
//#include "u_opt.h"
#include "library.h"
/*--------------------------------------------------------------------------*/
static char fix_case(char c)
{
  return c;
  // return ((OPT::case_insensitive) ? (static_cast<char>(tolower(c))) : (c));
}
/*--------------------------------------------------------------------------*/
bool wmatch_by_ptr(const char *s2, const char *s1)
{
  if (!*s2 && !*s1) {			// both end together -- match
    return true;
  }else if (!*s2 || !*s1) {		// ends don't match
    return false;
  }else if (fix_case(*s2) == fix_case(*s1)) { // one char matches - move on
    return wmatch_by_ptr(s2+1, s1+1);
  }else if (*s1 == '?') {		// ? wild card match - move on
    return wmatch_by_ptr(s2+1, s1+1);
  }else if (*s1 == '*') {		// * (repeat) wild card match
    if (wmatch_by_ptr(s2+1, s1)) {	// match 1, try for 2
      return true;
    }else if (wmatch_by_ptr(s2, s1+1)) {itested();	// match 0 - continue
      return true;
    }else{				// match 1, only 1
      return wmatch_by_ptr(s2+1, s1+1);
    }
  }else{				// mismatch
    return false;
  }
}
/*--------------------------------------------------------------------------*/
bool wmatch(const std::string& s1,const std::string& s2)
{
  bool r = wmatch_by_ptr(s1.c_str(), s2.c_str());
  trace3("wmatch", s1, s2, r);
  return r;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
