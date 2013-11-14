/*
 * matvec.h - matrix vector class definitions
 *
 * Copyright (C) 2004, 2005, 2006, 2007, 2009 Stefan Jahn <stefan@lkcc.org>
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
 * $Id$
 *
 */

#ifndef __MATVEC_H__
#define __MATVEC_H__

class matrix;

class matvec
{
 public:
  matvec ();
  matvec (int, int, int);
  matvec (const matvec &);
  ~matvec ();
  int getSize (void) { return size; }
  int getCols (void) { return cols; }
  int getRows (void) { return rows; }
  void setName (const char *);
  char * getName (void);
  void set (::vector, int, int);
  void set (matrix, int);
  ::vector get (int, int);
  matrix get (int);
  static char * createMatrixString (const char *, int, int);
  static char * createMatrixString (char, int, int);
  static char * isMatrixVector (char *, int&, int&);
  static matvec * getMatrixVector (::vector *, char *);
  static void getMatrixVectorSize (::vector *, char *, int&, int&, int&);

  // operator functions
  friend matvec operator + (matvec, matvec);
  friend matvec operator + (matvec, matrix);
  friend matvec operator + (matrix, matvec);
  friend matvec operator + (matvec, nr_complex_t);
  friend matvec operator + (nr_complex_t, matvec);
  friend matvec operator + (matvec, nr_double_t);
  friend matvec operator + (nr_double_t, matvec);
  friend matvec operator + (matvec, ::vector);
  friend matvec operator + (::vector, matvec);
  friend matvec operator - (matvec, matvec);
  friend matvec operator - (matvec, matrix);
  friend matvec operator - (matrix, matvec);
  friend matvec operator - (matvec, nr_complex_t);
  friend matvec operator - (nr_complex_t, matvec);
  friend matvec operator - (matvec, nr_double_t);
  friend matvec operator - (nr_double_t, matvec);
  friend matvec operator - (matvec, ::vector);
  friend matvec operator - (::vector, matvec);
  friend matvec operator / (matvec, nr_complex_t);
  friend matvec operator / (matvec, nr_double_t);
  friend matvec operator / (matvec, ::vector);
  friend matvec operator * (matvec, ::vector);
  friend matvec operator * (::vector, matvec);
  friend matvec operator * (matvec, nr_complex_t);
  friend matvec operator * (nr_complex_t, matvec);
  friend matvec operator * (matvec, nr_double_t);
  friend matvec operator * (nr_double_t, matvec);
  friend matvec operator * (matvec, matvec);
  friend matvec operator * (matvec, matrix);
  friend matvec operator * (matrix, matvec);

  // intrinsic operator functions
  matvec operator  - ();
  matvec operator += (matvec);
  matvec operator -= (matvec);

  // other operations
  friend matvec transpose (matvec);
  friend matvec conj      (matvec);
  friend ::vector det       (matvec);
  friend matvec inverse   (matvec);
  friend matvec pow       (matvec, int);
  friend matvec pow       (matvec, ::vector);
  friend matvec twoport   (matvec, char, char);
  friend matvec real      (matvec);
  friend matvec imag      (matvec);
  friend matvec abs       (matvec);
  friend matvec dB        (matvec);
  friend matvec arg       (matvec);
  friend matvec adjoint   (matvec);
  friend ::vector rollet    (matvec);
  friend ::vector b1        (matvec);

  friend matvec stos (matvec, nr_complex_t, nr_complex_t z0 = 50.0);
  friend matvec stos (matvec, nr_double_t, nr_double_t z0 = 50.0);
  friend matvec stos (matvec, ::vector, nr_complex_t z0 = 50.0);
  friend matvec stos (matvec, nr_complex_t, ::vector);
  friend matvec stos (matvec, ::vector, ::vector);
  friend matvec stoz (matvec, nr_complex_t z0 = 50.0);
  friend matvec stoz (matvec, ::vector);
  friend matvec ztos (matvec, nr_complex_t z0 = 50.0);
  friend matvec ztos (matvec, ::vector);
  friend matvec ztoy (matvec);
  friend matvec stoy (matvec, nr_complex_t z0 = 50.0);
  friend matvec stoy (matvec, ::vector);
  friend matvec ytos (matvec, nr_complex_t z0 = 50.0);
  friend matvec ytos (matvec, ::vector);
  friend matvec ytoz (matvec);

 private:
  int size;
  int rows;
  int cols;
  char * name;
  matrix * data;
};

#endif /* __MATVEC_H__ */
