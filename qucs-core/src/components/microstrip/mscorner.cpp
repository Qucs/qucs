/*
 * mscorner.cpp - microstrip corner class implementation
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2004 Michael Margraf <Michael.Margraf@alumni.TU-Berlin.DE>
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
 * $Id: mscorner.cpp,v 1.11 2004-11-24 19:15:52 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "substrate.h"
#include "constants.h"
#include "matrix.h"
#include "mscorner.h"

mscorner::mscorner () : circuit (2) {
  type = CIR_MSCORNER;
}

void mscorner::initSP (void) {

  // allocate S-parameter matrix
  allocMatrixS ();

  // get properties of substrate and corner
  nr_double_t W = getPropertyDouble ("W");
  substrate * subst = getSubstrate ();
  nr_double_t er = subst->getPropertyDouble ("er");
  h = subst->getPropertyDouble ("h");

  // local variables
  nr_double_t Wh = W/h;

  // check validity
  if ((Wh < 0.2) || (Wh > 6.0)) {
    logprint (LOG_ERROR, "WARNING: Model for microstrip corner defined for "
	      "0.2 <= W/h <= 6.0 (W/h = %g)\n", Wh);
  }
  if ((er < 2.36) || (er > 10.4)) {
    logprint (LOG_ERROR, "WARNING: Model for microstrip corner defined for "
	      "2.36 <= er <= 10.4 (er = %g)\n", er);
  }

  // capacitance in pF
  C = W * ((10.35 * er + 2.5) * Wh + (2.6 * er + 5.64));
  // inductivity in nH
  L = 220.0 * h * (1.0 - 1.35 * exp (-0.18 * pow (Wh, 1.39)));
}

void mscorner::calcSP (nr_double_t frequency) {
  setMatrixS (ztos (calcMatrixZ (frequency)));
}

matrix mscorner::calcMatrixZ (nr_double_t frequency) {
  // check frequency validity
  if (frequency * h > 12e6) {
    logprint (LOG_ERROR, "WARNING: Model for microstrip corner defined for "
	      "freq*h <= 12MHz (is %g)\n", frequency * h);
  }

  // create Z-parameter matrix
  matrix z (2);
  complex z21 = rect (0.0, -0.5e12 / (M_PI * frequency * C));
  complex z11 = rect (0.0, 2e-9 * M_PI * frequency * L) + z21;
  z.set (1, 1, z11);
  z.set (1, 2, z21);
  z.set (2, 1, z21);
  z.set (2, 2, z11);
  return z;
}

void mscorner::initDC (void) {
  // a DC short (voltage source V = 0 volts)
  setVoltageSources (1);
  setInternalVoltageSource (1);
  allocMatrixMNA ();
  clearY ();
  voltageSource (1, 1, 2);
}

void mscorner::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
  initSP ();
}

void mscorner::calcAC (nr_double_t frequency) {
  setMatrixY (ztoy (calcMatrixZ (frequency)));
}
