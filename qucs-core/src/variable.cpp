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
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *
 * $Id: variable.cpp,v 1.5 2005-06-02 18:17:51 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "equation.h"
#include "components/microstrip/substrate.h"
#include "analysis.h"
#include "variable.h"


// Constructor creates an unnamed instance of the variable class.
variable::variable () {
  name = NULL;
  next = NULL;
  type = VAR_UNKNOWN;
}

// This constructor creates a named instance of the variable class.
variable::variable (char * n) {
  name = n ? strdup (n) : NULL;
  next = NULL;
  type = VAR_UNKNOWN;
}

/* This copy constructor creates a instance of the variable class based
   on the given variable. */
variable::variable (const variable & o) {
  if (o.name != NULL) name = o.name ? strdup (o.name) : NULL;
  type = o.type;
  next = o.next;
  value = o.value;
}

/// Destructor deletes an instance of the variable class.
variable::~variable () {
  if (name) free (name);
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
