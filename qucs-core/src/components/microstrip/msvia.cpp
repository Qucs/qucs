/*
 * msvia.cpp - microstrip via hole class implementation
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
 * $Id: msvia.cpp,v 1.4 2005-01-17 12:19:03 raimi Exp $
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
#include "msvia.h"

msvia::msvia () : circuit (2) {
  R = 0;
  Z = 0;
  type = CIR_MSVIA;
}

void msvia::calcNoiseSP (nr_double_t) {
  // calculate noise correlation matrix
  nr_double_t r = real (Z);
  nr_double_t T = getPropertyDouble ("Temp");
  nr_double_t f = kelvin (T) * 4.0 * r * z0 / sqr (2.0 * z0 + r) / T0;
  setN (1, 1, +f); setN (2, 2, +f);
  setN (1, 2, -f); setN (2, 1, -f);
}

void msvia::initSP (void) {
  allocMatrixS ();
  R = calcResistance ();
}

void msvia::calcSP (nr_double_t frequency) {
  // calculate s-parameters
  Z = calcImpedance (frequency);
  complex z = Z / z0;
  setS (1, 1, z / (z + 2));
  setS (2, 2, z / (z + 2));
  setS (1, 2, 2 / (z + 2));
  setS (2, 1, 2 / (z + 2));
}

complex msvia::calcImpedance (nr_double_t frequency) {
  // fetch substrate and component properties
  substrate * subst = getSubstrate ();
  nr_double_t h   = subst->getPropertyDouble ("h");
  nr_double_t t   = subst->getPropertyDouble ("t");
  nr_double_t rho = subst->getPropertyDouble ("rho");
  nr_double_t r   = getPropertyDouble ("D") / 2;

  // check frequency validity
  if (frequency * h >= 0.03 * C0) {
    logprint (LOG_ERROR, "WARNING: Model for microstrip via hole defined for "
	      "freq/C0*h < 0.03 (is %g)\n", frequency / C0 * h);
  }

  // create Z-parameter
  nr_double_t fs  = M_PI * MU0 * rho * sqr (t);
  nr_double_t res = R * sqrt (1 + frequency * fs);
  nr_double_t a   = sqrt (sqr (r) + sqr (h));
  nr_double_t ind = MU0 * (h * log ((h + a) / r) + 1.5 * (r - a));
  return Z = rect (res, frequency * ind);
}

nr_double_t msvia::calcResistance (void) {
  // fetch substrate and component properties
  substrate * subst = getSubstrate ();
  nr_double_t h   = subst->getPropertyDouble ("h");
  nr_double_t t   = subst->getPropertyDouble ("t");
  nr_double_t rho = subst->getPropertyDouble ("rho");
  nr_double_t r   = getPropertyDouble ("D") / 2;
  nr_double_t v   = M_PI * (sqr (r) - sqr (r - t)) * h;
  return R = rho * v;
}

void msvia::initDC (void) {
  nr_double_t r = calcResistance ();

  // for non-zero resistances usual MNA entries
  if (r != 0.0) {
    nr_double_t g = 1.0 / r;
    setVoltageSources (0);
    allocMatrixMNA ();
    setY (1, 1, +g); setY (2, 2, +g);
    setY (1, 2, -g); setY (2, 1, -g);
  }
  // for zero resistances create a zero voltage source
  else {
    setVoltageSources (1);
    setInternalVoltageSource (1);
    allocMatrixMNA ();
    clearY ();
    voltageSource (1, 1, 2);
  }
}

void msvia::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
  initSP ();
}

void msvia::calcAC (nr_double_t frequency) {
  complex y = 1 / calcImpedance (frequency);
  setY (1, 1, +y); setY (2, 2, +y);
  setY (1, 2, -y); setY (2, 1, -y);
}
