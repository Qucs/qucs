/*
 * matvec.cpp - matrix vector class implementation
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
 * $Id: matvec.cpp,v 1.16 2005/05/03 17:57:36 raimi Exp $
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
#include "object.h"
#include "complex.h"
#include "vector.h"
#include "matrix.h"
#include "matvec.h"

#if !HAVE_STRCHR
# define strchr  index
# define strrchr rindex
#endif

// Constructor creates an unnamed instance of the matvec class.
matvec::matvec () {
  size = 0;
  rows = cols = 0;
  name = NULL;
  data = NULL;
}

/* Constructor creates an unnamed instance of the matvec class with a
   certain number of empty matrices. */
matvec::matvec (int length, int r, int c) {
  size = length;
  rows = r;
  cols = c;
  name = NULL;
  if (size > 0) {
    data = new matrix[size];
    for (int i = 0; i < size; i++) data[i] = matrix (r, c);
  } else {
    data = NULL;
  }
}

/* The copy constructor creates a new instance based on the given
   matvec object. */
matvec::matvec (const matvec & m) {
  size = m.size;
  rows = m.rows;
  cols = m.cols;
  name = m.name ? strdup (m.name) : NULL;
  data = NULL;

  // copy matvec elements
  if (size > 0) {
    data = new matrix[size];
    for (int i = 0; i < size; i++) data[i] = m.data[i];
  }
}

// Destructor deletes a matvec object.
matvec::~matvec () {
  if (name) free (name);
  if (data) delete[] data;
}

// Sets the name of the matvec object.
void matvec::setName (char * n) {
  if (name) free (name);
  name = n ? strdup (n) : NULL;
}

// Returns the name of the matvec object.
char * matvec::getName (void) {
  return name;
}

/* This function saves the given vector to the matvec object with the
   appropriate matrix indices. */
void matvec::set (vector v, int r, int c) {
  assert (v.getSize () == size && 
	  r >= 0 && r < rows && c >= 0 && c < cols);
  for (int i = 0; i < size; i++) data[i].set (r, c, v.get (i));
}

/* The function returns the vector specified by the given matrix
   indices.  If the matrix vector has a valid name 'A' the returned
   vector gets the name 'A[r,c]'. */
vector matvec::get (int r, int c) {
  assert (r >= 0 && r < rows && c >= 0 && c < cols);
  vector res;
  for (int i = 0; i < size; i++) res.add (data[i].get (r, c));
  if (name != NULL) {
    res.setName (createMatrixString (name, r, c));
  }
  return res;
}

/* This function returns a static text representation with the
   'n[r,c]' scheme indicating a matrix (vector) entry. */
char * matvec::createMatrixString (char * n, int r, int c) {
  static char str[256]; // hopefully enough
  sprintf (str, "%s[%d,%d]", n, r + 1, c + 1);
  return str;
}

/* This function also returns a static text representation with the
   'n[r,c]' scheme indicating a matrix (vector) entry but with
   different arguments. */
char * matvec::createMatrixString (char n, int r, int c) {
  static char str[256]; // hopefully enough
  sprintf (str, "%c[%d,%d]", n, r + 1, c + 1);
  return str;
}

/* The function investigates the given vectors name.  If this name
   matches the 'n[r,c]' pattern it returns the name 'n' and saves the
   row and column indices as well.  The caller is responsible to
   'free()' the returned string.  If the vectors name does not match
   the pattern the function returns NULL. */
char * matvec::isMatrixVector (char * n, int& r, int& c) {
  char * p; int len;
  if (n == NULL) return NULL;              // nothing todo here
  if ((p = strchr (n, '[')) != NULL) {     // find first '['
    r = atoi (p + 1) - 1;                  // get first index
    if ((p = strchr (p, ',')) != NULL) {   // find the ','
      c = atoi (p + 1) - 1;                // get second index
      if ((p = strchr (p, ']')) != NULL) { // find trailing ']'
	if (p[1] == '\0') {                // identifier must end in ']'
	  // parse actual identifier
	  if ((len = strchr (n, '[') - n) > 0) {
	    p = (char *) malloc (len + 1);
	    memcpy (p, n, len);
	    p[len] = '\0';
	    return p;
	  }
	}
      }
    }
  }
  return NULL;
}

/* This function saves the given matrix in the matrix vector at the
   specified position. */
void matvec::set (matrix m, int idx) {
  assert (m.getRows () == rows && m.getCols () == cols &&
	  idx >= 0 && idx < size);
  data[idx] = m;
}

/* The function returns the matrix stored within the matrix vector at
   the given position. */
matrix matvec::get (int idx) {
  assert (idx >= 0 && idx < size);
  matrix res (data[idx]);
  return res;
}

// Matrix vector addition.
matvec operator + (matvec a, matvec b) {
  assert (a.getRows () == b.getRows () && a.getCols () == b.getCols () &&
	  a.getSize () == b.getSize ());
  matvec res (a.getSize (), a.getRows (), a.getCols ());
  for (int i = 0; i < a.getSize (); i++) res.set (a.get (i) + b.get (i), i);
  return res;
}

// Matrix vector addition with single matrix.
matvec operator + (matvec a, matrix b) {
  assert (a.getRows () == b.getRows () && a.getCols () == b.getCols ());
  matvec res (a.getSize (), a.getRows (), a.getCols ());
  for (int i = 0; i < a.getSize (); i++) res.set (a.get (i) + b, i);
  return res;
}

// Matrix vector addition with single matrix in different order.
matvec operator + (matrix a, matvec b) {
  return b + a;
}

// Intrinsic matrix vector addition.
matvec matvec::operator += (matvec a) {
  assert (a.getRows () == rows && a.getCols () == cols &&
	  a.getSize () == size);
  for (int i = 0; i < size; i++) data[i] += a.get (i);
  return *this;
}

// Matrix vector subtraction.
matvec operator - (matvec a, matvec b) {
  assert (a.getRows () == b.getRows () && a.getCols () == b.getCols () &&
	  a.getSize () == b.getSize ());
  matvec res (a.getSize (), a.getRows (), a.getCols ());
  for (int i = 0; i < a.getSize (); i++) res.set (a.get (i) - b.get (i), i);
  return res;
}

// Matrix vector subtraction with single matrix.
matvec operator - (matvec a, matrix b) {
  assert (a.getRows () == b.getRows () && a.getCols () == b.getCols ());
  matvec res (a.getSize (), a.getRows (), a.getCols ());
  for (int i = 0; i < a.getSize (); i++) res.set (a.get (i) - b, i);
  return res;
}

// Matrix vector subtraction with single matrix in different order.
matvec operator - (matrix a, matvec b) {
  return -b + a;
}

// Unary minus.
matvec matvec::operator - () {
  matvec res (getSize (), getRows (), getCols ());
  for (int i = 0; i < getSize (); i++) res.set (-data[i], i);
  return res;
}

// Intrinsic matrix vector subtraction.
matvec matvec::operator -= (matvec a) {
  assert (a.getRows () == rows && a.getCols () == cols &&
	  a.getSize () == size);
  for (int i = 0; i < a.getSize (); i++) data[i] -= a.get (i);
  return *this;
}

// Matrix vector scaling.
matvec operator * (matvec a, complex z) {
  matvec res (a.getSize (), a.getRows (), a.getCols ());
  for (int i = 0; i < a.getSize (); i++) res.set (a.get (i) * z, i);
  return res;
}

// Matrix vector scaling in different order.
matvec operator * (complex z, matvec a) {
  return a * z;
}

// Scalar matrix vector scaling.
matvec operator * (matvec a, nr_double_t d) {
  matvec res (a.getSize (), a.getRows (), a.getCols ());
  for (int i = 0; i < a.getSize (); i++) res.set (a.get (i) * d, i);
  return res;
}

// Scalar matrix vector scaling in different order.
matvec operator * (nr_double_t d, matvec a) {
  return a * d;
}

// Matrix vector scaling by a second vector.
matvec operator * (matvec a, vector b) {
  assert (a.getSize () == b.getSize ());
  matvec res (a.getSize (), a.getRows (), a.getCols ());
  for (int i = 0; i < a.getSize (); i++) res.set (a.get (i) * b.get (i), i);
  return res;
}

// Matrix vector scaling by a second vector in different order.
matvec operator * (vector a, matvec b) {
  return b * a;
}

// Matrix vector scaling.
matvec operator / (matvec a, complex z) {
  matvec res (a.getSize (), a.getRows (), a.getCols ());
  for (int i = 0; i < a.getSize (); i++) res.set (a.get (i) / z, i);
  return res;
}

// Scalar matrix vector scaling.
matvec operator / (matvec a, nr_double_t d) {
  matvec res (a.getSize (), a.getRows (), a.getCols ());
  for (int i = 0; i < a.getSize (); i++) res.set (a.get (i) / d, i);
  return res;
}

// Matrix vector scaling by a second vector.
matvec operator / (matvec a, vector b) {
  assert (a.getSize () == b.getSize ());
  matvec res (a.getSize (), a.getRows (), a.getCols ());
  for (int i = 0; i < a.getSize (); i++) res.set (a.get (i) / b.get (i), i);
  return res;
}

// Matrix vector multiplication.
matvec operator * (matvec a, matvec b) {
  assert (a.getCols () == b.getRows () && a.getSize () == b.getSize ());
  matvec res (a.getSize (), a.getRows (), b.getCols ());
  for (int i = 0; i < a.getSize (); i++) res.set (a.get (i) * b.get (i), i);
  return res;
}

// Matrix vector multiplication with a single matrix.
matvec operator * (matvec a, matrix b) {
  assert (a.getCols () == b.getRows ());
  matvec res (a.getSize (), a.getRows (), b.getCols ());
  for (int i = 0; i < a.getSize (); i++) res.set (a.get (i) * b, i);
  return res;
}

// Matrix vector multiplication with a single matrix in different order.
matvec operator * (matrix a, matvec b) {
  return b * a;
}

// Compute determinants of the given matrix vector.
vector det (matvec a) {
  vector res (a.getSize ());
  for (int i = 0; i < a.getSize (); i++) res.set (det (a.get (i)), i);
  return res;
}

// Compute inverse matrices of the given matrix vector.
matvec inverse (matvec a) {
  matvec res (a.getSize (), a.getRows (), a.getCols ());
  for (int i = 0; i < a.getSize (); i++) res.set (inverse (a.get (i)), i);
  return res;
}

// Conjugate complex matrix vector.
matvec conj (matvec a) {
  matvec res (a.getSize (), a.getRows (), a.getCols ());
  for (int i = 0; i < a.getSize (); i++) res.set (conj (a.get (i)), i);
  return res;
}

// Computes magnitude of each matrix vector element.
matvec abs (matvec a) {
  matvec res (a.getSize (), a.getRows (), a.getCols ());
  for (int i = 0; i < a.getSize (); i++) res.set (abs (a.get (i)), i);
  return res;
}

// Computes the argument of each matrix vector element.
matvec arg (matvec a) {
  matvec res (a.getSize (), a.getRows (), a.getCols ());
  for (int i = 0; i < a.getSize (); i++) res.set (arg (a.get (i)), i);
  return res;
}

// Real part matrix vector.
matvec real (matvec a) {
  matvec res (a.getSize (), a.getRows (), a.getCols ());
  for (int i = 0; i < a.getSize (); i++) res.set (real (a.get (i)), i);
  return res;
}

// Real part matrix vector.
matvec imag (matvec a) {
  matvec res (a.getSize (), a.getRows (), a.getCols ());
  for (int i = 0; i < a.getSize (); i++) res.set (imag (a.get (i)), i);
  return res;
}

/* The function returns the adjoint complex matrix vector.  This is
   also called the adjugate or transpose conjugate. */
matvec adjoint (matvec a) {
  matvec res (a.getSize (), a.getRows (), a.getCols ());
  for (int i = 0; i < a.getSize (); i++) res.set (adjoint (a.get (i)), i);
  return res;
}

// Transpose the matrix vector.
matvec transpose (matvec a) {
  matvec res (a.getSize (), a.getCols (), a.getRows ());
  for (int i = 0; i < a.getSize (); i++) res.set (transpose (a.get (i)), i);
  return res;
}

// Convert scattering parameters to admittance matrix vector.
matvec stoy (matvec s, complex z0) {
  assert (s.getCols () == s.getRows ());
  matvec res (s.getSize (), s.getCols (), s.getRows ());
  for (int i = 0; i < s.getSize (); i++) res.set (stoy (s.get (i), z0), i);
  return res;  
}

// Convert admittance matrix to scattering parameter matrix vector.
matvec ytos (matvec y, complex z0) {
  assert (y.getCols () == y.getRows ());
  matvec res (y.getSize (), y.getCols (), y.getRows ());
  for (int i = 0; i < y.getSize (); i++) res.set (ytos (y.get (i), z0), i);
  return res;  
}

// Convert scattering parameters to impedance matrix vector.
matvec stoz (matvec s, complex z0) {
  assert (s.getCols () == s.getRows ());
  matvec res (s.getSize (), s.getCols (), s.getRows ());
  for (int i = 0; i < s.getSize (); i++) res.set (stoz (s.get (i), z0), i);
  return res;  
}

// Convert impedance matrix vector scattering parameter matrix vector.
matvec ztos (matvec z, complex z0) {
  assert (z.getCols () == z.getRows ());
  matvec res (z.getSize (), z.getCols (), z.getRows ());
  for (int i = 0; i < z.getSize (); i++) res.set (ztos (z.get (i), z0), i);
  return res;  
}

// Convert impedance matrix vector to admittance matrix vector.
matvec ztoy (matvec z) {
  assert (z.getCols () == z.getRows ());
  matvec res (z.getSize (), z.getCols (), z.getRows ());
  for (int i = 0; i < z.getSize (); i++) res.set (ztoy (z.get (i)), i);
  return res;  
}

// Convert admittance matrix vector to impedance matrix vector.
matvec ytoz (matvec y) {
  assert (y.getCols () == y.getRows ());
  matvec res (y.getSize (), y.getCols (), y.getRows ());
  for (int i = 0; i < y.getSize (); i++) res.set (ytoz (y.get (i)), i);
  return res;  
}

/* This function converts 2x2 matrix vectors from any of the matrix
   forms Y, Z, H, G and A to any other.  Also converts S<->(A, T, H, Y
   and Z) matrix vectors. */
matvec twoport (matvec m, char in, char out) {
  assert (m.getCols () >= 2 && m.getRows () >= 2);
  matvec res (m.getSize (), 2, 2);
  for (int i = 0; i < m.getSize (); i++)
    res.set (twoport (m.get (i), in, out), i);
  return res;  
}

/* The function returns the Rollet stability factor vector of the
   given S-parameter matrix vector. */
vector rollet (matvec m) {
  assert (m.getCols () >= 2 && m.getRows () >= 2);
  vector res (m.getSize ());
  for (int i = 0; i < m.getSize (); i++) res.set (rollet (m.get (i)), i);
  return res;
}
