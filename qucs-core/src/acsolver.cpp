/*
 * acsolver.cpp - AC solver class implementation
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
 * $Id: acsolver.cpp,v 1.5 2004/10/04 17:17:44 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>

#include "object.h"
#include "complex.h"
#include "circuit.h"
#include "sweep.h"
#include "net.h"
#include "analysis.h"
#include "nasolver.h"
#include "acsolver.h"

// Constructor creates an unnamed instance of the acsolver class.
acsolver::acsolver () : nasolver<complex> () {
  swp = NULL;
  type = ANALYSIS_AC;
  setDescription ("AC");
}

// Constructor creates a named instance of the acsolver class.
acsolver::acsolver (char * n) : nasolver<complex> (n) {
  swp = NULL;
  type = ANALYSIS_AC;
  setDescription ("AC");
}

// Destructor deletes the acsolver class object.
acsolver::~acsolver () {
  if (swp) delete swp;
}

/* The copy constructor creates a new instance of the acsolver class
   based on the given acsolver object. */
acsolver::acsolver (acsolver & o) : nasolver<complex> (o) {
  swp = o.swp ? new sweep (*o.swp) : NULL;
}

/* This is the AC netlist solver.  It prepares the circuit list for
   each requested frequency and solves it then. */
void acsolver::solve (void) {
  runs++;

  // create frequency sweep if necessary
  if (swp == NULL) {
    char * type = getPropertyString ("Type");
    nr_double_t start = getPropertyDouble ("Start");
    nr_double_t stop = getPropertyDouble ("Stop");
    int points = getPropertyInteger ("Points");

    if (!strcmp (type, "lin")) {
      swp = new linsweep ("acfrequency");
      ((linsweep *) swp)->create (start, stop, points);
    }
    else if (!strcmp (type, "log")) {
      swp = new logsweep ("acfrequency");
      ((logsweep *) swp)->create (start, stop, points);
    }
  }

  // initialize node voltages, first guess for non-linear circuits and
  // generate extra circuits if necessary
  init ();
  setCalculation ((calculate_func_t) &calc);
  solve_pre ();

  swp->reset ();
  for (int i = 0; i < swp->getSize (); i++) {
    freq = swp->next ();
    logprogressbar (i, swp->getSize (), 40);

#if DEBUG && 0
    logprint (LOG_STATUS, "NOTIFY: %s: solving netlist for f = %e\n",
	      getName (), (double) freq);
#endif
    
    // start the linear solver
    solve_linear ();

    // save results
    saveAllResults (freq);
  }
  solve_post ();
  logprogressclear (40);
}

/* Goes through the list of circuit objects and runs its calcAC()
   function. */
void acsolver::calc (acsolver * self) {
  circuit * root = self->getNet()->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    c->calcAC (self->freq);
  }
}

/* Goes through the list of circuit objects and runs its initAC()
   function. */
void acsolver::init (void) {
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    if (c->isNonLinear ()) c->calcOperatingPoints ();
    c->initAC ();
  }
}

/* This function saves the results of a single solve() functionality
   (for the given frequency) into the output dataset. */
void acsolver::saveAllResults (nr_double_t freq) {
  vector * f;
  // add current frequency to the dependency of the output dataset
  if ((f = data->findDependency ("acfrequency")) == NULL) {
    f = new vector ("acfrequency");
    data->addDependency (f);
  }
  if (runs == 1) f->add (freq);
  saveResults ("v", "i", 0, f);
}
