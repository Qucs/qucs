/*
 * property.cpp - generic property class implementation
 *
 * Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: property.cpp,v 1.4 2004/04/25 17:08:50 ela Exp $
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
#include "property.h"

// Constructor creates an unnamed instance of the property class.
property::property () {
  name = NULL;
  value = 0.0;
  str = NULL;
  var = NULL;
  next = NULL;
}

// Constructor creates a named instance of the property class.
property::property (char * n) {
  name = n ? strdup (n) : NULL;
  value = 0.0;
  str = NULL;
  var = NULL;
  next = NULL;
}

/* This full qualified constructor creates an instance of the property
   class containing both the key and the value of the property. */
property::property (char * n, char * val) {
  name = n ? strdup (n) : NULL;
  str = val ? strdup (val) : NULL;
  value = 0.0;
  var = NULL;
  next = NULL;
}

/* This full qualified constructor creates an instance of the property
   class containing both the key and the value of the property. */
property::property (char * n, nr_double_t val) {
  name = n ? strdup (n) : NULL;
  value = val;
  str = NULL;
  var = NULL;
  next = NULL;
}

/* This full qualified constructor creates an instance of the property
   class containing both the key and the value of the property. */
property::property (char * n, variable * val) {
  name = n ? strdup (n) : NULL;
  var = val;
  value = 0.0;
  str = NULL;
  next = NULL;
}

/* The copy constructor creates a new instance of the property class
   based on the given property object. */
property::property (const property & p) {
  name = p.name ? strdup (p.name) : NULL;
  str = p.str ? strdup (p.str) : NULL;
  value = p.value;
  next = p.next;
  var = p.var;
}

// Destructor deletes the property object.
property::~property () {
  if (name) free (name);
  if (str) free (str);
}

// Sets the name of the property.
void property::setName (char * n) {
  if (name) free (name);
  name = n ? strdup (n) : NULL;
}

// Returns the name of the property.
char * property::getName (void) {
  return name;
}

/* Goes through the chained list of the properties and looks for a
   property matching the given key and returns its value if possible.
   If there is no such property the function returns NULL. */
property * property::findProperty (char * n) {
  for (property * p = this; p != NULL; p = p->getNext ()) {
    if (!strcmp (p->getName (), n)) {
      return p;
    }
  }
  return NULL;
}

// Short macro in order to obtain the correct constant value.
#define D(con) ((constant *) (con))->d

// Returns the property's value as string.
char * property::getString (void) {
  //if (var != NULL) return var->getString ();
  return str;
}

// Returns the property's value as double.
nr_double_t property::getDouble (void) {
  if (var != NULL) return D (var->getConstant ());
  return value;
}

// Returns the property's value as integer.
int property::getInteger (void) {
  if (var != NULL) return (int) floor (D (var->getConstant ()));
  return (int) floor (value);
}
