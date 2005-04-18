/*
 * matrix.cpp - matrix class implementation
 *
 * Copyright (C) 2003, 2004, 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: matrix.cpp,v 1.19 2005/04/18 13:41:04 raimi Exp $
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
  data = (s > 0) ? new complex[s * s] : NULL;
}

/* Constructor creates an unnamed instance of the matrix class with a
   certain number of rows and columns.  */
matrix::matrix (int r, int c)  {
  rows = r;
  cols = c;
  data = (r > 0 && c > 0) ? new complex[r * c] : NULL;
}

/* The copy constructor creates a new instance based on the given
   matrix object. */
matrix::matrix (const matrix & m) {
  rows = m.rows;
  cols = m.cols;
  data = NULL;

  // copy matrix elements
  if (rows > 0 && cols > 0) {
    data = new complex[rows * cols];
    memcpy (data, m.data, sizeof (complex) * rows * cols);
  }
}

/* The assignment copy constructor creates a new instance based on the
   given matrix object. */
const matrix& matrix::operator=(const matrix & m) {
  if (&m != this) {
    rows = m.rows;
    cols = m.cols;
    if (data) { delete[] data; data = NULL; }
    if (rows > 0 && cols > 0) {
      data = new complex[rows * cols];
      memcpy (data, m.data, sizeof (complex) * rows * cols);
    }
  }
  return *this;
}

// Destructor deletes a matrix object.
matrix::~matrix () {
  if (data) delete[] data;
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
      fprintf (stderr, "%+.2e,%+.2e ", (double) real (get (r, c)),
      	       (double) imag (get (r, c)));
    }
    fprintf (stderr, "\n");
  }
}
#endif /* DEBUG */

// Matrix addition.
matrix operator + (matrix a, matrix b) {
  assert (a.getRows () == b.getRows () && a.getCols () == b.getCols ());
  matrix res (a.getRows (), a.getCols ());
  for (int r = 1; r <= a.getRows (); r++)
    for (int c = 1; c <= a.getCols (); c++)
      res.set (r, c, a.get (r, c) + b.get (r, c));
  return res;
}

// Intrinsic matrix addition.
matrix matrix::operator += (matrix a) {
  assert (a.getRows () == rows && a.getCols () == cols);
  int r, c, i;
  for (i = 0, r = 1; r <= a.getRows (); r++, i++)
    for (c = 1; c <= a.getCols (); c++, i++)
      data[i] += a.get (r, c);
  return *this;
}

// Matrix subtraction.
matrix operator - (matrix a, matrix b) {
  assert (a.getRows () == b.getRows () && a.getCols () == b.getCols ());
  matrix res (a.getRows (), a.getCols ());
  for (int r = 1; r <= a.getRows (); r++)
    for (int c = 1; c <= a.getCols (); c++)
      res.set (r, c, a.get (r, c) - b.get (r, c));
  return res;
}

// Unary minus.
matrix matrix::operator - () {
  matrix res (getRows (), getCols ());
  int r, c, i;
  for (i = 0, r = 1; r <= getRows (); r++, i++)
    for (c = 1; c <= getCols (); c++, i++)
      res.set (r, c, -data[i]);
  return res;
}

// Intrinsic matrix subtraction.
matrix matrix::operator -= (matrix a) {
  assert (a.getRows () == rows && a.getCols () == cols);
  int r, c, i;
  for (i = 0, r = 1; r <= a.getRows (); r++, i++)
    for (c = 1; c <= a.getCols (); c++, i++)
      data[i] -= a.get (r, c);
  return *this;
}

// Matrix scaling.
matrix operator * (matrix a, complex z) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 1; r <= a.getRows (); r++)
    for (int c = 1; c <= a.getCols (); c++)
      res.set (r, c, a.get (r, c) * z);
  return res;
}

// Matrix scaling.
matrix operator * (matrix a, nr_double_t d) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 1; r <= a.getRows (); r++)
    for (int c = 1; c <= a.getCols (); c++)
      res.set (r, c, a.get (r, c) * d);
  return res;
}

// Matrix scaling in different order.
matrix operator * (nr_double_t d, matrix a) {
  return a * d;
}

// Matrix scaling.
matrix operator / (matrix a, complex z) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 1; r <= a.getRows (); r++)
    for (int c = 1; c <= a.getCols (); c++)
      res.set (r, c, a.get (r, c) / z);
  return res;
}

// Matrix scaling in different order.
matrix operator * (complex z, matrix a) {
  return a * z;
}

// Matrix multiplication.
matrix operator * (matrix a, matrix b) {
  assert (a.getCols () == b.getRows ());
  int r, c, i, n = a.getCols ();
  complex z;
  matrix res (a.getRows (), b.getCols ());
  for (r = 1; r <= a.getRows (); r++) {
    for (c = 1; c <= b.getCols (); c++) {
      for (i = 1, z = 0; i <= n; i++) z += a.get (r, i) * b.get (i, c);
      res.set (r, c, z);
    }
  }
  return res;
}

// Transpose the matrix.
matrix transpose (matrix a) {
  matrix res (a.getCols (), a.getRows ());
  for (int r = 1; r <= a.getRows (); r++)
    for (int c = 1; c <= a.getCols (); c++)
      res.set (c, r, a.get (r, c));
  return res;
}

// Conjugate complex matrix.
matrix conj (matrix a) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 1; r <= a.getRows (); r++)
    for (int c = 1; c <= a.getCols (); c++)
      res.set (r, c, conj (a.get (r, c)));
  return res;
}

// Computes magnitude of each matrix element.
matrix abs (matrix a) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 1; r <= a.getRows (); r++)
    for (int c = 1; c <= a.getCols (); c++)
      res.set (r, c, abs (a.get (r, c)));
  return res;
}

// Computes the argument of each matrix element.
matrix arg (matrix a) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 1; r <= a.getRows (); r++)
    for (int c = 1; c <= a.getCols (); c++)
      res.set (r, c, arg (a.get (r, c)));
  return res;
}

// Real part matrix.
matrix real (matrix a) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 1; r <= a.getRows (); r++)
    for (int c = 1; c <= a.getCols (); c++)
      res.set (r, c, real (a.get (r, c)));
  return res;
}

// Imaginary part matrix.
matrix imag (matrix a) {
  matrix res (a.getRows (), a.getCols ());
  for (int r = 1; r <= a.getRows (); r++)
    for (int c = 1; c <= a.getCols (); c++)
      res.set (r, c, imag (a.get (r, c)));
  return res;
}

/* The function returns the adjoint complex matrix.  This is also
   called the adjugate or transpose conjugate. */
matrix adjoint (matrix a) {
  return transpose (conj (a));
}

// Create identity matrix with specified number of rows and columns.
matrix eye (int rs, int cs) {
  matrix res (rs, cs);
  for (int r = 1; r <= res.getRows (); r++)
    for (int c = 1; c <= res.getCols (); c++)
      if (r == c) res.set (r, c, 1);
  return res;
}

// Create a square identity matrix.
matrix eye (int s) {
  return eye (s, s);
}

/* Computes the cofactor of the given determinant (in matrix
   representation) for the given row and column. */
complex cofactor (matrix a, int u, int v) {
  matrix res (a.getRows () - 1, a.getCols () - 1);
  int r, c, ra, ca;
  for (ra = r = 1; r <= res.getRows (); r++, ra++) {
    if (ra == u) ra++;
    for (ca = c = 1; c <= res.getCols (); c++, ca++) {
      if (ca == v) ca++;
      res.set (r, c, a.get (ra, ca));
    }
  }
  complex z = detLaplace (res);
  return ((u + v) & 1) ? -z : z;
}

// Compute determinant of the given matrix using Laplace expansion.
complex detLaplace (matrix a) {
  assert (a.getRows () == a.getCols ());
  int s = a.getRows ();
  complex res = 0;
  if (s > 1) {
    /* always use the first row for sub-determinant, but you should
       use the row or column with most zeros in it */
    int r = 1;
    for (int i = 1; i <= s; i++) {
      res += a.get (r, i) * cofactor (a, r, i);
    }
    return res;
  }
  else if (s == 1) {
    return a.get (1, 1);
  }
  return 1;
}

/* Compute determinant of the given matrix using the Gaussian
   algorithm.  This means to triangulate the matrix and multiply all
   the diagonal elements. */
complex detGauss (matrix a) {
  assert (a.getRows () == a.getCols ());
  nr_double_t MaxPivot;
  complex f, res;
  matrix b;
  int i, c, r, pivot, n = a.getCols ();

  // return special matrix cases
  if (n == 0) return 1;
  if (n == 1) return a.get (1, 1);

  // make copy of original matrix
  b = matrix (a);

  // triangulate the matrix
  for (res = 1, i = 1; i < n; i++) {
    // find maximum column value for pivoting
    for (MaxPivot = 0, pivot = r = i; r <= n; r++) {
      if (abs (b.get (r, i)) > MaxPivot) {
	MaxPivot = abs (b.get (r, i));
	pivot = r;
      }
    }
    // exchange rows if necessary
    assert (MaxPivot != 0);
    if (i != pivot) { b.exchangeRows (i, pivot); res = -res; }
    // compute new rows and columns
    for (r = i + 1; r <= n; r++) {
      f = b.get (r, i) / b.get (i, i);
      for (c = i + 1; c <= n; c++) {
	b.set (r, c, b.get (r, c) - f * b.get (i, c));
      }
    }
  }

  // now compute determinant by multiplying diagonal
  for (i = 1; i <= n; i++) res *= b.get (i, i);
  return res;
}

// Compute determinant of the given matrix.
complex det (matrix a) {
#if 0
  return detLaplace (a);
#else
  return detGauss (a);
#endif
}

// Compute inverse matrix of the given matrix using Laplace expansion.
matrix inverseLaplace (matrix a) {
  matrix res (a.getRows (), a.getCols ());
  complex d = detLaplace (a);
  assert (abs (d) != 0); // singular matrix
  for (int r = 1; r <= a.getRows (); r++)
    for (int c = 1; c <= a.getCols (); c++)
      res.set (r, c, cofactor (a, c, r) / d);
  return res;
}

/* Compute inverse matrix of the given matrix by Gauss-Jordan
   elimination. */
matrix inverseGaussJordan (matrix a) {
  nr_double_t MaxPivot;
  complex f;
  matrix b, e;
  int i, c, r, pivot, n = a.getCols ();

  // create temporary matrix and the result matrix
  b = matrix (a);
  e = eye (n);

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

// Compute inverse matrix of the given matrix.
matrix inverse (matrix a) {
#if 0
  return inverseLaplace (a);
#else
  return inverseGaussJordan (a);
#endif
}

/* Convert scattering parameters with the reference impedance 'zref'
   to scattering parameters with the reference impedance 'z0'. */
matrix stos (matrix s, complex zref, complex z0) {
  assert (s.getRows () == s.getCols ());
  int d = s.getRows ();
  matrix e, r, n;
  e = eye (d);
  r = e * (z0 - zref) / (z0 + zref);
  return (s - r) * inverse (e - r * s);
}

// Convert scattering parameters to impedance matrix.
matrix stoz (matrix s, complex z0) {
  assert (s.getRows () == s.getCols ());
  int d = s.getRows ();
  matrix e, zref, gref;
  e = eye (d);
  zref = e * z0;
  gref = e / (2 * sqrt (fabs (real (z0))));
  return inverse (gref) * inverse (e - s) * (s * zref + conj (zref)) * gref;
}

// Convert impedance matrix scattering parameters.
matrix ztos (matrix z, complex z0) {
  assert (z.getRows () == z.getCols ());
  int d = z.getRows ();
  matrix e, zref, gref;
  e = eye (d);
  zref = e * z0;
  gref = e / (2 * sqrt (fabs (real (z0))));
  return gref * (z - conj (zref)) * inverse (z + zref) * inverse (gref);
}

// Convert impedance matrix to admittance matrix.
matrix ztoy (matrix z) {
  assert (z.getRows () == z.getCols ());
  return inverse (z);
}

// Convert scattering parameters to admittance matrix.
matrix stoy (matrix s, complex z0) {
  assert (s.getRows () == s.getCols ());
  int d = s.getRows ();
  matrix e, zref, gref;
  e = eye (d);
  zref = e * z0;
  gref = e / (2 * sqrt (fabs (real (z0))));
  return inverse (gref) * inverse (s * zref + conj (zref)) * (e - s) * gref;
}

// Convert admittance matrix to scattering parameters.
matrix ytos (matrix y, complex z0) {
  assert (y.getRows () == y.getCols ());
  int d = y.getRows ();
  matrix e, zref, gref, s;
  e = eye (d);
  zref = e * z0;
  gref = e / (2 * sqrt (fabs (real (z0))));
  s = gref * (e - conj (zref) * y) * inverse (e + zref * y) * inverse (gref);
  return s;
}

// Converts scattering parameters to chain matrix.
matrix stoa (matrix s, complex z1) {
  assert (s.getRows () == 2 && s.getCols () == 2);
  complex z2 = z1;
  complex d = s.get (1, 1) * s.get (2, 2) - s.get (1, 2) * s.get (2, 1);
  complex n = 2.0 * s.get (2, 1) * sqrt (fabs (real (z1) * real (z2)));
  matrix a (2);
  a.set (1, 1, (conj (z1) + z1 * s.get (1, 1) - 
		conj (z1) * s.get (2, 2) - z1 * d) / n);
  a.set (1, 2, (conj (z1) * conj (z2) + z1 * conj (z2) * s.get (1, 1) +
		conj (z1) * z2 * s.get (2, 2) + z1 * z2 * d) / n);
  a.set (2, 1, (1.0 - s.get (1, 1) - s.get (2, 2) + d) / n);
  a.set (2, 2, (conj (z2) - conj (z2) * s.get (1, 1) +
		z2 * s.get (2, 2) - z2 * d) / n);
  return a;
}

// Converts chain matrix to scattering parameters.
matrix atos (matrix a, complex z1) {
  assert (a.getRows () == 2 && a.getCols () == 2);
  complex z2 = z1;
  complex d = 2.0 * sqrt (fabs (real (z1) * real (z2)));
  complex n = a.get (1, 1) * z2 + a.get (1, 2) + 
    a.get (2, 1) * z1 * z2 + a.get (2, 2) * z1;
  matrix s (2);
  s.set (1, 1, (a.get (1, 1) * z2 + a.get (1, 2) - a.get (2, 1) * 
		conj (z1) * z2 - a.get (2, 2) * conj (z1)) / n);
  s.set (1, 2, (a.get (1, 1) * a.get (2, 2) - 
		a.get (1, 2) * a.get (2, 1)) * d / n);
  s.set (2, 1, d / n);
  s.set (2, 2, (a.get (2, 2) * z1 - a.get (1, 1) * conj (z2) +
		a.get (1, 2) - a.get (2, 1) * z1 * conj (z2)) / n);
  return s;
}

// Converts scattering parameters to hybrid matrix.
matrix stoh (matrix s, complex z1) {
  assert (s.getRows () == 2 && s.getCols () == 2);
  complex z2 = z1;
  complex n = s.get (1, 2) * s.get (2, 1);
  complex d = (1.0 - s.get (1, 1)) * (1.0 + s.get (2, 2)) + n;
  matrix h (2);
  h.set (1, 1, ((1.0 + s.get (1, 1)) * (1.0 + s.get (2, 2)) - n) * z1 / d);
  h.set (1, 2, +2.0 * s.get (1, 2) / d);
  h.set (2, 1, -2.0 * s.get (2, 1) / d);
  h.set (2, 2, ((1.0 - s.get (1, 1)) * (1.0 - s.get (2, 2)) - n) / z2 / d);
  return h;
}

// Converts hybrid matrix to scattering parameters.
matrix htos (matrix h, complex z1) {
  assert (h.getRows () == 2 && h.getCols () == 2);
  complex z2 = z1;
  complex n = h.get (1, 2) * h.get (2, 1);
  complex d = (1.0 + h.get (1, 1) / z1) * (1.0 + z2 * h.get (2, 2)) - n;
  matrix s (2);
  s.set (1, 1, ((h.get (1, 1) / z1 - 1) * (1 + z2 * h.get (2, 2)) - n) / d);
  s.set (1, 2, +2.0 * h.get (1, 2) / d);
  s.set (2, 1, -2.0 * h.get (2, 1) / d);
  s.set (2, 2, ((1 + h.get (1, 1) / z1) * (1 - z2 * h.get (2, 2)) + n) / d);
  return s;
}

// Converts scattering parameters to second hybrid matrix.
matrix stog (matrix s, complex z1) {
  assert (s.getRows () == 2 && s.getCols () == 2);
  complex z2 = z1;
  complex n = s.get (1, 2) * s.get (2, 1);
  complex d = (1.0 + s.get (1, 1)) * (1.0 - s.get (2, 2)) + n;
  matrix g (2);
  g.set (1, 1, ((1.0 - s.get (1, 1)) * (1.0 - s.get (2, 2)) - n) / z1 / d);
  g.set (1, 2, -2.0 * s.get (1, 2) / d);
  g.set (2, 1, +2.0 * s.get (2, 1) / d);
  g.set (2, 2, ((1.0 + s.get (1, 1)) * (1.0 + s.get (2, 2)) - n) * z2 / d);
  return g;
}

// Converts second hybrid matrix to scattering parameters.
matrix gtos (matrix g, complex z1) {
  assert (g.getRows () == 2 && g.getCols () == 2);
  complex z2 = z1;
  complex n = g.get (1, 2) * g.get (2, 1);
  complex d = (1.0 + g.get (1, 1) * z1) * (1.0 + g.get (2, 2) / z2) - n;
  matrix s (2);
  s.set (1, 1, ((1 - g.get (1, 1) * z1) * (1 + g.get (2, 2) / z2) + n) / d);
  s.set (1, 2, -2.0 * g.get (1, 2) / d);
  s.set (2, 1, +2.0 * g.get (2, 1) / d);
  s.set (2, 2, ((g.get (1, 1) * z1 + 1) * (g.get (2, 2) / z2 - 1) - n) / d);
  return s;
}

// Convert admittance matrix to impedance matrix.
matrix ytoz (matrix y) {
  assert (y.getRows () == y.getCols ());
  return inverse (y);
}

/* Converts admittance noise correlation matrix to S-parameter noise
   correlation matrix. */
matrix cytocs (matrix cy, matrix s) {
  assert (cy.getRows () == cy.getCols () && s.getRows () == s.getCols () &&
	  cy.getRows () == s.getRows ());
  matrix e = eye (s.getRows ());
  return (e + s) * cy * adjoint (e + s) / 4;
}

/* Converts impedance noise correlation matrix to S-parameter noise
   correlation matrix. */
matrix cztocs (matrix cz, matrix s) {
  assert (cz.getRows () == cz.getCols () && s.getRows () == s.getCols () &&
	  cz.getRows () == s.getRows ());
  matrix e = eye (s.getRows ());
  return (e - s) * cz * adjoint (e - s) / 4;
}

/* Converts impedance noise correlation matrix to admittance noise
   correlation matrix.  Both matrices are assumed to be normalized. */
matrix cztocy (matrix cz, matrix y) {
  assert (cz.getRows () == cz.getCols () && y.getRows () == y.getCols () &&
	  cz.getRows () == y.getRows ());
  return y * cz * adjoint (y);
}

/* Converts S-parameter noise correlation matrix to admittance noise
   correlation matrix.  Both matrices are assumed to be normalized. */
matrix cstocy (matrix cs, matrix y) {
  assert (cs.getRows () == cs.getCols () && y.getRows () == y.getCols () &&
	  cs.getRows () == y.getRows ());
  matrix e = eye (y.getRows ());
  return (e + y) * cs * adjoint (e + y);
}

/* Converts admittance noise correlation matrix to impedance noise
   correlation matrix.  Both matrices are assumed to be normalized. */
matrix cytocz (matrix cy, matrix z) {
  assert (cy.getRows () == cy.getCols () && z.getRows () == z.getCols () &&
	  cy.getRows () == z.getRows ());
  return z * cy * adjoint (z);
}

/* Converts S-parameter noise correlation matrix to impedance noise
   correlation matrix.  Both matrices are assumed to be normalized. */
matrix cstocz (matrix cs, matrix z) {
  assert (cs.getRows () == cs.getCols () && z.getRows () == z.getCols () &&
	  cs.getRows () == z.getRows ());
  matrix e = eye (z.getRows ());
  return (e + z) * cs * adjoint (e + z);
}

// The function swaps the given rows with each other.
void matrix::exchangeRows (int r1, int r2) {
  assert (r1 >= 1 && r2 >= 1 && r1 <= rows && r2 <= rows);
  complex * s = new complex[cols];
  int len = sizeof (complex) * cols;
  r1--; r2--;
  memcpy (s, &data[r1 * cols], len);
  memcpy (&data[r1 * cols], &data[r2 * cols], len);
  memcpy (&data[r2 * cols], s, len);
  delete s;
}

// The function swaps the given columns with each other.
void matrix::exchangeCols (int c1, int c2) {
  assert (c1 >= 1 && c2 >= 1 && c1 <= cols && c2 <= cols);
  complex s;
  c1--; c2--;
  for (int r = 0; r < rows * cols; r += cols) {
    s = data[r + c1];
    data[r + c1] = data[r + c2];
    data[r + c2] = s;
  }
}

/* This function converts 2x2 matrices from any of the matrix forms Y,
   Z, H, G and A to any other.  Also converts S<->(A, T, H, Y and Z)
   matrices. */
matrix twoport (matrix m, char in, char out) {
  assert (m.getRows () >= 2 && m.getCols () >= 2);
  complex d;
  matrix res (2);

  switch (in) {
  case 'Y':
    switch (out) {
    case 'Y': // Y to Y
      res = m;
      break;
    case 'Z': // Y to Z
      d = m.get (1, 1) * m.get (2, 2) - m.get (1, 2) * m.get (2, 1);
      res.set (1, 1, m.get (2, 2) / d);
      res.set (1, 2, -m.get (1, 2) / d);
      res.set (2, 1, -m.get (2, 1) / d);
      res.set (2, 2, m.get (1, 1) / d);
      break;
    case 'H': // Y to H
      d = m.get (1, 1);
      res.set (1, 1, 1.0 / d);
      res.set (1, 2, -m.get (1, 2) / d);
      res.set (2, 1, m.get (2, 1) / d);
      res.set (2, 2, m.get (2, 2) - m.get (1, 2) * m.get (2, 1) / d);
      break;
    case 'G': // Y to G
      d = m.get (2, 2);
      res.set (1, 1, m.get (1, 1) - m.get (1, 2) * m.get (2, 1) / d);
      res.set (1, 2, m.get (1, 2) / d);
      res.set (2, 1, -m.get (2, 1) / d);
      res.set (2, 2, 1.0 / d);
      break;
    case 'A': // Y to A
      d = m.get (2, 1);
      res.set (1, 1, -m.get (2, 2) / d);
      res.set (1, 2, -1.0 / d);
      res.set (2, 1, m.get (1, 2) - m.get (2, 2) * m.get (1, 1) / d);
      res.set (2, 2, -m.get (1, 1) / d);
      break;
    case 'S': // Y to S
      res = ytos (m);
      break;
    }
    break;
  case 'Z':
    switch (out) {
    case 'Y': // Z to Y
      d = m.get (1, 1) * m.get (2, 2) - m.get (1, 2) * m.get (2, 1);
      res.set (1, 1, m.get (2, 2) / d);
      res.set (1, 2, -m.get (1, 2) / d);
      res.set (2, 1, -m.get (2, 1) / d);
      res.set (2, 2, m.get (1, 1) / d);
      break;
    case 'Z': // Z to Z
      res = m;
      break;
    case 'H': // Z to H
      d = m.get (2, 2);
      res.set (1, 1, m.get (1, 1) - m.get (1, 2) * m.get (2, 1) / d);
      res.set (1, 2, m.get (1, 2) / d);
      res.set (2, 1, -m.get (2, 1) / d);
      res.set (2, 2, 1.0 / d);
      break;
    case 'G': // Z to G
      d = m.get (1, 1);
      res.set (1, 1, 1.0 / d);
      res.set (1, 2, -m.get (1, 2) / d);
      res.set (2, 1, m.get (2, 1) / d);
      res.set (2, 2, m.get (2, 2) - m.get (1, 2) * m.get (2, 1) / d);
      break;
    case 'A': // Z to A
      d = m.get (2, 1);
      res.set (1, 1, m.get (1, 1) / d);
      res.set (1, 2, m.get (1, 1) * m.get (2, 2) / d - m.get (1, 2));
      res.set (2, 1, 1.0 / d);
      res.set (2, 2, m.get (2, 2) / d);
      break;
    case 'S': // Z to S
      res = ztos (m);
      break;
    }
    break;
  case 'H':
    switch (out) {
    case 'Y': // H to Y
      d = m.get (1, 1);
      res.set (1, 1, 1.0 / d);
      res.set (1, 2, -m.get (1, 2) / d);
      res.set (2, 1, m.get (2, 1) / d);
      res.set (2, 2, m.get (2, 2) - m.get (1, 2) * m.get(2, 1) / d);
      break;
    case 'Z': // H to Z
      d = m.get (2, 2);
      res.set (1, 1, m.get (1, 1) - m.get (1, 2) * m.get (2, 1) / d);
      res.set (1, 2, m.get (1, 2) / d);
      res.set (2, 1, -m.get (2, 1) / d);
      res.set (2, 2, 1.0 / d);
      break;
    case 'H': // H to H
      res = m;
      break;
    case 'G': // H to G
      d = m.get (1, 1) * m.get (2, 2) - m.get (1, 2) * m.get (2, 1);
      res.set (1, 1, m.get (2, 2) / d);
      res.set (1, 2, -m.get (1, 2) / d);
      res.set (2, 1, -m.get (2, 1) / d);
      res.set (2, 2, m.get (1, 1) / d);
      break;
    case 'A': // H to A
      d = m.get (2, 1);
      res.set (1, 1, m.get (1, 2) - m.get (1, 1) * m.get (2, 2) / d);
      res.set (1, 2, -m.get (1, 1) / d);
      res.set (2, 1, -m.get (2, 2) / d);
      res.set (2, 2, -1.0 / d);
      break;
    case 'S': // H to S
      res = htos (m);
      break;
    }
    break;
  case 'G':
    switch (out) {
    case 'Y': // G to Y
      d = m.get (2, 2);
      res.set (1, 1, m.get (1, 1) - m.get (1, 2) * m.get (2, 1) / d);
      res.set (1, 2, m.get (1, 2) / d);
      res.set (2, 1, -m.get (2, 1) / d);
      res.set (2, 2, 1.0 / d);
      break;
    case 'Z': // G to Z
      d = m.get (1, 1);
      res.set (1, 1, 1.0 / d);
      res.set (1, 2, -m.get (1, 2) / d);
      res.set (2, 1, m.get (2, 1) / d);
      res.set (2, 2, m.get (2, 2) - m.get (1, 2) * m.get (2, 1) / d);
      break;
    case 'H': // G to H
      d = m.get (1, 1) * m.get (2, 2) - m.get (1, 2) * m.get (2, 1);
      res.set (1, 1, m.get (2, 2) / d);
      res.set (1, 2, -m.get (1, 2) / d);
      res.set (2, 1, -m.get (2, 1) / d);
      res.set (2, 2, m.get (1, 1) / d);
      break;
    case 'G': // G to G
      res = m;
      break;
    case 'A': // G to A
      d = m.get (2, 1);
      res.set (1, 1, 1.0 / d);
      res.set (1, 2, m.get (2, 2) / d);
      res.set (2, 1, m.get (1, 1) / d);
      res.set (2, 2, m.get (1, 1) * m.get (2, 2) / d - m.get (1, 2));
      break;
    case 'S': // G to S
      res = gtos (m);
      break;
    }
    break;
  case 'A':
    switch (out) {
    case 'Y': // A to Y
      d = m.get (1, 2);
      res.set (1, 1, m.get (2, 2) / d);
      res.set (1, 2, m.get (2, 1) - m.get (1, 1) * m.get (2, 2) / d);
      res.set (2, 1, -1.0 / d);
      res.set (2, 2, m.get (1, 1) / d);
      break;
    case 'Z': // A to Z
      d = m.get (2, 1);
      res.set (1, 1, m.get (1, 1) / d);
      res.set (1, 2, m.get (1, 1) * m.get (2, 2) / d - m.get (1, 2));
      res.set (2, 1, 1.0 / d);
      res.set (2, 2, m.get (2, 2) / d);
      break;
    case 'H': // A to H
      d = m.get (2, 2);
      res.set (1, 1, m.get (1, 2) / d);
      res.set (1, 2, m.get (1, 1) - m.get (1, 2) * m.get (2, 1) / d);
      res.set (2, 1, -1.0 / d);
      res.set (2, 2, m.get (2, 1) / d);
      break;
    case 'G': // A to G
      d = m.get (1, 1);
      res.set (1, 1, m.get (2, 1) / d);
      res.set (1, 2, m.get (2, 1) * m.get (1, 2) / d - m.get (2, 2));
      res.set (2, 1, 1.0 / d);
      res.set (2, 2, m.get (1, 2) / d);
      break;
    case 'A': // A to A
      res = m;
      break;
    case 'S': // A to S
      res = atos (m);
      break;
    }
    break;
  case 'S':
    switch (out) {
    case 'S': // S to S
      res = m;
      break;
    case 'T': // S to T
      d = m.get (2, 1);
      res.set (1, 1, m.get (1, 2) - m.get (1, 1) * m.get (2, 2) / d);
      res.set (1, 2, m.get (1, 1) / d);
      res.set (2, 1, -m.get (2, 2) / d);
      res.set (1, 2, 1.0 / d);
      break;
    case 'A': // S to A
      res = stoa (m);
      break;
    case 'H': // S to H
      res = stoh (m);
      break;
    case 'G': // S to G
      res = stog (m);
      break;
    case 'Y': // S to Y
      res = stoy (m);
      break;
    case 'Z': // S to Z
      res = stoz (m);
      break;
    }
    break;
  case 'T':
    switch (out) {
    case 'S': // T to S
      d = m.get (2, 2);
      res.set (1, 1, m.get (1, 2) / d); 
      res.set (1, 2, m.get (1, 1) - m.get (1, 2) * m.get (2, 1) / d);
      res.set (2, 1, 1.0 / d);
      res.set (1, 2, -m.get (2, 1) / d);
      break;
    case 'T': // T to T
      res = m;
      break;
    }
    break;
  }
  return res;
}

/* The function returns the Rollet stability factor of the given
   S-parameter matrix. */
nr_double_t rollet (matrix m) {
  assert (m.getRows () >= 2 && m.getCols () >= 2);
  nr_double_t res;
  res = (1 - norm (m.get (1, 1)) - norm (m.get (2, 2)) + norm (det (m))) /
    2 / abs (m.get (1, 2) * m.get (2, 1));
  return res;
}
