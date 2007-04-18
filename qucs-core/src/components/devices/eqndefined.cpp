/*
 * eqndefined.cpp - equation defined device class implementation
 *
 * Copyright (C) 2007 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: eqndefined.cpp,v 1.1 2007/04/18 19:18:02 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
#include "device.h"
#include "eqndefined.h"

using namespace eqn;

// Constructor for the equation defined device.
eqndefined::eqndefined () : circuit () {
  type = CIR_EQNDEFINED;
  setVariableSized (true);
  veqn = NULL;
  ieqn = NULL;
  qeqn = NULL;
  geqn = NULL;
  ceqn = NULL;
  _jstat = NULL;
  _jdyna = NULL;
  _charges = NULL;
}

// Destructor deletes equation defined device object from memory.
eqndefined::~eqndefined () {
  if (veqn) free (veqn);
  if (ieqn) free (ieqn);
  if (geqn) free (geqn);
  if (qeqn) free (qeqn);
  if (ceqn) free (ceqn);
  if (_jstat) free (_jstat);
  if (_jdyna) free (_jdyna);
  if (_charges) free (_charges);
}

// Callback for initializing the DC analysis.
void eqndefined::initDC (void) {
  allocMatrixMNA ();
  if (ieqn == NULL) initModel ();
}

// Some help macros.
#define A(a)  ((assignment *) (a))
#define C(c)  ((constant *) (c))
#define BP(n) real (getV (n * 2 + 0) - getV (n * 2 + 1))

// Creates a variable name from the given arguments.
char * eqndefined::createVariable (char * base, int n, bool prefix) {
  char * txt = (char *) malloc (strlen (getName ()) + strlen (base) + 3);
  if (prefix)
    sprintf (txt, "%s.%s%d", getName (), base, n);
  else
    sprintf (txt, "%s%d", base, n);
  return txt;
}

// Creates also a variable name from the given arguments.
char * eqndefined::createVariable (char * base, int r, int c, bool prefix) {
  char * txt = (char *) malloc (strlen (getName ()) + strlen (base) + 3);
  if (prefix)
    sprintf (txt, "%s.%s%d%d", getName (), base, r, c);
  else
    sprintf (txt, "%s%d%d", base, r, c);
  return txt;
}

// Saves the given value into the equation result.
void eqndefined::setResult (void * eqn, nr_double_t val) {
  A(eqn)->evaluate ();
  constant * c = A(eqn)->getResult ();
  c->d = val;
}

// Returns the result of the equation.
nr_double_t eqndefined::getResult (void * eqn) {
  A(eqn)->evaluate ();
  constant * c = A(eqn)->getResult ();
  return c->d;
}

// Initializes the equation defined device.
void eqndefined::initModel (void) {
  int i, j, s = getSize () / 2;
  char * in, * qn, * vn, * gn, * cn, * vnold;
  eqn::node * ivalue, * qvalue, * diff;

  // allocate space for equation pointers
  veqn = (void **) malloc (sizeof (assignment *) * s);
  ieqn = (void **) malloc (sizeof (assignment *) * s);
  geqn = (void **) malloc (sizeof (assignment *) * s * s);
  qeqn = (void **) malloc (sizeof (assignment *) * s);
  ceqn = (void **) malloc (sizeof (assignment *) * s * s);

  // allocate space for Jacobians and charges
  _jstat = (nr_double_t *) malloc (sizeof (nr_double_t) * s * s);
  _jdyna = (nr_double_t *) malloc (sizeof (nr_double_t) * s * s);
  _charges = (nr_double_t *) malloc (sizeof (nr_double_t) * s);

  // first create voltage variables
  for (i = 0; i < s; i++) {
    vn = createVariable ("V", i + 1);
    veqn[i] = getEnv()->getChecker()->addDouble ("#voltage", vn, 0);
    free (vn);
  }

  // create derivatives of currents and charges
  for (i = 0; i < s; i++) {

    // fetch current and charge equations
    in = createVariable ("I", i + 1);
    ivalue = getEnv()->getChecker()->findEquation (in);
    qn = createVariable ("Q", i + 1);
    qvalue = getEnv()->getChecker()->findEquation (qn);

    // replace voltage references
    for (j = 0; j < s; j++) {
      vn = createVariable ("V", i + 1);
      vnold = createVariable ("V", i + 1, false);
      ivalue->replace (vnold, vn);
      qvalue->replace (vnold, vn);
      free (vnold);
      free (vn);
    }

    // setup and save equations for currents and charges
    ivalue->evalType ();
    qvalue->evalType ();
    ieqn[i] = ivalue;
    qeqn[i] = qvalue;
    free (in);
    free (qn);

    // create differentiations
    for (j = 0; j < s; j++) {
      vn = createVariable ("V", i + 1);

      // create conductance equations
      diff = ivalue->differentiate (vn);
      getEnv()->getChecker()->addEquation (diff);
      diff->evalType ();
      geqn[i * s + j] = diff;
      gn = createVariable ("G", i + 1, j + 1);
      A(diff)->rename (gn);
      free (gn);
#if DEBUG
      diff->print();
      fprintf (stderr, "\n");
#endif

      // create capacitance equations
      diff = qvalue->differentiate (vn);
      getEnv()->getChecker()->addEquation (diff);
      diff->evalType ();
      ceqn[i * s + j] = diff;
      cn = createVariable ("C", i + 1, j + 1);
      A(diff)->rename (cn);
      free (cn);
#if DEBUG
      diff->print();
      fprintf (stderr, "\n");
#endif
    }
  }
}

// Callback for DC analysis.
void eqndefined::calcDC (void) {
  int i, j, s = getSize () / 2;

  // update voltages for equations
  for (i = 0; i < s; i++) {
    setResult (veqn[i], BP (i));
  }

  // calculate currents and put into right-hand side
  for (i = 0; i < s; i++) {
    nr_double_t c = getResult (ieqn[i]);
    setI (i * 2 + 0, -c);
    setI (i * 2 + 1, +c);
  }

  // calculate derivatives and put into Jacobian and right-hand side
  for (i = 0; i < s; i++) {
    for (j = 0; j < s; j++) {
      nr_double_t g = getResult (geqn[i * s + j]);
      setY (i * 2 + 0, j * 2 + 0, +g);
      setY (i * 2 + 1, j * 2 + 1, +g);
      setY (i * 2 + 0, j * 2 + 1, -g);
      setY (i * 2 + 1, j * 2 + 0, -g);
      addI (i * 2 + 0, +g * BP (i));
      addI (i * 2 + 1, -g * BP (i));
    }
  }
}

// Saves operating points.
void eqndefined::saveOperatingPoints (void) {
  int i, j, s = getSize () / 2;

  // update voltages for equations
  for (i = 0; i < s; i++) {
    setResult (veqn[i], BP (i));
  }

  // save values for charges, conductances and capacitances
  for (i = 0; i < s; i++) {
    nr_double_t q = getResult (qeqn[i]);
    _charges[i] = q;
    for (j = 0; j < s; j++) {
      nr_double_t g = getResult (geqn[i * s + j]);
      _jstat[i * s + j] = g;
      nr_double_t c = getResult (ceqn[i * s + j]);
      _jdyna[i * s + j] = c;
    }
  }
}

// Callback for initializing the AC analysis.
void eqndefined::initAC (void) {
  allocMatrixMNA ();
}

// Callback for AC analysis.
void eqndefined::calcAC (nr_double_t frequency) {
  setMatrixY (calcMatrixY (frequency));
}

// Computes Y-matrix for AC analysis.
matrix eqndefined::calcMatrixY (nr_double_t frequency) {
  int i, j, s = getSize () / 2;
  matrix y (2 * s);
  nr_double_t o = 2 * M_PI * frequency;

  // merge conductances and capacitances
  for (i = 0; i < s; i++) {
    for (j = 0; j < s; j++) {
      int r = i * 2;
      int c = j * 2;
      complex val = rect (_jstat[i * s + j], o * _jdyna[i * s + j]);
      y (r + 0, c + 0) = +val;
      y (r + 1, c + 1) = +val;
      y (r + 0, c + 1) = -val;
      y (r + 1, c + 0) = -val;
    }
  }  

  return y;
}

// Callback for initializing the TR analysis.
void eqndefined::initTR (void) {
  int s = getSize () / 2;
  setStates (2 * s);
  initDC ();
}

// Callback for the TR analysis.
void eqndefined::calcTR (nr_double_t) {
  int state, i, j, s = getSize () / 2;

  // run usual DC iteration, then save operating points
  calcDC ();
  saveOperatingPoints ();

  // charge integrations
  for (i = 0; i < s; i++) {
    int r = i * 2;
    state = 2 * i;
    transientCapacitanceQ (state, r + 0, r + 1, _charges[i]);
  }

  // charge: 2-node, voltage: 2-node
  for (i = 0; i < s; i++) {
    for (j = 0; j < s; j++) {
      int r = i * 2;
      int c = j * 2;
      nr_double_t v = BP (j);
      transientCapacitanceC (r + 0, r + 1, c + 0, c + 1, _jdyna[i * s + j], v);
    }
  }
}

// Callback for initializing the S-parameter analysis.
void eqndefined::initSP (void) {
  allocMatrixS ();
}

// Callback for S-parameter analysis.
void eqndefined::calcSP (nr_double_t frequency) {
  setMatrixS (ytos (calcMatrixY (frequency)));   
}
