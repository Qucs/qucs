/*
 * parasweep.cpp - parameter sweep class implementation
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
 * $Id: parasweep.cpp,v 1.3 2004/04/25 17:08:50 ela Exp $
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
#include "dataset.h"
#include "net.h"
#include "analysis.h"
#include "variable.h"
#include "environment.h"
#include "parasweep.h"

// Constructor creates an unnamed instance of the parasweep class.
parasweep::parasweep () : analysis () {
  runs = 0;
  var = NULL;
  type = ANALYSIS_SWEEP;
}

// Constructor creates a named instance of the parasweep class.
parasweep::parasweep (char * n) : analysis (n) {
  runs = 0;
  var = NULL;
  type = ANALYSIS_SWEEP;
}

// Destructor deletes the parasweep class object.
parasweep::~parasweep () {
}

/* The copy constructor creates a new instance of the parasweep class
   based on the given parasweep object. */
parasweep::parasweep (parasweep & p) : analysis (p) {
  runs = p.runs;
  var = new variable (*p.var);
}

// Short macro in order to obtain the correct constant value.
#define D(con) ((constant *) (con))->d

/* This is the parameter sweep solver. */
void parasweep::solve (void) {
  nr_double_t start, stop, step;
  char * n;
  constant * val;

  // get fixed properties
  start = getPropertyDouble ("Start");
  stop = getPropertyDouble ("Stop");
  step = getPropertyDouble ("Step");

  // get parameter name and the appropriate variable from the current
  // environment, possibly add the variable to the environment if it
  // does not exist yet (which is somehow useless at all)
  n = getPropertyString ("Param");
  if ((var = env->getVariable (n)) == NULL) {
    var = new variable (n);
    val = new constant (TAG_DOUBLE);
    var->setConstant (val);
    env->addVariable (var);
  }
  else val = var->getConstant ();

  // run the parameter sweep
  for (D (val) = start; D (val) <= stop; D (val) += step) {
    if (runs == 0) saveResults ();
    for (analysis * a = actions; a != NULL; a = (analysis *) a->getNext ()) {
      a->solve ();
      analysis * last = subnet->findLastOrder (a);
      data->assignDependency (last->getName (), var->getName ());
    }
  }
  runs++;
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
