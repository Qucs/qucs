/*
 * vector.cpp - vector class implementation
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
 * $Id: vector.cpp,v 1.15 2004-10-12 18:13:09 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <assert.h>

#include "complex.h"
#include "object.h"
#include "logging.h"
#include "strlist.h"
#include "vector.h"
#include "consts.h"

// Constructor creates an unnamed instance of the vector class.
vector::vector () : object () {
  capacity = size = 0;
  data = NULL;
  dependencies = NULL;
  origin = NULL;
  requested = 0;
}

/* Constructor creates an unnamed instance of the vector class with a
   given initial size. */
vector::vector (int s) : object () {
  assert (s >= 0);
  capacity = size = s;
  data = s > 0 ? (complex *) calloc (capacity, sizeof (complex)) : NULL;
  dependencies = NULL;
  origin = NULL;
  requested = 0;
}

// Constructor creates an named instance of the vector class.
vector::vector (char * n) : object (n) {
  capacity = size = 0;
  data = NULL;
  dependencies = NULL;
  origin = NULL;
  requested = 0;
}

/* This constructor creates a named instance of the vector class with
   a given initial size. */
vector::vector (char * n, int s) : object (n) {
  assert (s >= 0);
  capacity = size = s;
  data = s > 0 ? (complex *) calloc (capacity, sizeof (complex)) : NULL;
  dependencies = NULL;
  origin = NULL;
  requested = 0;
}

/* The copy constructor creates a new instance based on the given
   vector object. */
vector::vector (const vector & v) : object (v) {
  size = v.size;
  capacity = v.capacity;
  data = (complex *) malloc (sizeof (complex) * capacity);
  memcpy (data, v.data, sizeof (complex) * size);
  dependencies = v.dependencies ? new strlist (*v.dependencies) : NULL;
  origin = v.origin ? strdup (v.origin) : NULL;
  requested = v.requested;
}

/* The assignment copy constructor creates a new instance based on the
   given vector object.  It copies the data only and leaves any other
   properties untouched. */
const vector& vector::operator=(const vector & v) {
  if (&v != this) {
    size = v.size;
    capacity = v.capacity;
    if (data) { free (data); data = NULL; }
    if (capacity > 0) {
      data = (complex *) malloc (sizeof (complex) * capacity);
      if (size > 0) memcpy (data, v.data, sizeof (complex) * size);
    }
  }
  return *this;
}

// Destructor deletes a vector object.
vector::~vector () {
  if (data) free (data);
  if (dependencies) delete dependencies;
  if (origin) free (origin);
}

/* The function appends a new complex data item to the end of the
   vector and ensures that the vector can hold the increasing number
   of data items. */
void vector::add (complex c) {
  if (data == NULL) {
    size = 0; capacity = 64;
    data = (complex *) malloc (sizeof (complex) * capacity);
  }
  else if (size >= capacity) {
    capacity *= 2;
    data = (complex *) realloc (data, sizeof (complex) * capacity);
  }
  data[size++] = c;
}

/* This function appends the given vector to the vector. */
void vector::add (vector * v) {
  if (data == NULL) {
    size = 0; capacity = v->getSize ();
    data = (complex *) malloc (sizeof (complex) * capacity);
  }
  else if (size + v->getSize () > capacity) {
    capacity += v->getSize ();
    data = (complex *) realloc (data, sizeof (complex) * capacity);
  }
  for (int i = 0; i < v->getSize (); i++) data[size++] = v->get (i);
}

// Returns the complex data item at the given position.
complex vector::get (int i) {
  return data[i];
}

void vector::set (nr_double_t d, int i) {
  data[i] = complex (d);
}

void vector::set (const complex z, int i) {
  data[i] = z;
}

// The function returns the current size of the vector.
int vector::getSize (void) {
  return size;
}

int vector::checkSizes (vector v1, vector v2) {
  if (v1.getSize () != v2.getSize ()) {
    logprint (LOG_ERROR, "vector '%s' and '%s' have different sizes\n",
	      v1.getName (), v2.getName ());
    return 0;
  }
  return 1;
}

// searches the maximum value of the vector elements.
// complex numbers in the 1. and 4. quadrant are counted as "abs(c)".
// complex numbers in the 2. and 3. quadrant are counted as "-abs(c)".
nr_double_t vector::maximum (void) {
  complex c;
  nr_double_t d, max_D = -DBL_MAX;
  for (int i = 0; i < getSize (); i++) {
    c = data[i];
    d = fabs (arg (c)) < M_PI_2 ? abs (c) : -abs (c);
    if (d > max_D) max_D = d;
  }
  return max_D;
}

// searches the minimum value of the vector elements.
// complex numbers in the 1. and 4. quadrant are counted as "abs(c)".
// complex numbers in the 2. and 3. quadrant are counted as "-abs(c)".
nr_double_t vector::minimum (void) {
  complex c;
  nr_double_t d, min_D = DBL_MAX;
  for (int i = 0; i < getSize (); i++) {
    c = data[i];
    d = fabs (arg (c)) < M_PI_2 ? abs (c) : -abs (c);
    if (d < min_D) min_D = d;
  }
  return min_D;
}

complex sum (vector v) {
  complex result (0.0);
  for (int i = 0; i < v.getSize (); i++) result += v.get (i);
  return result;
}

complex prod (vector v) {
  complex result (1.0);
  for (int i = 0; i < v.getSize (); i++) result *= v.get (i);
  return result;
}

complex avg (vector v) {
  complex result (0.0);
  for (int i = 0; i < v.getSize (); i++) result += v.get (i);
  return result / v.getSize ();
}

vector abs (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (abs (v.get (i)), i);
  return result;
}

vector norm (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (norm (v.get (i)), i);
  return result;
}

vector arg (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (arg (v.get (i)), i);
  return result;
}

vector real (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (real (v.get (i)), i);
  return result;
}

vector imag (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (imag (v.get (i)), i);
  return result;
}

vector conj (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (conj (v.get (i)), i);
  return result;
}

vector dB (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++)
    result.set (10.0 * log10 (norm (v.get (i))), i);
  return result;
}

vector sqrt (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (sqrt (v.get (i)), i);
  return result;
}

vector exp (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (exp (v.get (i)), i);
  return result;
}

vector ln (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (ln (v.get (i)), i);
  return result;
}

vector log10 (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (log10 (v.get (i)), i);
  return result;
}

vector log2 (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (log2 (v.get (i)), i);
  return result;
}

vector pow (vector v, const complex z) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (pow (v.get(i), z), i);
  return result;
}

vector pow (vector v, const nr_double_t d) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (pow (v.get(i), d), i);
  return result;
}

vector pow (const complex z, vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (pow (z, v.get (i)), i);
  return result;
}

vector pow (const nr_double_t d, vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (pow (d, v.get (i)), i);
  return result;
}

vector pow (vector v1, vector v2) {
  assert (v1.checkSizes (v1, v2));
  vector result (v1);
  for (int i = 0; i < v1.getSize (); i++)
    result.set (pow (v1.get (i), v2.get (i)), i);
  return result;
}

vector sin (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (sin (v.get (i)), i);
  return result;
}

vector arcsin (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (arcsin (v.get (i)), i);
  return result;
}

vector arccos (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (arccos (v.get (i)), i);
  return result;
}

vector cos (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (cos (v.get (i)), i);
  return result;
}

vector tan (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (tan (v.get (i)), i);
  return result;
}

vector arctan (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (arctan (v.get (i)), i);
  return result;
}

vector cot (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (cot (v.get (i)), i);
  return result;
}

vector arccot (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (arccot (v.get (i)), i);
  return result;
}

vector sinh (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (sinh (v.get (i)), i);
  return result;
}

vector arsinh (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (arsinh (v.get (i)), i);
  return result;
}

vector cosh (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (cosh (v.get (i)), i);
  return result;
}

vector arcosh (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (arcosh (v.get (i)), i);
  return result;
}

vector tanh (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (tanh (v.get (i)), i);
  return result;
}

vector artanh (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (artanh (v.get (i)), i);
  return result;
}

vector coth (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (coth (v.get (i)), i);
  return result;
}

vector arcoth (vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (arcoth (v.get (i)), i);
  return result;
}

// converts impedance to reflexion coefficient
vector ztor (vector v, nr_double_t zref) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++)
    result.set (ztor (v.get (i), zref), i);
  return result;
}

// converts reflexion coefficient to impedance
vector rtoz (vector v, nr_double_t zref) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++)
    result.set (rtoz (v.get (i), zref), i);
  return result;
}

// differentiates 'var' with respect to 'dep' exactly 'n' times
vector diff (vector var, vector dep, int n) {
  int k, xi, yi, exchange = 0;
  vector x, y;
  // exchange dependent and independent variable if necessary
  if (var.getSize () < dep.getSize ()) {
    x = vector (var);
    y = vector (dep);
    exchange++;
  }
  else {
    x = vector (dep);
    y = vector (var);
  }
  assert (y.getSize () % x.getSize () == 0 && x.getSize () >= 2);
  vector result (y);
  complex c;

  for (k = 0; k < n; k++) {  // differentiate n times
    for (yi = xi = 0; yi < y.getSize (); yi++, xi++) {
      if (xi == x.getSize ()) xi = 0; // roll through independent vector
      if (xi == 0) {
	c = (y.get (yi + 1) - y.get (yi)) / (x.get (xi + 1) - x.get (xi));
      } else if (xi == x.getSize () - 1) {
	c = (y.get (yi) - y.get (yi - 1)) / (x.get (xi) - x.get (xi - 1));
      }
      else {
	c =
	  ((y.get (yi) - y.get (yi - 1)) / (x.get (xi) - x.get (xi - 1)) + 
	   (y.get (yi + 1) - y.get (yi)) / (x.get (xi + 1) - x.get (xi))) / 2;
      }
      result.set (exchange ? 1 / c : c, yi);
    }
    y = result;
  }
  return result;
}

vector vector::operator=(const complex c) {
  for (int i = 0; i < getSize (); i++) data[i] = c;
  return *this;
}

vector vector::operator=(const nr_double_t d) {
  for (int i = 0; i < getSize (); i++) data[i] = d;
  return *this;
}

vector vector::operator+=(vector v) {
  if (checkSizes (*this, v))
    for (int i = 0; i < getSize (); i++) data[i] += v.get (i);
  return *this;
}

vector vector::operator+=(const complex c) {
  for (int i = 0; i < getSize (); i++) data[i] += c;
  return *this;
}

vector vector::operator+=(const nr_double_t d) {
  for (int i = 0; i < getSize (); i++) data[i] += d;
  return *this;
}

vector operator+(vector v1, vector v2) {
  assert (v1.checkSizes (v1, v2));
  vector result (v1);
  result += v2;
  return result;
}

vector operator+(vector v, const complex c) {
  vector result (v);
  result += c;
  return result;
}

vector operator+(const complex c, vector v) {
  return v + c;
}

vector operator+(vector v, const nr_double_t d) {
  vector result (v);
  result += d;
  return result;
}

vector operator+(const nr_double_t d, vector v) {
  return v + d;
}

vector vector::operator-() {
  vector result (*this);
  for (int i = 0; i < getSize (); i++) result.set (-get (i), i);
  return result;
}

vector vector::operator-=(vector v) {
  if (checkSizes (*this, v))
    for (int i = 0; i < getSize (); i++) data[i] -= v.get (i);
  return *this;
}

vector vector::operator-=(const complex c) {
  for (int i = 0; i < getSize (); i++) data[i] -= c;
  return *this;
}

vector vector::operator-=(const nr_double_t d) {
  for (int i = 0; i < getSize (); i++) data[i] -= d;
  return *this;
}

vector operator-(vector v1, vector v2) {
  assert (v1.checkSizes (v1, v2));
  vector result (v1);
  result -= v2;
  return result;
}

vector operator-(vector v, const complex c) {
  vector result (v);
  result -= c;
  return result;
}

vector operator-(vector v, const nr_double_t d) {
  vector result (v);
  result -= d;
  return result;
}

vector operator-(const complex c, vector v) {
  vector result (v);
  result *= -1.0;
  result += c;
  return result;
}

vector operator-(const nr_double_t d, vector v) {
  vector result (v);
  result *= -1.0;
  result += d;
  return result;
}

vector vector::operator*=(vector v) {
  if (checkSizes (*this, v))
    for (int i = 0; i < getSize (); i++) data[i] *= v.get (i);
  return *this;
}

vector vector::operator*=(const complex c) {
  for (int i = 0; i < getSize (); i++) data[i] *= c;
  return *this;
}

vector vector::operator*=(const nr_double_t d) {
  for (int i = 0; i < getSize (); i++) data[i] *= d;
  return *this;
}

vector operator*(vector v1, vector v2) {
  assert (v1.checkSizes (v1, v2));
  vector result (v1);
  result *= v2;
  return result;
}

vector operator*(vector v, const complex c) {
  vector result (v);
  result *= c;
  return result;
}

vector operator*(vector v, const nr_double_t d) {
  vector result (v);
  result *= d;
  return result;
}

vector operator*(const complex c, vector v) {
  return v * c;
}

vector operator*(const nr_double_t d, vector v) {
  return v * d;
}

vector vector::operator/=(vector v) {
  if (checkSizes (*this, v))
    for (int i = 0; i < getSize (); i++) data[i] /= v.get (i);
  return *this;
}

vector vector::operator/=(const complex c) {
  for (int i = 0; i < getSize (); i++) data[i] /= c;
  return *this;
}

vector vector::operator/=(const nr_double_t d) {
  for (int i = 0; i < getSize (); i++) data[i] /= d;
  return *this;
}

vector operator/(vector v1, vector v2) {
  assert (v1.checkSizes (v1, v2));
  vector result (v1);
  result /= v2;
  return result;
}

vector operator/(vector v, const complex c) {
  vector result (v);
  result /= c;
  return result;
}

vector operator/(vector v, const nr_double_t d) {
  vector result (v);
  result /= d;
  return result;
}

vector operator/(const complex c, vector v) {
  vector result (v);
  result  = c;
  result /= v;
  return result;
}

vector operator/(const nr_double_t d, vector v) {
  vector result (v);
  result  = d;
  result /= v;
  return result;
}

vector operator%(vector v, const complex z) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (v.get (i) % z, i);
  return result;
}

vector operator%(vector v, const nr_double_t d) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (v.get (i) % d, i);
  return result;
}

vector operator%(const complex z, vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (z % v.get (i), i);
  return result;
}

vector operator%(const nr_double_t d, vector v) {
  vector result (v);
  for (int i = 0; i < v.getSize (); i++) result.set (d % v.get (i), i);
  return result;
}

vector operator%(vector v1, vector v2) {
  assert (v1.checkSizes (v1, v2));
  vector result (v1);
  for (int i = 0; i < v1.getSize (); i++)
    result.set (v1.get (i) % v2.get (i), i);
  return result;
}

/* This function reverses the order of the data list. */
void vector::reverse (void) {
  complex * buffer = (complex *) malloc (sizeof (complex) * size);
  for (int i = 0; i < size; i++)
    buffer[i] = data[size - 1 - i];
  free (data);
  data = buffer;
  capacity = size;
}

// Sets the origin (the analysis) of the vector.
void vector::setOrigin (char * n) {
  if (origin) free (origin);
  origin = n ? strdup (n) : NULL;
}

// Returns the origin (the analysis) of the vector.
char * vector::getOrigin (void) {
  return origin;
}
