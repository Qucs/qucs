/*
 * sweep.cpp - variable sweep class implementation
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
 * $Id: sweep.cpp,v 1.2 2004-06-30 18:08:04 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "object.h"
#include "sweep.h"

// Constructor creates an unnamed instance of the sweep class.
sweep::sweep () : object () {
  type = SWEEP_UNKNOWN;
  data = NULL;
  size = 0;
  txt = NULL;
  counter = 0;
}

// Constructor creates a named instance of the sweep class.
sweep::sweep (char * n) : object (n) {
  type = SWEEP_UNKNOWN;
  data = NULL;
  size = 0;
  txt = NULL;
  counter = 0;
}

// Destructor deletes the sweep class object.
sweep::~sweep () {
  if (data) free (data);
  if (txt) free (txt);
}

/* The copy constructor creates a new instance of the sweep class
   based on the given sweep object. */
sweep::sweep (sweep & s) : object (s) {
  type = s.type;
  size = s.size;
  counter = s.counter;
  data = (nr_double_t *) malloc (sizeof (nr_double_t) * size);
  if (s.data)
    memcpy (data, s.data, sizeof (nr_double_t) * size);
  else
    memset (data, 0, sizeof (nr_double_t) * size);
}

// The function returns the value at the given position.
nr_double_t sweep::get (int idx) {
  assert (idx >= 0 && idx < size && data != NULL);
  return data[idx];
}

// The function sets the given value at the given position.
void sweep::set (int idx, nr_double_t val) {
  assert (idx >= 0 && idx < size && data != NULL);
  data[idx] = val;
}

/* This function modifies the current size of the sweep.  If the the
   new number of points is larger than the current one it zeroes the
   new elements. */
void sweep::setSize (int points) {
  assert (points > 0);
  if (data != NULL) {
    data = (nr_double_t *) realloc (data, sizeof (nr_double_t) * points);
    if (points > size)
      memset (&data[size], 0, sizeof (nr_double_t) * (points - size));
  }
  else {
    data = (nr_double_t *) malloc (sizeof (nr_double_t) * points);
    memset (data, 0, sizeof (nr_double_t) * points);
  }
  size = points;
  counter = 0;
}

// The function creates a string representation of the sweep definition.
char * sweep::toString (void) {
  if (txt) free (txt);
  if (data == NULL || size == 0) return "";
  int len = 3 + size - 1;
  txt = (char *) malloc (len);
  strcpy (txt, "[");
  for (int i = 0; i < size; i++) {
    static char str[256];
    sprintf (str, "%g", get (i));
    txt = (char *) realloc (txt, len += strlen (str));
    strcat (txt, str);
    if (i != size - 1) strcat (txt, ";");
  }
  strcat (txt, "]");
  return txt;
}

/* The following function reverses the values order inside the sweep
   definition. */
void sweep::reverse (void) {
  if (data != NULL && size > 0) {
    nr_double_t * buf = (nr_double_t *) malloc (sizeof (nr_double_t) * size);
    for (int i = 0; i < size; i++) buf[i] = data[size - 1 - i];
    free (data);
    data = buf;
  }
}

/* The function returns the current sweep value and afterwards steps
   one value forward.  It wraps around at the end of sweep. */
nr_double_t sweep::next (void) {
  nr_double_t res = data[counter];
  if (++counter >= size) counter = 0;
  return res;
}

/* This function returns the sweep value before the current value and
   thereby steps one value back.  It wraps around at the beginning of
   the sweep. */
nr_double_t sweep::prev (void) {
  if (--counter < 0) counter = size - 1;
  return data[counter];
}

// Constructor creates an unnamed instance of the linsweep class.
linsweep::linsweep () : sweep () {
  type = SWEEP_LINEAR;
}

// Constructor creates a named instance of the linsweep class.
linsweep::linsweep (char * n) : sweep (n) {
  type = SWEEP_LINEAR;
}

/* This function creates a linear stepped vector of values starting at
   the given start value, ending with the given stop value and
   containing points elements. */
void linsweep::create (nr_double_t start, nr_double_t stop, int points) {
  nr_double_t step = (stop - start) / (points - 1);
  setSize (points);
  for (int i = 0; i < points; i++) set (i, start + (i * step));
}

// Destructor deletes the linsweep class object.
linsweep::~linsweep () {
}

// Constructor creates an unnamed instance of the logsweep class.
logsweep::logsweep () : sweep () {
  type = SWEEP_LOGARITHMIC;
}

// Constructor creates a named instance of the logsweep class.
logsweep::logsweep (char * n) : sweep (n) {
  type = SWEEP_LOGARITHMIC;
}

/* This function creates a logarithmic stepped vector of values
   starting at the given start value, ending with the given stop value
   and containing points elements. */
void logsweep::create (nr_double_t start, nr_double_t stop, int points) {
  assert (start * stop > 0);
  nr_double_t step, first, last, d;

  // ensure the last value being larger than the first
  if (fabs (start) > fabs (stop)) {
    first = fabs (stop);
    last = fabs (start);
  }
  else {
    first = fabs (start);
    last = fabs (stop);
  }
  // check direction and sign of sweep
  d = fabs (start) > fabs (stop) ? -1 : 1;
  // compute logarithmic step size
  step = (log (last) - log (first)) / (points - 1);
  setSize (points);
  for (int i = 0, j = points - 1; i < points; i++, j--) {
    if (d > 0)
      set (i, start * exp (step * i));
    else
      set (j, stop * exp (step * i));
  }
}

// Destructor deletes the logsweep class object.
logsweep::~logsweep () {
}

// Constructor creates an unnamed instance of the consweep class.
consweep::consweep () : sweep () {
  type = SWEEP_CONSTANT;
}

// Constructor creates a named instance of the consweep class.
consweep::consweep (char * n) : sweep (n) {
  type = SWEEP_CONSTANT;
}

/* This function creates a constant value in a sweep containing one
   element only. */
void consweep::create (nr_double_t val) {
  setSize (1);
  set (0, val);
}

// Destructor deletes the consweep class object.
consweep::~consweep () {
}

// Constructor creates an unnamed instance of the lstsweep class.
lstsweep::lstsweep () : sweep () {
  type = SWEEP_LIST;
}

// Constructor creates a named instance of the lstsweep class.
lstsweep::lstsweep (char * n) : sweep (n) {
  type = SWEEP_LIST;
}

/* This function creates arbitrary values in a sweep containing points
   elements.  The actual values must be assigned using the set()
   function. */
void lstsweep::create (int points) {
  setSize (points);
}

// Destructor deletes the lstsweep class object.
lstsweep::~lstsweep () {
}
