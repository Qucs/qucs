/*
 * object.cpp - generic object class implementation
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
 * $Id: object.cpp,v 1.2 2004/02/13 20:31:45 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "logging.h"
#include "complex.h"
#include "property.h"
#include "variable.h"
#include "object.h"


// Constructor creates an unnamed instance of the object class.
object::object () {
  name = NULL;
  prev = next = NULL;
  prop = NULL;
}

// This constructor creates a named instance of the object class.
object::object (char * n) {
  name = strdup (n);
  prev = next = NULL;
  prop = NULL;
}

/* This copy constructor creates a instance of the object class based
   on the given object. */
object::object (const object & o) {
  name = NULL;
  if (o.name != NULL)
    name = strdup (o.name);
  next = o.next;
  prev = o.prev;
  copyProperties (o.prop);
}

/// Destructor deletes an instance of the object class.
object::~object () {
  free (name);
  deleteProperties ();
}

// Sets the name of the object.
void object::setName (char * n) {
  if (name) free (name);
  name = n ? strdup (n) : NULL;
}

// Returns the name of the object.
char * object::getName (void) {
  return name;
}

/* This function adds a property consisting of a key and a value to
   the object. */
void object::addProperty (char * n, char * val) {
  property * p = new property (n, val);
  p->setNext (prop);
  prop = p;
}

/* This function adds a property consisting of a key and a value to
   the object. */
void object::addProperty (char * n, nr_double_t val) {
  property * p = new property (n, val);
  p->setNext (prop);
  prop = p;
}

/* This function adds a property consisting of a key and a value to
   the object. */
void object::addProperty (char * n, variable * val) {
  property * p = new property (n, val);
  p->setNext (prop);
  prop = p;
}

/* Returns the requested property value which has been previously
   added as its text representation.  If there is no such property the
   function returns NULL. */
char * object::getPropertyString (char * n) {
  property * p = prop->findProperty (n);
  if (p != NULL)
    return p->getString ();
  return NULL;
}

/* Returns the requested property value which has been previously
   added as its double representation.  If there is no such property the
   function returns zero. */
nr_double_t object::getPropertyDouble (char * n) {
  property * p = prop->findProperty (n);
  if (p != NULL)
    return p->getDouble ();
  return 0.0;
}

/* Returns the requested property value which has been previously
   added as its integer representation.  If there is no such property the
   function returns zero. */
int object::getPropertyInteger (char * n) {
  property * p = prop->findProperty (n);
  if (p != NULL)
    return p->getInteger ();
  return 0;
}

/* This function copies all properties in the given property list into
   an object. */
void object::copyProperties (property * org) {
  property * p;
  prop = NULL;
  while (org != NULL) {
    p = new property (*org);
    p->setNext (prop);
    prop = p;
    org = org->getNext ();
  }
}

// Deletes all properties of an object.
void object::deleteProperties (void) {
  property * n;
  for (property * p = prop; p != NULL; p = n) {
    n = p->getNext ();
    delete p;
  }
  prop = NULL;
}

#if DEBUG
// DEBUG function:  Lists the properties of an object.
void object::listProperties (void) {
  if (prop == NULL) return;
  logprint (LOG_STATUS, "DEBUG:");
  for (property * p = prop; p != NULL; p = p->getNext ()) {
    logprint (LOG_STATUS, " %s=""%s""", p->getName (), p->getString ());
  }
  logprint (LOG_STATUS, "\n");
}
#endif /* DEBUG */
