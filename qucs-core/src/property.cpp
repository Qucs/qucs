/*
 * property.cpp - generic property class implementation
 *
 * Copyright (C) 2003 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: property.cpp,v 1.1.1.1 2003-12-20 19:03:25 ela Exp $
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
#include "property.h"

// Constructor creates an unnamed instance of the property class.
property::property () {
  name = NULL;
  value = 0.0;
  str = NULL;
  next = NULL;
}

// Constructor creates a named instance of the property class.
property::property (char * n) {
  name = strdup (n);
  value = 0.0;
  str = NULL;
  next = NULL;
}

/* This full qualified constructor creates an instance of the property
   class containing both the key and the value of the property. */
property::property (char * n, char * val) {
  name = strdup (n);
  str = strdup (val);
  value = 0.0;
  next = NULL;
}

/* This full qualified constructor creates an instance of the property
   class containing both the key and the value of the property. */
property::property (char * n, nr_double_t val) {
  name = strdup (n);
  value = val;
  str = NULL;
  next = NULL;
}

/* The copy constructor creates a new instance of the property class
   based on the given property object. */
property::property (const property & p) {
  name = str = NULL;
  if (p.name)
    name = strdup (p.name);
  if (p.str)
    str = strdup (p.str);
  value = p.value;
  next = p.next;
}

// Destructor deletes the property object.
property::~property () {
  free (name);
  free (str);
}

// Sets the name of the property.
void property::setName (char * n) {
  if (name)
    free (name);
  name = strdup (n);
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

// Returns the property's value as string.
char * property::getString (void) {
  return str;
}

// Returns the property's value as double.
nr_double_t property::getDouble (void) {
  return value;
}

// Returns the property's value as integer.
int property::getInteger (void) {
  return (int) floor (value);
}
