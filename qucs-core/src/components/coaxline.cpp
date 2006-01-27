/*
 * coaxline.cpp - coaxial cable class implementation
 *
 * Copyright (C) 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: coaxline.cpp,v 1.1 2006/01/27 09:32:01 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "complex.h"
#include "logging.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "matrix.h"
#include "component_id.h"
#include "constants.h"
#include "coaxline.h"

coaxline::coaxline () : circuit (2) {
  alpha = beta = zl = fc = 0;
  type = CIR_COAXLINE;
}

void coaxline::calcPropagation (nr_double_t frequency) {
  nr_double_t er   = getPropertyDouble ("er");
  nr_double_t mur  = getPropertyDouble ("mur");
  nr_double_t rho  = getPropertyDouble ("rho");
  nr_double_t tand = getPropertyDouble ("tand");
  nr_double_t d    = getPropertyDouble ("d");
  nr_double_t D    = getPropertyDouble ("D");
  nr_double_t ad, ac, rs;

  // check cutoff frequency
  if (frequency > fc) {
    logprint (LOG_ERROR, "WARNING: Operating frequency (%g) beyond "
	      "cutoff frequency (%g).\n", frequency, fc);
  }

  // calculate losses
  ad = M_PI / C0 * frequency * sqrt (er) * tand;
  rs = sqrt (M_PI * frequency * mur * MU0 * rho);
  ac = sqrt (er) / 2 * (1 / d + 1 / D) / log (D / d) * rs / Z0;

  // calculate propagation constants and reference impedance
  alpha = ac + ad;
  beta  = sqrt (er * mur) * 2 * M_PI * frequency / C0;
  zl = Z0 / 2 / M_PI / sqrt (er) * log (D / d);
}

void coaxline::calcNoiseSP (nr_double_t) {
  nr_double_t l = getPropertyDouble ("L");
  if (l < 0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  matrix s = getMatrixS ();
  matrix e = eye (getSize ());
  setMatrixN (kelvin (T) / T0 * (e - s * transpose (conj (s))));
}

void coaxline::initCheck (void) {
  nr_double_t d = getPropertyDouble ("d");
  nr_double_t D = getPropertyDouble ("D");
  // check validity
  if (d >= D) {
    logprint (LOG_ERROR,
	      "ERROR: Inner diameter larger than outer diameter.\n");
  }
  nr_double_t f1, f2;
  f1 = C0 / (M_PI * (D + d)); // TE
  f2 = C0 / (2 * (D - d));    // TM
  fc = MIN (f1, f2);
}

void coaxline::initSP (void) {
  // allocate S-parameter matrix
  allocMatrixS ();
  initCheck ();
}

void coaxline::calcSP (nr_double_t frequency) {
  nr_double_t l = getPropertyDouble ("L");

  // calculate propagation constants
  calcPropagation (frequency);

  // calculate S-parameters
  nr_double_t z = zl / z0;
  nr_double_t y = 1 / z;
  complex g = rect (alpha, beta);
  complex n = 2 * cosh (g * l) + (z + y) * sinh (g * l);
  complex s11 = (z - y) * sinh (g * l) / n;
  complex s21 = 2 / n;
  setS (NODE_1, NODE_1, s11); setS (NODE_2, NODE_2, s11);
  setS (NODE_1, NODE_2, s21); setS (NODE_2, NODE_1, s21);
}

void coaxline::initDC (void) {
  nr_double_t l   = getPropertyDouble ("L");
  nr_double_t d   = getPropertyDouble ("d");
  nr_double_t rho = getPropertyDouble ("rho");

  if (d != 0.0 && rho != 0.0 && l != 0.0) {
    // a tiny resistance
    nr_double_t g = M_PI * sqr (d / 2) / rho / l;
    setVoltageSources (0);
    allocMatrixMNA ();
    setY (NODE_1, NODE_1, +g); setY (NODE_2, NODE_2, +g);
    setY (NODE_1, NODE_2, -g); setY (NODE_2, NODE_1, -g);
  }
  else {
    // a DC short
    setVoltageSources (1);
    setInternalVoltageSource (1);
    allocMatrixMNA ();
    voltageSource (VSRC_1, NODE_1, NODE_2);
  }
}

void coaxline::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
  initCheck ();
}

void coaxline::calcAC (nr_double_t frequency) {
  nr_double_t l = getPropertyDouble ("L");

  // calculate propagation constants
  calcPropagation (frequency);

  // calculate Y-parameters
  complex g = rect (alpha, beta);
  complex y11 = coth (g * l) / zl;
  complex y21 = -cosech (g * l) / zl;
  setY (NODE_1, NODE_1, y11); setY (NODE_2, NODE_2, y11);
  setY (NODE_1, NODE_2, y21); setY (NODE_2, NODE_1, y21);
}

void coaxline::calcNoiseAC (nr_double_t) {
  nr_double_t l = getPropertyDouble ("L");
  if (l < 0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  setMatrixN (4 * kelvin (T) / T0 * real (getMatrixY ()));
}
