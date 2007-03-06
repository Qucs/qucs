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
 * $Id: twistedpair.cpp,v 1.4 2007-03-06 18:21:32 ela Exp $
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

#ifdef __MINGW32__
#define acosh(x) log((x) + sqrt((x) * (x) - 1.0))
#endif

twistedpair::twistedpair () : circuit (4) {
  type = CIR_TWISTEDPAIR;
}

void twistedpair::calcSP (nr_double_t frequency) {
  calcPropagation (frequency);

  complex g = rect (alpha, beta);
  nr_double_t p = 2 * z0 + zl;
  nr_double_t n = 2 * z0 - zl;
  complex e = exp (2 * g * len);
  complex d = p * p * e - n * n;

  complex s11 = zl * (p * e + n) / d;
  complex s14 = 1.0 - s11;
  complex s12 = 4.0 * zl * z0 * exp (g * len) / d;

  setS (NODE_1, NODE_1, +s11); setS (NODE_2, NODE_2, +s11);
  setS (NODE_3, NODE_3, +s11); setS (NODE_4, NODE_4, +s11);
  setS (NODE_1, NODE_4, +s14); setS (NODE_4, NODE_1, +s14);
  setS (NODE_2, NODE_3, +s14); setS (NODE_3, NODE_2, +s14);
  setS (NODE_1, NODE_2, +s12); setS (NODE_2, NODE_1, +s12);
  setS (NODE_3, NODE_4, +s12); setS (NODE_4, NODE_3, +s12);
  setS (NODE_1, NODE_3, -s12); setS (NODE_3, NODE_1, -s12);
  setS (NODE_2, NODE_4, -s12); setS (NODE_4, NODE_2, -s12);
}

void twistedpair::calcNoiseSP (nr_double_t) {
  if (len < 0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  matrix s = getMatrixS ();
  matrix e = eye (getSize ());
  setMatrixN (kelvin (T) / T0 * (e - s * transpose (conj (s))));
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

nr_double_t twistedpair::calcLoss (nr_double_t frequency) {
  nr_double_t d    = getPropertyDouble ("d");
  nr_double_t rho  = getPropertyDouble ("rho");
  nr_double_t mur  = getPropertyDouble ("mur");
  nr_double_t tand = getPropertyDouble ("tand");

  nr_double_t delta, rout, rin, ad, ac, l0;
  // calculate conductor losses
  rout = d / 2;
  if (frequency > 0.0) {
    delta = sqrt (rho / (M_PI * frequency * MU0 * mur));
    rin = rout - delta;
    if (rin < 0.0) rin = 0.0;
  }
  else rin = 0.0;
  ac = (rho * M_1_PI) / (rout * rout - rin * rin);

  // calculate dielectric losses
  l0 = C0 / frequency;
  ad = M_PI * tand * ereff / l0;

  alpha = ac / zl + ad * zl;
  return alpha;
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
  angle = deg (p);
  len = l * T * M_PI * D * sqrt (1 + 1 / sqr (T * M_PI * D));
  alpha = calcLoss (frequency);
}

void twistedpair::saveCharacteristics (nr_double_t) {
  setCharacteristic ("Zl", zl);
  setCharacteristic ("Er", ereff);
  setCharacteristic ("Length", len);
  setCharacteristic ("Angle", angle);
}

void twistedpair::calcAC (nr_double_t frequency) {
  if (len != 0.0) {
    calcPropagation (frequency);
    complex g = rect (alpha, beta);
    complex y11 = coth (g * len) / zl;
    complex y21 = -cosech (g * len) / zl;
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

void twistedpair::calcNoiseAC (nr_double_t) {
  if (len < 0) return;
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  setMatrixN (4 * kelvin (T) / T0 * real (getMatrixY ()));
}

void twistedpair::initTR (void) {
  initDC ();
}
