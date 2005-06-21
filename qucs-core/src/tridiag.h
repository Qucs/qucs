/*
 * tridiag.h - tridiagonal matrix template class definitions
 *
 * Copyright (C) 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: tridiag.h,v 1.1 2005-06-21 07:21:06 raimi Exp $
 *
 */

#ifndef __TRIDIAG_H__
#define __TRIDIAG_H__

#include "tvector.h"

// Types of tridiagonal matrices.
enum tridiag_type {
  TRIDIAG_UNKNOWN = -1,
  TRIDIAG_NONSYM,
  TRIDIAG_SYM,
  TRIDIAG_NONSYM_CYCLIC,
  TRIDIAG_SYM_CYCLIC
};

template <class nr_type_t>
class tridiag
{
 public:
  tridiag ();
  tridiag (const tridiag &);
  const tridiag& operator = (const tridiag &);
  ~tridiag ();

  void setDiagonal (tvector<nr_type_t> *);
  void setOffDiagonal (tvector<nr_type_t> *);
  void setA (tvector<nr_type_t> *);
  void setB (tvector<nr_type_t> *);
  void setRHS (tvector<nr_type_t> *);
  void setType (int t) { type = t; }

  void solve (void);
  void solve_ns (void);
  void solve_ns_cyc (void);
  void solve_s (void);
  void solve_s_cyc (void);

 private:
  tvector<nr_type_t> * abov;
  tvector<nr_type_t> * belo;
  tvector<nr_type_t> * diag;
  tvector<nr_type_t> * offdiag;
  tvector<nr_type_t> * rhs;

  nr_type_t * d;
  nr_type_t * e;
  nr_type_t * f;
  nr_type_t * z;
  nr_type_t * c;
  nr_type_t * b;
  nr_type_t * x;
  nr_type_t * al;
  nr_type_t * be;
  nr_type_t * ga;
  nr_type_t * de;
  nr_type_t * ep;

  int type;
};

#include "tridiag.cpp"

#endif /* __TRIDIAG_H__ */
