/*
 * environment.cpp - variable environment class implementation
 *
 * Copyright (C) 2004, 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: environment.cpp,v 1.5 2006/10/17 09:00:04 raimi Exp $
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
  name = strdup (n);
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
  name = strdup (n);
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
    var = new variable (*org);
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
  ret |= equationSolver (NULL);
  for (ptrlistiterator<environment> it (*children); *it; ++it) {
    ret |= (*it)->runSolver ();
  }
  return ret;
}
