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
 * $Id: matvec.cpp,v 1.1 2004-07-03 10:56:40 ela Exp $
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

// Constructor creates an unnamed instance of the matvec class.
matvec::matvec () {
  size = 0;
  rows = cols = 0;
  data = NULL;
}

/* Constructor creates an unnamed instance of the matvec class with a
   certain number of empty matrices. */
matvec::matvec (int length, int r, int c) {
  size = length;
  data = (length > 0) ? new matrix[length] (r, c) : NULL;
}

/* The copy constructor creates a new instance based on the given
   matvec object. */
matvec::matvec (const matvec & m) {
  size = m.size;
  rows = m.rows;
  cols = m.cols;
  data = NULL;

  // copy matvec elements
  if (size > 0) {
    data = new matrix[size] (rows, cols);
    for (int i = 0; i < size; i++) data[i] = m.data[i];
  }
}

// Destructor deletes a matvec object.
matvec::~matvec () {
  if (data) delete[] data;
}

/* This function saves the given vector to the matvec object with the
   appropriate matrix indices. */
void matvec::set (vector * v, int r, int c) {
  assert (v->getSize () == size && 
	  r >= 1 && r <= rows && c >= 1 && c <= cols);
  for (int i = 0; i < size; i++) data[i].set (r, c, v->get (i));
}

/* The function returns the vector specified by the given matrix
   indices. */
vector * matvec::get (int r, int c) {
  assert (r >= 1 && r <= rows && c >= 1 && c <= cols);
  vector * res = new vector ();
  for (int i = 0; i < size; i++) res->add (data[i].get (r, c));
  return res;
}

/* This function saves the given matrix in the matrix vector at the
   specified position. */
void matvec::set (matrix * m, int idx) {
  assert (m->getRows () == rows && m->getCols () == cols &&
	  idx > 0 && idx < size);
  data[idx] = *m;
}

/* The function returns the matrix stored within the matrix vector at
   the given position. */
matrix * matvec::get (int idx) {
  assert (idx > 0 && idx < size);
  matrix * res = new matrix (data[idx]);
  return res;
}
