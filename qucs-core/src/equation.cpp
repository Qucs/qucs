/*
 * equation.cpp - checker for the Qucs equations
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
 * $Id: equation.cpp,v 1.2 2004-03-14 17:42:47 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "vector.h"
#include "matrix.h"
#include "dataset.h"
#include "strlist.h"
#include "equation.h"

using namespace eqn;

/* The global list of equations and expression lists. */
node * eqn::equations = NULL;
node * eqn::expressions = NULL;

// Constructor creates an untyped instance of the constant class.
constant::constant () : node (CONSTANT) {
  type = TAG_UNKNOWN;
}

// This constructor creates an typed instance of the constant class.
constant::constant (int tag) : node (CONSTANT) {
  type = tag;
}

// Destructor deletes an instance of the constant class.
constant::~constant () {
}

/* Depending on the type of constant the function prints the textual
   representation of the object. */
void constant::print () {
  switch (type) {
  case TAG_DOUBLE:
    logprint (LOG_STATUS, "%g", d);
    break;
  case TAG_COMPLEX:
    logprint (LOG_STATUS, "%g%cj%g", 
	      real (*c), imag (*c) >= 0 ? '+' : '-', imag (*c));
    break;
  }
}

// Constructor creates an instance of the reference class.
reference::reference () : node (REFERENCE) {
  n = NULL;
}

// Destructor deletes an instance of the reference class.
reference::~reference () {
  if (n) free (n);
}

// Prints textual representation of the reference object.
void reference::print (void) {
  logprint (LOG_STATUS, "%s", n);
}

// Adds the name of the reference to the list of dependencies.
void reference::addDependencies (strlist * depends) {
  depends->add (n);
}

// Constructor creates an instance of the assignment class.
assignment::assignment () : node (ASSIGNMENT) {
  body = NULL;
  result = NULL;
}

// Destructor deletes an instance of the assignment class.
assignment::~assignment () {
  delete body;
  if (result) free (result);
}

// Prints textual representation of the assignment object.
void assignment::print (void) {
  logprint (LOG_STATUS, "%s = ", result);
  body->print ();
}

// Adds the right hand side of the assigment to the list of dependencies.
void assignment::addDependencies (strlist * depends) {
  body->addDependencies (depends);
}

// Constructor creates an instance of the application class.
application::application () : node (APPLICATION) {
  n = NULL;
  nargs = 0;
  args = NULL;
}

// Destructor deletes an instance of the application class.
application::~application () {
  node * next;
  for (node * arg = args; arg != NULL; arg = next) {
    next = arg->getNext ();
    delete arg;
  }
  if (n) free (n);
}

// Prints textual representation of the application object.
void application::print (void) {
  logprint (LOG_STATUS, "%s(", n);
  for (node * arg = args; arg != NULL; arg = arg->getNext ()) {
    arg->print ();
    if (arg->getNext ()) logprint (LOG_STATUS, ",");
  }
  logprint (LOG_STATUS, ")");
}

// Adds the arguments of the application to the list of dependencies.
void application::addDependencies (strlist * depends) {
  for (node * arg = args; arg != NULL; arg = arg->getNext ()) {
    arg->addDependencies (depends);
  }  
}

// Constructor creates an untyped instance of the equation node class.
node::node () {
  tag = UNKNOWN;
  next = NULL;
  dependencies = NULL;
}

// This constructor creates an typed instance of the equation node class.
node::node (int type) {
  tag = type;
  next = NULL;
  dependencies = NULL;
}

// Destructor deletes an instance of the equation node class.
node::~node () {
  if (dependencies) delete dependencies;
}

// Counts the number of equations node attached to the node.
int node::count (void) {
  int c = 0;
  for (node * n = this; n != NULL; n = n->getNext ())
    c++;
  return c;
}

// Appends yet another node to the equation node object.
void node::append (node * last) {
  node * n;
  for (n = this; n->getNext () != NULL; n = n->getNext ());
  last->setNext (NULL);
  n->setNext (last);
}

// Assigns the dependency list to the equation node object.
void node::setDependencies (strlist * depends) {
  if (dependencies) delete dependencies;
  dependencies = depends;
}

// Returns the dependency list of the equation node object.
strlist * node::getDependencies (void) {
  return dependencies;
}

// Constructor creates an instance of the checker class.
checker::checker () {
  equations = NULL;
}

// Destructor deletes an instance of the checker class.
checker::~checker () {
  node * next;
  for (node * eqn = equations; eqn != NULL; eqn = next) {
    next = eqn->getNext ();
    delete eqn;
  }
}

/* The function goes through the list of equations assigned to the
   checker and applies the dependency list. */
void checker::collectDependencies (void) {
  for (node * eqn = equations; eqn != NULL; eqn = eqn->getNext ()) {
    strlist * depends = new strlist ();
    eqn->addDependencies (depends);
    eqn->setDependencies (depends);
  }
}

// Logs the textual representation of all equations.
void checker::list (void) {
  for (node * eqn = equations; eqn != NULL; eqn = eqn->getNext ()) {
    eqn->print ();
    logprint (LOG_STATUS, "\n");
  }
}

int checker::findUndefined (void) {
  int err = 0;

  /* Go through the list of equations and store the left hand side in
     a string list. */
  strlist * idents = new strlist ();
  for (assignment * eqn = (assignment *) equations; eqn != NULL; 
       eqn = (assignment *) eqn->getNext ()) {
    idents->add (eqn->result);
  }

  /* Check whether the variable references could be resolved within
     the equations. */
  for (assignment * eqn = (assignment *) equations; eqn != NULL;
       eqn = (assignment *) eqn->getNext ()) {
    strlist * depends = eqn->getDependencies ();
    for (int i = 0; i < depends->length (); i++) {
      char * var = depends->get (i);
      if (idents->contains (var) <= 0) {
	logprint (LOG_ERROR, "checker error, undefined variable `%s' in "
		  "equation `%s'\n", var, eqn->result);
	err++;
      }
    }
  }
  return err;
}
