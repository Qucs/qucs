/*
 * tmatrix.h - simple matrix template class definitions
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
 * $Id: tmatrix.h,v 1.2 2004/09/22 16:47:57 ela Exp $
 *
 */

#ifndef __TMATRIX_H__
#define __TMATRIX_H__

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
  int getCols (void) { return cols; }
  int getRows (void) { return rows; }
  nr_type_t * getData (void) { return data; }
  void exchangeRows (int, int);
  void exchangeCols (int, int);
  void print (void);

  // some basic matrix operations
  friend tmatrix& inverse<nr_type_t> (tmatrix &);
  static tmatrix& eye (int);
  friend tmatrix& operator *<nr_type_t> (tmatrix &, tmatrix &);

 private:
  int cols;
  int rows;
  nr_type_t * data;
};

#include "tmatrix.cpp"

#endif /* __TMATRIX_H__ */
