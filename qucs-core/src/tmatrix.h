/*
 * tmatrix.h - simple matrix template class definitions
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
 * $Id: tmatrix.h,v 1.11 2005-05-02 06:51:00 raimi Exp $
 *
 */

#ifndef __TMATRIX_H__
#define __TMATRIX_H__

template <class nr_type_t>
class tmatrix;

// Forward declarations of friend functions.
template <class nr_type_t>
tmatrix<nr_type_t> inverse (tmatrix<nr_type_t>);
template <class nr_type_t>
tmatrix<nr_type_t> teye (int);
template <class nr_type_t>
tmatrix<nr_type_t> operator * (tmatrix<nr_type_t>, tmatrix<nr_type_t>);
template <class nr_type_t>
tvector<nr_type_t> operator * (tmatrix<nr_type_t>, tvector<nr_type_t>);
template <class nr_type_t>
tvector<nr_type_t> operator * (tvector<nr_type_t>, tmatrix<nr_type_t>);

template <class nr_type_t>
class tmatrix
{
 public:
  tmatrix ();
  tmatrix (int);
  tmatrix (int, int);
  tmatrix (const tmatrix &);
  const tmatrix& operator = (const tmatrix &);
  ~tmatrix ();
  nr_type_t get (int, int);
  void set (int, int, nr_type_t);
  int  getCols (void) { return cols; }
  int  getRows (void) { return rows; }
  nr_type_t * getData (void) { return data; }
  void exchangeRows (int, int);
  void exchangeCols (int, int);
  void transpose (void);
  int  isFinite (void);
  void print (void);

  // some basic matrix operations
#ifndef _MSC_VER
  friend tmatrix inverse<> (tmatrix);
  friend tmatrix teye<nr_type_t> (int);
  friend tmatrix operator *<> (tmatrix, tmatrix);
  friend tvector<nr_type_t> operator *<> (tmatrix, tvector<nr_type_t>);
  friend tvector<nr_type_t> operator *<> (tvector<nr_type_t>, tmatrix);
#endif

  // easy accessor operators
  nr_type_t  operator () (int r, int c) const { return data[r * cols + c]; }
  nr_type_t& operator () (int r, int c) { return data[r * cols + c]; }

 private:
  int cols;
  int rows;
  nr_type_t * data;
};

#include "tmatrix.cpp"

#endif /* __TMATRIX_H__ */
