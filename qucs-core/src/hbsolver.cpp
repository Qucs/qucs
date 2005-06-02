/*
 * hbsolver.cpp - harmonic balance solver class implementation
 *
 * Copyright (C) 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: hbsolver.cpp,v 1.2 2005/06/02 18:17:50 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>

#include "object.h"
#include "complex.h"
#include "circuit.h"
#include "component_id.h"
#include "net.h"
#include "strlist.h"
#include "analysis.h"
#include "nasolver.h"
#include "hbsolver.h"

// Constructor creates an unnamed instance of the hbsolver class.
hbsolver::hbsolver () : nasolver<complex> () {
  type = ANALYSIS_HBALANCE;
  frequency = 0;
  frequencies = NULL;
  nlnodes = NULL;
  setDescription ("HB");
}

// Constructor creates a named instance of the hbsolver class.
hbsolver::hbsolver (char * n) : nasolver<complex> (n) {
  type = ANALYSIS_HBALANCE;
  frequency = 0;
  frequencies = NULL;
  nlnodes = NULL;
  setDescription ("HB");
}

// Destructor deletes the hbsolver class object.
hbsolver::~hbsolver () {
  if (frequencies) delete frequencies;
  if (nlnodes) delete nlnodes;
}

/* The copy constructor creates a new instance of the hbsolver class
   based on the given hbsolver object. */
hbsolver::hbsolver (hbsolver & o) : nasolver<complex> (o) {
  frequency = o.frequency;
  frequencies = o.frequencies;
  nlnodes = o.nlnodes;
}

/* This is the HB netlist solver.  It prepares the circuit list and
   solves it then. */
void hbsolver::solve (void) {

  // create frequency array
  collectHarmonics ();

  // find interconnects between the linear and non-linear subcircuit
  getNonLinearNodes ();

  do {
    // create constant transadmittance matrix

    // create variable transadmittance matrix

    /* transadmittance matrices are obtained by
       1. create MNA matrix of linear subcircuit
       2. derive transimpedance matrix entries by unity current excitation
       3. invert the transimpedance matrix -> transadmittance matrix
       4. fill in matrix entries into constant and variable transadmittance
          matrix
    */

    for (int i = 0; i < frequencies->getSize (); i++) {
      frequency = real (frequencies->get (i));
#if DEBUG
      logprint (LOG_STATUS, "NOTIFY: %s: solving linear netlist for f = %e\n",
		getName (), (double) frequency);
#endif
      solveLinear ();
    }

    // inverse FFT of frequency domain voltage vector

    // compute non-linear charges and currents

    // FFT of time domain currents
  }
  // check termination criteria (balanced frequency domain currents)
  while (!checkBalance ());

  // apply AC analysis to the complete network in order to obtain the
  // final results
}

/* Goes through the list of circuit objects and runs its calcHB()
   function. */
void hbsolver::calc (hbsolver * self) {
  circuit * root = self->getNet()->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    c->calcHB (self->frequency);
  }
}

/* Goes through the list of circuit objects and runs its initHB()
   function. */
void hbsolver::initHB (void) {
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    c->initHB ();
  }
}

/* Goes through the list of circuit objects and runs its initDC()
   function. */
void hbsolver::initDC (void) {
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    c->initDC ();
  }
}

/* The function computes the harmonic frequencies excited in the
   circuit list depending on the maximum number of harmonics per
   exitation and saves its results into the 'frequencies' vector.
   Duplicate excitations are dropped. */
void hbsolver::collectHarmonics (void) {
  vector f;
  circuit * root = subnet->getRoot ();
  int n = getPropertyInteger ("n");

  // go through the circuits and get sinusoidal excitations
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    int t = c->getType ();
    if (t == CIR_PAC || t == CIR_VAC || t == CIR_IAC) {
      nr_double_t freq = c->getPropertyDouble ("f");
      if (freq > 0 && !f.contains (freq)) f.add (freq);
    }
  }
  f.add (rect (0, 0));

  // now compute the harmonics of those excitations and sort them
  frequencies = new vector ("hbfrequency");
  frequencies->add (rect (0, 0));
  for (int r = 0; r < f.getSize (); r++) {
    nr_double_t f1 = real (f.get (r));
    for (int c = 0; c < f.getSize (); c++) {
      nr_double_t f2 = real (f.get (c));
      nr_double_t f3;
      if (c == r) continue;
      for (int i = 0; i <= n + 1; i++) {
	for (int l = 0; l <= n + 1; l++) {
	  f3 = l * f1 + i * f2;
	  if (f3 > 0 && !frequencies->contains (f3)) {
	    frequencies->add (f3);
	  }
	  f3 = l * f1 - i * f2;
	  if (f3 > 0 && !frequencies->contains (f3)) {
	    frequencies->add (f3);
	  }
	}
      }
    }
  }
  frequencies->sort ();

#if DEBUG
  fprintf (stderr, "%d frequencies: [ ", frequencies->getSize ());
  for (int i = 0; i < frequencies->getSize (); i++) {
    fprintf (stderr, "%g ", real (frequencies->get (i)));
  }
  fprintf (stderr, "]\n");
#endif /* DEBUG */
}

/* This function goes through the list of circuits and collects the
   nodes connected to a non-linear component. */
void hbsolver::getNonLinearNodes (void) {
  nlnodes = new strlist ();
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    if (c->isNonLinear ()) {
      for (int i = 1; i <= c->getSize (); i++) {
	char * n = c->getNode(i)->getName ();
	if (strcmp (n, "gnd")) {
	  if (!nlnodes->contains (n)) nlnodes->add (n);
	}
      }
    }
  }
#if DEBUG
  fprintf (stderr, "nonlinear nodes: [ %s ]\n", nlnodes->toString ());
#endif
}

/* The function creates the linear subcircuit matrices in the
   frequency domain. */
void hbsolver::solveLinear (void) {
}

/* Checks whether currents through the interconnects of the linear and
   non-linear subcircuit (in the frequency domain) are equal. */
int hbsolver::checkBalance (void) {
  return 1;
}
