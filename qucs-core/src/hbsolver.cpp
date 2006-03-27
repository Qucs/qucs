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
 * $Id: hbsolver.cpp,v 1.5 2006/03/27 09:55:49 raimi Exp $
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
#include "tvector.h"
#include "tmatrix.h"
#include "eqnsys.h"
#include "analysis.h"
#include "dataset.h"
#include "hbsolver.h"

// Constructor creates an unnamed instance of the hbsolver class.
hbsolver::hbsolver () : analysis () {
  type = ANALYSIS_HBALANCE;
  frequency = 0;
  nlnodes = lnnodes = banodes = NULL;
  YC = YV = Z = A = NULL;
  runs = 0;
}

// Constructor creates a named instance of the hbsolver class.
hbsolver::hbsolver (char * n) : analysis (n) {
  type = ANALYSIS_HBALANCE;
  frequency = 0;
  nlnodes = lnnodes = banodes = NULL;
  YC = YV = Z = A = NULL;
  runs = 0;
}

// Destructor deletes the hbsolver class object.
hbsolver::~hbsolver () {
  if (nlnodes) delete nlnodes;
  if (lnnodes) delete lnnodes;
  if (banodes) delete banodes;
  if (A) delete A;
  if (Z) delete Z;
  if (YV) delete YV;
  if (YC) delete YC;
}

/* The copy constructor creates a new instance of the hbsolver class
   based on the given hbsolver object. */
hbsolver::hbsolver (hbsolver & o) : analysis (o) {
  frequency = o.frequency;
  frequencies = o.frequencies;
  nlnodes = o.nlnodes;
  lnnodes = o.lnnodes;
  banodes = o.banodes;
  YC = YV = Z = A = NULL;
  runs = 0;
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

  prepareLinear ();
  
  runs++;
  logprint (LOG_STATUS, "NOTIFY: %s: solving for %d frequencies balanced at "
	    "%d nodes\n", getName (), nfreqs, banodes->length ());

  // create constant transadmittance matrix

  // create variable transadmittance matrix

  /* transadmittance matrices are obtained by
     1. create MNA matrix of linear subcircuit
     2. derive transimpedance matrix entries by unity current excitation
     3. invert the transimpedance matrix -> transadmittance matrix
     4. fill in matrix entries into constant and variable transadmittance
        matrix
  */

  do {

    solveLinear ();

    // inverse FFT of frequency domain voltage vector

    // compute non-linear charges and currents

    // FFT of time domain currents
  }
  // check termination criteria (balanced frequency domain currents)
  while (!checkBalance ());

  // apply AC analysis to the complete network in order to obtain the
  // final results
  saveResults ();
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
  for (ptrlistiterator<circuit> it (excitations); *it; ++it) {
    circuit * c = it.current ();
    nr_double_t freq = c->getPropertyDouble ("f");
    expandFrequencies (freq, n);
  }

  // no excitations
  if (frequencies.getSize () == 0)
    frequencies.add (0.0);
  nfreqs = (frequencies.getSize () - 1) / 2 + 1;

#if DEBUG && 0
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
  for (ptrlistiterator<circuit> it (circuits); *it; ++it) {
    circuit * c = it.current ();
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
#if DEBUG && 0
  fprintf (stderr, "nonlinear nodes: [ %s ]\n", nlnodes->toString ());
  fprintf (stderr, "   linear nodes: [ %s ]\n", lnnodes->toString ());
#endif
  banodes = new strlist ();
  for (int i = 0; i < nlnodes->length (); i++) {
    char * n = nlnodes->get (i);
    if (lnnodes->contains (n)) banodes->add (n);
  }
#if DEBUG && 0
  fprintf (stderr, " balanced nodes: [ %s ]\n", banodes->toString ());
#endif
}

/* The function applies unique voltage source identifiers to each
   built in internal voltage source in the list of circuits. */
int hbsolver::assignVoltageSources (ptrlist<circuit> circuits) {
  int sources = 0;
  for (ptrlistiterator<circuit> it (circuits); *it; ++it) {
    circuit * c = it.current ();
    if (c->getVoltageSources () > 0) {
      c->setVoltageSource (sources);
      sources += c->getVoltageSources ();
    }
  }
  return sources;
}

/* The function assigns unique node identifiers to each circuit node. */
int hbsolver::assignNodes (ptrlist<circuit> circuits, strlist * nodes) {
  // through all collected nodes
  for (int nr = 0; nr < nodes->length (); nr++) {
    char * nn = nodes->get (nr);
    // through all circuits
    for (ptrlistiterator<circuit> it (circuits); *it; ++it) {
      circuit * c = it.current ();
      // assign current identifier if any of the circuit nodes equals
      // the current one
      for (int i = 0; i < c->getSize (); i++) {
	node * n = c->getNode (i);
	if (!strcmp (n->getName (), nn)) n->setNode (nr + 1);
      }
    }
  }
  return nodes->length ();
}

// Prepares the linear operations.
void hbsolver::prepareLinear (void) {
  nlnvsrcs = assignVoltageSources (lncircuits);
  nlnnodes = assignNodes (lncircuits, lnnodes);
  for (ptrlistiterator<circuit> it (lncircuits); *it; ++it) (*it)->initHB ();
  createMatrixLinearA ();
}

/* The function creates the complex linear network MNA matrix.  It
   contains the MNA entries for all linear components for each
   requested frequency. */
void hbsolver::createMatrixLinearA (void) {
  int M = nlnvsrcs;
  int N = nlnnodes;
  int f = 0;
  nr_double_t freq;

  // create new MNA matrix
  A = new tmatrix<complex> ((N + M) * nfreqs);

  // through each frequency
  for (int i = 0; i < frequencies.getSize (); i++) {
    if ((freq = frequencies.get (i)) < 0.0) continue;
    // calculate components' MNA matrix for the given frequency
    for (ptrlistiterator<circuit> it (lncircuits); *it; ++it)
      (*it)->calcHB (freq);
    // fill in all matrix entries for the given frequency
    fillMatrixLinearA (A, f++);
  }
}

// some definitions for the matrix filler
#undef  A_
#undef  B_
#define A_(r,c) (*A) ((r)*nfreqs+f,(c)*nfreqs+f)
#define G_(r,c) A_(r,c)
#define B_(r,c) A_(r,c+N)
#define C_(r,c) A_(r+N,c)
#define D_(r,c) A_(r+N,c+N)

/* This function fills in the MNA matrix entries intp the A matrix for
   a given frequency index. */
void hbsolver::fillMatrixLinearA (tmatrix<complex> * A, int f) {
  int N = nlnnodes;

  // through each linear circuit
  for (ptrlistiterator<circuit> it (lncircuits); *it; ++it) {
    circuit * cir = it.current ();
    int s = cir->getSize ();
    int nr, nc, r, c, v;

    // apply G-matrix entries
    for (r = 0; r < s; r++) {
      if ((nr = cir->getNode(r)->getNode () - 1) < 0) continue;
      for (c = 0; c < s; c++) {
	if ((nc = cir->getNode(c)->getNode () - 1) < 0) continue;
	G_(nr, nc) += cir->getY (r, c);
      }
    }

    // augmented part -- built in voltage sources
    if ((v = cir->getVoltageSources ()) > 0) {

      // apply B-matrix entries
      for (r = 0; r < s; r++) {
	if ((nr = cir->getNode(r)->getNode () - 1) < 0) continue;
	for (c = 0; c < v; c++) {
	  nc = cir->getVoltageSource () + c;
	  B_(nr, nc) += cir->getB (r, nc);
	}
      }

      // apply C-matrix entries
      for (r = 0; r < v; r++) {
	nr = cir->getVoltageSource () + r;
	for (c = 0; c < s; c++) {
	  if ((nc = cir->getNode(c)->getNode () - 1) < 0) continue;
	  C_(nr, nc) += cir->getC (nr, c);
	}
      }
    
      // apply D-matrix entries
      for (r = 0; r < v; r++) {
	nr = cir->getVoltageSource () + r;
	for (c = 0; c < v; c++) {
	  nc = cir->getVoltageSource () + c;
	  D_(nr, nc) += cir->getD (nr, nc);
	}
      }
    }
  }
}

// The function inverts the given matrix A into the matrix H.
void hbsolver::invertMatrix (tmatrix<complex> * A, tmatrix<complex> * H) {
  eqnsys<complex> eqns;
  int N = A->getCols ();
  tvector<complex> * x = new tvector<complex> (N);
  tvector<complex> * z = new tvector<complex> (N);

  // create LU decomposition of the A matrix
  eqns.setAlgo (ALGO_LU_FACTORIZATION_CROUT);
  eqns.passEquationSys (A, x, z);
  eqns.solve ();

  // use the LU decomposition to obtain the inverse H
  eqns.setAlgo (ALGO_LU_SUBSTITUTION_CROUT);
  for (int c = 0; c < N; c++) {
    z->set (0.0);
    z->set (c, 1.0);
    eqns.solve ();
    for (int r = 0; r < N; r++) H->set (r, c, x->get (r));
  }
  delete x;
  delete z;
}

void hbsolver::createMatrixLinearY (void) {
  int M = nlnvsrcs;
  int N = nlnnodes;

  // allocate new transimpedance matrix
  Z = new tmatrix<complex> ((N + M) * nfreqs);
  // invert the linear MNA matrix to a Z matrix
  invertMatrix (A, Z);

  // prepare equation system
  eqnsys<complex> eqns;  
  tvector<complex> * V;
  tvector<complex> * I;

  // create variable transadmittance matrix relating voltages at the
  // balanced nodes to the currents through these nodes into the
  // non-linear part
  int sv = banodes->length ();
  int sn = sv * nfreqs;
  YV = new tmatrix<complex> (sn);
  V =  new tvector<complex> (sn);
  I =  new tvector<complex> (sn);
  // LU decompose the Z matrix
  eqns.setAlgo (ALGO_LU_FACTORIZATION_CROUT);
  eqns.passEquationSys (Z, I, V);
  eqns.solve ();
  eqns.setAlgo (ALGO_LU_SUBSTITUTION_CROUT);
  // aquire variable transadmittance matrix entries
  for (int c = 0; c < sn; c++) {
    V->set (0.0);
    V->set (c, 1.0);
    eqns.solve ();
    for (int r = 0; r < N; r++) YV->set (r, c, I->get (r));
  }

  int sc = excitations.length ();
  int ss = sc * nfreqs;
  YC = new tmatrix<complex> (sn, ss);
  // aquire constant transadmittance matrix entries
  for (int c = 0; c < sn; c++) {
  }
  delete I;
  delete V;
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

// Saves simulation results.
void hbsolver::saveResults (void) {
  vector * f;
  // add current frequency to the dependency of the output dataset
  if ((f = data->findDependency ("hbfrequency")) == NULL) {
    f = new vector ("hbfrequency");
    data->addDependency (f);
  }
  if (runs == 1) {
    for (int i = 0; i < frequencies.getSize (); i++) {
      nr_double_t freq;
      if ((freq = frequencies.get (i)) < 0.0) continue;
      f->add (freq);
    }
  }
}
