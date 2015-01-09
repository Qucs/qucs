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
 * $Id$
 *
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "nasolution.h"

namespace qucs {

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
void nasolution<nr_type_t>::add (const std::string &n, nr_type_t value, int current) {
  naentry<nr_type_t> entry(n, value, current);
  entries.insert ({{n, entry}});
}

// Finds the given nasolution entry in the list.
template <class nr_type_t>
naentry<nr_type_t> * nasolution<nr_type_t>::find (const std::string &name, int current) {
  for (auto & nait : entries) {
    if (nait.second.current == current) {
      if (nait.second.n == name)
	return &(nait.second);
    }
  }
  return NULL;
}




} // namespace qucs
