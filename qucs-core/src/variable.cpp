/*
 * variable.cpp - generic variable class implementation
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: variable.cpp,v 1.1 2004/02/09 18:26:08 ela Exp $
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
#include "object.h"
#include "vector.h"
#include "matrix.h"
#include "property.h"
#include "variable.h"


// Constructor creates an unnamed instance of the variable class.
variable::variable () {
  name = NULL;
  type = VAR_UNKNOWN;
}

// This constructor creates a named instance of the variable class.
variable::variable (char * n) {
  name = strdup (n);
  type = VAR_UNKNOWN;
}

/* This copy constructor creates a instance of the variable class based
   on the given variable. */
variable::variable (const variable & o) {
  name = NULL;
  if (o.name != NULL) name = strdup (o.name);
  type = o.type;
}

/// Destructor deletes an instance of the variable class.
variable::~variable () {
  free (name);
  switch (type) {
  case VAR_COMPLEX: delete value.c; break;
  case VAR_MATRIX: delete value.m; break;
  case VAR_VECTOR: delete value.v; break;
  }
}

// Sets the name of the variable.
void variable::setName (char * n) {
  if (name) free (name);
  name = n ? strdup (n) : NULL;
}

// Returns the name of the variable.
char * variable::getName (void) {
  return name;
}
