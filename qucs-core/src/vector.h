/*
 * vector.h - vector class definitions
 *
 * Copyright (C) 2003, 2004, 2005, 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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
 * $Id$
 *
 */

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "consts.h"
#include "precision.h"

#ifdef log2
#undef log2
#endif

class strlist;
class vector;

::vector linspace (nr_double_t, nr_double_t, int);
::vector logspace (nr_double_t, nr_double_t, int);
::vector runavg (::vector, const int);
::vector runavg (const nr_complex_t, const int);

class vector : public object
{
 public:
  vector ();
  vector (const char *);
  vector (int);
  vector (int, nr_complex_t);
  vector (const char *, int);
  vector (const vector &);
  const vector& operator = (const vector &);
  ~vector ();
  void add (nr_complex_t);
  void add (vector *);
  nr_complex_t get (int);
  void set (nr_double_t, int);
  void set (const nr_complex_t, int);
  int getSize (void) const;
  int checkSizes (vector, vector);
  int getRequested (void) { return requested; }
  void setRequested (int n) { requested = n; }
  void reverse (void);
  strlist * getDependencies (void);
  void setDependencies (strlist *);
  void setOrigin (char *);
  char * getOrigin (void);
  int contains (nr_complex_t, nr_double_t eps = NR_EPSI);
  void sort (bool ascending = true);
  void print (void);

  nr_double_t maximum  (void);
  nr_double_t minimum  (void);
  nr_double_t rms      (void);
  nr_double_t variance (void);
  nr_double_t stddev   (void);

  friend nr_complex_t sum     (vector);
  friend nr_complex_t prod    (vector);
  friend nr_complex_t avg     (vector);
  friend vector  cumsum  (vector);
  friend vector  cumprod (vector);
  friend vector  cumavg  (vector);
  friend vector  dbm     (vector, const nr_complex_t z = 50.0);
  friend nr_complex_t integrate (vector v, const nr_complex_t);
  friend nr_double_t integrate (vector v, const nr_double_t);

  // vector manipulations
  friend vector real   (vector);  // the real part
  friend vector imag   (vector);  // the imaginary part
  friend vector conj   (vector);  // the complex conjugate
  friend vector norm   (vector);  // the square of the magnitude
  friend vector arg    (vector);  // the angle in the plane
  friend vector dB     (vector);
  friend vector log    (vector);
  friend vector log2   (vector);
  friend vector pow    (vector, const nr_complex_t);
  friend vector pow    (vector, const nr_double_t);
  friend vector pow    (const nr_complex_t, vector);
  friend vector pow    (const nr_double_t, vector);
  friend vector pow    (vector, vector);
  friend vector ztor   (vector, nr_complex_t zref = 50.0);
  friend vector rtoz   (vector, nr_complex_t zref = 50.0);
  friend vector ytor   (vector, nr_complex_t zref = 50.0);
  friend vector rtoy   (vector, nr_complex_t zref = 50.0);
  friend vector diff   (vector, vector, int n = 1);
  friend vector unwrap (vector,
			nr_double_t tol = M_PI, nr_double_t step = 2 * M_PI);

  friend vector polar   (vector, const nr_complex_t);
  friend vector polar   (const nr_complex_t, vector);
  friend vector polar   (vector, vector);
  friend vector atan2   (vector, const nr_double_t);
  friend vector atan2   (const nr_double_t, vector);
  friend vector atan2   (vector, vector);
  friend vector dbm2w   (vector);
  friend vector w2dbm   (vector);
  friend vector xhypot  (vector, vector);
  friend vector xhypot  (vector, const nr_complex_t);
  friend vector xhypot  (vector, const nr_double_t);
  friend vector xhypot  (const nr_complex_t, vector);
  friend vector xhypot  (const nr_double_t, vector);

  // overloaded math functions
  friend vector abs     (vector);
  friend vector log10   (vector);
  friend vector exp     (vector);
  friend vector limexp  (vector);
  friend vector sqrt    (vector);
  friend vector sin     (vector);
  friend vector asin    (vector);
  friend vector cos     (vector);
  friend vector acos    (vector);
  friend vector tan     (vector);
  friend vector atan    (vector);
  friend vector cot     (vector);
  friend vector acot    (vector);
  friend vector sinh    (vector);
  friend vector asinh   (vector);
  friend vector cosh    (vector);
  friend vector acosh   (vector);
  friend vector asech   (vector);
  friend vector tanh    (vector);
  friend vector atanh   (vector);
  friend vector coth    (vector);
  friend vector acoth   (vector);
  friend vector signum  (vector);
  friend vector sign    (vector);
  friend vector sinc    (vector);
  friend vector ceil    (vector);
  friend vector floor   (vector);
  friend vector fix     (vector);
  friend vector round   (vector);
  friend vector sqr     (vector);
  friend vector step    (vector);
  friend vector jn      (const int, vector);
  friend vector yn      (const int, vector);
  friend vector i0      (vector);
  friend vector erf     (vector);
  friend vector erfc    (vector);
  friend vector erfinv  (vector);
  friend vector erfcinv (vector);

  // operator functions
  friend vector operator + (vector, vector);
  friend vector operator + (vector, const nr_complex_t);
  friend vector operator + (vector, const nr_double_t);
  friend vector operator + (const nr_complex_t, vector);
  friend vector operator + (const nr_double_t, vector);
  friend vector operator - (vector, vector);
  friend vector operator - (vector, const nr_complex_t);
  friend vector operator - (vector, const nr_double_t);
  friend vector operator - (const nr_complex_t, vector);
  friend vector operator - (const nr_double_t, vector);
  friend vector operator * (vector, vector);
  friend vector operator * (vector, const nr_complex_t);
  friend vector operator * (vector, const nr_double_t);
  friend vector operator * (const nr_complex_t, vector);
  friend vector operator * (const nr_double_t, vector);
  friend vector operator / (vector, vector);
  friend vector operator / (vector, const nr_complex_t);
  friend vector operator / (vector, const nr_double_t);
  friend vector operator / (const nr_complex_t, vector);
  friend vector operator / (const nr_double_t, vector);
  friend vector operator % (vector, vector);
  friend vector operator % (vector, const nr_complex_t);
  friend vector operator % (vector, const nr_double_t);
  friend vector operator % (const nr_complex_t, vector);
  friend vector operator % (const nr_double_t, vector);

  // comparisons
  //  friend int      operator == (const vector *, const vector *);
  //  friend int      operator != (const vector *, const vector *);

  // assignment operations
  vector operator  - ();
  vector operator  = (const nr_complex_t);
  vector operator  = (const nr_double_t);
  vector operator += (vector);
  vector operator += (const nr_complex_t);
  vector operator += (const nr_double_t);
  vector operator -= (vector);
  vector operator -= (const nr_complex_t);
  vector operator -= (const nr_double_t);
  vector operator *= (vector);
  vector operator *= (const nr_complex_t);
  vector operator *= (const nr_double_t);
  vector operator /= (vector);
  vector operator /= (const nr_complex_t);
  vector operator /= (const nr_double_t);

  // easy accessor operators
  nr_complex_t  operator () (int i) const { return data[i]; }
  nr_complex_t& operator () (int i) { return data[i]; }

 private:
  int requested;
  int size;
  int capacity;
  strlist * dependencies;
  nr_complex_t * data;
  char * origin;
};

#endif /* __VECTOR_H__ */
