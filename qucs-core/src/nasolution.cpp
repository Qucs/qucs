/*
 * nasolution.cpp - nodal analysis solution template class implementation
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
 * $Id: nasolution.cpp,v 1.1 2006-07-24 08:07:41 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nasolution.h"

// Constructor creates an instance of the nasolution class.
template <class nr_type_t>
nasolution<nr_type_t>::nasolution () {
}

/* This copy constructor creates a instance of the nasolution class based
   on the given nasolution. */
template <class nr_type_t>
nasolution<nr_type_t>::nasolution (const nasolution<nr_type_t> & p) {
  entries = valuelist< naentry<nr_type_t> > (p.entries);
}

// Destructor deletes a nasolution object.
template <class nr_type_t>
nasolution<nr_type_t>::~nasolution () {
  clear ();
}

// Resets the nasolution object.
template <class nr_type_t>
void nasolution<nr_type_t>::clear (void) {
  entries.clear ();
}

// Adds a new solution entry into the nasolution list.
template <class nr_type_t>
void nasolution<nr_type_t>::add (char * n, nr_type_t value, int current) {
  naentry<nr_type_t> * entry = new naentry<nr_type_t> (n, value, current);
  entries.add (n, entry);
}

// Finds the given nasolution entry in the list.
template <class nr_type_t>
naentry<nr_type_t> * nasolution<nr_type_t>::find (char * n, int current) {
  for (valuelistiterator< naentry<nr_type_t> > it (entries); *it; ++it) {
    naentry<nr_type_t> * na = it.currentVal ();
    if (na->current == current) {
      if (na->n && n && !strcmp (na->n, n))
	return na;
    }
  }
  return NULL;
}

// Constructor creates an instance of the naentry class.
template <class nr_type_t>
naentry<nr_type_t>::naentry () {
  value = 0;
  n = NULL;
  current = -1;
}

// Constructor creates an instance of the naentry class.
template <class nr_type_t>
naentry<nr_type_t>::naentry (char * na, nr_type_t val, int cur) {
  value = val;
  n = na ? strdup (na) : NULL;
  current = cur;
}

/* This copy constructor creates a instance of the naentry class based
   on the given naentry. */
template <class nr_type_t>
naentry<nr_type_t>::naentry (const naentry<nr_type_t> & o) {
  value = o.value;
  n = o.n ? strdup (o.n) : NULL;
  current = o.current;
}

// Destructor deletes a naentry object.
template <class nr_type_t>
naentry<nr_type_t>::~naentry () {
  if (n) free (n);
}
