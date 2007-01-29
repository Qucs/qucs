/*
 * twistedpair.cpp - twisted pair line class implementation
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
 * $Id: twistedpair.cpp,v 1.2 2007-01-29 19:14:17 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "constants.h"
#include "matrix.h"
#include "twistedpair.h"

twistedpair::twistedpair () : circuit (4) {
  type = CIR_TWISTEDPAIR;
}

void twistedpair::calcSP (nr_double_t frequency) {
  nr_double_t l = getPropertyDouble ("L");
  calcPropagation (frequency);

  complex g = rect (alpha, beta);
  nr_double_t p = 2 * z0 + zl;
  nr_double_t n = 2 * z0 - zl;
  complex e = exp (2 * g * l);
  complex d = p * p * e - n * n;

  complex s11 = zl * (p * e + n) / d;
  complex s14 = 1.0 - s11;
  complex s12 = 4.0 * zl * z0 * exp (g * l) / d;

  setS (NODE_1, NODE_1, +s11); setS (NODE_2, NODE_2, +s11);
  setS (NODE_3, NODE_3, +s11); setS (NODE_4, NODE_4, +s11);
  setS (NODE_1, NODE_4, +s14); setS (NODE_4, NODE_1, +s14);
  setS (NODE_2, NODE_3, +s14); setS (NODE_3, NODE_2, +s14);
  setS (NODE_1, NODE_2, +s12); setS (NODE_2, NODE_1, +s12);
  setS (NODE_3, NODE_4, +s12); setS (NODE_4, NODE_3, +s12);
  setS (NODE_1, NODE_3, -s12); setS (NODE_3, NODE_1, -s12);
  setS (NODE_2, NODE_4, -s12); setS (NODE_4, NODE_2, -s12);
}

void twistedpair::initDC (void) {
  setVoltageSources (2);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
  voltageSource (VSRC_2, NODE_3, NODE_4);
}

void twistedpair::initAC (void) {
  nr_double_t l = getPropertyDouble ("L");
  if (l != 0.0) {
    setVoltageSources (0);
    allocMatrixMNA ();
  } else {
    setVoltageSources (2);
    allocMatrixMNA ();
    voltageSource (VSRC_1, NODE_1, NODE_2);
    voltageSource (VSRC_2, NODE_3, NODE_4);
  }
}

void twistedpair::calcPropagation (nr_double_t frequency) {
  nr_double_t d  = getPropertyDouble ("d");
  nr_double_t D  = getPropertyDouble ("D");
  nr_double_t er = getPropertyDouble ("er");
  nr_double_t T  = getPropertyDouble ("T");
  nr_double_t l  = getPropertyDouble ("L");

  nr_double_t q, p;
  p = atan (T * M_PI * D);
  q = 0.25 + 0.001 * p * p;  // soft PTFE
  q = 0.25 + 0.0004 * p * p; // usual
  ereff = 1.0 + q * (er - 1.0);
  zl = Z0 / M_PI / sqrt (ereff) * acosh (D / d);
  beta = 2 * M_PI * frequency / C0 * sqrt (ereff);
  alpha = 0;
  angle = deg (p);
  len = l * T * M_PI * D * sqrt (1 + 1 / sqr (T * M_PI * D));
}

void twistedpair::saveCharacteristics (nr_double_t) {
  setCharacteristic ("Zl", zl);
  setCharacteristic ("Er", ereff);
  setCharacteristic ("Length", len);
  setCharacteristic ("Angle", angle);
}

void twistedpair::calcAC (nr_double_t frequency) {
  nr_double_t l = getPropertyDouble ("L");
  if (l != 0.0) {
    calcPropagation (frequency);
    complex g = rect (alpha, beta);
    complex y11 = coth (g * l) / zl;
    complex y21 = -cosech (g * l) / zl;
    setY (NODE_1, NODE_1, +y11); setY (NODE_2, NODE_2, +y11);
    setY (NODE_3, NODE_3, +y11); setY (NODE_4, NODE_4, +y11);
    setY (NODE_1, NODE_4, -y11); setY (NODE_4, NODE_1, -y11);
    setY (NODE_2, NODE_3, -y11); setY (NODE_3, NODE_2, -y11);
    setY (NODE_1, NODE_2, +y21); setY (NODE_2, NODE_1, +y21);
    setY (NODE_3, NODE_4, +y21); setY (NODE_4, NODE_3, +y21);
    setY (NODE_1, NODE_3, -y21); setY (NODE_3, NODE_1, -y21);
    setY (NODE_2, NODE_4, -y21); setY (NODE_4, NODE_2, -y21);
  }
}

void twistedpair::initTR (void) {
  initDC ();
}
