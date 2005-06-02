/*
 * spline.cpp - spline class implementation
 *
 * Copyright (C) 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: spline.cpp,v 1.2 2005/06/02 18:17:51 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "vector.h"
#include "poly.h"
#include "spline.h"

// Constructor creates an instance of the spline class.
spline::spline () {
  x = f0 = f1 = f2 = f3 = NULL;
  d0 = dn = 0;
  n = 0;
  boundary = SPLINE_BC_NATURAL;
}

// Constructor creates an instance of the spline class with data given.
spline::spline (vector * y, vector * t) {
  x = f0 = f1 = f2 = f3 = NULL;
  d0 = dn = 0;
  n = 0;
  boundary = SPLINE_BC_NATURAL;
  vectors (y, t);
  construct ();
}

#define t_ (*t)
#define y_ (*y)

// Pass interpolation datapoints.
void spline::vectors (vector * y, vector * t) {
  int i = t->getSize ();
  assert (y->getSize () == i && i >= 3);

  // create local copy of f(x)
  if (n != i - 1) {
    n = i - 1;
    if (f0) delete[] f0;
    f0 = new nr_double_t[n+1];
    if (x) delete[] x;
    x  = new nr_double_t[n+1];
  }
  for (i = 0; i <= n; i++) {
    f0[i] = real (y_(i)); x[i] = real (t_(i));
  }
}

// Construct cubic spline interpolation coefficients.
void spline::construct (void) {

  // calculate first derivative h = f'(x)
  int i;
  nr_double_t * h  = new nr_double_t[n+1];
  for (i = 0; i < n; i++) {
    h[i] = x[i+1] - x[i];
    if (h[i] == 0.0) {
      logprint (LOG_ERROR, "ERROR: Duplicate points in spline: %g, %g\n",
		x[i], x[i+1]);
    }
  }

  // setup right hand side
  nr_double_t * b = new nr_double_t[n+1]; // b as in Ax = b
  for (i = 1; i < n; i++) {
    nr_double_t _n = f0[i+1] * h[i-1] - f0[i] * (h[i] + h[i-1]) +
      f0[i-1] * h[i];
    nr_double_t _d = h[i-1] * h[i];
    b[i] = 3 * _n / _d;
  }
  if (boundary == SPLINE_BC_NATURAL) {
    // natural boundary condition
    b[0] = 0;
    b[n] = 0;
  } else if (boundary == SPLINE_BC_CLAMPED) {
    // start and end derivatives given
    b[0] = 3 * ((f0[1] - f0[0]) / h[0] - d0);
    b[n] = 3 * (dn - (f0[n] - f0[n-1]) / h[n-1]);
  } else if (boundary == SPLINE_BC_PERIODIC) {
    // TODO: non-trigdiagonal equations - periodic boundary condition
    b[0] = 0;
    b[n] = 3 * ((f0[1] - f0[0]) / h[0] - (f0[n] - f0[n-1]) / h[n-1]);
  }

  nr_double_t * u = new nr_double_t[n+1];
  nr_double_t * z = b; // reuse storage
  if (boundary == SPLINE_BC_NATURAL) {
    u[0] = 0;
    z[0] = 0;
  } else if (boundary == SPLINE_BC_CLAMPED) {
    u[0] = h[0] / (2 * h[0]);
    z[0] = b[0] / (2 * h[0]);
  }

  for (i = 1; i < n; i++) {
    nr_double_t p = 2 * (h[i] + h[i-1]) - h[i-1] * u[i-1]; // pivot
    u[i] = h[i] / p;
    z[i] = (b[i] - z[i-1] * h[i-1]) / p;
  }
  if (boundary == SPLINE_BC_NATURAL) {
    z[n] = 0;
  } else if (boundary == SPLINE_BC_CLAMPED) {
    nr_double_t p = h[n-1] * (2 - u[n-1]);
    z[n] = (b[n] - z[n-1] * h[n-1]) / p;
  }

  // back substitution
  f1 = u; // reuse storage
  f2 = z;
  f3 = h;
  f2[n] = z[n];
  f3[n] = 0;
  for (i = n - 1; i >= 0; i--) {
    f2[i] = z[i] - u[i] * f2[i+1];
    f1[i] = (f0[i+1] - f0[i]) / h[i] - h[i] * (f2[i+1] + 2 * f2[i]) / 3;
    f3[i] = (f2[i+1] - f2[i]) / (3 * h[i]);
  }
  d0 = f1[0];

  // set up last slot for extrapolation above
  if (boundary == SPLINE_BC_NATURAL) {
    f1[n] = f1[n-1] + (x[n] - x[n-1]) * f2[n-1];
    f2[n] = 0;
    f3[n] = 0;
  } else if (boundary == SPLINE_BC_CLAMPED) {
    f1[n] = dn;
    f2[n] = 0;
    f3[n] = 0;
  }
  f2[n] = 0;
  f3[n] = 0;
}

// Returns pointer to the first value greater than the given one.
nr_double_t * spline::upper_bound (nr_double_t * first, nr_double_t * last,
				   nr_double_t value) {
  int half, len = last - first;
  nr_double_t * centre;

  while (len > 0) {
    half = len >> 1;
    centre = first;
    centre += half;
    if (value < *centre)
      len = half;
    else {
      first = centre;
      ++first;
      len = len - half - 1;
    }
  }
  return first;
}

// Evaluates the spline at the given position.
poly spline::evaluate (nr_double_t t) {
  nr_double_t * here = upper_bound (x, x+n+1, t);
  nr_double_t y0, y1, y2;
  if (here == x) {
    nr_double_t dx = t - x[0];
    y0 = f0[0] + dx * d0;
    y1 = d0;
    return poly (t, y0, y1);
  }
  else {
    int i = here - x - 1;
    nr_double_t dx = t - x[i];
    // value
    y0 = f0[i] + dx * (f1[i] + dx * (f2[i] + dx * f3[i]));
    // first derivative
    y1 = f1[i] + dx * (2 * f2[i] + 3 * dx * f3[i]);
    // second derivative
    y2 = 2 * f2[i] + 6 * dx * f3[i];
    return poly (t, y0, y1, y2);
  }
}

// Destructor deletes an instance of the spline class.
spline::~spline () {
  if (x)  delete[] x;
  if (f0) delete[] f0;
  if (f1) delete[] f1;
  if (f2) delete[] f2;
  if (f3) delete[] f3;
}
