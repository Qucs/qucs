/*
 * acsolver.cpp - AC solver class implementation
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: acsolver.cpp,v 1.7 2005-01-24 19:36:59 raimi Exp $
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
  vn = NULL;
  noise = 0;
}

// Constructor creates a named instance of the acsolver class.
acsolver::acsolver (char * n) : nasolver<complex> (n) {
  swp = NULL;
  type = ANALYSIS_AC;
  setDescription ("AC");
  vn = NULL;
  noise = 0;
}

// Destructor deletes the acsolver class object.
acsolver::~acsolver () {
  if (swp) delete swp;
  if (vn) delete vn;
}

/* The copy constructor creates a new instance of the acsolver class
   based on the given acsolver object. */
acsolver::acsolver (acsolver & o) : nasolver<complex> (o) {
  swp = o.swp ? new sweep (*(o.swp)) : NULL;
  vn = o.vn ? new tvector<complex> (*(o.vn)) : NULL;
  noise = o.noise;
}

/* This is the AC netlist solver.  It prepares the circuit list for
   each requested frequency and solves it then. */
void acsolver::solve (void) {
  runs++;

  // run additional noise analysis ?
  noise = !strcmp (getPropertyString ("Noise"), "yes") ? 1 : 0;

  // create frequency sweep if necessary
  if (swp == NULL) {
    swp = createSweep ("acfrequency");
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

    // compute noise if requested
    if (noise) solve_noise ();

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
    if (self->noise) c->calcNoiseAC (self->freq);
  }
}

/* Goes through the list of circuit objects and runs its initAC()
   function. */
void acsolver::init (void) {
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    if (c->isNonLinear ()) c->calcOperatingPoints ();
    c->initAC ();
    if (noise) c->initNoise ();
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

  // additionally save noise results if requested
  if (noise) {
    saveNoiseResults (f);
  }
}

/* The function computes the final noise results and puts them into
   the dataset. */
void acsolver::saveNoiseResults (vector * f) {
  for (int r = 1; r <= countNodes (); r++) {
    x->set (r, vn->get (r) * sqrt (kB * T0));
  }
  saveResults ("vn", NULL, 0, f);
}

/* This function runs the AC noise analysis.  It saves its results in
   the 'vn' vector. */
void acsolver::solve_noise (void) {
  int N = countNodes ();

  // create the Cy matrix
  createNoiseMatrix ();
  // create noise result vector if necessary
  if (vn == NULL) vn = new tvector<complex> (N);

  // temporary result vector for transimpedances
  tvector<complex> vz = tvector<complex> (N);

  // transpose the MNA matrix and ensure skipping LU decomposition
  A->transpose ();
  updateMatrix = 0;
  convHelper = CONV_None;

  // compute noise voltage for each node
  for (int i = 1; i <= N; i++) {
    z->set (0); z->set (i, -1); // modify right hand side appropriately
    runMNA ();                  // solve
    vz.set (*x, 1, N);          // save transimpedance vector

    // compute actual noise voltage
    vn->set (i, sqrt (scalar (vz * (*C), conj (vz))));
  }
}
