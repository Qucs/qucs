/*
 * pair.cpp - key/value pair class implementation
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
 * $Id: pair.cpp,v 1.1 2006/01/09 09:11:07 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pair.h"

// Constructor creates an unnamed instance of the pair class.
pair::pair () {
  name = NULL;
  value = 0.0;
}

// Constructor creates a named instance of the pair class.
pair::pair (char * n) {
  name = n ? strdup (n) : NULL;
  value = 0.0;
}

/* This full qualified constructor creates an instance of the pair
   class containing both the key and the value of the pair. */
pair::pair (char * n, nr_double_t val) {
  name = n ? strdup (n) : NULL;
  value = val;
}

/* The copy constructor creates a new instance of the pair class based
   on the given pair object. */
pair::pair (const pair & p) {
  name = NULL;
  if (p.name) name = strdup (p.name);
  value = p.value;
}

// Destructor deletes the pair object.
pair::~pair () {
  if (name) free (name);
}

// Sets the name of the pair.
void pair::setName (char * n) {
  if (name) free (name);
  name = n ? strdup (n) : NULL;
}

// Returns the name of the pair.
char * pair::getName (void) {
  return name;
}
