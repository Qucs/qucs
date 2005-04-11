/*
 * tvector.h - simple vector template class definitions
 *
 * Copyright (C) 2004, 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: tvector.h,v 1.12 2005-04-11 06:40:49 raimi Exp $
 *
 */

#ifndef __TVECTOR_H__
#define __TVECTOR_H__

template <class nr_type_t>
class tvector;

// Forward declarations of friend functions.
template <class nr_type_t>
nr_type_t   scalar (tvector<nr_type_t>, tvector<nr_type_t>);
template <class nr_type_t>
nr_double_t maxnorm (tvector<nr_type_t>);
template <class nr_type_t>
nr_double_t norm (tvector<nr_type_t>);
template <class nr_type_t>
nr_type_t   sum (tvector<nr_type_t>);
template <class nr_type_t>
tvector<nr_type_t> conj (tvector<nr_type_t>);
template <class nr_type_t>
tvector<nr_type_t> operator + (tvector<nr_type_t>, tvector<nr_type_t>);
template <class nr_type_t>
tvector<nr_type_t> operator + (tvector<nr_type_t>, nr_type_t);
template <class nr_type_t>
tvector<nr_type_t> operator + (nr_type_t, tvector<nr_type_t>);
template <class nr_type_t>
tvector<nr_type_t> operator - (tvector<nr_type_t>, tvector<nr_type_t>);
template <class nr_type_t>
tvector<nr_type_t> operator * (tvector<nr_type_t>, nr_double_t);
template <class nr_type_t>
tvector<nr_type_t> operator * (nr_double_t, tvector<nr_type_t>);
template <class nr_type_t>
tvector<nr_type_t> operator * (tvector<nr_type_t>, tvector<nr_type_t>);
template <class nr_type_t>
tvector<nr_type_t> operator - (tvector<nr_type_t>);
template <class nr_type_t>
bool operator < (tvector<nr_type_t>, tvector<nr_type_t>);
template <class nr_type_t>
bool operator > (tvector<nr_type_t>, tvector<nr_type_t>);

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
  void set (nr_type_t);
  void set (nr_type_t, int, int);
  void set (tvector, int, int);
  int  getSize (void) { return size; }
  nr_type_t * getData (void) { return data; }
  void exchangeRows (int, int);
  int  isFinite (void);
  void print (void);
  void reorder (int *);

  // some basic vector operations
#ifndef _MSC_VER
  friend tvector operator +<> (tvector, tvector);
  friend tvector operator -<> (tvector, tvector);
  friend tvector operator *<> (tvector, nr_double_t);
  friend tvector operator *<> (nr_double_t, tvector);
  friend tvector operator *<> (tvector, tvector);
  friend tvector operator -<> (tvector);
  friend tvector operator +<> (tvector, nr_type_t);
  friend tvector operator +<> (nr_type_t, tvector);
#endif

  // other operations
#ifndef _MSC_VER
  friend nr_double_t norm<> (tvector);
  friend nr_double_t maxnorm<> (tvector);
  friend nr_type_t   sum<> (tvector);
  friend nr_type_t   scalar<> (tvector, tvector);
  friend tvector     conj<> (tvector);
#endif

  // comparisons
#ifndef _MSC_VER
  friend bool operator < <> (tvector, tvector);
  friend bool operator > <> (tvector, tvector);
#endif

  // assignment operators
  tvector operator = (const nr_type_t);

  // easy accessor operators
  nr_type_t  operator () (int i) const { return data[i - 1]; }
  nr_type_t& operator () (int i) { return data[i - 1]; }

 private:
  int size;
  nr_type_t * data;
};

#include "tvector.cpp"

#endif /* __TVECTOR_H__ */
