/*
 * vector.h - vector class definitions
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
 * $Id: vector.h,v 1.12 2004-10-13 14:43:18 ela Exp $
 *
 */

#ifndef __VECTOR_H__
#define __VECTOR_H__

#ifdef log2
#undef log2
#endif

class complex;
class strlist;

class vector : public object
{
 public:
  vector ();
  vector (char *);
  vector (int);
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
  strlist * getDependencies (void) { return dependencies; }
  void setDependencies (strlist * s) { dependencies = s; }
  void setOrigin (char *);
  char * getOrigin (void);
  nr_double_t maximum (void);
  nr_double_t minimum (void);

  friend complex sum  (vector);
  friend complex prod (vector);
  friend complex avg  (vector);

  // vector manipulations
  friend vector real  (vector);  // the real part
  friend vector imag  (vector);  // the imaginary part
  friend vector conj  (vector);  // the complex conjugate
  friend vector norm  (vector);  // the square of the magnitude
  friend vector arg   (vector);  // the angle in the plane
  friend vector dB    (vector);
  friend vector ln    (vector);
  friend vector log2  (vector);
  friend vector pow   (vector, const complex);
  friend vector pow   (vector, const nr_double_t);
  friend vector pow   (const complex, vector);
  friend vector pow   (const nr_double_t, vector);
  friend vector pow   (vector, vector);
  friend vector ztor  (vector, nr_double_t zref = 50.0);
  friend vector rtoz  (vector, nr_double_t zref = 50.0);
  friend vector diff  (vector, vector, int n = 1);

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

 private:
  int requested;
  int size;
  int capacity;
  strlist * dependencies;
  complex * data;
  char * origin;
};

#endif /* __VECTOR_H__ */
