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
 * $Id: equation.cpp,v 1.23 2004-08-21 13:29:07 ela Exp $
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
#include "matvec.h"
#include "dataset.h"
#include "strlist.h"
#include "equation.h"
#include "constants.h"

using namespace eqn;

/* The global list of equations and expression lists. */
node * eqn::equations = NULL;
node * eqn::expressions = NULL;

#define A(a) ((assignment *) (a))
#define N(n) ((node *) (n))
#define C(c) ((constant *) (c))
#define R(r) ((reference *) (r))

// Constructor creates an untyped instance of the constant class.
constant::constant () : node (CONSTANT) {
  type = TAG_UNKNOWN;
  setType (type);
}

// This constructor creates an typed instance of the constant class.
constant::constant (int tag) : node (CONSTANT) {
  type = tag;
  setType (type);
}

// Destructor deletes an instance of the constant class.
constant::~constant () {
}

/* Depending on the type of constant the function prints the textual
   representation of the object. */
void constant::print (void) {
  logprint (LOG_STATUS, "%s", toString ());
}

// Returns the string representation of a complex value.
static char * Cplx2String (complex c) {
  static char str[256]; // enough for a real or complex number
  if (imag (c) == 0.0) {
    sprintf (str, "%g", (double) real (c));
  }
  else {
    sprintf (str, "(%g%cj%g)", (double ) real (c), 
	     imag (c) >= 0.0 ? '+' : '-', (double) fabs (imag (c)));
  }
  return str;
}

/* This function returns a string representation depending on the type
   of constant. */
char * constant::toString (void) {
  char str[256];
  if (txt != NULL) free (txt);
  switch (type) {
  case TAG_DOUBLE:
    sprintf (str, "%g", (double) d);
    txt = strdup (str);
    break;
  case TAG_COMPLEX:
    txt = strdup (Cplx2String (*c));
    break;
  case TAG_VECTOR:
    {
      int len = 3 + v->getSize () - 1;
      txt = (char *) malloc (len);
      strcpy (txt, "[");
      for (int i = 0; i < v->getSize (); i++) {
	char * s = Cplx2String (v->get (i));
	txt = (char *) realloc (txt, len += strlen (s));
	strcat (txt, s);
	if (i != v->getSize () - 1) strcat (txt, ";");
      }
      strcat (txt, "]");
    }
    break;
  case TAG_MATRIX:
    {
      int len = 3 + (m->getRows () - 1) * (m->getCols () - 1);
      txt = (char *) malloc (len);
      strcpy (txt, "[");
      for (int r = 1; r <= m->getRows (); r++) {
	for (int c = 1; c <= m->getCols (); c++) {
	  char * s = Cplx2String (m->get (r, c));
	  txt = (char *) realloc (txt, len += strlen (s));
	  strcat (txt, s);
	  if (c != m->getCols ()) strcat (txt, " ");
	}
	if (r != m->getRows ()) strcat (txt, ";");
      }
      strcat (txt, "]");
    }
    break;
  case TAG_MATVEC:
    sprintf (str, "[%dx%d](%d)",
	     mv->getRows (), mv->getCols (), mv->getSize ());
    txt = strdup (str);
    break;
  default:
    txt = strdup ("(no such type)");
    break;
  }
  return txt;
}

// Returns the type of constant.
int constant::evalType (void) {
  return getType ();
}

// Returns the result stored in the constant.
constant * constant::evaluate (void) {
  setResult (this);
  return getResult ();
}

// Constructor creates an instance of the reference class.
reference::reference () : node (REFERENCE) {
  n = NULL;
  ref = NULL;
}

// Destructor deletes an instance of the reference class.
reference::~reference () {
  if (n) free (n);
}

// Prints textual representation of the reference object.
void reference::print (void) {
  logprint (LOG_STATUS, "%s", toString ());
}

// Returns textual representation of the reference object.
char * reference::toString (void) {
  if (txt) free (txt);
  txt = strdup (n);
  return txt;
}

// Adds the name of the reference to the list of dependencies.
void reference::addDependencies (strlist * depends) {
  depends->add (n);
  // Additionally find and save the actual equation reference.
  for (node * eqn = eqn::equations; eqn != NULL; eqn = eqn->getNext ()) {
    if (!strcmp (n, A(eqn)->result)) {
      ref = eqn;
      break;
    }
  }
}

// Returns the type of reference.
int reference::evalType (void) {
  setType (TAG_UNKNOWN);
  if (ref != NULL) {
    setType (A(ref)->body->evalType ());
  }
  return getType ();
}

// Returns the actual result of the reference.
constant * reference::evaluate (void) {
  setResult (NULL);
  if (ref != NULL) {
    setResult (A(ref)->body->getResult ());
  }
  return getResult ();
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
  logprint (LOG_STATUS, "%s", toString ());
}

// Returns textual representation of the assignment object.
char * assignment::toString (void) {
  if (txt) free (txt);
  char * str = body->toString ();
  txt = (char *) malloc (strlen (result) + strlen (str) + 4);
  sprintf (txt, "%s = %s", result, str);
  return txt;
}

// Adds the right hand side of the assignment to the list of dependencies.
void assignment::addDependencies (strlist * depends) {
  body->addDependencies (depends);
}

// Returns the type of assignment.
int assignment::evalType (void) {
  setType (body->evalType ());
  return getType ();
}

// Returns the result of the assignment.
constant * assignment::evaluate (void) {
  setResult (body->evaluate ());
  return getResult ();
}

// Constructor creates an instance of the application class.
application::application () : node (APPLICATION) {
  n = NULL;
  nargs = 0;
  args = NULL;
  eval = NULL;
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
  logprint (LOG_STATUS, "%s", toString ());
}

// Returns textual representation of the application object.
char * application::toString (void) {
  if (txt) free (txt);
  // binary operations
  if ((!strcmp (n, "+") || !strcmp (n, "-") || !strcmp (n, "*") ||
       !strcmp (n, "/") || !strcmp (n, "^") || !strcmp (n, "%"))
      && nargs == 2) {
    char * arg1 = args->toString ();
    char * arg2 = args->getNext()->toString ();
    txt = (char *) malloc (strlen (n) + strlen (arg1) + strlen (arg2) + 3);
    sprintf (txt, "(%s%s%s)", arg1, n, arg2);
  }
  // array indices
  else if (!strcmp (n, "array")) {
    int len = strlen (args->toString ()) + 3 + nargs - 1;
    txt = (char *) malloc (len);
    sprintf (txt, "%s[", args->toString ());
    for (node * arg = args->getNext (); arg != NULL; arg = arg->getNext ()) {
      char * str = arg->toString ();
      txt = (char *) realloc (txt, len += strlen (str));
      strcat (txt, str);
      if (arg->getNext ()) strcat (txt, ",");
    }
    strcat (txt, "]");
  }
  // unary and n-ary operations here
  else {
    int len = strlen (n) + 3 + nargs - 1;
    txt = (char *) malloc (len);
    sprintf (txt, "%s(", n);
    for (node * arg = args; arg != NULL; arg = arg->getNext ()) {
      char * str = arg->toString ();
      txt = (char *) realloc (txt, len += strlen (str));
      strcat (txt, str);
      if (arg->getNext ()) strcat (txt, ",");
    }
    strcat (txt, ")");
  }
  return txt;
}

// Adds the arguments of the application to the list of dependencies.
void application::addDependencies (strlist * depends) {
  for (node * arg = args; arg != NULL; arg = arg->getNext ()) {
    arg->addDependencies (depends);
  }  
}

/* Returns the type of application and applies the appropriate
   evaluation function if any. */
int application::evalType (void) {
  setType (TAG_UNKNOWN);
  // Go through the list of available applications.
  for (int i = 0; applications[i].application != NULL; i++) {
    application_t * app = &applications[i];
    // The correct application?
    if (!strcmp (n, app->application)) {
      int nr = 0;
      // The correct number of arguments?
      if (nargs != app->nargs) continue;
      // The correct types of arguments?
      for (node * arg = args; arg != NULL; arg = arg->getNext (), nr++) {
	if (arg->evalType () != app->args[nr]) { nr = -1; break; }
      }
      if (nr == -1) continue;
      // A valid application function?
      if (app->eval == NULL) continue;
      // Everything just fine here.
      eval = app->eval;
      setType (app->retval);
      break;
    }
  }
  // Emit error message if necessary.
  if (getType () == TAG_UNKNOWN) {
    logprint (LOG_ERROR, "checker error, no appropriate function for `%s'"
	      " found\n", toString ());
  }
  return getType ();
}

/* This function runs the actual evaluation function and the returns
   the result. */
constant * application::evaluate (void) {
  // first evaluate each argument
  for (node * arg = args; arg != NULL; arg = arg->getNext ()) {
    if (arg->evaluated == 0) {
      arg->evaluate ();
      arg->evaluated++;
    }
  }
  // then evaluate the application
  setResult (eval (C (args)));
  // check the returned type once again
  if (getResult()->getType () != getType ()) {
    logprint (LOG_ERROR, "evaluate error, function `%s' returned invalid "
	      "constant type\n", toString ());
  }
  return getResult ();
}

// Constructor creates an untyped instance of the equation node class.
node::node () {
  tag = UNKNOWN;
  output = evaluated = evalPossible = cycle = duplicate = 0;
  next = NULL;
  dependencies = NULL;
  dataDependencies = NULL;
  txt = NULL;
  res = NULL;
  instance = NULL;
}

// This constructor creates an typed instance of the equation node class.
node::node (int type) {
  tag = type;
  output = evaluated = evalPossible = cycle = duplicate = 0;
  next = NULL;
  dependencies = NULL;
  dataDependencies = NULL;
  txt = NULL;
  res = NULL;
  instance = NULL;
}

// Destructor deletes an instance of the equation node class.
node::~node () {
  if (dependencies) delete dependencies;
  if (txt) free (txt);
  if (instance) free (instance);
}

// Sets the instance name where the node occurred.
void node::setInstance (char * n) {
  if (instance) free (instance);
  instance = n ? strdup (n) : NULL;
}

// Returns the instance name where the node occurred.
char * node::getInstance (void) {
  return instance;
}

/* The function applies the instance name of the current equation node
   to any following node within the list up to the node with a valid
   instance name. */
void node::applyInstance (void) {
  char * i = getInstance ();
  for (node * n = getNext (); n != NULL; n = n->getNext ()) {
    if (n->getInstance () == NULL)
      n->setInstance (i);
    else
      break;
  }
}

// Counts the number of equations node attached to the node.
int node::count (void) {
  int c = 0;
  for (node * n = this; n != NULL; n = n->getNext ()) c++;
  return c;
}

// Appends yet another node to the equation node object.
void node::append (node * last) {
  node * n;
  for (n = this; n->getNext () != NULL; n = n->getNext ());
  last->setNext (NULL);
  n->setNext (last);
}

// Returns the equation node at the given argument position.
node * node::get (int pos) {
  node * n = this;
  for (int i = 0; i < pos && n != NULL; n = n->getNext (), i++);
  return n;
}

// Returns the constant equation node at the given argument position.
constant * node::getResult (int pos) {
  node * n = this;
  for (int i = 0; i < pos && n != NULL; n = n->getNext (), i++);
  return n ? n->getResult () : NULL;
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

// Local macro definition to go through the list of equations.
#define foreach_equation(eqn)                        \
  for (assignment * (eqn) = A (equations);           \
       (eqn) != NULL; (eqn) = A ((eqn)->getNext ()))

/* The function goes through the list of equations assigned to the
   checker and applies the dependency list. */
void checker::collectDependencies (void) {
  foreach_equation (eqn) {
    strlist * depends = new strlist ();
    eqn->addDependencies (depends);
    eqn->setDependencies (depends);
  }
}

/* The following function goes through the list of equations and
   checks whether there is any kind of 'Export="yes|no"' assignment in
   it.  Depending on the value the referred equation results are saved
   into the dataset or not. */
int checker::checkExport (void) {
  int errors = 0;
  assignment * next;
  // go through all equations
  for (assignment * eqn = A (equations); eqn != NULL; eqn = next) {
    next = A (eqn->getNext ());
    // 'Export' equation found ?
    if (!strcmp (eqn->result, "Export")) {
      // is the type and value correct ?
      if (eqn->body->getTag () != REFERENCE ||
	  (strcmp (R (eqn->body)->n, "yes") &&
	   strcmp (R (eqn->body)->n, "no"))) {
	logprint (LOG_ERROR, "checker error, variable `%s' alternatives "
		  "are `yes' or `no'\n", eqn->result);
	errors++;
      }
      else {
	int flag = !strcmp (R (eqn->body)->n, "yes") ? 1 : 0;
	char * i = eqn->getInstance ();
	int found = 0;
	// set output flag for each equation with the same instance name
	foreach_equation (res) {
	  if (!strcmp (res->getInstance (), i))
	    res->output = flag;
	  if (!strcmp (res->result, "Export") &&
	      !strcmp (res->getInstance (), i)) {
	    found++;
	  }
	}
	// check for duplicate definitions of 'Export'
	if (found > 1) {
	  logprint (LOG_ERROR, "checker error, variable `%s' "
		    "occurred %dx in `Eqn:%s'\n", eqn->result, found, i);
	  errors++;
	}
	// drop the 'Export' equation being useless now
	dropEquation (eqn);
	delete eqn;
      }
    }
  }
  eqn::equations = equations;
  return errors;
}

// Logs the textual representation of all equations.
void checker::list (void) {
  for (node * eqn = equations; eqn != NULL; eqn = eqn->getNext ()) {
    logprint (LOG_STATUS, "%s", eqn->evalPossible ? "!" : "?");
    logprint (LOG_STATUS, "%s", eqn->evalPossible ? 
	      (eqn->getType () == TAG_UNKNOWN ? "U!" :
	       eqn->getType () == TAG_DOUBLE  ? "D!" :
	       eqn->getType () == TAG_COMPLEX ? "C!" :
	       eqn->getType () == TAG_VECTOR  ? "V!" :
	       eqn->getType () == TAG_MATVEC  ? "MV!" :
	       eqn->getType () == TAG_MATRIX  ? "M!" : "?!") : "");
    eqn->print ();
    logprint (LOG_STATUS, "\n");
  }
}

/* This function checks whether the variable references could be
   resolved within the equations and returns zero if so. */
int checker::findUndefined (int noundefined) {
  int err = 0;
  strlist * idents = getVariables ();

  foreach_equation (eqn) {
    strlist * depends = eqn->getDependencies ();
    for (int i = 0; i < depends->length (); i++) {
      char * var = depends->get (i);
      if (idents->contains (var) <= 0) {
	if (noundefined) {
	  logprint (LOG_ERROR, "checker error, undefined variable `%s' in "
		    "equation `%s'\n", var, eqn->result);
	  err++;
	}
	else {
	  logprint (LOG_STATUS, "checker notice, variable `%s' in "
		    "equation `%s' not yet defined\n", var, eqn->result);
	}
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

  // Collect duplicate entries.
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
      logprint (LOG_ERROR, "checker error, variable `%s' assigned %dx\n",
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
  for (int i = 0; deps && i < deps->length (); i++) {
    char * var = deps->get (i);
    if (!res->contains (var)) res->append (var);
  }
  if (deps) delete deps;
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
  for (eqn = root; eqn && eqn->getNext () != NULL; eqn = eqn->getNext ());
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
  if (root != NULL) {
    last = lastEquation (root);
    last->setNext (equations);
    equations = root;
  }
}

/* The function evaluates the types for each equation and recursively
   checks the availability of the appropriate function. */
int checker::applyTypes (void) {
  int err = 0;
  foreach_equation (eqn) {
    if (eqn->evalPossible) {
      if (eqn->evalType () == TAG_UNKNOWN)
	err++;
    }
    else break;
  }
  return err;
}

/* This function is the checker routine for a parsed equations.  It
   returns zero on success or non-zero if the parsed equations
   contained errors. */
int equation_checker (int noundefined) {
  int err = 0;
  eqn::checker * check = new eqn::checker ();
  check->setEquations (eqn::equations);
  err += check->checkExport ();
  check->collectDependencies ();
  err += check->findUndefined (noundefined);
  err += check->findDuplicate ();
  err += check->detectCycles ();
  check->reorderEquations ();
  err += check->applyTypes ();
#if DEBUG && 0
  check->list ();
#endif /* DEBUG */
  eqn::equations = check->getEquations ();
  check->setEquations (NULL);
  delete check;
  return err;
}

// Constructor creates an instance of the solver class.
solver::solver () {
  equations = NULL;
  data = NULL;
}

// Destructor deletes an instance of the solver class.
solver::~solver () {
  node * next;
  for (node * eqn = equations; eqn != NULL; eqn = next) {
    next = eqn->getNext ();
    delete eqn;
  }
}

// The function finally evaluates each equation passed to the solver.
void solver::solve (void) {
  foreach_equation (eqn) {
    if (eqn->evalPossible && eqn->evaluated == 0) {
      eqn->evaluate ();
      eqn->evaluated++;
#if DEBUG
      logprint (LOG_STATUS, "%s = %s\n", A(eqn)->result, 
		eqn->getResult()->toString ());
#endif
    }
  }
}

/* This function adds the given dataset vector to the set of equations
   stored in the equation solver. */
node * solver::addEquationData (vector * v) {
  constant * con = new constant (TAG_VECTOR);
  con->v = v;
  assignment * assign = new assignment ();
  assign->result = strdup (v->getName ());
  assign->body = con;
  assign->setNext (equations);
  equations = assign;
  return assign;
}

/* Depending on the type of equation result the function converts the
   given equation node to one or more valid dataset vector(s). */
vector * solver::dataVector (node * eqn) {
  vector * v = NULL;
  switch (eqn->getType ()) {
  case TAG_VECTOR: // simple vector
    v = new vector (* (eqn->getResult()->v));
    v->setNext (NULL); v->setPrev (NULL);
    break;
  case TAG_DOUBLE: // double value
    v = new vector ();
    v->add (eqn->getResult()->d);
    break;
  case TAG_COMPLEX: // complex value
    v = new vector ();
    v->add (* (eqn->getResult()->c));
    break;
  case TAG_MATVEC: // matrix vector
    {
      // convert matrix vector to a list of vectors
      matvec * mv = eqn->getResult()->mv;
      mv->setName (A(eqn)->result);
      for (int r = 1; r <= mv->getRows (); r++) {
	for (int c = 1; c <= mv->getCols (); c++) {
	  // name gets automatically assigned
	  vector * t = new vector (mv->get (r, c));
	  // chain the vectors appropriately
	  t->setNext (v); v = t;
	}
      }
    }
    return v;
  case TAG_MATRIX: // single matrix
    {
      // convert matrix to a list of vectors
      matrix * m = eqn->getResult()->m;
      for (int r = 1; r <= m->getRows (); r++) {
	for (int c = 1; c <= m->getCols (); c++) {
	  vector * t = new vector ();
	  t->setName (matvec::createMatrixString (A(eqn)->result, r, c));
	  t->add (m->get (r, c));
	  // chain the vectors appropriately
	  t->setNext (v); v = t;
	}
      }
    }
    return v;
  default:
    return NULL;
  }
  v->setName (A(eqn)->result);
  return v;
}

/* This function collects the data vectors in a dataset and appends
   these to the list of equation node inside the equation solver. */
void solver::checkinDataset (void) {
  if (data == NULL) return;
  vector * v;
  findMatrixVectors (data->getDependencies ());
  findMatrixVectors (data->getVariables ());
  for (v = data->getDependencies (); v != NULL; v = (vector *) v->getNext ()) {
    if (v->getRequested () != -1) {
      node * eqn = addEquationData (v);
      strlist * deps = new strlist ();
      deps->add (v->getName ());
      eqn->setDataDependencies (deps);
    }
  }
  for (v = data->getVariables (); v != NULL; v = (vector *) v->getNext ()) {
    if (v->getRequested () != -1) {
      node * eqn = addEquationData (v);
      eqn->setDataDependencies (v->getDependencies ());
    }
  }
}

/* This function searches through the dataset of the equation solver
   for possible matrix vectors.  These are detected by the vectors'
   names (e.g. S[1,1]).  The matrix vectors found in the dataset get
   converted and saved into the set of equations.  */
void solver::findMatrixVectors (vector * v) {
  vector * vec;
  strlist * deps;
  char * p, * cand;
  int s, r, c, a, b, n = 1;

  // initialize the  'found' flag
  for (vec = v; vec != NULL; vec = (vector *) vec->getNext ())
    vec->setRequested (0);

  // loop through the dataset vector until no more matrix vector is found
  do {
    r = c = s = 0; cand = NULL; deps = NULL;
    // go through the dataset
    for (vec = v; vec != NULL; vec = (vector *) vec->getNext ()) {
      // skip detected vectors
      if (vec->getRequested ()) continue;
      // is the vector a possible matrix vector element ?
      if ((p = matvec::isMatrixVector (vec->getName (), a, b)) != NULL) {
	if (cand != NULL) {
	  // does this vectors name equals the current one ?
	  if (!strcmp (p, cand) && s == vec->getSize ()) {
	    // save largest row and column index and set the 'found' flag
	    if (a > r) r = a;
	    if (b > c) c = b;
	    vec->setRequested (n);
	  }
	}
	else {
	  /* new possible matrix vector:
	     save its name, row and column index, its size (length of
	     the vector) and data dependencies; then set the 'found' flag */
	  cand = strdup (p);
	  r = a;
	  c = b;
	  s = vec->getSize ();
	  vec->setRequested (n);
	  deps = vec->getDependencies ();
	}
	free (p);
      }
    }

    // new matrix vector detected
    if (cand != NULL) {
      // create a new matrix vector and set the appropriate name
      matvec * mv = new matvec (s, r, c);
      mv->setName (cand);
      // go through the dataset vector once again
      for (vec = v; vec != NULL; vec = (vector *) vec->getNext ()) {
	// and collect the vectors with the same 'found' flags
	if (vec->getRequested () == n) {
	  p = matvec::isMatrixVector (vec->getName (), a, b);
	  mv->set (*vec, a, b);
	  free (p);
	  vec->setRequested (-1);
	}
      }
      // now store this new matrix vector into the set of equations
      node * eqn = addEquationData (mv);
      if (deps == NULL) {
	strlist * deps = new strlist ();
	deps->add (mv->getName ());
      }
      eqn->setDataDependencies (deps);
      free (cand); cand = NULL;
    }
    // increase the current 'found' flag
    n++;
  } while (cand != NULL);
}

/* The function creates an assignment equation from the given matrix
   vector and returns it.  The new assignment is appended to the list
   of available equations. */
node * solver::addEquationData (matvec * mv) {
  constant * con = new constant (TAG_MATVEC);
  con->mv = mv;
  assignment * assign = new assignment ();
  assign->result = strdup (mv->getName ());
  assign->body = con;
  assign->setNext (equations);
  equations = assign;
  return assign;
}

/* This function goes through the given string list and calculates the
   data entries within these dataset dependencies.  It returns at
   least one no matter whether the data vectors can be found or not. */
int solver::dataSize (strlist * deps) {
  int size = 1;
  for (int i = 0; i < deps->length (); i++) {
    char * str = deps->get (i);
    vector * dep = data->findDependency (str);
    vector * var = data->findVariable (str);
    size *= dep ? dep->getSize () : var ? var->getSize () : 1;
  }
  return size;
}

// The function stores the equation solver results back into a dataset.
void solver::checkoutDataset (void) {
  // return if nothing todo
  if (data == NULL) return;
  // go through each equation
  foreach_equation (eqn) {

    // skip variables which don't need to be exported
    if (!eqn->output) continue;

    // is the equation result already in the dataset ?
    if (!findEquationResult (eqn)) {
      vector * v = dataVector (eqn);
      if (v == NULL) continue;

      // collect inherited dataset dependencies
      strlist * sub, * datadeps = NULL;
      strlist * deps = eqn->getDependencies ();
      for (int i = 0; i < deps->length (); i++) {
	char * var = deps->get (i);
	node * n = checker::findEquation (equations, var);
	sub = strlist::join (datadeps, n->getDataDependencies ());
	if (datadeps) delete datadeps;
	datadeps = sub;
      }
      datadeps = checker::foldDependencies (datadeps);

      // check whether dataset is smaller than its dependencies
      if (v->getSize () <= 1 && dataSize (datadeps) > v->getSize ()) {
	delete datadeps;
	datadeps = NULL;
      }
	
      // store variable vector(s)
      if (datadeps && datadeps->length () > 0) {
	v->setDependencies (datadeps);
	if (v->getNext () != NULL) {
	  data->applyDependencies (v);
	  data->addVariables (v);
	}
	else {
	  data->addVariable (v);
	}
      }
      // store independent vector(s)
      else {
	if (v->getNext () != NULL)
	  data->addDependencies (v);
	else
	  data->addDependency (v);
	delete datadeps;
      }
    }
  }
}

/* This function checks whether the given equation solver result is
   already within the dataset.  It returns non-zero if so, otherwise
   the function returns zero. */
int solver::findEquationResult (node * eqn) {
  // check each vector of a given matrix vector
  if (eqn->getType () == TAG_MATVEC) {
    matvec * mv = eqn->getResult()->mv;
    for (int r = 1; r <= mv->getRows (); r++) {
      for (int c = 1; c <= mv->getCols (); c++) {
	char * str = matvec::createMatrixString (A(eqn)->result, r, c);
	if (data->findDependency (str) || data->findVariable (str))
	  return 1;
      }
    }
  }
  // check normal data vectors
  else {
    char * str = A(eqn)->result;
    if (data->findDependency (str) || data->findVariable (str))
      return 1;
  }
  return 0;
}

/* This function is called in order to run the equation checker and
   the solver.  The optional dataset passed to the function receives
   the results of the calculations. */
int equation_solver (dataset * data) {
  solver * solve = new solver ();
  solve->setEquations (eqn::equations);
  solve->setData (data);
  solve->checkinDataset ();
  eqn::equations = solve->getEquations ();
  if (equation_checker (data ? 1 : 0) != 0)
    return -1;
  solve->setEquations (eqn::equations);
  solve->solve ();
  solve->checkoutDataset ();
  eqn::equations = solve->getEquations ();
  solve->setEquations (NULL);
  delete solve;
  return 0;
}

/* Go through the list of equations and store the left hand side in
   a string list. */
strlist * equation_variables (void) {
  strlist * idents = new strlist ();
  foreach_equation (eqn) {
    idents->add (eqn->result);
  }
  return idents;
}

// Structure defining a predefined constant.
struct pconstant {
  char * ident;
  nr_double_t value;
};

// List of global constant variables.
static struct pconstant pconstants[] = {
  { "pi", M_PI },
  { "e",  M_E },
  { "kB", kB },
  { NULL, 0 }
};

/* The function should be called before parsing the netlist.  It
   appends the predefined constants to the list of equations. */
void equation_constants (void) {
  for (int i = 0; pconstants[i].ident != NULL; i++) {
    // create constant double value
    constant * c = new constant (eqn::TAG_DOUBLE);
    c->d = pconstants[i].value;
    // create the appropriate assignment
    assignment * a = new assignment ();
    a->result = strdup (pconstants[i].ident);
    a->body = c;
    a->output = 0;
    a->setInstance ("#predefined");
    // append the assignment to equations
    a->setNext (eqn::equations);
    eqn::equations = a;
  }
}
