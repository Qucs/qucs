/*
 * mscoupled.cpp - parallel coupled microstrip lines class implementation
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
 * $Id: mscoupled.cpp,v 1.8 2004/09/02 08:02:15 ela Exp $
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
#include "msline.h"
#include "mscoupled.h"

mscoupled::mscoupled () : circuit (4) {
  type = CIR_MSCOUPLED;
}

void mscoupled::calcSP (nr_double_t frequency) {

  // fetch line properties
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t W = getPropertyDouble ("W");
  nr_double_t s = getPropertyDouble ("S");
  char * SModel = getPropertyString ("Model");
  char * DModel = getPropertyString ("DispModel");

  // fetch substrate properties
  substrate * subst = getSubstrate ();
  nr_double_t er    = subst->getPropertyDouble ("er");
  nr_double_t h     = subst->getPropertyDouble ("h");
  nr_double_t t     = subst->getPropertyDouble ("t");
  nr_double_t tand  = subst->getPropertyDouble ("tand");
  nr_double_t rho   = subst->getPropertyDouble ("rho");
  nr_double_t D     = subst->getPropertyDouble ("D");

  // quasi-static analysis
  nr_double_t Zle, ErEffe, Zlo, ErEffo;
  analysQuasiStatic (W, h, s, t, er, SModel, Zle, Zlo, ErEffe, ErEffo);

  // analyse dispersion of Zl and Er
  nr_double_t ZleFreq, ErEffeFreq, ZloFreq, ErEffoFreq;
  analyseDispersion (W, h, s, er, Zle, Zlo, ErEffe, ErEffo, frequency, DModel,
		     ZleFreq, ZloFreq, ErEffeFreq, ErEffoFreq);

  // analyse losses of line
  nr_double_t ace, aco, ade, ado;
  msline::analyseLoss (W, t, er, rho, D, tand, ZleFreq, ZloFreq, ErEffeFreq,
		       frequency, "Hammerstad", ace, ade);
  msline::analyseLoss (W, t, er, rho, D, tand, ZloFreq, ZleFreq, ErEffoFreq,
		       frequency, "Hammerstad", aco, ado);

  // compute propagation constants for even and odd mode
  complex ge, go;
  nr_double_t ae, ao, be, bo, k0;
  k0 = 2 * M_PI * frequency / C0;
  ae = ace + ade;
  ao = aco + ado;
  be = sqrt (ErEffeFreq) * k0;
  bo = sqrt (ErEffoFreq) * k0;
  ge = rect (ae, be);
  go = rect (ao, bo);
  
  // compute abbreviations
  complex Ee, Eo, De, Do, Xe, Xo, Ye, Yo;
  Ee = (sqr (ZleFreq) + sqr (z0)) * sinh (ge * l);
  Eo = (sqr (ZloFreq) + sqr (z0)) * sinh (go * l);
  De = 2 * ZleFreq * z0 * cosh (ge * l) + Ee;
  Do = 2 * ZloFreq * z0 * cosh (go * l) + Eo;
  Xe = (sqr (ZleFreq) - sqr (z0)) * sinh (ge * l) / 2 / De;
  Xo = (sqr (ZloFreq) - sqr (z0)) * sinh (go * l) / 2 / Do;
  Ye = Zle * z0 / De;
  Yo = Zlo * z0 / Do;

  // reflexion coefficients
  setS (1, 1, Xe + Xo); setS (2, 2, Xe + Xo);
  setS (3, 3, Xe + Xo); setS (4, 4, Xe + Xo);
  // through paths
  setS (1, 2, Ye + Yo); setS (2, 1, Ye + Yo);
  setS (3, 4, Ye + Yo); setS (4, 3, Ye + Yo);
  // coupled paths
  setS (1, 4, Xe - Xo); setS (4, 1, Xe - Xo);
  setS (2, 3, Xe - Xo); setS (3, 2, Xe - Xo);
  // isolated paths
  setS (1, 3, Ye - Yo); setS (3, 1, Ye - Yo);
  setS (2, 4, Ye - Yo); setS (4, 2, Ye - Yo);
}

/* The function calculates the quasi-static dielectric constants and
   characteristic impedances for the even and odd mode based upon the
   given line and substrate properties for parallel coupled microstrip
   lines. */
void mscoupled::analysQuasiStatic (nr_double_t W, nr_double_t h, nr_double_t s,
				   nr_double_t t, nr_double_t er,
				   char * SModel, nr_double_t& Zle,
				   nr_double_t& Zlo, nr_double_t& ErEffe,
				   nr_double_t& ErEffo) {
  // initialize default return values
  ErEffe = ErEffo = er;
  Zlo = 42.2; Zle = 55.7;

  // normalized width and gap
  nr_double_t u = W / h;
  nr_double_t g = s / h;

  // HAMMERSTAD and JENSEN
  if (!strcmp (SModel, "Hammerstad")) {
    nr_double_t Zl1, Fe, Fo, a, b, fo, Mu, Alpha, Beta, ErEff;
    nr_double_t Pe, Po, r, fo1, q, p, n, Psi, Phi, m, Theta;

    // modifying equations for even mode
    m = 0.2175 + pow (4.113 + pow (20.36 / g, 6), -0.251) +
      log (pow (g, 10) / (1 + pow (g / 13.8, 10))) / 323;
    Alpha = 0.5 * exp (-g);
    Psi = 1 + g / 1.45 + pow (g, 2.09) / 3.95;
    Phi = 0.8645 * pow (u, 0.172);
    Pe = Phi / (Psi / (Alpha * pow (u, m) + (1 - Alpha) * pow (u, -m)));

    // modifying equations for odd mode
    n = (1 / 17.7 + exp (-6.424 - 0.76 * log (g) - pow (g / 0.23, 5))) *
      log ((10 + 68.3 * sqr (g)) / (1 + 32.5 * pow (g, 3.093)));
    Beta = 0.2306 + log (pow (g, 10) / (1 + pow (g / 3.73, 10))) / 301.8 +
      log (1 + 0.646 * pow (g, 1.175)) / 5.3;
    Theta = 1.729 + 1.175 * log (1 + 0.627 / (g + 0.327 * pow (g, 2.17)));
    Po = Pe - Theta / Psi * exp (Beta * pow (u, -n) * log (u));

    // further modifying equations
    r = 1 + 0.15 * (1 - exp (1 - sqr (er - 1) / 8.2) / (1 + pow (g, -6)));
    fo1 = 1 - exp (-0.179 * pow (g, 0.15) -
		   0.328 * pow (g, r) / log (M_E + pow (g / 7, 2.8)));
    q = exp (-1.366 - g);
    p = exp (-0.745 * pow (g, 0.295)) / cosh (pow (g, 0.68));
    fo = fo1 * exp (p * log (u) + q * sin (M_PI * log (u) / M_LN10));

    Mu = g * exp (-g) + u * (20 + sqr (g)) / (10 + sqr (g));
    msline::Hammerstad_ab (Mu, er, a, b);
    Fe = pow (1 + 10 / Mu, -a * b);
    msline::Hammerstad_ab (u, er, a, b);
    Fo = fo * pow (1 + 10 / u, -a * b);

    // finally compute effective dielectric constants and impedances
    ErEffe = (er + 1) / 2 + (er - 1) / 2 * Fe;
    ErEffo = (er + 1) / 2 + (er - 1) / 2 * Fo;

    msline::Hammerstad_er (u, er, a, b, ErEff);  // single microstrip

    // first variant
    Zl1 = Z0 / (u + 1.98 * pow (u, 0.172));
    Zl1 /= sqrt (ErEff);

    // second variant
    msline::Hammerstad_zl (u, Zl1);
    Zl1 /= sqrt (ErEff);

    Zle = Zl1 / (1 - Zl1 * Pe / Z0);
    Zlo = Zl1 / (1 - Zl1 * Po / Z0);
  }
  // KIRSCHNING and JANSEN
  else if (!strcmp (SModel, "Kirschning")) {
    nr_double_t a, b, ae, be, ao, bo, v, co, d, ErEff, Zl1;
    nr_double_t q1, q2, q3, q4, q5, q6, q7, q8, q9, q10;

    // consider effect of finite strip thickness (JANSEN only)
    nr_double_t ue = u;
    nr_double_t uo = u;
    if (t != 0 && s > 10 * (2 * t)) {
      nr_double_t dW = 0;
      if (u >= M_1_PI / 2 && M_1_PI / 2 > 2 * t)
	dW = t * (1 + log (2 * h / t)) / M_PI;
      else if (W > 2 * t)
	dW = t * (1 + log (4 * M_PI * W / t)) / M_PI;
      nr_double_t dt = t * h / s / er;
      nr_double_t We = W + dW * (1 - 0.5 * exp (-0.69 * dW / dt));
      nr_double_t Wo = We + dt;
      ue = We / h;
      uo = Wo / h;
    }
    
    // even relative dielectric constant
    v = ue * (20 + sqr (g)) / (10 + sqr (g)) + g * exp (-g);
    msline::Hammerstad_ab (v, er, ae, be);
    msline::Hammerstad_er (v, er, ae, be, ErEffe);

    // odd relative dielectric constant
    msline::Hammerstad_ab (uo, er, a, b);
    msline::Hammerstad_er (uo, er, a, b, ErEff);
    d = 0.593 + 0.694 * exp (-0.562 * uo);
    bo = 0.747 * er / (0.15 + er);
    co = bo - (bo - 0.207) * exp (-0.414 * uo);
    ao = 0.7287 * (ErEff - (er + 1) / 2) * (1 - exp (-0.179 * uo));
    ErEffo = ((er + 1) / 2 + ao - ErEff) * exp (-co * pow (g, d)) + ErEff;

    // characteristic impedance of single line
    msline::Hammerstad_zl (u, Zl1);
    Zl1 /= sqrt (ErEff);

    // even characteristic impedance
    q1 = 0.8695 * pow (ue, 0.194);
    q2 = 1 + 0.7519 * g + 0.189 * pow (g, 2.31);
    q3 = 0.1975 + pow (16.6 + pow (8.4 / g, 6), -0.387) +
      log (pow (g, 10) / (1 + pow (g / 3.4, 10))) / 241;
    q4 = q1 / q2 * 2 /
      (exp (-g) * pow (ue, q3) + (2 - exp (-g)) * pow (ue, -q3));
    Zle = sqrt (ErEff / ErEffe) * Zl1 / (1 - Zl1 * sqrt (ErEff) * q4 / Z0);

    // odd characteristic impedance
    q5 = 1.794 + 1.14 * log (1 + 0.638 / (g + 0.517 * pow (g, 2.43)));
    q6 = 0.2305 + log (pow (g, 10) / (1 + pow (g / 5.8, 10))) / 281.3 +
      log (1 + 0.598 * pow (g, 1.154)) / 5.1;
    q7 = (10 + 190 * sqr (g)) / (1 + 82.3 * cubic (g));
    q8 = exp (-6.5 - 0.95 * log (g) - pow (g / 0.15, 5));
    q9 = log (q7) * (q8 + 1 / 16.5);
    q10 = (q2 * q4 - q5 * exp (log (uo) * q6 * pow (uo, -q9))) / q2;
    Zlo = sqrt (ErEff / ErEffo) * Zl1 / (1 - Zl1 * sqrt (ErEff) * q10 / Z0);
  }
}

/* The function computes the dispersion effects on the dielectric
   constants and characteristic impedances for the even and odd mode
   of parallel coupled microstrip lines. */
void mscoupled::analyseDispersion (nr_double_t W, nr_double_t h, nr_double_t s,
				   nr_double_t er, nr_double_t Zle,
				   nr_double_t Zlo, nr_double_t ErEffe,
				   nr_double_t ErEffo, nr_double_t frequency,
				   char * DModel, nr_double_t& ZleFreq,
				   nr_double_t& ZloFreq,
				   nr_double_t& ErEffeFreq,
				   nr_double_t& ErEffoFreq) {

  // initialize default return values
  ZleFreq = Zle;
  ErEffeFreq = ErEffe;
  ZloFreq = Zlo;
  ErEffoFreq = ErEffo;

  // normalized width and gap
  nr_double_t u = W / h;
  nr_double_t g = s / h;

  // GETSINGER
  if (!strcmp (DModel, "Getsinger")) {
    // even mode dispersion
    msline::Getsinger_disp (h, er, ErEffe, Zle / 2,
			    frequency, ErEffeFreq, ZleFreq);
    ZleFreq *= 2;
    // odd mode dispersion
    msline::Getsinger_disp (h, er, ErEffo, Zlo * 2,
			    frequency, ErEffoFreq, ZloFreq);
    ZloFreq /= 2;
  }
  // KIRSCHNING and JANSEN
  else if (!strcmp (DModel, "Kirschning")) {
    nr_double_t p1, p2, p3, p4, p5, p6, p7, Fe;
    nr_double_t fn = frequency * h * 1e-6;

    // even relative dielectric constant dispersion
    p1 = 0.27488 * (0.6315 + 0.525 / pow (1 + 0.0157 * fn, 20)) * u -
      0.065683 * exp (-8.7513 * u);
    p2 = 0.33622 * (1 - exp (-0.03442 * er));
    p3 = 0.0363 * exp (-4.6 * u) * (1 - exp (- pow (fn / 38.7, 4.97)));
    p4 = 1 + 2.751 * (1 - exp (- pow (er / 15.916, 8)));
    p5 = 0.334 * exp (-3.3 * cubic (er / 15)) + 0.746;
    p6 = p5 * exp (- pow (fn / 18, 0.368));
    p7 = 1 + 4.069 * p6 * pow (g, 0.479) *
      exp (-1.347 * pow (g, 0.595) - 0.17 * pow (g, 2.5));
    Fe = p1 * p2 * pow ((p3 * p4 + 0.1844 * p7) * fn, 1.5763);
    ErEffeFreq = er - (er - ErEffe) / (1 + Fe);

    // odd relative dielectric constant dispersion
    nr_double_t p8, p9, p10, p11, p12, p13, p14, p15, Fo;
    p8 = 0.7168 * (1 + 1.076 / (1 + 0.0576 * (er - 1)));
    p9 = p8 - 0.7913 * (1 - exp (- pow (fn / 20, 1.424))) *
      atan (2.481 * pow (er / 8, 0.946));
    p10 = 0.242 * pow (er - 1, 0.55);
    p11 = 0.6366 * (exp (-0.3401 * fn) - 1) *
      atan (1.263 * pow (u / 3, 1.629));
    p12 = p9 + (1 - p9) / (1 + 1.183 * pow (u, 1.376));
    p13 = 1.695 * p10 / (0.414 + 1.605 * p10);
    p14 = 0.8928 + 0.1072 * (1 - exp (-0.42 * pow (fn / 20, 3.215)));
    p15 = fabs (1 - 0.8928 * (1 + p11) *
		exp (-p13 * pow (g, 1.092)) * p12 / p14);
    Fo = p1 * p2 * pow ((p3 * p4 + 0.1844) * fn * p15, 1.5763);
    ErEffoFreq = er - (er - ErEffo) / (1 + Fo);
  }
}

void mscoupled::initDC (dcsolver *) {
  nr_double_t l     = getPropertyDouble ("L");
  nr_double_t W     = getPropertyDouble ("W");
  substrate * subst = getSubstrate ();
  nr_double_t t     = subst->getPropertyDouble ("t");
  nr_double_t rho   = subst->getPropertyDouble ("rho");

  if (t != 0.0 && rho != 0.0) {
    // tiny resistances
    nr_double_t g = t * W / rho / l;
    setY (1, 1, +g); setY (2, 2, +g); setY (1, 2, -g); setY (2, 1, -g);
    setY (3, 3, +g); setY (4, 4, +g); setY (3, 4, -g); setY (4, 3, -g);
    setVoltageSources (0);
  }
  else {
    // DC shorts (voltage sources V = 0 volts)
    setY (1, 1, 0); setY (2, 2, 0); setY (1, 2, 0); setY (2, 1, 0);
    setY (3, 3, 0); setY (4, 4, 0); setY (3, 4, 0); setY (4, 3, 0);
    setC (1, 1, +1.0); setC (1, 2, -1.0); setC (2, 3, +1.0); setC (2, 4, -1.0);
    setB (1, 1, +1.0); setB (2, 1, -1.0); setB (3, 2, +1.0); setB (4, 2, -1.0);
    setE (1, 0.0); setE (2, 0.0);
    setD (1, 1, 0.0); setD (1, 2, 0.0); setD (2, 1, 0.0); setD (2, 2, 0.0);
    setVoltageSources (2);
    setInternalVoltageSource (1);
  }
}
