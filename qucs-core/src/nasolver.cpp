/*
 * nasolver.cpp - nodal analysis solver class implementation
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
 * $Id: nasolver.cpp,v 1.1 2004/09/06 06:40:07 ela Exp $
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
#include "tmatrix.h"
#include "eqnsys.h"
#include "component_id.h"
#include "operatingpoint.h"
#include "exception.h"
#include "exceptionstack.h"
#include "nasolver.h"

using namespace qucs;

// Constructor creates an unnamed instance of the nasolver class.
template <class nr_type_t>
nasolver<nr_type_t>::nasolver () : analysis () {
  nlist = NULL;
  A = z = x = NULL;
  xprev = zprev = NULL;
  reltol = abstol = vntol = 0;
  desc = NULL;
}

// Constructor creates a named instance of the nasolver class.
template <class nr_type_t>
nasolver<nr_type_t>::nasolver (char * n) : analysis (n) {
  nlist = NULL;
  A = z = x = NULL;
  xprev = zprev = NULL;
  reltol = abstol = vntol = 0;
  desc = NULL;
}

// Destructor deletes the nasolver class object.
template <class nr_type_t>
nasolver<nr_type_t>::~nasolver () {
  if (nlist) delete nlist;
  delete A;
  delete z;
  delete x;
  delete xprev;
  delete zprev;
}

/* The copy constructor creates a new instance of the nasolver class
   based on the given nasolver object. */
template <class nr_type_t>
nasolver<nr_type_t>::nasolver (nasolver & o) : analysis (o) {
  nlist = o.nlist ? new nodelist (*(o.nlist)) : NULL;
  A = o.A ? new tmatrix<nr_type_t> (*(o.A)) : NULL;
  z = o.z ? new tmatrix<nr_type_t> (*(o.z)) : NULL;
  x = o.x ? new tmatrix<nr_type_t> (*(o.x)) : NULL;
  xprev = zprev = NULL;
  reltol = o.reltol;
  abstol = o.abstol;
  vntol = o.vntol;
  desc = o.desc;
}

/* The function runs the nodal analysis solver once, reports errors if
   any and save the results into each circuit. */
template <class nr_type_t>
int nasolver<nr_type_t>::solve_once (void) {
  qucs::exception * e;
  int error = 0, d;

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
    d = top_exception()->getData (); pop_exception ();
    if (d > countNodes ()) {
      d -= countNodes ();
      e->setText ("voltage source `%s' conflicts with some other voltage "
		  "source", findVoltageSource(d)->getName ());
    }
    else {
      e->setText ("circuit admittance matrix in %s solver is singular at "
		  "node `%s' connected to [%s]", desc, nlist->get (d),
		  nlist->getNodeString (d));
    }
    throw_exception (e);
    error++;
    break;
  default:
    estack.print ();
    break;
  }

  // save results into circuits
  if (!error) saveNodeVoltages ();
  return error;
}

/* Run this function after the actual solver run and before evaluating
   the results. */
template <class nr_type_t>
void nasolver<nr_type_t>::solve_post (void) {
  delete nlist; nlist = NULL;
}

/* Run this function before the actual solver. */
template <class nr_type_t>
void nasolver<nr_type_t>::solve_pre (void) {
  // create node list, enumerate nodes and voltage sources
#if DEBUG
  logprint (LOG_STATUS, "NOTIFY: %s: creating node list for %s analysis\n",
	    getName (), desc);
#endif
  nlist = new nodelist (subnet);
  nlist->assignNodes ();
  assignVoltageSources ();
#if DEBUG && 0
  nlist->print ();
#endif

#if DEBUG
  logprint (LOG_STATUS, "NOTIFY: %s: solving %s netlist\n", getName (), desc);
#endif
}

/* This is the non-linear iterative nodal analysis netlist solver. */
template <class nr_type_t>
void nasolver<nr_type_t>::solve_nonlinear (void) {
  qucs::exception * e;
  int convergence, run = 0, MaxIterations, error = 0;

  // fetch simulation properties
  MaxIterations = getPropertyInteger ("MaxIter");
  reltol = getPropertyDouble ("reltol");
  abstol = getPropertyDouble ("abstol");
  vntol  = getPropertyDouble ("vntol");

  // run solving loop until convergence is reached
  do {
    error = solve_once ();
    if (!error) {
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
    e->setText ("no convergence in %s analysis after %d iterations",
		desc, run);
    throw_exception (e);
  }
#if DEBUG
  else {
    logprint (LOG_STATUS, "NOTIFY: convergence reached after %d iterations\n",
	      run);
  }
#endif /* DEBUG */
}

/* This is the linear nodal analysis netlist solver. */
template <class nr_type_t>
void nasolver<nr_type_t>::solve_linear (void) {
  solve_once ();
}

/* Applying the MNA (Modified Nodal Analysis) to a circuit with
   passive elements and independent current and voltage sources
   results in a matrix equation of the form Ax = z.  This function
   generates the A and z matrix. */
template <class nr_type_t>
void nasolver<nr_type_t>::createMatrix (void) {
  int M = subnet->getVoltageSources ();
  int N = countNodes ();

  /* Generate the A matrix.  The A matrix consists of four (4) minor
     matrices in the form     +-   -+
                          A = | G B |
                              | C D |
			      +-   -+.
     Each of these minor matrices is going to be generated here. */
  if (A != NULL) delete A;
  A = new tmatrix<nr_type_t> (M + N);
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
  z = new tmatrix<nr_type_t> (N + M, 1);
  createIMatrix ();
  createEMatrix ();

  // Create empty solution vector.
  if (x == NULL) x = new tmatrix<nr_type_t> (N + M, 1);
}

/* This MatVal() functionality is just helper to get the correct
   values from the circuit's matrices.  The additional (unsused)
   argument is used to differentiate between the two possible
   types. */
#define MatVal(x) MatValX (x, (nr_type_t *) 0)

template <class nr_type_t>
nr_type_t nasolver<nr_type_t>::MatValX (complex z, complex *) {
    return z;
}

template <class nr_type_t>
nr_type_t nasolver<nr_type_t>::MatValX (complex z, nr_double_t *) {
    return real (z);
}

/* The B matrix is an MxN matrix with only 0, 1 and -1 elements.  Each
   location in the matrix corresponds to a particular voltage source
   (first dimension) or a node (second dimension).  If the positive
   terminal of the ith voltage source is connected to node k, then the
   element (i,k) in the B matrix is a 1.  If the negative terminal of
   the ith voltage source is connected to node k, then the element
   (i,k) in the B matrix is a -1.  Otherwise, elements of the B matrix
   are zero. */
template <class nr_type_t>
void nasolver<nr_type_t>::createBMatrix (void) {
  int N = countNodes ();
  int M = subnet->getVoltageSources ();
  circuit * vs;
  struct nodelist_t * n;
  nr_type_t val;

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
	  val = MatVal (vs->getB (n->nodes[i]->getPort (), c));
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
template <class nr_type_t>
void nasolver<nr_type_t>::createCMatrix (void) {
  int N = countNodes ();
  int M = subnet->getVoltageSources ();
  circuit * vs;
  struct nodelist_t * n;
  nr_type_t val;

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
	  val = MatVal (vs->getC (r, n->nodes[i]->getPort ()));
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
template <class nr_type_t>
void nasolver<nr_type_t>::createDMatrix (void) {
  int M = subnet->getVoltageSources ();
  int N = countNodes ();
  circuit * vsr, * vsc;
  nr_type_t val;
  for (int r = 1; r <= M; r++) {
    vsr = findVoltageSource (r);
    for (int c = 1; c <= M; c++) {
      vsc = findVoltageSource (c);
      val = 0.0;
      if (vsr == vsc) {
	val = MatVal (vsr->getD (r, c));
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
template <class nr_type_t>
void nasolver<nr_type_t>::createGMatrix (void) {
  int pr, pc, N = countNodes ();
  nr_type_t g;
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
	  g += MatVal (cir->getY (pr, pc));
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
	      g += MatVal (cir->getY (pr, pc));
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
template <class nr_type_t>
void nasolver<nr_type_t>::createIMatrix (void) {
  int N = countNodes ();
  nr_type_t val;
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
      if (is->getType () == CIR_IDC || is->getType () == CIR_IAC ||
	  is->getType () == CIR_PAC || is->isNonLinear ()) {
	val += MatVal (is->getI (n->nodes[i]->getPort ()));
      }
    }
    // put value into i matrix
    z->set (r, 1, val);
  }  
}

/* The e matrix is a 1xM matrix with each element of the matrix equal
   in value to the corresponding independent voltage source. */
template <class nr_type_t>
void nasolver<nr_type_t>::createEMatrix (void) {
  int N = countNodes ();
  int M = subnet->getVoltageSources ();
  nr_type_t val;
  circuit * vs;

  // go through each voltage source
  for (int r = 1; r <= M; r++) {
    vs = findVoltageSource (r);
    val = MatVal (vs->getE (r));
    // put value into e matrix
    z->set (r + N, 1, val);
  }  
}

// Returns the number of nodes in the nodelist, excluding the ground node.
template <class nr_type_t>
int nasolver<nr_type_t>::countNodes (void) {
  return nlist->length () - 1;
}

/* The function returns the voltage source circuit object
   corresponding to the given number.  If there is no such voltage
   source it returns NULL. */
template <class nr_type_t>
circuit * nasolver<nr_type_t>::findVoltageSource (int n) {
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    if (n >= c->isVoltageSource () && 
	n <= c->isVoltageSource () + c->getVoltageSources () - 1)
      return c;
  }
  return NULL;
}

/* The function applies unique voltage source identifiers to each
   voltage source (explicit and built in internal ones) in the list of
   registered circuits. */
template <class nr_type_t>
void nasolver<nr_type_t>::assignVoltageSources (void) {
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
template <class nr_type_t>
void nasolver<nr_type_t>::runMNA (void) {
#if DEBUG && 0
  logprint (LOG_ERROR, "A =\n");
  A->print ();
#endif
#if DEBUG && 0
  logprint (LOG_ERROR, "z =\n");
  z->print ();
#endif
  eqnsys<nr_type_t> * e = new eqnsys<nr_type_t> ();
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
template <class nr_type_t>
int nasolver<nr_type_t>::checkConvergence (void) {
  int N = countNodes ();
  int M = subnet->getVoltageSources ();
  nr_double_t v_abs, v_rel, i_abs, i_rel;

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
    v_abs = abs (z->get (r + N, 1) - zprev->get (r + N, 1));
    v_rel = abs (z->get (r + N, 1));
    if (v_abs >= vntol + reltol * v_rel) return 0;
  }
  return 1;
}

/* The function saves the solution and right hand vector of the previous
   iteration. */
template <class nr_type_t>
void nasolver<nr_type_t>::savePreviousIteration (void) {
  if (xprev != NULL) delete xprev;
  xprev = new tmatrix<nr_type_t> (*x);
  if (zprev != NULL) delete zprev;
  zprev = new tmatrix<nr_type_t> (*z);
}

/* This function goes through solution (the x vector) and saves the
   node voltages of the last iteration into each non-linear
   circuit. */
template <class nr_type_t>
void nasolver<nr_type_t>::saveNodeVoltages (void) {
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


/* This function saves the results of a single solve() functionality
   into the output dataset. */
template <class nr_type_t>
void nasolver<nr_type_t>::saveResults (char * volts, char * amps, int saveOPs,
				       vector * f) {
  int N = countNodes ();
  int M = subnet->getVoltageSources ();
  char * n;

  // add node voltage variables
  for (int r = 1; r <= N; r++) {
    if ((n = createV (r, volts)) != NULL) {
      saveVariable (n, x->get (r, 1), f);
      free (n);
    }
  }

  // add branch current variables
  for (int r = 1; r <= M; r++) {
    if ((n = createI (r, amps, saveOPs)) != NULL) {
      saveVariable (n, x->get (r + N, 1), f);
      free (n);
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
	saveVariable (n, p->getValue (), f);
	free (n);
      }
    }
  }
}

/* Saves the given variable into dataset.  Creates the dataset vector
   if necessary. */
template <class nr_type_t>
void nasolver<nr_type_t>::saveVariable (char * n, nr_type_t z, vector * f) {
  vector * d;
  if ((d = data->findVariable (n)) == NULL) {
    d = new vector (n);
    if (f != NULL) {
      d->setDependencies (new strlist ());
      d->getDependencies()->add (f->getName ());
    }
    d->setOrigin (getName ());
    data->addVariable (d);
  }
  d->add (z);
}

/* Create an appropriate variable name for operating points.  The
   caller is responsible to free() the returned string. */
template <class nr_type_t>
char * nasolver<nr_type_t>::createOP (char * c, char * n) {
  char * text = (char *) malloc (strlen (c) + strlen (n) + 2);
  sprintf (text, "%s.%s", c, n);
  return text;
}

/* Creates an appropriate variable name for voltages.  The caller is
   responsible to free() the returned string. */
template <class nr_type_t>
char * nasolver<nr_type_t>::createV (int n, char * volts) {
  if (nlist->isInternal (n)) return NULL;
  char * node = nlist->get (n);
  char * text = (char *) malloc (strlen (node) + 2 + strlen (volts));
  sprintf (text, "%s.%s", node, volts);
  return text;
}

/* Create an appropriate variable name for currents.  The caller is
   responsible to free() the returned string. */
template <class nr_type_t>
char * nasolver<nr_type_t>::createI (int n, char * amps, int saveOPs) {
  circuit * vs = findVoltageSource (n);

  // don't output internal (helper) voltage sources
  if (vs->isInternalVoltageSource ())
    return NULL;

  /* save only current through real voltage sources and explicit
     current probes */
  int type = vs->getType ();
  if (type != CIR_VDC && type != CIR_VAC && type != CIR_IPROBE && !saveOPs)
    return NULL;
                 
  // create appropriate current name for single/multiple voltage sources
  char * name = vs->getName ();
  char * text = (char *) malloc (strlen (name) + 4 + strlen (amps));
  if (vs->getVoltageSources () > 1)
    sprintf (text, "%s.%s%d", name, amps, n - vs->isVoltageSource () + 1);
  else
    sprintf (text, "%s.%s", name, amps);
  return text;
}
