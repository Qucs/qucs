/*
 * tmatrix.cpp - simple matrix template class implementation
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
 * $Id: tmatrix.cpp,v 1.4 2004-10-12 18:13:09 ela Exp $
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

#include "logging.h"
#include "complex.h"
#include "tmatrix.h"

// Constructor creates an unnamed instance of the tmatrix class.
template <class nr_type_t>
tmatrix<nr_type_t>::tmatrix () {
  rows = 0;
  cols = 0;
  data = NULL;
}

/* Constructor creates an unnamed instance of the tmatrix class with a
   certain number of rows and columns.  Creates a square tmatrix.  */
template <class nr_type_t>
tmatrix<nr_type_t>::tmatrix (int s)  {
  rows = cols = s;
  if (s > 0) {
    data = new nr_type_t[s * s];
    memset (data, 0, sizeof (nr_type_t) * s * s);
  }
  else data = NULL;
}

/* Constructor creates an unnamed instance of the tmatrix class with a
   certain number of rows and columns.  */
template <class nr_type_t>
tmatrix<nr_type_t>::tmatrix (int r, int c)  {
  rows = r;
  cols = c;
  if (r > 0 && c > 0) {
    data = new nr_type_t[r * c];
    memset (data, 0, sizeof (nr_type_t) * r * c);
  }
  else data = NULL;
}

/* The copy constructor creates a new instance based on the given
   tmatrix object. */
template <class nr_type_t>
tmatrix<nr_type_t>::tmatrix (const tmatrix & m) {
  rows = m.rows;
  cols = m.cols;
  data = NULL;

  // copy tmatrix elements
  if (rows > 0 && cols > 0) {
    data = new nr_type_t[rows * cols];
    memcpy (data, m.data, sizeof (nr_type_t) * rows * cols);
  }
}

/* The assignment copy constructor creates a new instance based on the
   given tmatrix object. */
template <class nr_type_t>
const tmatrix<nr_type_t>&
tmatrix<nr_type_t>::operator=(const tmatrix<nr_type_t> & m) {
  if (&m != this) {
    rows = m.rows;
    cols = m.cols;
    if (data) { delete[] data; data = NULL; }
    if (rows > 0 && cols > 0) {
      data = new nr_type_t[rows * cols];
      memcpy (data, m.data, sizeof (nr_type_t) * rows * cols);
    }
  }
  return *this;
}

// Destructor deletes a tmatrix object.
template <class nr_type_t>
tmatrix<nr_type_t>::~tmatrix () {
  if (data) delete[] data;
}

// Returns the tmatrix element at the given row and column.
template <class nr_type_t>
nr_type_t tmatrix<nr_type_t>::get (int r, int c) {
  return data[(r - 1) * cols + c - 1];
}

// Sets the tmatrix element at the given row and column.
template <class nr_type_t>
void tmatrix<nr_type_t>::set (int r, int c, nr_type_t z) {
  data[(r - 1) * cols + c - 1] = z;
}

// The function swaps the given rows with each other.
template <class nr_type_t>
void tmatrix<nr_type_t>::exchangeRows (int r1, int r2) {
  assert (r1 >= 1 && r2 >= 1 && r1 <= rows && r2 <= rows);
  nr_type_t * s = new nr_type_t[cols];
  int len = sizeof (nr_type_t) * cols;
  r1--; r2--;
  memcpy (s, &data[r1 * cols], len);
  memcpy (&data[r1 * cols], &data[r2 * cols], len);
  memcpy (&data[r2 * cols], s, len);
  delete s;
}

// The function swaps the given columns with each other.
template <class nr_type_t>
void tmatrix<nr_type_t>::exchangeCols (int c1, int c2) {
  assert (c1 >= 1 && c2 >= 1 && c1 <= cols && c2 <= cols);
  nr_type_t s;
  c1--; c2--;
  for (int r = 0; r < rows * cols; r += cols) {
    s = data[r + c1];
    data[r + c1] = data[r + c2];
    data[r + c2] = s;
  }
}

// Compute inverse matrix of the given matrix by Gauss-Jordan elimination.
template <class nr_type_t>
tmatrix<nr_type_t> inverse (tmatrix<nr_type_t> a) {
  nr_double_t MaxPivot;
  nr_type_t f;
  tmatrix<nr_type_t> b;
  tmatrix<nr_type_t> e;
  int i, c, r, pivot, n = a.getCols ();

  // create temporary matrix and the result matrix
  b = tmatrix<nr_type_t> (a);
  e = tmatrix<nr_type_t>::eye (n);

  // create the eye matrix in 'b' and the result in 'e'
  for (i = 1; i <= n; i++) {
    // find maximum column value for pivoting
    for (MaxPivot = 0, pivot = r = i; r <= n; r++) {
      if (abs (b.get (r, i)) > MaxPivot) {
	MaxPivot = abs (b.get (r, i));
	pivot = r;
      }
    }
    // exchange rows if necessary
    assert (MaxPivot != 0); // singular matrix
    if (i != pivot) {
      b.exchangeRows (i, pivot);
      e.exchangeRows (i, pivot);
    }

    // compute current row
    f = b.get (i, i);
    for (c = 1; c <= n; c++) {
      b.set (i, c, b.get (i, c) / f);
      e.set (i, c, e.get (i, c) / f);
    }

    // compute new rows and columns
    for (r = 1; r <= n; r++) {
      if (r != i) {
	f = b.get (r, i);
	for (c = 1; c <= n; c++) {
	  b.set (r, c, b.get (r, c) - f * b.get (i, c));
	  e.set (r, c, e.get (r, c) - f * e.get (i, c));
	}
      }
    }
  }
  return e;
}

// Create identity matrix with specified number of rows and columns.
template <class nr_type_t>
tmatrix<nr_type_t> tmatrix<nr_type_t>::eye (int n) {
  tmatrix<nr_type_t> res (n);
  for (int r = 1; r <= n; r++) res.set (r, r, 1);
  return res;
}

// Matrix multiplication.
template <class nr_type_t>
tmatrix<nr_type_t> operator * (tmatrix<nr_type_t> a, tmatrix<nr_type_t> b) {
  assert (a.getCols () == b.getRows ());
  int r, c, i, n = a.getCols ();
  nr_type_t z;
  tmatrix<nr_type_t> res (a.getRows (), b.getCols ());
  for (r = 1; r <= a.getRows (); r++) {
    for (c = 1; c <= b.getCols (); c++) {
      for (i = 1, z = 0; i <= n; i++) z += a.get (r, i) * b.get (i, c);
      res.set (r, c, z);
    }
  }
  return res;
}

// Multiplication of matrix and vector.
template <class nr_type_t>
tvector<nr_type_t> operator * (tmatrix<nr_type_t> a, tvector<nr_type_t> b) {
  assert (a.getCols () == b.getSize ());
  int r, c, n = a.getCols ();
  nr_type_t z;
  tvector<nr_type_t> res (n);

  for (r = 1; r <= n; r++) {
    for (c = 1, z = 0; c <= n; c++) z += a.get (r, c) * b.get (c);
    res.set (r, z);
  }
  return res;
}

#ifdef DEBUG
// Debug function: Prints the matrix object.
template <class nr_type_t>
void tmatrix<nr_type_t>::print (void) {
  for (int r = 1; r <= rows; r++) {
    for (int c = 1; c <= cols; c++) {
      fprintf (stderr, "%+.2e ", (double) real (get (r, c)));
    }
    fprintf (stderr, "\n");
  }
}
#endif /* DEBUG */
