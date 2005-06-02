/*
 * parasweep.cpp - parameter sweep class implementation
 *
 * Copyright (C) 2004, 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: parasweep.cpp,v 1.11 2005-06-02 18:17:51 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "vector.h"
#include "dataset.h"
#include "net.h"
#include "ptrlist.h"
#include "analysis.h"
#include "variable.h"
#include "environment.h"
#include "sweep.h"
#include "parasweep.h"

// Constructor creates an unnamed instance of the parasweep class.
parasweep::parasweep () : analysis () {
  var = NULL;
  swp = NULL;
  type = ANALYSIS_SWEEP;
}

// Constructor creates a named instance of the parasweep class.
parasweep::parasweep (char * n) : analysis (n) {
  var = NULL;
  swp = NULL;
  type = ANALYSIS_SWEEP;
}

// Destructor deletes the parasweep class object.
parasweep::~parasweep () {
  if (swp) delete swp;
}

/* The copy constructor creates a new instance of the parasweep class
   based on the given parasweep object. */
parasweep::parasweep (parasweep & p) : analysis (p) {
  var = new variable (*p.var);
  if (p.swp) swp = new sweep (*p.swp);
}

// Short macro in order to obtain the correct constant value.
#define D(con) ((constant *) (con))->d

/* This is the parameter sweep solver. */
void parasweep::solve (void) {
  char * n;
  constant * val;

  runs++;

  // get fixed simulation properties
  n = getPropertyString ("Param");

  // create sweep if necessary
  if (swp == NULL) {
    swp = createSweep (n);
  }

  // get parameter name and the appropriate variable from the current
  // environment, possibly add the variable to the environment if it
  // does not exist yet (which is somehow useless at all)
  if ((var = env->getVariable (n)) == NULL) {
    var = new variable (n);
    val = new constant (TAG_DOUBLE);
    var->setConstant (val);
    env->addVariable (var);
  }
  else val = var->getConstant ();

  // run the parameter sweep
  swp->reset ();
  for (int i = 0; i < swp->getSize (); i++) {
    D (val) = swp->next ();
    if (runs == 1) saveResults ();
#if DEBUG
    logprint (LOG_STATUS, "NOTIFY: %s: running netlist for %s = %g\n",
	      getName (), n, D (val));
#endif
    for (int k = 0; k < actions->length (); k++) {
      analysis * a = actions->get (k);
      a->solve ();
      // assign variable dataset dependencies to last order analyses
      analysis * last = subnet->findLastOrder (a);
      for (; last != NULL; last = (analysis *) last->getNext ()) {
	data->assignDependency (last->getName (), var->getName ());
      }
    }
  }
}

/* This function saves the results of a single solve() functionality
   into the output dataset. */
void parasweep::saveResults (void) {
  vector * v;

  // add current frequency to the dependencies of the output dataset
  if ((v = data->findDependency (var->getName ())) == NULL) {
    v = new vector (var->getName ());
    v->setOrigin (getName ());
    data->addDependency (v);
  }
  v->add (D (var->getConstant ()));
}
