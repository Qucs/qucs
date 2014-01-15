/*
 * mslange.cpp - parallel coupled microstrip lines class implementation
 *
 * Copyright (C) 2004, 2005, 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: mslange.cpp,v 1.25 2008/10/07 20:15:33 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "component.h"
#include "substrate.h"
#include "msline.h"
#include "mslange.h"

mslange::mslange () : circuit (4) {
  type = CIR_MSLANGE;
}

void mslange::calcPropagation (nr_double_t frequency) {

  // fetch line properties
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
  msline::analyseLoss (W, t, er, rho, D, tand, Zle, Zlo, ErEffe,
		       frequency, "Hammerstad", ace, ade);
  msline::analyseLoss (W, t, er, rho, D, tand, Zlo, Zle, ErEffo,
		       frequency, "Hammerstad", aco, ado);

  // compute propagation constants for even and odd mode
  nr_double_t k0 = 2 * M_PI * frequency / C0;
  ae = ace + ade;
  ao = aco + ado;
  be = sqrt (ErEffeFreq) * k0;
  bo = sqrt (ErEffoFreq) * k0;
  ze = ZleFreq;
  zo = ZloFreq;
  ee = ErEffeFreq;
  eo = ErEffoFreq;
}
  
void mslange::saveCharacteristics (nr_double_t) {
  setCharacteristic ("ZlEven", ze);
  setCharacteristic ("ErEven", ee);
  setCharacteristic ("ZlOdd", zo);
  setCharacteristic ("ErOdd", eo);
}

void mslange::calcSP (nr_double_t frequency) {
  // fetch line properties
  nr_double_t l = getPropertyDouble ("L");

  // compute propagation constants for even and odd mode
  calcPropagation (frequency);
  nr_complex_t ge = nr_complex_t (ae, be);
  nr_complex_t go = nr_complex_t (ao, bo);

  // compute abbreviations
  nr_complex_t Ee, Eo, De, Do, Xe, Xo, Ye, Yo;
  Ee = (sqr (ze) + sqr (z0)) * sinh (ge * l);
  Eo = (sqr (zo) + sqr (z0)) * sinh (go * l);
  De = 2 * ze * z0 * cosh (ge * l) + Ee;
  Do = 2 * zo * z0 * cosh (go * l) + Eo;
  Xe = (sqr (ze) - sqr (z0)) * sinh (ge * l) / 2.0 / De;
  Xo = (sqr (zo) - sqr (z0)) * sinh (go * l) / 2.0 / Do;
  Ye = ze * z0 / De;
  Yo = zo * z0 / Do;

  // reflexion coefficients
  setS (NODE_1, NODE_1, Xe + Xo); setS (NODE_2, NODE_2, Xe + Xo);
  setS (NODE_3, NODE_3, Xe + Xo); setS (NODE_4, NODE_4, Xe + Xo);
  // through paths
  setS (NODE_1, NODE_2, Ye + Yo); setS (NODE_2, NODE_1, Ye + Yo);
  setS (NODE_3, NODE_4, Ye + Yo); setS (NODE_4, NODE_3, Ye + Yo);
  // coupled paths
  setS (NODE_1, NODE_4, Xe - Xo); setS (NODE_4, NODE_1, Xe - Xo);
  setS (NODE_2, NODE_3, Xe - Xo); setS (NODE_3, NODE_2, Xe - Xo);
  // isolated paths
  setS (NODE_1, NODE_3, Ye - Yo); setS (NODE_3, NODE_1, Ye - Yo);
  setS (NODE_2, NODE_4, Ye - Yo); setS (NODE_4, NODE_2, Ye - Yo);
}

void mslange::calcNoiseSP (nr_double_t) {
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  matrix s = getMatrixS ();
  matrix e = eye (getSize ());
  setMatrixN (kelvin (T) / T0 * (e - s * transpose (conj (s))));
}

/* The function calculates the quasi-static dielectric constants and
   characteristic impedances for the even and odd mode based upon the
   given line and substrate properties for parallel coupled microstrip
   lines. */
void mslange::analysQuasiStatic (nr_double_t W, nr_double_t h, nr_double_t s,
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
    Pe = Phi / (Psi * (Alpha * pow (u, m) + (1 - Alpha) * pow (u, -m)));
    // TODO: is this ... Psi * (Alpha ... or ... Psi / (Alpha ... ?

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
    fo = fo1 * exp (p * log (u) + q * sin (M_PI * log10 (u)));

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
      // SCHNEIDER, referred by JANSEN
      if (u >= M_1_PI / 2 && M_1_PI / 2 > 2 * t / h)
	dW = t * (1 + log (2 * h / t)) / M_PI;
      else if (W > 2 * t)
	dW = t * (1 + log (4 * M_PI * W / t)) / M_PI;
      // JANSEN
      nr_double_t dt = 2 * t * h / s / er;
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
//  nr_double_t Zle4, Zlo4, C, Z04;
  
  Zle = ((Zlo+Zle)/(3*Zlo+Zle))*Zle;	//Pozar - Microwave engineering: eq 7.79a
  Zlo = ((Zlo+Zle)/(3*Zle+Zlo))*Zlo;	//Pozar - Microwave engineering: eq 7.79b
  
}

/* The function computes the dispersion effects on the dielectric
   constants and characteristic impedances for the even and odd mode
   of parallel coupled microstrip lines. */
void mslange::analyseDispersion (nr_double_t W, nr_double_t h, nr_double_t s,
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

    // dispersion of even characteristic impedance
    nr_double_t t, q11, q12, q13, q14, q15, q16, q17, q18, q19, q20, q21;
    q11 = 0.893 * (1 - 0.3 / (1 + 0.7 * (er - 1)));
    t = pow (fn / 20, 4.91);
    q12 = 2.121 * t / (1 + q11 * t) * exp (-2.87 * g) * pow (g, 0.902);
    q13 = 1 + 0.038 * pow (er / 8, 5.1);
    t = quadr (er / 15);
    q14 = 1 + 1.203 * t / (1 + t);
    q15 = 1.887 * exp (-1.5 * pow (g, 0.84)) * pow (g, q14) /
      (1 + 0.41 * pow (fn / 15, 3) *
       pow (u, 2 / q13) / (0.125 + pow (u, 1.626 / q13)));
    q16 = q15 * (1 + 9 / (1 + 0.403 * sqr (er - 1)));
    q17 = 0.394 * (1 - exp (-1.47 * pow (u / 7, 0.672))) *
      (1 - exp (-4.25 * pow (fn / 20, 1.87)));
    q18 = 0.61 * (1 - exp (-2.31 * pow (u / 8, 1.593))) /
      (1 + 6.544 * pow (g, 4.17));
    q19 = 0.21 * quadr (g) / (1 + 0.18 * pow (g, 4.9)) / (1 + 0.1 * sqr (u)) /
      (1 + pow (fn / 24, 3));
    q20 = q19 * (0.09 + 1 / (1 + 0.1 * pow (er - 1, 2.7)));
    t = pow (u, 2.5);
    q21 = fabs (1 - 42.54 * pow (g, 0.133) * exp (-0.812 * g) * t /
		(1 + 0.033 * t));
    
    nr_double_t re, qe, pe, de, Ce, q0, ZlFreq, ErEffFreq;
    msline::Kirschning_er (u, fn, er, ErEffe, ErEffFreq);
    msline::Kirschning_zl (u, fn, er, ErEffe, ErEffFreq, Zle, q0, ZlFreq);
    re = pow (fn / 28.843, 12);
    qe = 0.016 + pow (0.0514 * er * q21, 4.524);
    pe = 4.766 * exp (-3.228 * pow (u, 0.641));
    t = pow (er - 1, 6);
    de = 5.086 * qe * re / (0.3838 + 0.386 * qe) *
      exp (-22.2 * pow (u, 1.92)) / (1 + 1.2992 * re) * t / (1 + 10 * t);
    Ce = 1 + 1.275 * (1 - exp (-0.004625 * pe * pow (er, 1.674) * 
	 pow (fn / 18.365, 2.745))) - q12 + q16 - q17 + q18 + q20;
    ZleFreq = Zle * pow ((0.9408 * pow (ErEffFreq, Ce) - 0.9603) /
			 ((0.9408 - de) * pow (ErEffe, Ce) - 0.9603), q0);
    
    // dispersion of odd characteristic impedance
    nr_double_t q22, q23, q24, q25, q26, q27, q28, q29;
    msline::Kirschning_er (u, fn, er, ErEffo, ErEffFreq);
    msline::Kirschning_zl (u, fn, er, ErEffo, ErEffFreq, Zlo, q0, ZlFreq);
    q29 = 15.16 / (1 + 0.196 * sqr (er - 1));
    t = sqr (er - 1);
    q25 = 0.3 * sqr (fn) / (10 + sqr (fn)) * (1 + 2.333 * t / (5 + t));
    t = pow ((er - 1) / 13, 12);
    q26 = 30 - 22.2 * t / (1 + 3 * t) - q29;
    t = pow (er - 1, 1.5);
    q27 = 0.4 * pow (g, 0.84) * (1 + 2.5 * t / (5 + t));
    t = pow (er - 1, 3);
    q28 = 0.149 * t / (94.5 + 0.038 * t);
    q22 = 0.925 * pow (fn / q26, 1.536) / (1 + 0.3 * pow (fn / 30, 1.536));
    q23 = 1 + 0.005 * fn * q27 / (1 + 0.812 * pow (fn / 15, 1.9)) /
      (1 + 0.025 * sqr (u));
    t = pow (u, 0.894);
    q24 = 2.506 * q28 * t / (3.575 + t) *
      pow ((1 + 1.3 * u) * fn / 99.25, 4.29);
    ZloFreq = ZlFreq + (Zlo * pow (ErEffoFreq / ErEffo, q22) - ZlFreq * q23) /
      (1 + q24 + pow (0.46 * g, 2.2) * q25);
    
  }
}

void mslange::initDC (void) {
  nr_double_t l     = getPropertyDouble ("L");
  nr_double_t W     = getPropertyDouble ("W")/2;
  substrate * subst = getSubstrate ();
  nr_double_t t     = subst->getPropertyDouble ("t");
  nr_double_t rho   = subst->getPropertyDouble ("rho");

  if (t != 0.0 && rho != 0.0) {
    // tiny resistances
    nr_double_t g = t * W / rho / l;
    setVoltageSources (0);
    allocMatrixMNA ();
    setY (NODE_1, NODE_1, +g); setY (NODE_2, NODE_2, +g);
    setY (NODE_1, NODE_2, -g); setY (NODE_2, NODE_1, -g);
    setY (NODE_3, NODE_3, +g); setY (NODE_4, NODE_4, +g);
    setY (NODE_3, NODE_4, -g); setY (NODE_4, NODE_3, -g);
  }
  else {
    // DC shorts (voltage sources V = 0 volts)
    setVoltageSources (2);
    setInternalVoltageSource (1);
    allocMatrixMNA ();
    clearY ();
    voltageSource (VSRC_1, NODE_1, NODE_2);
    voltageSource (VSRC_2, NODE_3, NODE_4);
    setD (VSRC_1, VSRC_2, 0.0); setD (VSRC_2, VSRC_1, 0.0);
  }
}

void mslange::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
}

void mslange::calcAC (nr_double_t frequency) {
  // fetch line properties
  nr_double_t l = getPropertyDouble ("L");

  // compute propagation constants for even and odd mode
  calcPropagation (frequency);
  nr_complex_t ge = nr_complex_t (ae, be);
  nr_complex_t go = nr_complex_t (ao, bo);

  // compute abbreviations
  nr_complex_t De, Do, y1, y2, y3, y4;
  De = 0.5 / (ze * sinh (ge * l));
  Do = 0.5 / (zo * sinh (go * l));
  y2 = -De - Do;
  y3 = -De + Do;
  De *= cosh (ge * l);
  Do *= cosh (go * l);
  y1 = De + Do;
  y4 = De - Do;

  // store Y-parameters
  setY (NODE_1, NODE_1, y1); setY (NODE_2, NODE_2, y1);
  setY (NODE_3, NODE_3, y1); setY (NODE_4, NODE_4, y1);
  setY (NODE_1, NODE_2, y2); setY (NODE_2, NODE_1, y2);
  setY (NODE_3, NODE_4, y2); setY (NODE_4, NODE_3, y2);
  setY (NODE_1, NODE_3, y3); setY (NODE_2, NODE_4, y3);
  setY (NODE_3, NODE_1, y3); setY (NODE_4, NODE_2, y3);
  setY (NODE_1, NODE_4, y4); setY (NODE_2, NODE_3, y4);
  setY (NODE_3, NODE_2, y4); setY (NODE_4, NODE_1, y4);
}

void mslange::calcNoiseAC (nr_double_t) {
  // calculate noise using Bosma's theorem
  nr_double_t T = getPropertyDouble ("Temp");
  setMatrixN (4 * kelvin (T) / T0 * real (getMatrixY ()));
}

// properties
PROP_REQ [] = {
  { "W", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "L", PROP_REAL, { 10e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "S", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_POS_RANGE },
  { "Subst", PROP_STR, { PROP_NO_VAL, "Subst1" }, PROP_NO_RANGE },
  { "Model", PROP_STR, { PROP_NO_VAL, "Kirschning" },
    PROP_RNG_STR2 ("Kirschning", "Hammerstad") },
  { "DispModel", PROP_STR, { PROP_NO_VAL, "Kirschning" },
    PROP_RNG_STR2 ("Kirschning", "Getsinger") },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };
struct define_t mslange::cirdef =
  { "MLANGE", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
