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
 * $Id: hbsolver.cpp,v 1.7 2006-04-05 08:27:06 raimi Exp $
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
  nlnodes = lnnodes = banodes = nanodes = NULL;
  NA = YV = Z = A = NULL;
  IS = IC = x = NULL;
  runs = 0;
}

// Constructor creates a named instance of the hbsolver class.
hbsolver::hbsolver (char * n) : analysis (n) {
  type = ANALYSIS_HBALANCE;
  frequency = 0;
  nlnodes = lnnodes = banodes = nanodes = NULL;
  NA = YV = Z = A = NULL;
  IS = IC = x = NULL;
  runs = 0;
}

// Destructor deletes the hbsolver class object.
hbsolver::~hbsolver () {
  if (nlnodes) delete nlnodes;
  if (lnnodes) delete lnnodes;
  if (banodes) delete banodes;
  if (nanodes) delete nanodes;
  if (A) delete A;
  if (Z) delete Z;
  if (NA) delete NA;
  if (YV) delete YV;
  if (IC) delete IC;
  if (IS) delete IS;
  if (x) delete x;
}

/* The copy constructor creates a new instance of the hbsolver class
   based on the given hbsolver object. */
hbsolver::hbsolver (hbsolver & o) : analysis (o) {
  frequency = o.frequency;
  frequencies = o.frequencies;
  nlnodes = o.nlnodes;
  lnnodes = o.lnnodes;
  banodes = o.banodes;
  nanodes = o.nanodes;
  NA = YV = Z = A = NULL;
  IS = IC = x = NULL;
  runs = o.runs;
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

  // preapares the linear part --> 0 = IC + [YV] * V
  prepareLinear ();
  
  runs++;
  logprint (LOG_STATUS, "NOTIFY: %s: solving for %d frequencies balanced at "
	    "%d nodes\n", getName (), nfreqs, banodes->length ());

  // start iteration
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
  finalSolution ();
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
  if (type == CIR_PAC || type == CIR_VAC || type == CIR_VDC)
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
  nr_double_t freq;
  for (ptrlistiterator<circuit> it (excitations); *it; ++it) {
    circuit * c = it.current ();
    if (c->getType () != CIR_VDC) {
      freq = c->getPropertyDouble ("f");
      expandFrequencies (freq, n);
    }
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

  // build list of positive frequencies including DC
  for (n = 0; n < frequencies.getSize (); n++) {
    if ((freq = frequencies (n)) < 0.0) continue;
    pfreqs.add (freq);
  }
}

// Split netlist into excitation, linear and non-linear part.
void hbsolver::splitCircuits (void) {
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    if (c->isNonLinear ()) {
      // non-linear part
      nolcircuits.add (c);
    }
    else if (isExcitation (c)) {
      // get sinusoidal sources
      excitations.add (c);
    }
    else if (c->getType () != CIR_GROUND) {
      // linear part
      lincircuits.add (c);
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
  // non-linear nodes
  nlnodes = circuitNodes (nolcircuits);
  // linear nodes
  lnnodes = circuitNodes (lincircuits);
  // excitation nodes
  exnodes = circuitNodes (excitations);

#if DEBUG && 0
  fprintf (stderr, "nonlinear nodes: [ %s ]\n", nlnodes->toString ());
  fprintf (stderr, "   linear nodes: [ %s ]\n", lnnodes->toString ());
#endif

  // organization of the nodes for the MNA:
  // --------------------------------------
  // 1.)  balanced nodes: all connected to at least one non-linear device
  // 2.a) the excitation nodes
  // 2.b) all linear nodes not contained in 1. and 2.a
  // 2.c) additional gyrators of linear nodes (built-in voltage sources)
  // please note: excitation nodes also in 2.b; 1. and 2.a are 'ports'

  nanodes = new strlist (*nlnodes); // list 1.
  // add excitation nodes; list 2.a
  for (strlistiterator it (exnodes); *it; ++it)
    nanodes->append (*it);
  // add linear nodes; list 2.b
  for (strlistiterator it (lnnodes); *it; ++it) {
    if (!nanodes->contains (*it))
      nanodes->append (*it);
  }

  banodes = new strlist (*nlnodes);
#if DEBUG && 0
  fprintf (stderr, " balanced nodes: [ %s ]\n", banodes->toString ());
  fprintf (stderr, "  exnodes nodes: [ %s ]\n", exnodes->toString ());
  fprintf (stderr, "  nanodes nodes: [ %s ]\n", nanodes->toString ());
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
int hbsolver::assignNodes (ptrlist<circuit> circuits, strlist * nodes,
			   int offset) {
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
	if (!strcmp (n->getName (), nn)) n->setNode (offset + nr + 1);
      }
    }
  }
  return nodes->length ();
}

// Prepares the linear operations.
void hbsolver::prepareLinear (void) {
  for (ptrlistiterator<circuit> it (lincircuits); *it; ++it) (*it)->initHB ();
  nlnvsrcs = assignVoltageSources (lincircuits);
  nnanodes = nanodes->length ();
  nexnodes = exnodes->length ();
  assignNodes (nolcircuits, nanodes);
  assignNodes (lincircuits, nanodes);
  assignNodes (excitations, nanodes);
  createMatrixLinearA ();
  createMatrixLinearY ();
  calcConstantCurrent ();
}

/* The function creates the complex linear network MNA matrix.  It
   contains the MNA entries for all linear components for each
   requested frequency. */
void hbsolver::createMatrixLinearA (void) {
  int M = nlnvsrcs;
  int N = nnanodes;
  int f = 0;
  nr_double_t freq;

  // create new MNA matrix
  A = new tmatrix<complex> ((N + M) * nfreqs);

  // through each frequency
  for (int i = 0; i < pfreqs.getSize (); i++) {
    freq = pfreqs (i);
    // calculate components' MNA matrix for the given frequency
    for (ptrlistiterator<circuit> it (lincircuits); *it; ++it)
      (*it)->calcHB (freq);
    // fill in all matrix entries for the given frequency
    fillMatrixLinearA (A, f++);
  }

  // save a copy of the original MNA matrix
  NA = new tmatrix<complex> (*A);
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
  int N = nnanodes;

  // through each linear circuit
  for (ptrlistiterator<circuit> it (lincircuits); *it; ++it) {
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

  try_running () {
    // create LU decomposition of the A matrix
    eqns.setAlgo (ALGO_LU_FACTORIZATION_CROUT);
    eqns.passEquationSys (A, x, z);
    eqns.solve ();
  }
  // appropriate exception handling
  catch_exception () {
  case EXCEPTION_PIVOT:
  default:
    logprint (LOG_ERROR, "WARNING: %s: during TI inversion\n", getName ());
    estack.print ();
  }

  // use the LU decomposition to obtain the inverse H
  eqns.setAlgo (ALGO_LU_SUBSTITUTION_CROUT);
  for (int c = 0; c < N; c++) {
    z->set (0.0);
    z->set (c, 1.0);
    eqns.passEquationSys (A, x, z);
    eqns.solve ();
    for (int r = 0; r < N; r++) H->set (r, c, x->get (r));
  }
  delete x;
  delete z;
}

// Some defines for matrix element access.
#define V_(r) (*V) (r)
#define I_(r) (*I) (r)

#define Z_(r,c) (*Z) (r,c)
#define Y_(r,c) (*Y) (r,c)

#define ZVU_(r,c) Z_(r,c)
#define ZVL_(r,c) Z_((r)*nfreqs+f+sn,c)
#define ZCU_(r,c) Z_(r,(c)*nfreqs+f+sn)
#define ZCL_(r,c) Z_((r)*nfreqs+f+sn,(c)*nfreqs+f+sn)

#define YV_(r,c) Y_(r,c)

/* The following function performs the following steps:
   1. form the MNA matrix A including all nodes (linear, non-linear and
      excitations)
   2. compute the variable transimpedance matrix entries for the nodes
      to be balanced
   3. compute the constant transimpedance matrix entries for the constant
      current vector caused by the excitations
   4. invert this overall transimpedance matrix
   5. extract the variable transadmittance matrix entries
*/
void hbsolver::createMatrixLinearY (void) {
  int M = nlnvsrcs;
  int N = nnanodes;
  int c, r, f;
  ptrlistiterator<circuit> ite;

  // size of overall MNA matrix
  int sa = (N + M) * nfreqs;
  int sv = banodes->length ();
  int se = excitations.length ();
  int sy = sv + se;

  // allocate new transimpedance matrix
  Z = new tmatrix<complex> (sy * nfreqs);

  // prepare equation system
  eqnsys<complex> eqns;  
  tvector<complex> * V;
  tvector<complex> * I;

  // 1. create variable transimpedance matrix entries relating
  // voltages at the balanced nodes to the currents through these
  // nodes into the non-linear part
  int sn = sv * nfreqs;
  V = new tvector<complex> (sa);
  I = new tvector<complex> (sa);

  // connect a 100 Ohm resistor (to ground) to each port in the MNA matrix
  for (c = 0; c < (sv + nexnodes) * nfreqs; c++) (*A) (c, c) += 0.01;

  // LU decompose the MNA matrix
  try_running () {
    eqns.setAlgo (ALGO_LU_FACTORIZATION_CROUT);
    eqns.passEquationSys (A, V, I);
    eqns.solve ();
  }
  // appropriate exception handling
  catch_exception () {
  case EXCEPTION_PIVOT:
  default:
    logprint (LOG_ERROR, "WARNING: %s: during A factorization\n", getName ());
    estack.print ();
  }

  // aquire variable transimpedance matrix entries
  eqns.setAlgo (ALGO_LU_SUBSTITUTION_CROUT);
  for (c = 0; c < sn; c++) {
    I->set (0.0);
    I_(c) = 1.0;
    eqns.passEquationSys (A, V, I);
    eqns.solve ();
    // ZV | ..
    // ---+---
    // .. | ..
    for (r = 0; r < sn; r++) ZVU_(r, c) = V_(r);
    // .. | ..
    // ---+---
    // ZV | ..
    r = 0;
    for (ite = ptrlistiterator<circuit> (excitations); *ite; ++ite, r++) {
      // lower part entries
      for (f = 0; f < nfreqs; f++) {
	ZVL_(r, c) = excitationZ (V, *ite, f);
      }
    }
  }

  // create constant transimpedance matrix entries relating the
  // source voltages to the interconnection currents
  int vsrc = 0;
  // aquire constant transadmittance matrix entries
  for (ptrlistiterator<circuit> it (excitations); *it; ++it, vsrc++) {
    circuit * vs = it.current ();
    // get positive and negative node
    int pnode = vs->getNode(NODE_1)->getNode ();
    int nnode = vs->getNode(NODE_2)->getNode ();
    for (f = 0; f < nfreqs; f++) { // for each frequency
      int pn = (pnode - 1) * nfreqs + f;
      int nn = (nnode - 1) * nfreqs + f;
      I->set (0.0);
      if (pnode) I_(pn) = +1.0;
      if (nnode) I_(nn) = -1.0;
      eqns.passEquationSys (A, V, I);
      eqns.solve ();
      // .. | ZC
      // ---+---
      // .. | ..
      for (r = 0; r < sn; r++) {
	// upper part of the entries
	ZCU_(r, vsrc) = V_(r);
      }
      // .. | ..
      // ---+---
      // .. | ZC
      r = 0;
      for (ite = ptrlistiterator<circuit> (excitations); *ite; ++ite, r++) {
	// lower part entries
	ZCL_(r, vsrc) = excitationZ (V, *ite, f);
      }
    }
  }
  delete I;
  delete V;

  // allocate new transadmittance matrix
  Y = new tmatrix<complex> (sy * nfreqs);

  // invert the Z matrix to a Y matrix
  invertMatrix (Z, Y);

  // substract the 100 Ohm resistor
  for (c = 0; c < sy * nfreqs; c++) Y_(c, c) -= 0.01;

  // extract the variable transadmittance matrix
  YV = new tmatrix<complex> (sn);
  for (r = 0; r < sn ; r++)
    for (c = 0; c < sn; c++)
      YV_(r, c) = Y_(r, c);
}

/* Little helper function obtaining a transimpedance value for the
   given voltage source (excitation) and for a given frequency
   index. */
complex hbsolver::excitationZ (tvector<complex> * V, circuit * vs, int f) {
  // get positive and negative node
  int pnode = vs->getNode(NODE_1)->getNode ();
  int nnode = vs->getNode(NODE_2)->getNode ();
  complex z = 0.0;
  if (pnode) z += V_((pnode - 1) * nfreqs + f);
  if (nnode) z -= V_((nnode - 1) * nfreqs + f);
  return z;
}

/* This function computes the constant current vectors using the
   voltage of the excitations and the transadmittance matrix
   entries. */
void hbsolver::calcConstantCurrent (void) {
  int se = excitations.length () * nfreqs;
  int sn = banodes->length () * nfreqs;
  int r, c, vsrc = 0;

  // collect excitation voltages
  tvector<complex> VC (se);
  for (ptrlistiterator<circuit> it (excitations); *it; ++it, vsrc++) {
    circuit * vs = it.current ();
    vs->initHB ();
    vs->setVoltageSource (0);
    for (int f = 0; f < pfreqs.getSize (); f++) { // for each frequency
      nr_double_t freq = pfreqs (f);
      vs->calcHB (freq);
      VC (vsrc * nfreqs + f) = vs->getE (VSRC_1);
    }
  }

  // compute constant current vector for balanced nodes
  IC = new tvector<complex> (sn);
  // .. | YC * VC
  // ---+---
  // .. | ..
  for (r = 0; r < sn; r++) {
    complex i = 0.0;
    for (c = 0; c < se; c++) {
      i += Y_(r, c + sn) * VC (c);
    }
    IC->set (r, i);
  }

  // compute constant current vector for sources itself
  IS = new tvector<complex> (se);
  // .. | ..
  // ---+---
  // .. | YC * VC
  for (r = 0; r < se; r++) {
    complex i = 0.0;
    for (c = 0; c < se; c++) {
      i += Y_(r + sn, c + sn) * VC (c);
    }
    IS->set (r, i);
  }
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

/* The function calculates and saves the final solution. */
void hbsolver::finalSolution (void) {
  int S = NA->getCols ();
  int N = nnanodes * nfreqs;

  // right hand side vector
  tvector<complex> * I = new tvector<complex> (S);
  // temporary solution
  tvector<complex> * V = new tvector<complex> (S);
  // final solution
  x = new tvector<complex> (N);

  // put computed currents caused by the HB sources into the right
  // hand side (already saved in IS)
  int vsrc = 0;
  for (ptrlistiterator<circuit> it (excitations); *it; ++it, vsrc++) {
    circuit * vs = it.current ();
    // get nodes of original HB sources
    int pnode = vs->getNode(NODE_1)->getNode ();
    int nnode = vs->getNode(NODE_2)->getNode ();
    for (int f = 0; f < nfreqs; f++) { // for each frequency
      complex i = IS->get (vsrc * nfreqs + f);
      int pn = (pnode - 1) * nfreqs + f;
      int nn = (nnode - 1) * nfreqs + f;
      if (pnode) I_(pn) += +i;
      if (nnode) I_(nn) += -i;
    }
  }

  // TODO: put currents through balanced nodes into right hand side

  // use LU decomposition for the final solution
  try_running () {
    eqnsys<complex> eqns;
    eqns.setAlgo (ALGO_LU_DECOMPOSITION);
    eqns.passEquationSys (NA, V, I);
    eqns.solve ();
  }
  // appropriate exception handling
  catch_exception () {
  case EXCEPTION_PIVOT:
  default:
    logprint (LOG_ERROR, "WARNING: %s: during final AC analysis\n",
	      getName ());
    estack.print ();
  }
  for (int i = 0; i < N; i++) x->set (i, V_(i));
}

// Saves simulation results.
void hbsolver::saveResults (void) {
  vector * f;
  // add current frequency to the dependency of the output dataset
  if ((f = data->findDependency ("hbfrequency")) == NULL) {
    f = new vector ("hbfrequency");
    data->addDependency (f);
  }
  // save frequency vector
  if (runs == 1) {
    for (int i = 0; i < pfreqs.getSize (); i++) f->add (pfreqs (i));
  }
  // save node voltage vectors
  int nanode = 0;
  for (strlistiterator it (nanodes); *it; ++it, nanode++) {
    int l = strlen (*it);
    char * n = (char *) malloc (l + 4);
    sprintf (n, "%s.Vb", *it);
    for (int i = 0; i < pfreqs.getSize (); i++) {
      saveVariable (n, x->get (i + nanode * nfreqs), f);
    }
  }
}
