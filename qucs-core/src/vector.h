/*
 * vector.h - vector class definitions
 *
 * Copyright (C) 2003, 2004, 2005, 2006 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2006 Gunther Kraut <gn.kraut@t-online.de>
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
 * $Id: vector.h,v 1.22 2006-03-13 08:26:25 raimi Exp $
 *
 */

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "consts.h"
#include "precision.h"

#ifdef log2
#undef log2
#endif

class complex;
class strlist;
class vector;

vector linspace (nr_double_t, nr_double_t, int);
vector logspace (nr_double_t, nr_double_t, int);
vector runavg (vector, const int);
vector runavg (const complex, const int);

class vector : public object
{
 public:
  vector ();
  vector (char *);
  vector (int);
  vector (int, complex);
  vector (char *, int);
  vector (const vector &);
  const vector& operator = (const vector &);
  ~vector ();
  void add (complex);
  void add (vector *);
  complex get (int);
  void set (nr_double_t, int);
  void set (const complex, int);
  int getSize (void);
  int checkSizes (vector, vector);
  int getRequested (void) { return requested; }
  void setRequested (int n) { requested = n; }
  void reverse (void);
  strlist * getDependencies (void);
  void setDependencies (strlist *);
  void setOrigin (char *);
  char * getOrigin (void);
  int contains (complex, nr_double_t eps = NR_EPSI);
  void sort (bool ascending = true);

  nr_double_t maximum  (void);
  nr_double_t minimum  (void);
  nr_double_t rms      (void);
  nr_double_t variance (void);
  nr_double_t stddev   (void);

  friend complex sum     (vector);
  friend complex prod    (vector);
  friend complex avg     (vector);
  friend vector  cumsum  (vector);
  friend vector  cumprod (vector);
  friend vector  cumavg  (vector);
  friend vector  dbm     (vector, const complex z = 50.0);
  friend complex integrate (vector v, const complex);
  friend nr_double_t integrate (vector v, const nr_double_t);

  // vector manipulations
  friend vector real   (vector);  // the real part
  friend vector imag   (vector);  // the imaginary part
  friend vector conj   (vector);  // the complex conjugate
  friend vector norm   (vector);  // the square of the magnitude
  friend vector arg    (vector);  // the angle in the plane
  friend vector dB     (vector);
  friend vector ln     (vector);
  friend vector log2   (vector);
  friend vector pow    (vector, const complex);
  friend vector pow    (vector, const nr_double_t);
  friend vector pow    (const complex, vector);
  friend vector pow    (const nr_double_t, vector);
  friend vector pow    (vector, vector);
  friend vector ztor   (vector, complex zref = 50.0);
  friend vector rtoz   (vector, complex zref = 50.0);
  friend vector ytor   (vector, complex zref = 50.0);
  friend vector rtoy   (vector, complex zref = 50.0);
  friend vector diff   (vector, vector, int n = 1);
  friend vector unwrap (vector, nr_double_t tol = M_PI);

  friend vector polar   (vector, const complex);
  friend vector polar   (const complex, vector);
  friend vector polar   (vector, vector);
  friend vector arctan2 (vector, const nr_double_t);
  friend vector arctan2 (const nr_double_t, vector);
  friend vector arctan2 (vector, vector);
  friend vector dbm2w   (vector);
  friend vector w2dbm   (vector);

  // overloaded math functions
  friend vector abs    (vector);
  friend vector log10  (vector);
  friend vector exp    (vector);
  friend vector sqrt   (vector);
  friend vector sin    (vector);
  friend vector arcsin (vector);
  friend vector cos    (vector);
  friend vector arccos (vector);
  friend vector tan    (vector);
  friend vector arctan (vector);
  friend vector cot    (vector);
  friend vector arccot (vector);
  friend vector sinh   (vector);
  friend vector arsinh (vector);
  friend vector cosh   (vector);
  friend vector arcosh (vector);
  friend vector tanh   (vector);
  friend vector artanh (vector);
  friend vector coth   (vector);
  friend vector arcoth (vector);
  friend vector sign   (vector);
  friend vector sinc   (vector);
  friend vector ceil   (vector);
  friend vector floor  (vector);
  friend vector fix    (vector);
  friend vector round  (vector);
  friend vector sqr    (vector);
  friend vector step   (vector);
  friend vector jn     (const int, vector);
  friend vector yn     (const int, vector);
  friend vector erf    (vector);
  friend vector erfc   (vector);

  // operator functions
  friend vector operator + (vector, vector);
  friend vector operator + (vector, const complex);
  friend vector operator + (vector, const nr_double_t);
  friend vector operator + (const complex, vector);
  friend vector operator + (const nr_double_t, vector);
  friend vector operator - (vector, vector);
  friend vector operator - (vector, const complex);
  friend vector operator - (vector, const nr_double_t);
  friend vector operator - (const complex, vector);
  friend vector operator - (const nr_double_t, vector);
  friend vector operator * (vector, vector);
  friend vector operator * (vector, const complex);
  friend vector operator * (vector, const nr_double_t);
  friend vector operator * (const complex, vector);
  friend vector operator * (const nr_double_t, vector);
  friend vector operator / (vector, vector);
  friend vector operator / (vector, const complex);
  friend vector operator / (vector, const nr_double_t);
  friend vector operator / (const complex, vector);
  friend vector operator / (const nr_double_t, vector);
  friend vector operator % (vector, vector);
  friend vector operator % (vector, const complex);
  friend vector operator % (vector, const nr_double_t);
  friend vector operator % (const complex, vector);
  friend vector operator % (const nr_double_t, vector);

  // comparisons
  //  friend int      operator == (const vector *, const vector *);
  //  friend int      operator != (const vector *, const vector *);

  // assignment operations
  vector operator  - ();
  vector operator  = (const complex);
  vector operator  = (const nr_double_t);
  vector operator += (vector);
  vector operator += (const complex);
  vector operator += (const nr_double_t);
  vector operator -= (vector);
  vector operator -= (const complex);
  vector operator -= (const nr_double_t);
  vector operator *= (vector);
  vector operator *= (const complex);
  vector operator *= (const nr_double_t);
  vector operator /= (vector);
  vector operator /= (const complex);
  vector operator /= (const nr_double_t);

  // easy accessor operators
  complex  operator () (int i) const { return data[i]; }
  complex& operator () (int i) { return data[i]; }

 private:
  int requested;
  int size;
  int capacity;
  strlist * dependencies;
  complex * data;
  char * origin;
};

#endif /* __VECTOR_H__ */
