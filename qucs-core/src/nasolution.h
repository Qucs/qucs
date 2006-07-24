/*
 * nasolution.h - nodal analysis solution template class definitions
 *
 * Copyright (C) 2006 Stefan Jahn <stefan@lkcc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *
 * $Id: nasolution.h,v 1.1 2006/07/24 08:07:41 raimi Exp $
 *
 */

#ifndef __NASOLUTION_H__
#define __NASOLUTION_H__

/* The solution entry class. */
template <class nr_type_t>
class naentry
{
 public:
  naentry ();
  naentry (const naentry &);
  naentry (char *, nr_type_t, int);
  ~naentry ();

 public:
  char * n;
  int current;
  nr_type_t value;
};

/* The solution class. */
template <class nr_type_t>
class nasolution
{
 public:
  nasolution ();
  ~nasolution ();
  nasolution (const nasolution &);

  void add (char *, nr_type_t, int);
  naentry<nr_type_t> * find (char *, int);
  void clear (void);

 private:
  valuelist< naentry<nr_type_t> > entries;
};

#include "nasolution.cpp"

#endif /* __NASOLUTION_H__ */
