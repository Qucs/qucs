/*
 * cpwline.cpp - coplanar waveguide line class implementation
 *
 * Copyright (C) 2004, 2005 Vincent Habchi, F5RCS <10.50@free.fr>
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
 * $Id: cpwline.cpp,v 1.8 2005/03/14 21:59:09 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "substrate.h"
#include "constants.h"
#include "matrix.h"
#include "cpwline.h"

#ifdef __MINGW32__
# define finite(x) _finite(x)
# ifndef isnan
# define isnan(x)  _isnan(x)
# endif
# ifndef isinf
# define isinf(x)  (!_finite(x) && !_isnan(x))
# endif
#endif

cpwline::cpwline () : circuit (2) {
  type = CIR_CPWLINE;
}

/* The function computes the complete elliptic integral of first kind
   K() and the second kind E() using the arithmetic-geometric mean
   algorithm (AGM) by Abramowitz and Stegun. */
void cpwline::ellipke (nr_double_t arg, nr_double_t &k, nr_double_t &e) {
  int iMax = 16;
  if (arg == 1.0) {
    k = DBL_MAX / DBL_MIN; // infinite
    e = 0;
  }
  else if (isinf (arg) && arg < 0) {
    k = 0;
    e = DBL_MAX / DBL_MIN; // infinite
  }
  else {
    nr_double_t a, b, c, f, s, fk = 1, fe = 1, t, da = arg;
    int i;
    if (arg < 0) {
      fk = 1 / sqrt (1 - arg);
      fe = sqrt (1 - arg);
      da = -arg / (1 - arg);
    }
    a = 1;
    b = sqrt (1 - da);
    c = sqrt (da);
    f = 0.5;
    s = f * c * c;
    for (i = 0; i < iMax; i++) {
      t = (a + b) / 2;
      c = (a - b) / 2;
      b = sqrt (a * b);
      a = t;
      f *= 2;
      s += f * c * c;
      if (c / a < 1e-16) break;
    }
    if (i >= iMax) {
      k = 0; e = 0;
    }
    else {
      k = M_PI_2 / a;
      e = M_PI_2 * (1 - s) / a;
      if (arg < 0) {
	k *= fk;
	e *= fe;
      }
    }
  }
}

/* We need to know only K(k), and if possible KISS. */
nr_double_t cpwline::ellipk (nr_double_t k) {
  nr_double_t r, lost;
  ellipke (k, r, lost);
  return r;
}

void cpwline::initSP (void) {
  // allocate S-parameter matrix
  allocMatrixS ();
  // pre-compute propagation factors
  initPropagation ();
}

void cpwline::initPropagation (void) {
  // get properties of substrate and coplanar line
  nr_double_t W =  getPropertyDouble ("W");
  nr_double_t s =  getPropertyDouble ("S");
  substrate * subst = getSubstrate ();
  nr_double_t er = subst->getPropertyDouble ("er");
  nr_double_t h  = subst->getPropertyDouble ("h");
  nr_double_t t  = subst->getPropertyDouble ("t");
  char * back    = getPropertyString ("Backside");

  tand = subst->getPropertyDouble ("tand");
  rho  = subst->getPropertyDouble ("rho");
  len  = getPropertyDouble ("L");

  // other local variables (quasi-static constants)
  nr_double_t k1, kk1, kpk1, k2, q1, q2, er0 = 0;
  
  // compute the necessary quasi-static approx. (K1, K3, er(0) and Z(0))
  k1   = W / (W + s + s);
  kk1  = ellipk (k1);
  kpk1 = ellipk (sqrt (1 - k1 * k1));
  q1   = kk1 / kpk1;

  // backside is metal
  if (!strcmp (back, "Metal")) {
    nr_double_t k3, q3, qz;
    k3  = tanh ((M_PI / 4) * (W / h)) / tanh ((M_PI / 4) * (W + s + s) / h);
    q3  = ellipk (k3) / ellipk (sqrt (1 - k3 * k3));
    qz  = 1 / (q1 + q3);
    er0 = 1 + q3 * qz * (er - 1);
    zl_factor = Z0 / 2 * qz;
  }
  // backside is air
  else if (!strcmp (back, "Air")) {
    k2  = sinh ((M_PI / 4) * (W / h)) / sinh ((M_PI / 4) * (W + s + s) / h);
    q2  = ellipk (k2) / ellipk (sqrt (1 - k2 * k2));
    er0 = 1 + (er - 1) / 2 * q2 * q1;
    zl_factor = Z0 / 4 * q1;
  }

  if (t > 0) {
    // adds effect of strip thickness
    nr_double_t d;
    d = (t * 1.25 / M_PI) * (1 + log (4 * M_PI * W / t));
    s = s - d;
    W = W + d;

    // modifies k1 accordingly (k1 = ke)
    k1 = W / (W + s + s);

    // modifies er0 as well
    er0 = er0 - (0.7 * (er0 - 1) * t / s) / (q1 + (0.7 * t / s));
  }

  // pre-compute square roots
  sr_er = sqrt (er);
  sr_er0 = sqrt (er0);

  // cut-off frequency of the TE0 mode
  fte = (C0 / 4) / (h * sqrt (er - 1));

  // dispersion factor G
  nr_double_t p = log (W / h);
  nr_double_t u = 0.54 - (0.64 - 0.015 * p) * p;
  nr_double_t v = 0.43 - (0.86 - 0.54 * p) * p;
  G = exp (u * log (W / s) + v);

  // loss constant factors (computed only once for efficency sake)
  nr_double_t n  = (1 - k1) * 8 * M_PI / (t * (1 + k1)); 
  nr_double_t a  = W / 2;
  nr_double_t b  = a + s;
  nr_double_t ac = (M_PI + log (n * a)) / a + (M_PI + log (n * b)) / b;

  ac_factor  = ac / (4 * Z0 * kk1 * kpk1 * (1 - k1 * k1));
  ac_factor *= sqrt (M_PI * MU0 * rho); // Rs factor
  ad_factor  = (er / (er - 1)) * tand * M_PI / C0;

  bt_factor  = 2 * M_PI / C0;
}

void cpwline::calcAB (nr_double_t f, nr_double_t& zl, nr_double_t& al,
		      nr_double_t& bt) {
  nr_double_t sr_er_f = sr_er0;
  nr_double_t ac = ac_factor;
  nr_double_t ad = ad_factor;

  // by initializing as much as possible outside this function, the
  // overhead is minimal

  // add the dispersive effects to er0
  sr_er_f += (sr_er - sr_er0) / (1 + G * pow (f / fte, -1.8));

  // computes impedance
  zl /= sr_er_f;

  // for now, the loss are limited to strip losses (no radiation
  // losses yet) losses in neper/length
  ad *= f * (sr_er_f - 1 / sr_er_f);
  ac *= sqrt (f) * sr_er_f;

  al  = ac + ad;
  bt *= sr_er_f * f;
}

void cpwline::calcSP (nr_double_t frequency) {

  nr_double_t zl = zl_factor;
  nr_double_t beta = bt_factor;
  nr_double_t alpha;

  calcAB (frequency, zl, alpha, beta);

  // calculate and set S-parameters
  nr_double_t z = zl / z0;
  nr_double_t y = 1 / z;
  complex g = rect (alpha, beta);
  complex n = 2 * cosh (g * len) + (z + y) * sinh (g * len);
  complex s11 = (z - y) * sinh (g * len) / n;
  complex s21 = 2 / n;

  setS (1, 1, s11); setS (2, 2, s11);
  setS (1, 2, s21); setS (2, 1, s21);
}

void cpwline::calcNoiseSP (nr_double_t) {
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  matrix s = getMatrixS ();
  matrix e = eye (getSize ());
  setMatrixN (kelvin (T) / T0 * (e - s * transpose (conj (s))));
}

void cpwline::initDC (void) {
  // a DC short (voltage source V = 0 volts)
  setVoltageSources (1);
  setInternalVoltageSource (1);
  allocMatrixMNA ();
  clearY ();
  voltageSource (1, 1, 2);
}

void cpwline::initTR (void) {
  initDC ();
}

void cpwline::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
  initPropagation ();
}

void cpwline::calcAC (nr_double_t frequency) {

  nr_double_t zl = zl_factor;
  nr_double_t beta = bt_factor;
  nr_double_t alpha;

  calcAB (frequency, zl, alpha, beta);

  // calculate and set Y-parameters
  complex g = rect (alpha, beta);
  complex y11 = coth (g * len) / zl;
  complex y21 = -cosech (g * len) / zl;

  setY (1, 1, y11); setY (2, 2, y11);
  setY (1, 2, y21); setY (2, 1, y21);
}

void cpwline::calcNoiseAC (nr_double_t) {
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  setMatrixN (4 * kelvin (T) / T0 * real (getMatrixY ()));
}
