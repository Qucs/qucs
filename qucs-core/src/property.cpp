/*
 * property.cpp - generic property class implementation
 *
 * Copyright (C) 2003-2009 Stefan Jahn <stefan@lkcc.org>
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
  type = PROPERTY_UNKNOWN;
  name = NULL;
  value = 0.0;
  str = NULL;
  txt = NULL;
  var = NULL;
  next = NULL;
  def = false;
}

// Constructor creates a named instance of the property class.
property::property (const char * n) {
  type = PROPERTY_UNKNOWN;
  name = n ? strdup (n) : NULL;
  value = 0.0;
  str = NULL;
  txt = NULL;
  var = NULL;
  next = NULL;
  def = false;
}

/* This full qualified constructor creates an instance of the property
   class containing both the key and the value of the property. */
property::property (const char * n, const char * val) {
  type = PROPERTY_STR;
  name = n ? strdup (n) : NULL;
  str = val ? strdup (val) : NULL;
  value = 0.0;
  txt = NULL;
  var = NULL;
  next = NULL;
  def = false;
}

/* This full qualified constructor creates an instance of the property
   class containing both the key and the value of the property. */
property::property (const char * n, nr_double_t val) {
  type = PROPERTY_DOUBLE;
  name = n ? strdup (n) : NULL;
  value = val;
  str = NULL;
  txt = NULL;
  var = NULL;
  next = NULL;
  def = false;
}

/* This full qualified constructor creates an instance of the property
   class containing both the key and the value of the property. */
property::property (const char * n, variable * val) {
  type = PROPERTY_VAR;
  name = n ? strdup (n) : NULL;
  var = val;
  value = 0.0;
  txt = NULL;
  str = NULL;
  next = NULL;
  def = false;
}

/* The copy constructor creates a new instance of the property class
   based on the given property object. */
property::property (const property & p) {
  type = p.type;
  name = p.name ? strdup (p.name) : NULL;
  str = p.str ? strdup (p.str) : NULL;
  value = p.value;
  txt = p.txt ? strdup (p.txt) : NULL;
  next = p.next;
  var = p.var;
  def = p.def;
}

// Destructor deletes the property object.
property::~property () {
#if 0 /* FIXME: do this at another code location */
  if (type == PROPERTY_VAR) {
    constant * c = var->getConstant ();
    if (c->getType () == TAG_VECTOR) {
      delete c;
      delete var;
    }
  }
#endif
  if (name) free (name);
  if (str) free (str);
  if (txt) free (txt);
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
property * property::findProperty (const char * n) {
  for (property * p = this; p != NULL; p = p->getNext ()) {
    if (!strcmp (p->getName (), n)) return p;
  }
  return NULL;
}

// Short macro in order to obtain the correct constant value.
#define D(con) ((constant *) (con))->d
#define S(con) ((constant *) (con))->s
#define V(con) ((constant *) (con))->v

// Returns the property's value as vector.
::vector * property::getVector (void) {
  if (var != NULL) {
    if (var->getType () == VAR_CONSTANT)
      return V (var->getConstant ());
    else if (var->getType () == VAR_REFERENCE)
      return V (var->getReference()->getResult ());
  }
  return NULL;
}

// Returns the property's value as string.
char * property::getString (void) {
  if (var != NULL) return S (var->getConstant ());
  return str;
}

// Returns the property's reference if it is a variable.
char * property::getReference (void) {
  if (var != NULL) return var->getName ();
  return str;
}

// Returns the property's value as double.
nr_double_t property::getDouble (void) {
  if (var != NULL) {
    if (var->getType () == VAR_CONSTANT)
      return D (var->getConstant ());
    else if (var->getType () == VAR_REFERENCE)
      return D (var->getReference()->getResult ());
  }
  return value;
}

// Returns the property's value as integer.
int property::getInteger (void) {
  if (var != NULL) return (int) floor (D (var->getConstant ()));
  return (int) floor (value);
}

// Sets the property's value being a double.
void property::set (nr_double_t val) {
  type = PROPERTY_DOUBLE;
  value = val;
}

// Sets the property's value being an integer.
void property::set (int val) {
  type = PROPERTY_INT;
  value = val;
}

// Sets the property's value being a variable.
void property::set (variable * val) {
  type = PROPERTY_VAR;
  var = val;
}

// Sets the property's value being a string.
void property::set (char * val) {
  type = PROPERTY_STR;
  if (str) free (str);
  str = val ? strdup (val) : NULL;
}

// This function returns a text representation of the property object.
char * property::toString (void) {
  char text[256];
  if (txt) free (txt);
  switch (type) {
  case PROPERTY_UNKNOWN:
    txt = strdup ("(no such type)");
    break;
  case PROPERTY_INT:
    sprintf (text, "%d", (int) floor (value));
    txt = strdup (text);
    break;
  case PROPERTY_STR:
    txt = strdup (str);
    break;
  case PROPERTY_DOUBLE:
    sprintf (text, "%g", (double) value);
    txt = strdup (text);
    break;
  case PROPERTY_VAR:
    sprintf (text, "%s", var->getName ());
    txt = strdup (text);
    break;
  }
  return txt;
}
