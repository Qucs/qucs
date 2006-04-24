/*
 * tvector.cpp - simple vector template class implementation
 *
 * Copyright (C) 2004, 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: tvector.cpp,v 1.16 2006-04-24 08:25:46 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#include "complex.h"
#include "tvector.h"

#ifdef __MINGW32__
# define finite(x) _finite(x)
#endif

// Constructor creates an unnamed instance of the tvector class.
template <class nr_type_t>
tvector<nr_type_t>::tvector () {
  external = 0;
  capacity = size = 0;
  data = NULL;
}

/* Constructor creates an unnamed instance of the tvector class with a
   certain length. */
template <class nr_type_t>
tvector<nr_type_t>::tvector (int s)  {
  external = 0;
  capacity = size = s;
  if (s > 0) {
    data = new nr_type_t[s];
    memset (data, 0, sizeof (nr_type_t) * s);
  }
  else data = NULL;
}

/* The copy constructor creates a new instance based on the given
   tvector object. */
template <class nr_type_t>
tvector<nr_type_t>::tvector (const tvector & v) {
  external = 0;
  size = v.size;
  capacity = v.capacity;
  data = NULL;

  // copy tvector elements
  if (size > 0) {
    data = new nr_type_t[size];
    memcpy (data, v.data, sizeof (nr_type_t) * size);
  }
}

/* The assignment copy constructor creates a new instance based on the
   given tvector object. */
template <class nr_type_t>
const tvector<nr_type_t>&
tvector<nr_type_t>::operator=(const tvector<nr_type_t> & v) {
  if (&v != this) {
    size = v.size;
    capacity = v.capacity;
    if (data && !external) { delete[] data; data = NULL; }
    external = 0;
    if (size > 0) {
      data = new nr_type_t[size];
      memcpy (data, v.data, sizeof (nr_type_t) * size);
    }
  }
  return *this;
}

// Destructor deletes a tvector object.
template <class nr_type_t>
tvector<nr_type_t>::~tvector () {
  if (data && !external) delete[] data;
}

// Returns the tvector element at the given position.
template <class nr_type_t>
nr_type_t tvector<nr_type_t>::get (int i) {
  assert (i >= 0 && i < size);
  return data[i];
}

// Sets the tvector element at the given position.
template <class nr_type_t>
void tvector<nr_type_t>::set (int i, nr_type_t z) {
  assert (i >= 0 && i < size);
  data[i] = z;
}

// Sets all the tvector elements to the given value.
template <class nr_type_t>
void tvector<nr_type_t>::set (nr_type_t z) {
  for (int i = 0; i < size; i++) data[i] = z;
}

// Sets the specified tvector elements to the given value.
template <class nr_type_t>
void tvector<nr_type_t>::set (nr_type_t z, int start, int stop) {
  for (int i = start; i < stop; i++) data[i] = z;
}

// Appends the given value to the tvector.
template <class nr_type_t>
void tvector<nr_type_t>::add (nr_type_t z) {
  if (size >= capacity) {
    if (data) {
      // double the vectors capacity
      capacity *= 2;
      data = (nr_type_t *) realloc (data, capacity * sizeof (nr_type_t));
    }
    else {
      // initial capacity
      capacity = 4;
      data = (nr_type_t *) malloc (capacity * sizeof (nr_type_t));
    }
  }
  data[size++] = z;
}

// Rejects the given number of values in the tvector.
template <class nr_type_t>
void tvector<nr_type_t>::drop (int n) {
  if (n < size) {
    for (int i = 0; i < size - n; i++) data[i] = data[i + n];
    size -= n;
  }
  else size = 0;
}

// Sets size to zero.  Does not reduce the capacity.
template <class nr_type_t>
void tvector<nr_type_t>::clear (void) {
  size = 0;
}

/* The function returns the number of entries with the given value
   deviating no more than the given epsilon. */
template <class nr_type_t>
int tvector<nr_type_t>::contains (nr_type_t val, nr_double_t eps) {
  int count = 0;
  for (int i = 0; i < size; i++) if (abs (data[i] - val) <= eps) count++;
  return count;
}

// Copies the specified elements from the given tvector.
template <class nr_type_t>
void tvector<nr_type_t>::set (tvector<nr_type_t> a, int start, int stop) {
  for (int i = start; i < stop; i++) data[i] = a.get (i);
}

// Applies external data vector to the vector.
template <class nr_type_t>
void tvector<nr_type_t>::setData (nr_type_t * d, int len) {
  if (data && !external) delete[] data;
  external = 1;
  data = d;
  capacity = size = len;
}

// The function swaps the given rows with each other.
template <class nr_type_t>
void tvector<nr_type_t>::exchangeRows (int r1, int r2) {
  assert (r1 >= 0 && r2 >= 0 && r1 < size && r2 < size);
  nr_type_t s = data[r1];
  data[r1] = data[r2];
  data[r2] = s;
}

// Addition.
template <class nr_type_t>
tvector<nr_type_t> operator + (tvector<nr_type_t> a, tvector<nr_type_t> b) {
  assert (a.getSize () == b.getSize ());
  int n = a.getSize ();
  tvector<nr_type_t> res (n);
  for (int i = 0; i < n; i++) res.set (i, a.get (i) + b.get (i));
  return res;
}

// Intrinsic vector addition.
template <class nr_type_t>
tvector<nr_type_t> tvector<nr_type_t>::operator += (tvector<nr_type_t> a) {
  assert (a.getSize () == size);
  nr_type_t * src = a.getData ();
  nr_type_t * dst = data;
  for (int i = 0; i < size; i++) *dst++ += *src++;
  return *this;
}

// Subtraction.
template <class nr_type_t>
tvector<nr_type_t> operator - (tvector<nr_type_t> a, tvector<nr_type_t> b) {
  assert (a.getSize () == b.getSize ());
  int n = a.getSize ();
  tvector<nr_type_t> res (n);
  for (int i = 0; i < n; i++) res.set (i, a.get (i) - b.get (i));
  return res;
}

// Intrinsic vector substration.
template <class nr_type_t>
tvector<nr_type_t> tvector<nr_type_t>::operator -= (tvector<nr_type_t> a) {
  assert (a.getSize () == size);
  nr_type_t * src = a.getData ();
  nr_type_t * dst = data;
  for (int i = 0; i < size; i++) *dst++ -= *src++;
  return *this;
}

// Scalar multiplication.
template <class nr_type_t>
tvector<nr_type_t> operator * (nr_double_t s, tvector<nr_type_t> a) {
  int n = a.getSize ();
  tvector<nr_type_t> res (n);
  for (int i = 0; i < n; i++) res.set (i, s * a.get (i));
  return res;
}

template <class nr_type_t>
tvector<nr_type_t> operator * (tvector<nr_type_t> a, nr_double_t s) {
  return s * a;
}

// Vector multiplication (element by element).
template <class nr_type_t>
tvector<nr_type_t> operator * (tvector<nr_type_t> a, tvector<nr_type_t> b) {
  assert (a.getSize () == b.getSize ());
  int n = a.getSize ();
  tvector<nr_type_t> res (n);
  for (int i = 0; i < n; i++) res.set (i, a.get (i) * b.get (i));
  return res;
}

// Computes the scalar product of two vectors.
template <class nr_type_t>
nr_type_t scalar (tvector<nr_type_t> a, tvector<nr_type_t> b) {
  assert (a.getSize () == b.getSize ());
  nr_type_t n = 0;
  for (int i = 0; i < a.getSize (); i++) n += a.get (i) * b.get (i);
  return n;
}

// Constant assignment operation.
template <class nr_type_t>
tvector<nr_type_t> tvector<nr_type_t>::operator = (const nr_type_t val) {
  for (int i = 0; i < size; i++) data[i] = val;
  return *this;
}

// Returns the sum of the vector elements.
template <class nr_type_t>
nr_type_t sum (tvector<nr_type_t> a) {
  nr_type_t res = 0;
  for (int i = 0; i < a.getSize (); i++) res += a.get (i);
  return res;
}

// Vector negation.
template <class nr_type_t>
tvector<nr_type_t> operator - (tvector<nr_type_t> a) {
  int n = a.getSize ();
  tvector<nr_type_t> res (n);
  for (int i = 0; i < n; i++) res.set (i, -a.get (i));
  return res;
}

// Vector less comparison.
template <class nr_type_t>
bool operator < (tvector<nr_type_t> a, tvector<nr_type_t> b) {
  assert (a.getSize () == b.getSize ());
  int n = a.getSize ();
  for (int i = 0; i < n; i++) if (a.get (i) >= b.get (i)) return false;
  return true;
}

// Vector greater comparison.
template <class nr_type_t>
bool operator > (tvector<nr_type_t> a, tvector<nr_type_t> b) {
  assert (a.getSize () == b.getSize ());
  int n = a.getSize ();
  for (int i = 0; i < n; i++) if (a.get (i) <= b.get (i)) return false;
  return true;
}

// Scalar addition.
template <class nr_type_t>
tvector<nr_type_t> operator + (nr_type_t s, tvector<nr_type_t> a) {
  int n = a.getSize ();
  tvector<nr_type_t> res (n);
  for (int i = 0; i < n; i++) res.set (i, s + a.get (i));
  return res;
}

template <class nr_type_t>
tvector<nr_type_t> operator + (tvector<nr_type_t> a, nr_type_t s) {
  return s + a;
}

// Mean square norm.
template <class nr_type_t>
nr_double_t norm (tvector<nr_type_t> a) {
#if 0
  nr_double_t k = 0;
  for (int i = 0; i < a.getSize (); i++) k += norm (a.get (i));
  return n;
#else
  nr_double_t scale = 0, n = 1, x, ax;
  for (int i = 0; i < a.getSize (); i++) {
    if ((x = real (a (i))) != 0) {
      ax = fabs (x);
      if (scale < ax) {
	x = scale / ax;
	n = 1 + n * x * x;
	scale = ax;
      }
      else {
	x = ax / scale;
	n += x * x;
      }
    }
    if ((x = imag (a (i))) != 0) {
      ax = fabs (x);
      if (scale < ax) {
	x = scale / ax;
	n = 1 + n * x * x;
	scale = ax;
      }
      else {
	x = ax / scale;
	n += x * x;
      }
    }
  }
  return scale * scale * n;
#endif
}

// Maximum norm.
template <class nr_type_t>
nr_double_t maxnorm (tvector<nr_type_t> a) {
  nr_double_t nMax = 0, n;
  for (int i = 0; i < a.getSize (); i++) {
    n = norm (a.get (i));
    if (n > nMax) nMax = n;
  }
  return nMax;
}

// Conjugate vector.
template <class nr_type_t>
tvector<nr_type_t> conj (tvector<nr_type_t> a) {
  int n = a.getSize ();
  tvector<nr_type_t> res (n);
  for (int i = 0; i < n; i++) res.set (i, conj (a.get (i)));
  return res;
}

// Checks validity of vector.
template <class nr_type_t>
int tvector<nr_type_t>::isFinite (void) {
  for (int i = 0; i < size; i++)
    if (!finite (real (data[i]))) return 0;
  return 1;
}

// The functions reorders the vector according to the given index array.
template <class nr_type_t>
void tvector<nr_type_t>::reorder (int * idx) {
  tvector<nr_type_t> old = *this;
  for (int i = 0; i < size; i++) data[i] = old.get (idx[i]);
}

#ifdef DEBUG
// Debug function: Prints the vector object.
template <class nr_type_t>
void tvector<nr_type_t>::print (void) {
  for (int r = 0; r < size; r++) {
    fprintf (stderr, "%+.2e%+.2ei\n", (double) real (get (r)),
	     (double) imag (get (r)));
  }
}
#endif /* DEBUG */
