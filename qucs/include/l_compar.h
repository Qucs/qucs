/*$Id: l_compar.h,v 26.98 2008/10/24 06:10:07 al Exp $ -*- C++ -*-
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
 */
//testing=script,complete 2006.07.13
#ifndef L_COMPAR_H
#define L_COMPAR_H
// #include "md.h"
/*--------------------------------------------------------------------------*/
/* uporder: returns true if a,b,c are in non-decreasing order */
template <class T>
inline bool up_order(T a, T b, T c)
{
  return (a<=b) && (b<=c);
}

/* inorder: returns true if b is between a and c */
template <class T>
inline bool in_order(T a, T b, T c)
{
  return up_order(a,b,c) || up_order(c,b,a);
}

/* torange: returns b, clipped to range a..c */
template <class T>
inline T to_range(T a, T b, T c)
{
  return std::min(std::max(a,b),c);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
