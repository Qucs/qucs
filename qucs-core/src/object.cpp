/*
 * object.cpp - generic object class implementation
 *
 * Copyright (C) 2003, 2004, 2005, 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: object.cpp 1870 2013-03-06 12:52:07Z crobarcro $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "logging.h"
#include "complex.h"
#include "property.h"
#include "object.h"
#include "variable.h"


// Constructor creates an unnamed instance of the object class.
object::object () {
  name = NULL;
  prev = next = NULL;
  prop = NULL;
  ptxt = NULL;
}

// This constructor creates a named instance of the object class.
object::object (const char * n) {
  // create a copy of the character array pointed to by n
  // and get a pointer to the copy using strdup
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
void object::setName (const char * n) {
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
property * object::addProperty (const char * n, const char * val) {
  property * p = new property (n, val);
  addProperty (p);
  return p;
}

/* This function sets the specified property consisting of a key and a
   string value in the object. */
void object::setProperty (const char * n, char * val) {
  property * p = prop->findProperty (n);
  if (p != NULL)
    p->set (val);
  else
    addProperty (n, val);
}

/* This function adds a property consisting of a key and a double
   value to the object. */
property * object::addProperty (const char * n, nr_double_t val) {
  property * p = new property (n, val);
  addProperty (p);
  return p;
}

/* This function sets the specified property consisting of a key and a
   double value in the object. */
void object::setProperty (const char * n, nr_double_t val) {
  property * p = prop->findProperty (n);
  if (p != NULL)
    p->set (val);
  else
    addProperty (n, val);
}

/* Th function sets the specified property consisting of a key and a
   double value in the object.  The property is marked a scalability
   property. */
void object::setScaledProperty (const char * n, nr_double_t val) {
  char prop[64];
  sprintf (prop, "Scaled:%s", n);
  setProperty (prop, val);
}

/* This function adds a property consisting of a key and a variable
   value to the object. */
property * object::addProperty (const char * n, variable * val) {
  property * p = new property (n, val);
  addProperty (p);
  return p;
}

/* Returns the requested property value which has been previously
   added as its vector representation.  If there is no such property
   the function returns NULL. */
::vector * object::getPropertyVector (const char * n) {
  property * p = prop->findProperty (n);
  if (p != NULL) return p->getVector ();
  return NULL;
}

/* Returns the requested property value which has been previously
   added as its text representation.  If there is no such property the
   function returns NULL. */
char * object::getPropertyString (const char * n) {
  property * p = prop->findProperty (n);
  if (p != NULL) return p->getString ();
  return NULL;
}

/* Returns the requested property reference variable name.  If there
   is no such property the function returns NULL. */
char * object::getPropertyReference (const char * n) {
  property * p = prop->findProperty (n);
  if (p != NULL) return p->getReference ();
  return NULL;
}

/* Returns the requested property value which has been previously
   added as its double representation.  If there is no such property
   the function returns zero. */
nr_double_t object::getPropertyDouble (const char * n) {
  property * p = prop->findProperty (n);
  if (p != NULL) return p->getDouble ();
  return 0.0;
}

/* The functions returns the requested (scalability) property value
   which has been previously added.  If there is no such scaled
   property the function returns the standard property or zero. */
nr_double_t object::getScaledProperty (const char * n) {
  char txt[64];
  sprintf (txt, "Scaled:%s", n);
  // try to find scaled property
  property * p = prop->findProperty (txt);
  if (p != NULL) return p->getDouble ();
  // default to standard property
  return getPropertyDouble (n);
}

/* Returns the requested property value which has been previously
   added as its integer representation.  If there is no such property
   the function returns zero. */
int object::getPropertyInteger (const char * n) {
  property * p = prop->findProperty (n);
  if (p != NULL) return p->getInteger ();
  return 0;
}

/* The function checks whether the object has got a certain property
   value.  If so it returns non-zero, otherwise it returns zero. */
bool object::hasProperty (const char * n) {
  return (prop && prop->findProperty (n)) ? true : false;
}

/* The function checks whether the object has got a certain property
   value and if this has its default value.  If so it returns  non-zero,
   otherwise it returns zero. */
bool object::isPropertyGiven (const char * n) {
  if (prop != NULL) {
    property * p = prop->findProperty (n);
    if (p != NULL && !p->isDefault ()) return true;
  }
  return false;
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
  int len = countProperties () + 1;
  ptxt = (char *) malloc (len); ptxt[0] = '\0';
  for (property * p = prop; p != NULL; p = p->getNext ()) {
    char * n = p->getName ();
    char * val = p->toString ();
    char * text = (char *) malloc (strlen (n) + strlen (val) + 4);
    sprintf (text, "%s=\"%s\"", n, val);
    len += strlen (text);
    ptxt = (char *) realloc (ptxt, len);
    strcat (ptxt, text);
    if (p->getNext () != NULL) strcat (ptxt, " ");
    free (text);
  }
  return ptxt;
}
