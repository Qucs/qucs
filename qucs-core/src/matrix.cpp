/*
 * matrix.cpp - matrix class implementation
 *
 * Copyright (C) 2003 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: matrix.cpp,v 1.2 2003/12/26 14:04:07 ela Exp $
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
#include "matrix.h"

// Constructor creates an unnamed instance of the matrix class.
matrix::matrix () {
  rows = 0;
  cols = 0;
  data = NULL;
}

/* Constructor creates an unnamed instance of the matrix class with a
   certain number of rows and columns.  Creates a square matrix.  */
matrix::matrix (int s)  {
  rows = cols = s;
  data = new complex[s * s];
}

/* Constructor creates an unnamed instance of the matrix class with a
   certain number of rows and columns.  */
matrix::matrix (int r, int c)  {
  rows = r;
  cols = c;
  data = new complex[r * c];
}

/* The copy constructor creates a new instance based on the given
   matrix object. */
matrix::matrix (const matrix & m) {
  rows = m.rows;
  cols = m.cols;
  data = new complex[rows * cols];

  // copy each matrix element
  for (int i = 0; i < rows * cols; i++) {
    complex * z = new complex (m.data[i]);
    data[i] = * z;
    delete z;
  }
}

// Destructor deletes a matrix object.
matrix::~matrix () {
  delete[] data;
}

// Returns the matrix element at the given row and column.
complex matrix::get (int r, int c) {
  return data[(r - 1) * cols + c - 1];
}

// Sets the matrix element at the given row and column.
void matrix::set (int r, int c, complex z) {
  data[(r - 1) * cols + c - 1] = z;
}

#ifdef DEBUG
// Debug function: Prints the matrix object.
void matrix::print (void) {
  for (int r = 1; r <= rows; r++) {
    for (int c = 1; c <= cols; c++) {
      fprintf (stderr, "%.2e,%.2e ", (double) real (get (r, c)), 
	       (double) imag (get (r, c)));
    }
    fprintf (stderr, "\n");
  }
}
#endif /* DEBUG */

// Matrix addition.
matrix& operator + (matrix& a, matrix& b) {
  assert (a.getRows () == b.getRows () && a.getCols () == b.getCols ());
  matrix * res = new matrix (a.getRows (), a.getCols ());
  for (int r = 1; r <= a.getRows (); r++)
    for (int c = 1; c <= a.getCols (); c++)
      res->set (r, c, a.get (r, c) + b.get (r, c));
  return *res;
}

// Intrinsic matrix addition.
matrix& matrix::operator += (matrix& a) {
  assert (a.getRows () == rows && a.getCols () == cols);
  int r, c, i;
  for (i = 0, r = 1; r <= a.getRows (); r++, i++)
    for (c = 1; c <= a.getCols (); c++, i++)
      data[i] += a.get (r, c);
  return *this;
}

// Matrix subtraction.
matrix& operator - (matrix& a, matrix& b) {
  assert (a.getRows () == b.getRows () && a.getCols () == b.getCols ());
  matrix * res = new matrix (a.getRows (), a.getCols ());
  for (int r = 1; r <= a.getRows (); r++)
    for (int c = 1; c <= a.getCols (); c++)
      res->set (r, c, a.get (r, c) - b.get (r, c));
  return *res;
}

// Intrinsic matrix subtraction.
matrix& matrix::operator -= (matrix& a) {
  assert (a.getRows () == rows && a.getCols () == cols);
  int r, c, i;
  for (i = 0, r = 1; r <= a.getRows (); r++, i++)
    for (c = 1; c <= a.getCols (); c++, i++)
      data[i] -= a.get (r, c);
  return *this;
}

// Matrix scaling.
matrix& operator * (matrix& a, complex z) {
  matrix * res = new matrix (a.getRows (), a.getCols ());
  for (int r = 1; r <= a.getRows (); r++)
    for (int c = 1; c <= a.getCols (); c++)
      res->set (r, c, a.get (r, c) * z);
  return *res;
}

// Matrix scaling.
matrix& operator / (matrix& a, complex z) {
  matrix * res = new matrix (a.getRows (), a.getCols ());
  for (int r = 1; r <= a.getRows (); r++)
    for (int c = 1; c <= a.getCols (); c++)
      res->set (r, c, a.get (r, c) / z);
  return *res;
}

// Matrix scaling in different order.
matrix& operator * (complex z, matrix& a) {
  return a * z;
}

// Matrix multiplication.
matrix& operator * (matrix& a, matrix& b) {
  assert (a.getCols () == b.getRows ());
  int r, c, i, n = a.getCols ();
  complex z;
  matrix * res = new matrix (a.getRows (), b.getCols ());
  for (r = 1; r <= a.getRows (); r++) {
    for (c = 1; c <= b.getCols (); c++) {
      for (i = 1, z = 0; i <= n; i++) z += a.get (r, i) * b.get (i, c);
      res->set (r, c, z);
    }
  }
  return *res;
}

// Transpose the matrix.
matrix& transpose (matrix& a) {
  matrix * res = new matrix (a.getCols (), a.getRows ());
  for (int r = 1; r <= a.getRows (); r++)
    for (int c = 1; c <= a.getCols (); c++)
      res->set (c, r, a.get (r, c));
  return *res;
}

// Conjugate complex matrix.
matrix& conj (matrix& a) {
  matrix * res = new matrix (a.getRows (), a.getCols ());
  for (int r = 1; r <= a.getRows (); r++)
    for (int c = 1; c <= a.getCols (); c++)
      res->set (r, c, conj (a.get (r, c)));
  return *res;
}

// Create identity matrix with specified number of rows and columns.
matrix& eye (int r, int c) {
  matrix * res = new matrix (r, c);
  for (int r = 1; r <= res->getRows (); r++)
    for (int c = 1; c <= res->getCols (); c++)
      if (r == c) res->set (r, c, 1);
  return *res;
}

// Create a square identity matrix.
matrix& eye (int s) {
  return eye (s, s);
}

/* Computes the adjoint of the given determinant (in matrix
   representation) for the given row and column. */
complex adjoint (matrix& a, int u, int v) {
  matrix * res = new matrix (a.getRows () - 1, a.getCols () - 1);
  int r, c, ra, ca;
  for (ra = r = 1; r <= res->getRows (); r++, ra++) {
    if (ra == u) ra++;
    for (ca = c = 1; c <= res->getCols (); c++, ca++) {
      if (ca == v) ca++;
      res->set (r, c, a.get (ra, ca));
    }
  }
  complex z = det (*res);
  delete res;
  return ((u + v) % 2) ? -z : z;
}

// Compute determinant of the given matrix.
complex det (matrix& a) {
  assert (a.getRows () == a.getCols ());
  int s = a.getRows ();
  complex res = 0;
  if (s > 1) {
    // always use the first row for sub-determinant
    int r = 1;
    for (int i = 1; i <= s; i++) {
      res += a.get (r, i) * adjoint (a, r, i);
    }
    return res;
  }
  res = a.get (1, 1);
  return res;
}

// Compute inverse matrix of the given matrix.
matrix& inverse (matrix& a) {
  matrix * res = new matrix (a.getRows (), a.getCols ());
  complex d = det (a);
  assert (abs (d) != 0); // singular matrix
  for (int r = 1; r <= a.getRows (); r++)
    for (int c = 1; c <= a.getCols (); c++)
      res->set (r, c, adjoint (a, c, r) / d);
  return *res;
}

// Convert scattering parameters to impedance matrix.
matrix& stoz (matrix& s, complex z0 = 50.0) {
  assert (s.getRows () == s.getCols ());
  int d = s.getRows ();
  matrix e, zref, gref, z, * res;
  e = eye (d);
  zref = e * z0;
  gref = e / (2 * sqrt (fabs (real (z0))));
  z = inverse (gref) * inverse (e - s) * (s * zref + conj (zref)) * gref;
  res = new matrix (z);
  return *res;
}

// Convert scattering parameters to admittance matrix.
matrix& stoy (matrix& s, complex z0 = 50.0) {
  assert (s.getRows () == s.getCols ());
  int d = s.getRows ();
  matrix e, zref, gref, y, * res;
  e = eye (d);
  zref = e * z0;
  gref = e / (2 * sqrt (fabs (real (z0))));
  y = inverse (gref) * inverse (s * zref + conj (zref)) * (e - s) * gref;
  res = new matrix (y);
  return *res;
}
