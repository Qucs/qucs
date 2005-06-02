/*
 * operatingpoint.cpp - operating point class implementation
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
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *
 * $Id: operatingpoint.cpp,v 1.3 2005-06-02 18:17:51 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "complex.h"
#include "variable.h"
#include "operatingpoint.h"

// Constructor creates an unnamed instance of the operating point class.
operatingpoint::operatingpoint () {
  name = NULL;
  value = 0.0;
  next = NULL;
}

// Constructor creates a named instance of the operating point class.
operatingpoint::operatingpoint (char * n) {
  name = n ? strdup (n) : NULL;
  value = 0.0;
  next = NULL;
}

/* This full qualified constructor creates an instance of the
   operating point class containing both the key and the value of the
   operating point. */
operatingpoint::operatingpoint (char * n, nr_double_t val) {
  name = n ? strdup (n) : NULL;
  value = val;
  next = NULL;
}

/* The copy constructor creates a new instance of the operating point
   class based on the given operating point object. */
operatingpoint::operatingpoint (const operatingpoint & p) {
  name = NULL;
  if (p.name) name = strdup (p.name);
  value = p.value;
  next = p.next;
}

// Destructor deletes the operating point object.
operatingpoint::~operatingpoint () {
  if (name) free (name);
}

// Sets the name of the operating point.
void operatingpoint::setName (char * n) {
  if (name) free (name);
  name = n ? strdup (n) : NULL;
}

// Returns the name of the operating point.
char * operatingpoint::getName (void) {
  return name;
}

/* Goes through the chained list of the operating points and looks for
   a operating point matching the given key and returns its value if
   possible.  If there is no such operating point the function returns
   NULL. */
operatingpoint * operatingpoint::findOperatingPoint (char * n) {
  for (operatingpoint * p = this; p != NULL; p = p->getNext ()) {
    if (!strcmp (p->getName (), n)) return p;
  }
  return NULL;
}
