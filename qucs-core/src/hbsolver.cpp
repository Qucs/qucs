/*
 * hbsolver.cpp - harmonic balance solver class implementation
 *
 * Copyright (C) 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: hbsolver.cpp,v 1.4 2006-03-22 09:39:19 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>

#include "object.h"
#include "logging.h"
#include "complex.h"
#include "vector.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "net.h"
#include "strlist.h"
#include "ptrlist.h"
#include "analysis.h"
#include "hbsolver.h"

// Constructor creates an unnamed instance of the hbsolver class.
hbsolver::hbsolver () : analysis () {
  type = ANALYSIS_HBALANCE;
  frequency = 0;
  nlnodes = lnnodes = banodes = NULL;
}

// Constructor creates a named instance of the hbsolver class.
hbsolver::hbsolver (char * n) : analysis (n) {
  type = ANALYSIS_HBALANCE;
  frequency = 0;
  nlnodes = lnnodes = banodes = NULL;
}

// Destructor deletes the hbsolver class object.
hbsolver::~hbsolver () {
  if (nlnodes) delete nlnodes;
  if (lnnodes) delete lnnodes;
  if (banodes) delete banodes;
}

/* The copy constructor creates a new instance of the hbsolver class
   based on the given hbsolver object. */
hbsolver::hbsolver (hbsolver & o) : analysis (o) {
  frequency = o.frequency;
  frequencies = o.frequencies;
  nlnodes = o.nlnodes;
  lnnodes = o.lnnodes;
  banodes = o.banodes;
}

/* This is the HB netlist solver.  It prepares the circuit list and
   solves it then. */
void hbsolver::solve (void) {

  // collect different parts of the circuit
  splitCircuits ();

  // create frequency array
  collectFrequencies ();

  // find interconnects between the linear and non-linear subcircuit
  getNodeLists ();

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

    for (int i = 0; i < frequencies.getSize (); i++) {
      frequency = frequencies.get (i);
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

// Returns true if circuit is a HB source.
bool hbsolver::isExcitation (circuit * c) {
  int type = c->getType ();
  if (type == CIR_PAC || type == CIR_VAC || type == CIR_IAC)
    return true;
  return false;
}

// Expands the frequency array using the given frequency and the order.
void hbsolver::expandFrequencies (nr_double_t f, int n) {
  tvector<nr_double_t> freqs = frequencies;
  int len = freqs.getSize ();
  frequencies.clear ();
  if (len > 0) {
    // frequency expansion
    for (int i = -n; i <= n; i++) {
      for (int k = 0; k < len; k++) {
	frequencies.add (i * f + freqs.get (k));
      }
    }
  }
  else {
    // first frequency
    for (int i = -n; i <= n; i++) frequencies.add (i * f);
  }
}

/* The function computes the harmonic frequencies excited in the
   circuit list depending on the maximum number of harmonics per
   exitation and saves its results into the 'frequencies' vector. */
void hbsolver::collectFrequencies (void) {

  // obtain order
  int n = getPropertyInteger ("n");

  // expand frequencies for each exitation
  for (int i = 0; i < excitations.length (); i++) {
    circuit * c = excitations.get (i);
    nr_double_t freq = c->getPropertyDouble ("f");
    expandFrequencies (freq, n);
  }

  // no excitations
  if (frequencies.getSize () == 0)
    frequencies.add (0.0);

#if DEBUG
  fprintf (stderr, "%d frequencies: [ ", frequencies.getSize ());
  for (int i = 0; i < frequencies.getSize (); i++) {
    fprintf (stderr, "%g ", (double) real (frequencies.get (i)));
  }
  fprintf (stderr, "]\n");
#endif /* DEBUG */
}

// Split netlist into excitation, linear and non-linear part.
void hbsolver::splitCircuits (void) {
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    if (c->isNonLinear ()) {
      // non-linear part
      nlcircuits.add (c);
    }
    else if (isExcitation (c)) {
      // get sinusoidal sources
      excitations.add (c);
    }
    else if (c->getType () != CIR_GROUND) {
      // linear part
      lncircuits.add (c);
    }
  }
}

// Creates a nodelist for the given circuit list.
strlist * hbsolver::circuitNodes (ptrlist<circuit> circuits) {
  strlist * nodes = new strlist ();
  for (int k = 0; k < circuits.length (); k++) {
    circuit * c = circuits.get (k);
    for (int i = 0; i < c->getSize (); i++) {
      char * n = c->getNode(i)->getName ();
      if (strcmp (n, "gnd")) {
	if (!nodes->contains (n)) nodes->add (n);
      }
    }
  }
  return nodes;
}

// Obtain node lists for linear and non-linear part.
void hbsolver::getNodeLists (void) {
  nlnodes = circuitNodes (nlcircuits);
  lnnodes = circuitNodes (lncircuits);
#if DEBUG
  fprintf (stderr, "nonlinear nodes: [ %s ]\n", nlnodes->toString ());
  fprintf (stderr, "   linear nodes: [ %s ]\n", lnnodes->toString ());
#endif
  banodes = new strlist ();
  for (int i = 0; i < nlnodes->length (); i++) {
    char * n = nlnodes->get (i);
    if (lnnodes->contains (n)) banodes->add (n);
  }
#if DEBUG
  fprintf (stderr, " balanced nodes: [ %s ]\n", banodes->toString ());
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
