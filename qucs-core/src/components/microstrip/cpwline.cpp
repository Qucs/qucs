/*
 * cpwline.cpp - coplanar waveguide line class implementation
 *
 * Copyright (C) 2004 Vincent Habchi, F5RCS <10.50@free.fr>
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
 * $Id: cpwline.cpp,v 1.2 2004-12-15 19:55:32 raimi Exp $
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
#include "cpline.h"
#include "cpwline.h"

cpwline::cpwline () : circuit (2) {
  type = CIR_CPWLINE;
}

// We need to know only K(k), and if possible KISS.
nr_double_t cpwline::ellipk (nr_double_t k) {
  nr_double_t r, lost;
  cpline::ellipke (k, r, lost);
  return r;
}

void cpwline::initSP (void) {

  // allocate S-parameter matrix
  allocMatrixS ();

  // get properties of substrate and corner
  nr_double_t W =     getPropertyDouble ("W");
  nr_double_t s =     getPropertyDouble ("S");
  substrate * subst = getSubstrate ();
  nr_double_t er =    subst->getPropertyDouble ("er");
  nr_double_t h =     subst->getPropertyDouble ("h");
  nr_double_t t =     subst->getPropertyDouble ("t");

  // other local variables (quasi-static constants)
  nr_double_t k1;
  nr_double_t kk1;
  nr_double_t kpk1;
  nr_double_t q1;
  nr_double_t k3;
  nr_double_t q3;
  nr_double_t qz;
  nr_double_t er_0;
  
  // compute the necessary quasi-static approx. (K1, K3, er(0) and Z(0))
  k1 = W / (W + s + s);
  kk1 = ellipk (k1);
  kpk1 = ellipk (sqrt (1 - k1 * k1));
  q1 = kk1 / kpk1;

  k3 = tanh ((M_PI / 4) * (W / h)) / tanh ((M_PI / 4) * (W + s + s) / h);
  q3 = ellipk (k3) / ellipk (sqrt (1 - k3 * k3));

  qz = 1 / (q1 + q3);

  er_0 = 1 + q3 * qz * (er - 1);

  if (t != 0) {
    // adds effect of strip thickness
    nr_double_t d;
    d = (t * 1.25 / M_PI) * (1 + log (4 * M_PI * s / t));
    s = s - d;
    W = W + d;

    // uncertain formula, q3 addition maybe suspicious.
    er_0 = er_0 - (0.7 * (er_0 - 1) * t / W) / (q1 + q3 + (0.7 * t / W));
    er_0 = er_0 - (0.7 * (er_0 - 1) * t / W) / (q1 + (0.7 * t / W));
  }

  sr_er = sqrt (er);
  sr_er_0 = sqrt (er_0);

  // cutoff frequency of the TE0 mode
  fte = (C0 / 4) / (h * sqrt (er_0 - 1));

  // dispersion factor G
  nr_double_t p = log (W / h);
  nr_double_t u = 0.54 - (0.64 - 0.015 * p) * p;
  nr_double_t v = 0.43 - (0.86 - 0.54 * p) * p;

  G = exp (u * log (W / s) + v);

  // loss constant factors (computed only once for efficency sake)
  {
    nr_double_t m = (1 - k1) * 8 * M_PI / (t * (1 + k1)); 
    nr_double_t a = W / 2;
    nr_double_t b = a + s;
    nr_double_t ac = (M_PI + log (m * a)) / a + (M_PI + log (m * b)) / b;

    ac_factor = ac / ((480 * M_PI) * kk1 * kpk1 * (1 - k1 * k1));
    ac_factor *= sqrt (M_PI * MU0 * rho); // Rs factor

    ad_factor =  (er / (er - 1)) * tand / (M_PI * C0);
    tand = subst->getPropertyDouble ("tand");
    rho = subst->getPropertyDouble ("rho");
  }

  beta_factor = 2 * M_PI / C0;
  Z_factor = (60 * M_PI) * qz;
  l = getPropertyDouble ("L");
}

void cpwline::calcAB (nr_double_t f, nr_double_t& Z, nr_double_t& al,
		      nr_double_t& bt) {
  nr_double_t sr_er_f = sr_er_0;
  nr_double_t ac =	ac_factor;
  nr_double_t ad =	ad_factor;


  // by initializing as much as possible outside this function, the
  // overlay is minimal

  // add the dispersive effects to er_0
  sr_er_f += (sr_er - sr_er_0) / (1 + G * pow (f / fte, -1.8));

  // computes impedance
  Z /= sr_er_f;

  // for now, the loss are limited to strip losses (no radiation
  // losses yet) losses in neper/length
  if (tand != 0) 
    ad *= f * (sr_er_f - 1 / sr_er_f);
  else ad = 0;
  
  if (rho != 0) 
    ac *=  sqrt (f) * sr_er_f;
  else ac = 0;

  al = ac + ad;
  bt *= sr_er_f * f;
}

void cpwline::calcSP (nr_double_t frequency) {

  nr_double_t Z = Z_factor;
  nr_double_t alpha;
  nr_double_t beta = beta_factor;

  calcAB (frequency, Z, alpha, beta);

  // calculate S-parameters
  nr_double_t z = Z / z0;
  nr_double_t y = 1 / z;
  complex g = rect (alpha, beta);
  complex n = 2 * cosh (g * l) + (z + y) * sinh (g * l);
  complex s11 = (z - y) * sinh (g * l) / n;
  complex s21 = 2 / n;

  setS (1, 1, s11); setS (2, 2, s11);
  setS (1, 2, s21); setS (2, 1, s21);
}

void cpwline::initDC (void) {
  // a DC short (voltage source V = 0 volts)
  setVoltageSources (1);
  setInternalVoltageSource (1);
  allocMatrixMNA ();
  clearY ();
  voltageSource (1, 1, 2);
}

void cpwline::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
}

void cpwline::calcAC (nr_double_t frequency) {

  nr_double_t Z = Z_factor;
  nr_double_t alpha;
  nr_double_t beta = beta_factor;

  calcAB (frequency, Z, alpha, beta);

  // calculate Y-parameters
  complex g = rect (alpha, beta);
  complex y11 = coth (g * l) / Z;
  complex y21 = -cosech (g * l) / Z;

  setY (1, 1, y11); setY (2, 2, y11);
  setY (1, 2, y21); setY (2, 1, y21);
}
