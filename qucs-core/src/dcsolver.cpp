/*
 * dcsolver.cpp - DC solver class implementation
 *
 * Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: dcsolver.cpp,v 1.20 2004-07-11 17:06:09 margraf Exp $
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
#include "node.h"
#include "circuit.h"
#include "vector.h"
#include "dataset.h"
#include "net.h"
#include "analysis.h"
#include "nodelist.h"
#include "strlist.h"
#include "matrix.h"
#include "eqnsys.h"
#include "component_id.h"
#include "operatingpoint.h"
#include "exception.h"
#include "exceptionstack.h"
#include "dcsolver.h"

using namespace qucs;

// Constructor creates an unnamed instance of the dcsolver class.
dcsolver::dcsolver () : analysis () {
  nlist = NULL;
  A = z = x = NULL;
  xprev = zprev = NULL;
  saveOPs = 0;
  reltol = abstol = vntol = 0;
  type = ANALYSIS_DC;
}

// Constructor creates a named instance of the dcsolver class.
dcsolver::dcsolver (char * n) : analysis (n) {
  nlist = NULL;
  A = z = x = NULL;
  xprev = zprev = NULL;
  saveOPs = 0;
  reltol = abstol = vntol = 0;
  type = ANALYSIS_DC;
}

// Destructor deletes the dcsolver class object.
dcsolver::~dcsolver () {
  if (nlist) delete nlist;
  delete A;
  delete z;
  delete x;
  delete xprev;
  delete zprev;
}

/* The copy constructor creates a new instance of the dcsolver class
   based on the given dcsolver object. */
dcsolver::dcsolver (dcsolver & o) : analysis (o) {
  nlist = new nodelist (*(o.nlist));
  A = new matrix (*(o.A));
  z = new matrix (*(o.z));
  x = new matrix (*(o.x));
  xprev = zprev = NULL;
  saveOPs = o.saveOPs;
  reltol = o.reltol;
  abstol = o.abstol;
  vntol = o.vntol;
}

/* This is the DC netlist solver.  It prepares the circuit list for each
   requested frequency and solves it then. */
void dcsolver::solve (void) {
  int convergence, run = 0, MaxIterations, error = 0, d;
  qucs::exception * e;

  // fetch simulation properties
  saveOPs = !strcmp (getPropertyString ("saveOPs"), "yes") ? 1 : 0;
  MaxIterations = getPropertyInteger ("MaxIter");
  reltol = getPropertyDouble ("reltol");
  abstol = getPropertyDouble ("abstol");
  vntol  = getPropertyDouble ("vntol");

  // initialize node voltages, first guess for non-linear circuits and
  // generate extra circuits if necessary
  init ();

  // create node list, enumerate nodes and voltage sources
#if DEBUG
  logprint (LOG_STATUS, "NOTIFY: creating node list for DC analysis\n");
#endif
  createNodeList ();
  nlist->assignNodes ();
  assignVoltageSources ();
#if DEBUG
  nlist->print ();
#endif

#if DEBUG
  logprint (LOG_STATUS, "NOTIFY: solving DC netlist\n");
#endif

  // run solving loop until convergence is reached
  do {
    // generate A matrix and z vector
    createMatrix ();

    // solve equation system
    try_running () {
      runMNA ();
    }
    // appropriate exception handling
    catch_exception () {
    case EXCEPTION_PIVOT: case EXCEPTION_WRONG_VOLTAGE:
      e = new qucs::exception (EXCEPTION_DC_FAILED);
      d = top_exception()->getData ();
      if (d > countNodes ()) {
	d -= countNodes ();
	e->setText ("voltage source `%s' conflicts with some other voltage "
		    "source", findVoltageSource(d)->getName ());
      }
      else {
	e->setText ("circuit admittance matrix in DC solver is singular at "
		    "node `%s' connected to [%s]", nlist->get (d),
		    nlist->getNodeString (d));
      }
      throw_exception (e);
      error++;
      break;
    default:
      error++;
      break;
    }

    if (!error) {
      // save results into circuits
      saveNodeVoltages ();

      // convergence check
      convergence = (run > 0) ? checkConvergence () : 0;
      savePreviousIteration ();
      run++;
    }
    else break;
  }
  while (!convergence && run < MaxIterations);

  if (run >= MaxIterations || error) {
    e = new qucs::exception (EXCEPTION_NO_CONVERGENCE);
    e->setText ("no convergence in DC analysis after %d iterations", run);
    throw_exception (e);
  }
#if DEBUG
  else {
    logprint (LOG_STATUS, "NOTIFY: convergence reached after %d iterations\n",
	      run);
  }
#endif /* DEBUG */
  saveResults ();
  delete nlist; nlist = NULL;
}

/* The function creates a nodelist for the DC analysis.  The nodelist
   consists is based on the circuit list and consists of unique nodes
   inside the circuit list only.  Each node in the list has references
   to their actual circuit nodes and thereby to the circuits it is
   connected to. */
void dcsolver::createNodeList (void) {
  circuit * root = subnet->getRoot ();
  nlist = new nodelist ();
  // go through circuit list and find unique nodes
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    for (int i = 1; i <= c->getSize (); i++) {
      node * n = c->getNode (i);
      if (nlist->contains (n->getName ()) == 0) {
	nlist->add (n->getName (), n->getInternal ());
      }
    }
  }
  // add circuit nodes to each unique node in the list
  for (struct nodelist_t * n = nlist->getRoot (); n != NULL; n = n->next) {
    for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
      for (int i = 1; i <= c->getSize (); i++) {
	if (!strcmp (n->name, c->getNode(i)->getName ())) {
	  nlist->addCircuitNode (n, c->getNode (i));
	}
      }
    }
  }
}

/* Applying the MNA (Modified Nodal Analysis) to a circuit with
   passive elements and independent current and voltage sources
   results in a matrix equation of the form Ax = z.  This function
   generates the A and z matrix. */
void dcsolver::createMatrix (void) {
  int M = subnet->getVoltageSources ();
  int N = countNodes ();

  /* Generate the A matrix.  The A matrix consists of four (4) minor
     matrices in the form     +-   -+
                          A = | G B |
                              | C D |
			      +-   -+.
     Each of these minor matrices is going to be generated here. */
  if (A != NULL) delete A;
  A = new matrix (M + N);
  createGMatrix ();
  createBMatrix ();
  createCMatrix ();
  createDMatrix ();

  /* Generate the z Matrix.  The z Matrix consists of two (2) minor
     matrices in the form     +- -+
                          z = | i |
                              | e |
			      +- -+.
     Each of these minor matrices is going to be generated here. */
  if (z != NULL) delete z;
  z = new matrix (N + M, 1);
  createIMatrix ();
  createEMatrix ();

  // Create empty solution vector.
  if (x == NULL) x = new matrix (N + M, 1);
}

/* The B matrix is an MxN matrix with only 0, 1 and -1 elements.  Each
   location in the matrix corresponds to a particular voltage source
   (first dimension) or a node (second dimension).  If the positive
   terminal of the ith voltage source is connected to node k, then the
   element (i,k) in the B matrix is a 1.  If the negative terminal of
   the ith voltage source is connected to node k, then the element
   (i,k) in the B matrix is a -1.  Otherwise, elements of the B matrix
   are zero. */
void dcsolver::createBMatrix (void) {
  int N = countNodes ();
  int M = subnet->getVoltageSources ();
  circuit * vs;
  struct nodelist_t * n;
  nr_double_t val;

  // go through each voltage sources (first dimension)
  for (int c = 1; c <= M; c++) {
    vs = findVoltageSource (c);
    // go through each node (second dimension)
    for (int r = 1; r <= N; r++) {
      val = 0.0;
      n = nlist->getNode (r);
      for (int i = 0; i < n->nNodes; i++) {
	// is voltage source connected to node ?
	if (n->nodes[i]->getCircuit () == vs) {
	  val = real (vs->getB (n->nodes[i]->getPort (), c));
	  break;
	}
      }
      // put value into B matrix
      A->set (r, c + N, val);
    }
  }  
}

/* The C matrix is an NxM matrix with only 0, 1 and -1 elements.  Each
   location in the matrix corresponds to a particular node (first
   dimension) or a voltage source (first dimension).  If the positive
   terminal of the ith voltage source is connected to node k, then the
   element (k,i) in the C matrix is a 1.  If the negative terminal of
   the ith voltage source is connected to node k, then the element
   (k,i) in the C matrix is a -1.  Otherwise, elements of the C matrix
   are zero. */
void dcsolver::createCMatrix (void) {
  int N = countNodes ();
  int M = subnet->getVoltageSources ();
  circuit * vs;
  struct nodelist_t * n;
  nr_double_t val;

  // go through each voltage sources (second dimension)
  for (int r = 1; r <= M; r++) {
    vs = findVoltageSource (r);
    // go through each node (first dimension)
    for (int c = 1; c <= N; c++) {
      val = 0.0;
      n = nlist->getNode (c);
      for (int i = 0; i < n->nNodes; i++) {
	// is voltage source connected to node ?
	if (n->nodes[i]->getCircuit () == vs) {
	  val = real (vs->getC (r, n->nodes[i]->getPort ()));
	  break;
	}
      }
      // put value into C matrix
      A->set (r + N, c, val);
    }
  }  
}

/* The D matrix is an MxM matrix that is composed entirely of zeros.
   It can be non-zero if dependent sources are considered. */
void dcsolver::createDMatrix (void) {
  int M = subnet->getVoltageSources ();
  int N = countNodes ();
  circuit * vsr, * vsc;
  nr_double_t val;
  for (int r = 1; r <= M; r++) {
    vsr = findVoltageSource (r);
    for (int c = 1; c <= M; c++) {
      vsc = findVoltageSource (c);
      val = 0.0;
      if (vsr == vsc) {
	val = real (vsr->getD (r, c));
      }
      A->set (r + N, c + N, val);
    }
  }
}

/* The G matrix is an NxN matrix formed in two steps.
   1. Each element in the diagonal matrix is equal to the sum of the
   conductance of each element connected to the corresponding node.
   2. The off diagonal elements are the negative conductance of the
   element connected to the pair of corresponding nodes.  Therefore a
   resistor between nodes 1 and 2 goes into the G matrix at location
   (1,2) and location (2,1).  If an element is grounded, it will only
   have contribute to one entry in the G matrix -- at the appropriate
   location on the diagonal. */
void dcsolver::createGMatrix (void) {
  int pr, pc, N = countNodes ();
  nr_double_t g;
  struct nodelist_t * nr, * nc;
  circuit * cir;

  // run the calculation of the conductance for each circuit
  calc ();

  // go through each column of the G matrix
  for (int c = 1; c <= N; c++) {
    nc = nlist->getNode (c);
    // go through each row of the G matrix
    for (int r = 1; r <= N; r++) {
      nr = nlist->getNode (r);
      g = 0.0;
      // diagonal matrix element ?
      if (c == r) {
	// sum up the conductance of each connected circuit
	for (int i = 0; i < nc->nNodes; i++) {
	  cir = nc->nodes[i]->getCircuit ();
	  pc = pr = nc->nodes[i]->getPort ();
	  g += cir->getG (pr, pc);
	}
      }
      // off diagonal
      else {
	// sum up negative conductance of each circuit in between both nodes
	for (int a = 0; a < nc->nNodes; a++)
	  for (int b = 0; b < nr->nNodes; b++)
	    if (nc->nodes[a]->getCircuit () == nr->nodes[b]->getCircuit ()) {
	      cir = nc->nodes[a]->getCircuit ();
	      pc = nc->nodes[a]->getPort ();
	      pr = nr->nodes[b]->getPort ();
	      g += cir->getG (pr, pc);
	    }
      }
      // put value into G matrix
      A->set (r, c, g);
    }
  }
}

/* The i matrix is an 1xN matrix with each element of the matrix
   corresponding to a particular node.  The value of each element of i
   is determined by the sum of current sources into the corresponding
   node.  If there are no current sources connected to the node, the
   value is zero. */
void dcsolver::createIMatrix (void) {
  int N = countNodes ();
  nr_double_t val;
  struct nodelist_t * n;
  circuit * is;

  // go through each node
  for (int r = 1; r <= N; r++) {
    val = 0.0;
    n = nlist->getNode (r);
    // go through each circuit connected to the node
    for (int i = 0; i < n->nNodes; i++) {
      is = n->nodes[i]->getCircuit ();
      // is this a current source ?
      if (is->getType () == CIR_IDC || is->isNonLinear ()) {
	val += real (is->getI (n->nodes[i]->getPort ()));
      }
    }
    // put value into i matrix
    z->set (r, 1, val);
  }  
}

/* The e matrix is a 1xM matrix with each element of the matrix equal
   in value to the corresponding independent voltage source. */
void dcsolver::createEMatrix (void) {
  int N = countNodes ();
  int M = subnet->getVoltageSources ();
  nr_double_t val;
  circuit * vs;

  // go through each voltage source
  for (int r = 1; r <= M; r++) {
    vs = findVoltageSource (r);
    val = real (vs->getE (r));
    // put value into e matrix
    z->set (r + N, 1, val);
  }  
}

// Returns the number of nodes in the nodelist, excluding the ground node.
int dcsolver::countNodes (void) {
  return nlist->length () - 1;
}

/* The function returns the voltage source circuit object
   corresponding to the given number.  If there is no such voltage
   source it returns NULL. */
circuit * dcsolver::findVoltageSource (int n) {
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    if (n >= c->isVoltageSource () && 
	n <= c->isVoltageSource () + c->getVoltageSources () - 1)
      return c;
  }
  return NULL;
}

/* The function applies unique voltage source identifiers to each DC
   voltage source (explicit and built in internal ones) in the list of
   registered circuits. */
void dcsolver::assignVoltageSources (void) {
  circuit * root = subnet->getRoot ();
  int nSources = 0;
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    if (c->getVoltageSources () > 0) {
      c->setVoltageSource (nSources + 1);
      nSources += c->getVoltageSources ();
    }
  }
  subnet->setVoltageSources (nSources);
}

/* The matrix equation Ax = z is solved by x = A^-1*z.  The function
   applies the operation to the previously generated matrices. */
void dcsolver::runMNA (void) {
#if DEBUG && 0
  logprint (LOG_ERROR, "A =\n");
  A->print ();
#endif
#if DEBUG && 0
  logprint (LOG_ERROR, "z =\n");
  z->print ();
#endif
  eqnsys * e = new eqnsys ();
  e->setAlgo (ALGO_LU_DECOMPOSITION);
  e->passEquationSys (A, x, z);
  e->solve ();
  delete e;
#if DEBUG && 0
  logprint (LOG_ERROR, "x =\n");
  x->print ();
#endif
}

/* The function checks whether the iterative algorithm for linearizing
   the non-linear components in the network shows convergence.  It
   returns non-zero if it converges and zero otherwise. */
int dcsolver::checkConvergence (void) {
  int N = countNodes ();
  int M = subnet->getVoltageSources ();
  nr_double_t v_abs, v_rel, i_abs, i_rel;
  nr_double_t reltol = 1e-3;
  nr_double_t abstol = 1e-12; // 1pA
  nr_double_t vntol  = 1e-6;  // 1uV

  for (int r = 1; r <= N; r++) {
    v_abs = abs (x->get (r, 1) - xprev->get (r, 1));
    v_rel = abs (x->get (r, 1));
    if (v_abs >= vntol + reltol * v_rel) return 0;
    i_abs = abs (z->get (r, 1) - zprev->get (r, 1));
    i_rel = abs (z->get (r, 1));
    if (i_abs >= abstol + reltol * i_rel) return 0;
  }
  for (int r = 1; r <= M; r++) {
    i_abs = abs (x->get (r + N, 1) - xprev->get (r + N, 1));
    i_rel = abs (x->get (r + N, 1));
    if (i_abs >= abstol + reltol * i_rel) return 0;
  }
  return 1;
}

/* The function saves the solution and right hand vector of the previous
   iteration. */
void dcsolver::savePreviousIteration (void) {
  if (xprev != NULL) delete xprev;
  xprev = new matrix (*x);
  if (zprev != NULL) delete zprev;
  zprev = new matrix (*z);
}

/* This function goes through solution (the x vector) and saves the
   node voltages of the last iteration into each non-linear
   circuit. */
void dcsolver::saveNodeVoltages (void) {
  int N = countNodes ();
  struct nodelist_t * n;
  // save all nodes except reference node
  for (int r = 1; r <= N; r++) {
    n = nlist->getNode (r);
    for (int i = 0; i < n->nNodes; i++) {
      n->nodes[i]->getCircuit()->setV (n->nodes[i]->getPort (), x->get (r, 1));
    }
  }
  // save reference node
  n = nlist->getNode (0);
  for (int i = 0; i < n->nNodes; i++) {
    n->nodes[i]->getCircuit()->setV (n->nodes[i]->getPort (), 0.0);
  }
}


/* Goes through the list of circuit objects and runs its calcDC()
   function. */
void dcsolver::calc (void) {
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    c->calcDC ();
  }
}

/* Goes through the list of circuit objects and runs its initDC()
   function. */
void dcsolver::init (void) {
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    c->initDC (this);
  }
}

/* This function saves the results of a single solve() functionality
   into the output dataset. */
void dcsolver::saveResults (void) {
  int N = countNodes ();
  int M = subnet->getVoltageSources ();
  char * n;

  // add node voltage variables
  for (int r = 1; r <= N; r++) {
    if ((n = createV (r)) != NULL) {
      saveVariable (n, x->get (r, 1));
    }
  }

  // add branch current variables
  for (int r = 1; r <= M; r++) {
    if ((n = createI (r)) != NULL) {
      saveVariable (n, x->get (r + N, 1));
    }
  }

  // save operating points of non-linear circuits if requested
  if (saveOPs) {
    circuit * root = subnet->getRoot ();
    for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
      if (!c->isNonLinear ()) continue;
      c->calcOperatingPoints ();
      operatingpoint * p = c->getOperatingPoints ();
      for (; p != NULL; p = p->getNext ()) {
	n = createOP (c->getName (), p->getName ());
	saveVariable (n, p->getValue ());
      }
    }
  }
}

/* Saves the given variable into dataset.  Creates the dataset vector
   if necessary. */
void dcsolver::saveVariable (char * n, complex z) {
  vector * d;
  if ((d = data->findVariable (n)) == NULL) {
    d = new vector (n);
    d->setOrigin (getName ());
    data->addVariable (d);
  }
  d->add (z);
}

// Create an appropriate variable name for operating points.
char * dcsolver::createOP (char * c, char * n) {
  static char text[128];
  sprintf (text, "%s.%s", c, n);
  return text;
}

// Create an appropriate variable name for voltages.
char * dcsolver::createV (int n) {
  static char text[128];
  if (nlist->isInternal (n)) return NULL;
  sprintf (text, "%s.V", nlist->get (n));
  return text;
}

// Create an appropriate variable name for currents.
char * dcsolver::createI (int n) {
  static char text[128];
  circuit * vs = findVoltageSource (n);

  if (vs->isInternalVoltageSource ())
    return NULL; // don't output internal (helper) voltage sources
  if (vs->getType () != CIR_VDC && vs->getType () != CIR_IPROBE && !saveOPs)
    return NULL; // save only current through real voltage sources and
                 // explicit current probes

  // create appropriate current name for single/multiple voltage sources
  if (vs->getVoltageSources () > 1)
    sprintf (text, "%s.I%d", vs->getName (), n - vs->isVoltageSource () + 1);
  else
    sprintf (text, "%s.I", vs->getName ());
  return text;
}
