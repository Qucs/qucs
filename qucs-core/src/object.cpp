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
 * $Id: object.cpp,v 1.6 2004-07-08 06:38:43 ela Exp $
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
#include <assert.h>

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
  ptxt = NULL;
}

// This constructor creates a named instance of the object class.
object::object (char * n) {
  name = strdup (n);
  prev = next = NULL;
  prop = NULL;
  ptxt = NULL;
}

/* This copy constructor creates a instance of the object class based
   on the given object. */
object::object (const object & o) {
  name = o.name ? strdup (o.name) : NULL;
  next = o.next;
  prev = o.prev;
  ptxt = o.ptxt ? strdup (o.ptxt) : NULL;
  copyProperties (o.prop);
}

// Destructor deletes an instance of the object class.
object::~object () {
  if (name) free (name);
  if (ptxt) free (ptxt);
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

// The function adds a complete property to the object property list.
void object::addProperty (property * p) {
  p->setNext (prop);
  prop = p;
}

/* This function adds a property consisting of a key and a string
   value to the object. */
void object::addProperty (char * n, char * val) {
  addProperty (new property (n, val));
}

/* This function sets the specified property consisting of a key and a
   string value in the object. */
void object::setProperty (char * n, char * val) {
  property * p = prop->findProperty (n);
  if (p != NULL)
    p->set (val);
  else
    addProperty (n, val);
}

/* This function adds a property consisting of a key and a double
   value to the object. */
void object::addProperty (char * n, nr_double_t val) {
  addProperty (new property (n, val));
}

/* This function sets the specified property consisting of a key and a
   double value in the object. */
void object::setProperty (char * n, nr_double_t val) {
  property * p = prop->findProperty (n);
  if (p != NULL)
    p->set (val);
  else
    addProperty (n, val);
}

/* This function adds a property consisting of a key and a variable
   value to the object. */
void object::addProperty (char * n, variable * val) {
  addProperty (new property (n, val));
}

/* Returns the requested property value which has been previously
   added as its text representation.  If there is no such property the
   function returns NULL. */
char * object::getPropertyString (char * n) {
  property * p = prop->findProperty (n);
  if (p != NULL) return p->getString ();
  return NULL;
}

/* Returns the requested property value which has been previously
   added as its double representation.  If there is no such property
   the function returns zero. */
nr_double_t object::getPropertyDouble (char * n) {
  property * p = prop->findProperty (n);
  if (p != NULL) return p->getDouble ();
  return 0.0;
}

/* Returns the requested property value which has been previously
   added as its integer representation.  If there is no such property
   the function returns zero. */
int object::getPropertyInteger (char * n) {
  property * p = prop->findProperty (n);
  if (p != NULL) return p->getInteger ();
  return 0;
}

/* The function checks whether the object has got a certain property
   value.  If so it returns non-zero, otherwise it returns zero. */
int object::hasProperty (char * n) {
  return (prop && prop->findProperty (n)) ? 1 : 0;
}

/* This function copies all properties in the given property list into
   an object. */
void object::copyProperties (property * org) {
  prop = NULL;
  while (org != NULL) {
    addProperty (new property (*org));
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

// The function returns the number of properties in the object.
int object::countProperties (void) {
  int res = 0;
  for (property * p = prop; p != NULL; p = p->getNext ()) res++;
  return res;
}

// This function returns a text representation of the objects properties.
char * object::propertyList (void) {
  if (ptxt) free (ptxt);
  char text[256];
  int len = countProperties () + 1;
  ptxt = (char *) malloc (len); ptxt[0] = '\0';
  for (property * p = prop; p != NULL; p = p->getNext ()) {
    sprintf (text, "%s=\"%s\"", p->getName (), p->toString ());
    len += strlen (text);
    ptxt = (char *) realloc (ptxt, len);
    strcat (ptxt, text);
    if (p->getNext () != NULL) strcat (ptxt, " ");
  }
  return ptxt;
}
