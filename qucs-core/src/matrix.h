/*
 * matrix.h - matrix class definitions
 *
 * Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: matrix.h,v 1.2 2004-02-17 15:30:57 ela Exp $
 *
 */

#ifndef __MATRIX_H__
#define __MATRIX_H__

class matrix
{
 public:
  matrix ();
  matrix (int);
  matrix (int, int);
  matrix (const matrix &);
  const matrix& operator = (const matrix &);
  ~matrix ();
  complex get (int, int);
  void set (int, int, complex);
  int getCols (void) { return cols; }
  int getRows (void) { return rows; }
  void print (void);
  void exchangeRows (int, int);
  void exchangeCols (int, int);

  // operator functions
  friend matrix& operator + (matrix&, matrix&);
  friend matrix& operator - (matrix&, matrix&);
  friend matrix& operator * (matrix&, complex);
  friend matrix& operator / (matrix&, complex);
  friend matrix& operator * (complex, matrix&);
  friend matrix& operator * (matrix&, matrix&);

  // intrinsic operator functions
  matrix& operator += (matrix&);
  matrix& operator -= (matrix&);

  // other operations
  friend matrix& transpose (matrix&);
  friend matrix& conj (matrix&);
  friend matrix& eye (int, int);
  friend matrix& eye (int);
  friend complex adjoint (matrix&, int, int);
  friend complex det (matrix&);
  friend matrix& inverse (matrix&);
  friend matrix& stoz (matrix&, complex);
  friend matrix& stoy (matrix&, complex);

 private:
  int cols;
  int rows;
  complex * data;
};

#endif /* __MATRIX_H__ */
