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
 * $Id: equation.cpp,v 1.4 2004-03-21 09:57:10 ela Exp $
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
  evalPossible = cycle = duplicate = 0;
  next = NULL;
  dependencies = NULL;
}

// This constructor creates an typed instance of the equation node class.
node::node (int type) {
  tag = type;
  evalPossible = cycle = duplicate = 0;
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

#define A(a) ((assignment *) (a))
#define N(n) ((node *) (n))

/* This function recursively finds the variable dependencies for each
   equation initially passed to the equation checker and returns the
   list of variable dependencies regarding this equation instance.
   The caller is responsible for deleting the returned string list
   object. */
strlist * node::recurseDependencies (checker * check, strlist * deps) {

  strlist * res, * sub = NULL;

  /* The abort condition for recursion first. */
  if (deps->contains (A(this)->result)) {
    res = new strlist (*deps);
    cycle = 1;
    return res;
  }

  /* Go through the list of passed dependency variables. */
  for (int i = 0; i < deps->length (); i++) {
    char * var = deps->get (i);
    node * child = check->findEquation (check->equations, var);
    /* Check each child equation. */
    if (child != NULL) {
      if (child->cycle == 0) {
	strlist * cdeps = child->getDependencies ();
	/* And append the dependencies of the child equation. */
	if (cdeps->length () > 0) {
	  res = strlist::join (sub, cdeps);
	  if (sub) delete sub; sub = res;
	}
      }
      /* If any child is cyclic, the parent is too. */
      else {
	cycle = 1;
      }
    }
  }

  /* Recurse once again if the child equations revealed any more
     dependencies. */
  if (sub && sub->length () > 0) {
    res = recurseDependencies (check, sub);
    delete sub;
    sub = res;
  }

  /* Return the result. */
  res = strlist::join (deps, sub);
  if (sub) delete (sub);
  return res;
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
    logprint (LOG_STATUS, "%s", eqn->evalPossible ? "!" : "?");
    eqn->print ();
    logprint (LOG_STATUS, "\n");
  }
}

// Local macro definition to go through the list of equations.
#define foreach_equation(eqn)                        \
  for (assignment * (eqn) = A (equations);           \
       (eqn) != NULL; (eqn) = A ((eqn)->getNext ()))

/* This function checks whether the variable references could be
   resolved within the equations and returns zero if so. */
int checker::findUndefined (void) {
  int err = 0;
  strlist * idents = getVariables ();

  foreach_equation (eqn) {
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
  delete idents;
  return err;
}

/* Go through the list of equations and store the left hand side in
   a string list. */
strlist * checker::getVariables (void) {
  strlist * idents = new strlist ();
  foreach_equation (eqn) {
    idents->add (eqn->result);
  }
  return idents;
}

/* Finds duplicate equation definitions in the list of equations,
   emits appropriate error messages and returns zero if everything is
   ok. */
int checker::findDuplicate (void) {
  int err = 0;
  strlist * idents = getVariables ();
  strlist * dups = new strlist ();

  // Collect duplicate enries.
  foreach_equation (eqn) {
    if (!eqn->duplicate && dups->contains (eqn->result) == 0) {
      eqn->duplicate = idents->contains (eqn->result);
      dups->add (eqn->result);
    }
    else {
      eqn->duplicate = 1;
    }
  }
  // Emit appropriate error messages.
  foreach_equation (eqn) {
    if (eqn->duplicate > 1) {
      logprint (LOG_ERROR, "checker error, variable `%s' occurred %dx\n",
		eqn->result, eqn->duplicate);
      err++;
    }
  }
  delete idents;
  delete dups;
  return err;
}

/* The function returns the equations resulting in the passed variable
   or NULL if there is no such equation. */
node * checker::findEquation (node * root, char * n) {
  for (node * eqn = root; eqn != NULL; eqn = eqn->getNext ()) {
    if (!strcmp (A(eqn)->result, n)) return eqn;
  }
  return NULL;
}

/* This function display the error messages due to equation cycles and
   returns zero if there are no such cycles. */
int checker::detectCycles (void) {
  int err = 0;

  foreach_equation (eqn) {
    strlist * deps = eqn->recurseDependencies (this, eqn->getDependencies ());
    if (deps->contains (eqn->result) || eqn->cycle) {
      logprint (LOG_ERROR, "checker error, cyclic definition of variable "
		"`%s' involves: `%s'\n", eqn->result, deps->toString ());
      err++;
      delete deps;
    }
    else {
      // Set folded variable dependencies.
      deps = foldDependencies (deps);
      eqn->setDependencies (deps);
    }
  }
  return err;
}

/* The function returns a variable dependency list with unique entries
   only.  The given string list gets deleted and a new one is created
   and returned. */
strlist * checker::foldDependencies (strlist * deps) {
  strlist * res = new strlist ();
  for (int i = 0; i < deps->length (); i++) {
    char * var = deps->get (i);
    if (!res->contains (var)) res->append (var);
  }
  delete deps;
  return res;
}

// The function appends the given last node to the given equation root.
node * checker::appendEquation (node * root, node * last) {
  last->setNext (NULL);
  if (root != NULL) {
    node * eqn = lastEquation (root);
    eqn->setNext (last);
  }
  else root = last;
  return root;
}

// Returns the last node in the given equation root.
node * checker::lastEquation (node * root) {
  node * eqn;
  for (eqn = root; eqn->getNext () != NULL; eqn = eqn->getNext ());
  return eqn;
}

// Removes the given equation node from the list of known equations.
void checker::dropEquation (node * eqn) {
  if (eqn == equations) {
    equations = eqn->getNext ();
  }
  else {
    node * prev;
    for (prev = equations; prev->getNext () != eqn; prev = prev->getNext());
    prev->setNext (eqn->getNext ());
  }
}

/* This function reorders the list of equations.  The new order can be
   used to evaluate the list step by step.  Each equation being
   evaluable is properly marked, remaining equations are appended. */
void checker::reorderEquations (void) {
  node * root = NULL, * next, * last;

  // Go through the list of equations.
  for (node * eqn = equations; eqn != NULL; eqn = next) {
    strlist * deps = eqn->getDependencies ();
    int i, found;
    next = eqn->getNext ();
    /* Check whether the variable dependencies can be found in
       previous equations. */
    for (found = i = 0; i < deps->length (); i++) {
      char * var = deps->get (i);
      if (findEquation (root, var) != NULL) found++;
    }
    // Yes.
    if (found == deps->length ()) {
      /* Remove the equation from the current list and append it to
         the new list. */
      dropEquation (eqn);
      root = appendEquation (root, eqn);
      eqn->evalPossible = 1;
      // Now start over from the beginning.
      next = equations;
    }
  }
  // Any remaining equations get appended.
  last = lastEquation (root);
  last->setNext (equations);
  equations = root;
}
