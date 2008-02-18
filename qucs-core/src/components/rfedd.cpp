/*
 * rfedd.cpp - equation defined RF device class implementation
 *
 * Copyright (C) 2008 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: rfedd.cpp,v 1.2 2008-02-18 18:03:55 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#if !HAVE_STRCHR
# define strchr  index
# define strrchr rindex
#endif

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "net.h"
#include "component_id.h"
#include "equation.h"
#include "environment.h"
#include "constants.h"
#include "rfedd.h"

using namespace eqn;

// Constructor for the equation defined RF device.
rfedd::rfedd () : circuit () {
  type = CIR_RFEDD;
  setVariableSized (true);
  peqn = NULL;
}

// Destructor deletes equation defined RF device object from memory.
rfedd::~rfedd () {
  if (peqn) free (peqn);
}

// Callback for initializing the DC analysis.
void rfedd::initDC (void) {

  // get appropriate property value
  char * dc = getPropertyString ("duringDC");

  // a short during DC
  if (!strcmp (dc, "short")) {
    int v, n, lastnode = getSize () - 1;
    setVoltageSources (lastnode);
    allocMatrixMNA ();
    // place zero voltage sources
    for (v = VSRC_1, n = NODE_1; n < lastnode; n++, v++) {
      voltageSource (v, n, lastnode);
    }
    return;
  }
  // an open during DC
  else if (!strcmp (dc, "open")) {
    setVoltageSources (0);
    allocMatrixMNA ();
    return;
  }
  // none specified, DC value of IDFT ?
  else {
    setVoltageSources (0);
    allocMatrixMNA ();
  }
}

// Some help macros.
#define A(a)  ((assignment *) (a))

// Creates a parameter variable name from the given arguments.
char * rfedd::createVariable (const char * base, int r, int c, bool pfx) {
  char * str = strchr (getName (), '.');
  if (str != NULL)
    str = strrchr (str, '.') + 1;
  else
    str = getName ();
  char * txt = (char *) malloc (strlen (str) + strlen (base) + 4);
  if (pfx)
    sprintf (txt, "%s.%s%d%d", str, base, r, c);
  else
    sprintf (txt, "%s%d%d", base, r, c);
  return txt;
}

// Creates a variable name from the given arguments.
char * rfedd::createVariable (const char * base, bool pfx) {
  char * str = strchr (getName (), '.');
  if (str != NULL)
    str = strrchr (str, '.') + 1;
  else
    str = getName ();
  char * txt = (char *) malloc (strlen (str) + strlen (base) + 2);
  if (pfx)
    sprintf (txt, "%s.%s", str, base);
  else
    sprintf (txt, "%s", base);
  return txt;
}

// Saves the given real value into the equation result.
void rfedd::setResult (void * eqn, nr_double_t val) {
  A(eqn)->evaluate ();
  constant * c = A(eqn)->getResult ();
  c->d = val;
}

// Saves the given complex value into the equation result.
void rfedd::setResult (void * eqn, nr_complex_t val) {
  A(eqn)->evaluate ();
  constant * c = A(eqn)->getResult ();
  *(c->c) = val;
}

// Returns the result of the equation.
nr_complex_t rfedd::getResult (void * eqn) {
  A(eqn)->evaluate ();
  return A(eqn)->getResultComplex ();
}

// Initializes the equation defined device.
void rfedd::initModel (void) {
  int i, j, k, ports = getSize ();
  char * type, * pn, * sn, * snold, * fn, * fnold, * vr;
  eqn::node * pvalue;

  // allocate space for equation pointers
  peqn = (void **) malloc (sizeof (assignment *) * ports * ports);

  // first create frequency variables
  sn = createVariable ("S");
  snold = createVariable ("S", false);
  fn = createVariable ("F");
  fnold = createVariable ("F", false);
  seqn = getEnv()->getChecker()->addComplex ("#laplace", sn, rect (0, 0));
  feqn = getEnv()->getChecker()->addDouble ("#frequency", fn, 0);
  A(seqn)->evalType (); A(seqn)->skip = 1;
  A(feqn)->evalType (); A(feqn)->skip = 1;

  // obtain type of parameters
  type = getPropertyString ("Type");

  // prepare device equations
  for (k = 0, i = 0; i < ports; i++) {
    for (j = 0; j < ports; j++, k++) {
      // find equation referenced in property
      pn = createVariable ("P", i + 1, j + 1, false);
      vr = getPropertyReference (pn);
      pvalue = getEnv()->getChecker()->findEquation (vr);
      if (!pvalue) {
	logprint (LOG_ERROR, "ERROR: %s-parameter equation `%s' not found for "
		  "RFEDD `%s'\n", type, vr, getName ());
      }
      else {
	// replace references to S and F by local references
	pvalue->replace (snold, sn);
	pvalue->replace (fnold, fn);
	// evaluate types of parameters
	A(pvalue)->evalType ();
	A(pvalue)->skip = 1;
      }
      // save equations for parameters
      peqn[k] = pvalue;
      free (pn);
    }
  }

  free (sn); free (snold);
  free (fn); free (fnold);
}

// Update local variable equations.
void rfedd::updateLocals (nr_double_t frequency) {

  // update frequency variables for equations
  setResult (seqn, rect (0, 2 * M_PI * frequency));
  setResult (feqn, frequency);

  // get local subcircuit values
  getEnv()->passConstants ();
  getEnv()->equationSolver ();
}

// Callback for DC analysis.
void rfedd::calcDC (void) {
}

// Callback for initializing the AC analysis.
void rfedd::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
}

// Callback for AC analysis.
void rfedd::calcAC (nr_double_t frequency) {
  char * type = getPropertyString ("Type");
  switch (type[0]) {
  case 'Y':
    setMatrixY (calcMatrix (frequency));
    break;
  case 'Z':
    setMatrixY (ztoy (calcMatrix (frequency)));
    break;
  case 'S':
    setMatrixY (stoy (calcMatrix (frequency)));
    break;
  }
}

// Computes parameter matrix.
matrix rfedd::calcMatrix (nr_double_t frequency) {
  int i, j, k, ports = getSize ();
  matrix p (ports);

  // update local equations
  updateLocals (frequency);

  // calculate parameters and put into Jacobian
  for (k = 0, i = 0; i < ports; i++) {
    for (j = 0; j < ports; j++, k++) {
      p (i, j) = getResult (peqn[k]);
    }
  }

  return p;
}

// Callback for initializing the TR analysis.
void rfedd::initTR (void) {
  initDC ();
}

// Callback for the TR analysis.
void rfedd::calcTR (nr_double_t) {
  calcDC ();
}

// Callback for initializing the S-parameter analysis.
void rfedd::initSP (void) {
  allocMatrixS ();
  if (peqn == NULL) initModel ();
}

// Callback for S-parameter analysis.
void rfedd::calcSP (nr_double_t frequency) {
  char * type = getPropertyString ("Type");
  switch (type[0]) {
  case 'Y':
    setMatrixS (ytos (calcMatrix (frequency)));
    break;
  case 'Z':
    setMatrixS (ztos (calcMatrix (frequency)));
    break;
  case 'S':
    setMatrixS (calcMatrix (frequency));
    break;
  }
}
