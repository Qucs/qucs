/*
 * tvector.h - simple vector template class definitions
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: tvector.h,v 1.1 2004-10-12 18:13:09 ela Exp $
 *
 */

#ifndef __TVECTOR_H__
#define __TVECTOR_H__

template <class nr_type_t>
class tvector
{
 public:
  tvector ();
  tvector (int);
  tvector (const tvector &);
  const tvector& operator = (const tvector &);
  ~tvector ();
  nr_type_t get (int);
  void set (int, nr_type_t);
  int getSize (void) { return size; }
  nr_type_t * getData (void) { return data; }
  void exchangeRows (int, int);
  void print (void);

  // some basic vector operations
  friend tvector operator +<nr_type_t> (tvector, tvector);
  friend tvector operator -<nr_type_t> (tvector, tvector);
  friend tvector operator *<nr_type_t> (tvector, nr_double_t);
  friend tvector operator *<nr_type_t> (nr_double_t, tvector);

  friend nr_double_t norm<nr_type_t> (tvector);

 private:
  int size;
  nr_type_t * data;
};

#include "tvector.cpp"

#endif /* __TVECTOR_H__ */
