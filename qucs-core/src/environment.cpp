/*
 * environment.cpp - variable environment class implementation
 *
 * Copyright (C) 2004, 2005, 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: environment.cpp,v 1.8 2007-02-22 17:26:43 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "variable.h"
#include "equation.h"
#include "ptrlist.h"
#include "logging.h"
#include "environment.h"

// Constructor creates an unnamed instance of the environment class.
environment::environment () {
  name = NULL;
  root = NULL;
  solvee = NULL;
  checkee = NULL;
  children = new ptrlist<environment>;
}

// Constructor creates a named instance of the environment class.
environment::environment (char * n) {
  name = n ? strdup (n) : NULL;
  root = NULL;
  solvee = NULL;
  checkee = NULL;
  children = new ptrlist<environment>;
}

/* The copy constructor creates a new instance of the environment
   class based on the given environment object. */
environment::environment (const environment & e) {
  name = e.name ? strdup (e.name) : NULL;
  copyVariables (e.root);
  solvee = e.solvee;
  checkee = e.checkee;
  children = new ptrlist<environment>;
}

/* Very alike the copy constructor the function copies the content of
   the given environment into the calling environment. */
void environment::copy (const environment & e) {
  if (name) free (name);
  name = e.name ? strdup (e.name) : NULL;
  deleteVariables ();
  copyVariables (e.root);
  solvee = e.solvee;
  checkee = e.checkee;
  delete children;
  children = new ptrlist<environment>;
}

// Destructor deletes the environment object.
environment::~environment () {
  if (name) free (name);
  deleteVariables ();
  if (solvee) delete solvee;
  delete children;
}

// Sets the name of the environment.
void environment::setName (char * n) {
  if (name) free (name);
  name = n ? strdup (n) : NULL;
}

// Returns the name of the environment.
char * environment::getName (void) {
  return name;
}

/* This function copies all variables in the given variable list into
   an environment. */
void environment::copyVariables (variable * org) {
  variable * var;
  root = NULL;
  while (org != NULL) {
    // copy variable (references only)
    var = new variable (*org);
    constant * c; reference * r;
    // depending on variable type copy values too
    switch (var->getType ()) {
    case VAR_CONSTANT:
      c = new constant (TAG_DOUBLE);
      c->d = var->getConstant()->d;
      var->setConstant (c);
      break;
    case VAR_REFERENCE:
      r = new reference ();
      r->n = strdup (var->getReference()->n);
      var->setReference (r);
      break;
    }
    var->setNext (root);
    root = var;
    org = org->getNext ();
  }
}

// Deletes all variable in the environment.
void environment::deleteVariables (void) {
  variable * n;
  for (variable * var = root; var != NULL; var = n) {
    n = var->getNext ();
    if (var->getType () == VAR_CONSTANT)
      delete var->getConstant ();
    else if (var->getType () == VAR_SUBSTRATE)
      delete var->getSubstrate ();
    else if (var->getType () == VAR_REFERENCE)
      delete var->getReference ();
    delete var;
  }
  root = NULL;
}

/* This function adds a variable to the environment. */
void environment::addVariable (variable * var) {
  var->setNext (root);
  root = var;
}

/* This function looks for the variable name in the environment and
   returns it if possible.  Otherwise the function returns NULL. */
variable * environment::getVariable (char * n) {
  for (variable * var = root; var != NULL; var = var->getNext ()) {
    if (!strcmp (var->getName (), n))
      return var;
  }
  return NULL;
}

// The function runs the equation checker for this environment.
int environment::equationChecker (int noundefined) {
  return checkee->check (noundefined);
}

// The function runs the equation solver for this environment.
int environment::equationSolver (dataset * data) {
  solvee->setEquations (checkee->getEquations ());
  int err = solvee->solve (data);
  checkee->setEquations (solvee->getEquations ());
  return err;
}

// Adds a child to the environment.
void environment::addChild (environment * child) {
  children->add (child);
}

// Removes a child from the environment.
void environment::delChild (environment * child) {
  children->del (child);
}

/* The function solves the equations of the current environment object
   as well as these of its children, updates the variables and passes
   the arguments to each children. */
int environment::runSolver (void) {
  int ret = 0;

  // solve equations in current environment
  ret |= equationSolver (NULL);
  fetchConstants ();

  // cycle through children
  for (ptrlistiterator<environment> it (*children); *it; ++it) {
    // pass constants to solver
    (*it)->passConstants ();
    // pass references
    (*it)->updateReferences (this);
    // actually run the solver
    ret |= (*it)->runSolver ();
  }

  return ret;
}

/* Passes the constants of the environment to the equation solver.
   This is necessary since equally typed environments use the same
   equation checker and solver. */
void environment::passConstants (void) {
  for (variable * var = root; var != NULL; var = var->getNext ()) {
    if (var->getType () == VAR_CONSTANT) {
      constant * c = var->getConstant ();
      setDouble (var->getName (), c->d);
    }
  }
}

/* Fetches the values of variables from the equation solver. */
void environment::fetchConstants (void) {
  for (variable * var = root; var != NULL; var = var->getNext ()) {
    if (var->getType () == VAR_CONSTANT) {
      constant * c = var->getConstant ();
      c->d = getDouble (var->getName ());
    }
  }
}

/* This function looks through all variables which are references.  If
   found the variable gets resolved in the upper (parent) environment
   and the value put into the result of the reference as well as into
   the equation checker of the current environment. */
void environment::updateReferences (environment * up) {
  for (variable * var = root; var != NULL; var = var->getNext ()) {
    if (var->getType () == VAR_REFERENCE) {
      reference * r = var->getReference ();
      // possible because no self-referring subcircuit types possible
      nr_double_t d = up->getDouble (r->n);
      constant * c = r->getResult ();
      c->d = d;
      setDouble (var->getName (), d);
    }
  }
}

// Returns double value of an assignment in the equation checker.
nr_double_t environment::getDouble (char * ident) {
  return checkee->getDouble (ident);
}

// Sets the double value of an assignment in the equation checker.
void environment::setDouble (char * ident, nr_double_t val) {
  checkee->setDouble (ident, val);
}

// Return double value of a variable in the environment.
nr_double_t environment::getDoubleConstant (char * ident) {
  variable * var = getVariable (ident);
  if (var != NULL && var->getType () == VAR_CONSTANT) {
    constant * c = var->getConstant ();
    return c->d;
  }
  return 0.0;
}

// Sets the double value of a variable in the environment.
void environment::setDoubleConstant (char * ident, nr_double_t val) {
  variable * var = getVariable (ident);
  if (var != NULL && var->getType () == VAR_CONSTANT) {
    constant * c = var->getConstant ();
    c->d = val;
  }
}

// Returns the referenced value of a variable in the environment. 
char * environment::getDoubleReference (char * ident) {
  variable * var = getVariable (ident);
  if (var != NULL &&  var->getType () == VAR_REFERENCE) {
    reference * r = var->getReference ();
    return r->n;
  }
  return NULL;
}

// Sets the referenced value of a variable in the environment. 
void environment::setDoubleReference (char * ident, char * val) {
  variable * var = getVariable (ident);
  if (var != NULL) {
    if (var->getType () == VAR_CONSTANT) {
      // its a constant, so make it a reference
      delete var->getConstant ();
      reference * r = new reference ();
      r->n = strdup (val);
      constant * c = new constant (TAG_DOUBLE);
      r->setResult (c);
      var->setReference (r);
    }
    else if (var->getType () == VAR_REFERENCE) {
      // just apply the reference
      reference * r = var->getReference ();
      if (r->n) free (r->n);
      r->n = strdup (val);
    }
  }
}

// Prints the environment.
void environment::print (bool all) {
  ptrlistiterator<environment> it;
  logprint (LOG_STATUS, "environment %s\n", getName () ? getName () : "?env?");
  for (variable * var = root; var != NULL; var = var->getNext ()) {
    logprint (LOG_STATUS, "  %s [%s]\n", var->getName (), var->toString ());
  }
  for (it = ptrlistiterator<environment> (*children); *it; ++it) {
    logprint (LOG_STATUS, "  %s\n", (*it)->getName ());
  }
  if (all) {
    for (it = ptrlistiterator<environment> (*children); *it; ++it)
      (*it)->print ();
  }
}
