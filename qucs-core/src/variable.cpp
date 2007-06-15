/*
 * variable.cpp - generic variable class implementation
 *
 * Copyright (C) 2004, 2007 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: variable.cpp,v 1.7 2007/06/15 21:13:30 ela Exp $
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
  text = NULL;
  next = NULL;
  type = VAR_UNKNOWN;
}

// This constructor creates a named instance of the variable class.
variable::variable (char * n) {
  name = n ? strdup (n) : NULL;
  text = NULL;
  next = NULL;
  type = VAR_UNKNOWN;
}

/* This copy constructor creates a instance of the variable class based
   on the given variable. */
variable::variable (const variable & o) {
  name = o.name ? strdup (o.name) : NULL;
  text = o.text ? strdup (o.text) : NULL;
  type = o.type;
  next = o.next;
  value = o.value;
}

/// Destructor deletes an instance of the variable class.
variable::~variable () {
  if (name) free (name);
  if (text) free (text);
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

// Creates textual representation of a variable.
char * variable::toString (void) {
  char * str = NULL;
  char * val = NULL;
  if (text) { free (text); text = NULL; }
  switch (type) {
  case VAR_UNKNOWN:
    text = strdup ("variable");
    break;
  case VAR_CONSTANT:
    str = value.c->toString ();
    text = (char *) malloc (strlen (str) + 11);
    sprintf (text, "constant: %s", str);
    break;
  case VAR_VALUE:
    str = value.v->toString ();
    text = (char *) malloc (strlen (str) + 8);
    sprintf (text, "value: %s", str);
    break;
  case VAR_REFERENCE:
    str = value.r->toString ();
    val = value.r->getResult()->toString ();
    text = (char *) malloc (strlen (str) + strlen (val) + 15);
    sprintf (text, "reference: %s = %s", str, val);
    break;
  case VAR_SUBSTRATE:
    str = value.s->getName ();
    text = (char *) malloc (strlen (str) + 12);
    sprintf (text, "substrate: %s", str);
    break;
  case VAR_ANALYSIS:
    str = value.a->getName ();
    text = (char *) malloc (strlen (str) + 11);
    sprintf (text, "analysis: %s", str);
    break;
  default:
    text = strdup ("?variable?");
    break;
  }
  return text;
}
