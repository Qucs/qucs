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

namespace qucs {

/* This copy constructor creates a instance of the object class based
   on the given object. */
object::object (const object & o) {
  name = o.name;
  next = o.next;
  prev = o.prev;
  copyProperties (o.prop);
}

// Destructor deletes an instance of the object class.
object::~object () {
  deleteProperties ();
}


// The function adds a complete property to the object property list.
void object::addProperty (const std::string &n, property * const p) {
  // for now
  (void) n;
  p->setNext (prop);
  prop = p;
}

/* This function adds a property consisting of a key and a string
   value to the object. */
void object::addProperty (const std::string &n, const char * const val, const bool def) {
  property * p = new property (n, val);
  p->setDefault(def);
  this->addProperty (n, p);
}

/* This function sets the specified property consisting of a key and a
   string value in the object. */
  void object::setProperty (const std::string &n, const char * const val) {
  property * p = prop->findProperty (n);
  if (p != NULL)
    p->set (val);
  else
    addProperty (n, val);
}

/* This function adds a property consisting of a key and a double
   value to the object. */
void object::addProperty (const std::string &n, const nr_double_t val, const bool def) {
  property * p = new property (n, val);
  p->setDefault(def);
  addProperty (n, p);
}

/* This function sets the specified property consisting of a key and a
   double value in the object. */
void object::setProperty (const std::string &n, const nr_double_t val) {
  property * p = prop->findProperty (n);
  if (p != NULL)
    p->set (val);
  else
    addProperty (n, val);
}

/* Th function sets the specified property consisting of a key and a
   double value in the object.  The property is marked a scalability
   property. */
void object::setScaledProperty (const std::string &n, const nr_double_t val) {
  setProperty ("Scaled:"+n, val);
}

/* This function adds a property consisting of a key and a variable
   value to the object. */
void object::addProperty (const std::string &n, variable * const val, const bool def) {
  property * p = new property (n, val);
  p->setDefault(def);
  addProperty (n,p);
}

/* Returns the requested property value which has been previously
   added as its vector representation.  If there is no such property
   the function returns NULL. */
qucs::vector * object::getPropertyVector (const std::string &n) const {
  const property * p = prop->findProperty (n);
  if (p != NULL)
    return p->getVector ();
  return NULL;
}

/* Returns the requested property value which has been previously
   added as its text representation.  If there is no such property the
   function returns NULL. */
const char * object::getPropertyString (const std::string &n) const {
  const property * p = prop->findProperty (n);
  if (p != NULL)
    return p->getString ();
  return NULL;
}

/* Returns the requested property reference variable name.  If there
   is no such property the function returns NULL. */
const char * object::getPropertyReference (const std::string &n) const {
  const property * p = prop->findProperty (n);
  if (p != NULL)
    return p->getReference ();
  return NULL;
}

/* Returns the requested property value which has been previously
   added as its double representation.  If there is no such property
   the function returns zero. */
nr_double_t object::getPropertyDouble (const std::string &n) const {
  const property * p = prop->findProperty (n);
  if (p != NULL)
    return p->getDouble ();
  return 0.0;
}

/* The functions returns the requested (scalability) property value
   which has been previously added.  If there is no such scaled
   property the function returns the standard property or zero. */
nr_double_t object::getScaledProperty (const std::string &n) const{
  std::string txt = "Scaled:"+n;
  // try to find scaled property
  const property * p = prop->findProperty (txt);
  if (p != NULL)
    return p->getDouble ();
  // default to standard property
  return getPropertyDouble (n);
}

/* Returns the requested property value which has been previously
   added as its integer representation.  If there is no such property
   the function returns zero. */
int object::getPropertyInteger (const std::string &n) const {
  const property * p = prop->findProperty (n);
  if (p != NULL)
    return p->getInteger ();
  return 0;
}

/* The function checks whether the object has got a certain property
   value.  If so it returns non-zero, otherwise it returns zero. */
bool object::hasProperty (const std::string &n) const {
  return (prop && prop->findProperty (n)) ? true : false;
}

/* The function checks whether the object has got a certain property
   value and if this has its default value.  If so it returns  non-zero,
   otherwise it returns zero. */
bool object::isPropertyGiven (const std::string &n) const {
  if (prop != NULL) {
    const property * p = prop->findProperty (n);
    if (p != NULL && !p->isDefault ())
      return true;
  }
  return false;
}

/* This function copies all properties in the given property list into
   an object. */
void object::copyProperties (property * org) {
  prop = NULL;
  while (org != NULL) {
    addProperty (org->getName(),new property (*org));
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
int object::countProperties (void) const {
  int res = 0;
  for (property * p = prop; p != NULL; p = p->getNext ()) res++;
  return res;
}

// This function returns a text representation of the objects properties.
const char * object::propertyList (void) const {
  std::string ptxt;
  for (property * p = prop; p != NULL; p = p->getNext ()) {
    std::string n = std::string(p->getName ());    
    std::string val = std::string(p->toString ());
    std::string text = n+"=\""+val+"\"";
    ptxt += text;
    if (p->getNext () != NULL)
      ptxt += " ";
  }
  return ptxt.c_str();
}

} // namespace qucs
