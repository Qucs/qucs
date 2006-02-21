/*
 * history.cpp - history class implementation
 *
 * Copyright (C) 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: history.cpp,v 1.1 2006-02-21 20:56:17 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "precision.h"
#include "tvector.h"
#include "history.h"

// Constructor creates an unnamed instance of the history class.
history::history () {
  age = 0;
  t = values = NULL;
}

/* The copy constructor creates a new instance based on the given
   history object. */
history::history (const history & h) {
  age = h.age;
  t = h.t;
  values = h.values ? new tvector<nr_double_t> (*h.values) : NULL;
}

// Destructor deletes a history object.
history::~history () {
  if (values) delete values;
}

// The function appends the given value to the history.
void history::append (nr_double_t val) {
  if (values == NULL) values = new tvector<nr_double_t>;
  values->add (val);
  drop ();
}

// Returns the first (oldest) time value in the history.
nr_double_t history::first (void) {
  return (t != NULL) ? t->get (t->getSize () - values->getSize ()) : 0.0;
}

// Returns the last (youngest) time value in the history.
nr_double_t history::last (void) {
  return (t != NULL) ? t->get (t->getSize () - 1) : 0.0;
}

// Returns the duration of the history.
nr_double_t history::duration (void) {
  return last () - first ();
}

/* This function drops those values in the history which are older
   than the specified age of the history instance. */
void history::drop (void) {
  nr_double_t f = first ();
  nr_double_t l = last ();
  if (age > 0.0 && l - f > age) {
    int r, i = t->getSize () - values->getSize ();
    for (r = 0; i < t->getSize (); r++, i++)
      if (l - t->get (i) < age)	break;
    r -= 2; // keep 2 values being older than specified age
    if (r > 127) values->drop (r);
  }
}

/* The function returns the value nearest to the given time value. */
nr_double_t history::nearest (nr_double_t tval) {
  if (t != NULL) {
    int l = t->getSize () - values->getSize ();
    int r = t->getSize () - 1;
    int i = -1;
    nr_double_t diff = NR_MAX;
    i = seek (tval, l, r, diff, i);
    i = i - l;
    return values->get (i);
  }
  return 0.0;
}

/* The function is utilized in order to find the nearest value to a
   given time value.  Since the time vector is ordered a recursive
   lookup algorithm can be used.  The function returns the current
   index into the time vector as well as the error in time. */
int history::seek (nr_double_t tval, int l, int r, nr_double_t& diff,
		   int idx) {
  int i = (l + r) / 2;
  if (l == r) return i;
  nr_double_t v = t->get (i);
  nr_double_t d = v - tval;
  if (fabs (d) < diff) {
    // better approximation
    diff = fabs (d);
    idx = i;
  }
  else if (i == l) {
    // no better
    return idx;
  }
  if (d < 0.0) {
    // look later
    return seek (tval, i, r, diff, idx);
  }
  else if (d > 0.0) {
    // look earlier
    return seek (tval, l, i, diff, idx);
  }
  return idx;
}
