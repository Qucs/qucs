/*
 * msline.cpp - microstrip transmission line class implementation
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
 * $Id: msline.cpp,v 1.30 2004/09/04 06:50:59 ela Exp $
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

msline::msline () : circuit (2) {
  alpha = zref = 0;
  type = CIR_MSLINE;
}

void msline::calcNoise (nr_double_t) {
  nr_double_t T = getPropertyDouble ("Temp");
  nr_double_t l = exp (alpha * getPropertyDouble ("L"));
  nr_double_t z = zref;
  nr_double_t r = (z - z0) / (z + z0);
  nr_double_t f = (l - 1) * (r * r - 1) / sqr (l - r * r) * kelvin (T) / T0;
  setN (1, 1, -f * (r * r + l));
  setN (2, 2, -f * (r * r + l));
  setN (1, 2, +f * 2 * r * sqrt (l));
  setN (2, 1, +f * 2 * r * sqrt (l));
}

void msline::calcSP (nr_double_t frequency) {

  /* how to get properties of this component, e.g. L, W */
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t W = getPropertyDouble ("W");
  char * SModel = getPropertyString ("Model");
  char * DModel = getPropertyString ("DispModel");

  /* how to get properties of the substrate, e.g. Er, H */
  substrate * subst = getSubstrate ();
  nr_double_t er    = subst->getPropertyDouble ("er");
  nr_double_t h     = subst->getPropertyDouble ("h");
  nr_double_t t     = subst->getPropertyDouble ("t");
  nr_double_t tand  = subst->getPropertyDouble ("tand");
  nr_double_t rho   = subst->getPropertyDouble ("rho");
  nr_double_t D     = subst->getPropertyDouble ("D");

  /* local variables */
  complex s11, s21, n, g;
  nr_double_t z, y, a, b, k0, ac, ad;
  nr_double_t ZlEff, ErEff, WEff, ZlEffFreq, ErEffFreq;

  // quasi-static effective dielectric constant of substrate + line and
  // the impedance of the microstrip line
  analyseQuasiStatic (W, h, t, er, SModel, ZlEff, ErEff, WEff);

  // analyse dispersion of Zl and Er (use WEff here?)
  analyseDispersion (WEff, h, er, ZlEff, ErEff, frequency, DModel,
		     ZlEffFreq, ErEffFreq);
  zref = ZlEffFreq;

  // analyse losses of line
  analyseLoss (WEff, t, er, rho, D, tand, ZlEffFreq, ZlEffFreq, ErEffFreq,
	       frequency, "Hammerstad", ac, ad);

  // calculate propagation constant and S-parameters
  z = ZlEffFreq / z0;
  y = 1 / z;
  k0 = 2 * M_PI * frequency / C0;
  alpha = a = ac + ad;
  b = sqrt (ErEffFreq) * k0;
  g = rect (a, b);
  n = 2 * cosh (g * l) + (z + y) * sinh (g * l);
  s11 = (z - y) * sinh (g * l) / n;
  s21 = 2 / n;

  setS (1, 1, s11);
  setS (2, 2, s11);
  setS (1, 2, s21);
  setS (2, 1, s21);
}

/* This function calculates the quasi-static impedance of a microstrip
   line, the value of the effective dielectric constant and the
   effective width due to the finite conductor thickness for the given
   microstrip line and substrate properties. */
void msline::analyseQuasiStatic (nr_double_t W, nr_double_t h, nr_double_t t,
				 nr_double_t er, char * Model,
				 nr_double_t& ZlEff, nr_double_t& ErEff,
				 nr_double_t& WEff) {

  nr_double_t z, e;

  // default values
  e = er;
  z = z0;
  WEff = W;

  // WHEELER
  if (!strcmp (Model, "Wheeler")) {
    nr_double_t a, b, c, d, x, dW1, dWr, Wr;

    // compute strip thickness effect
    if (t != 0) {
      dW1 = t / M_PI * log (4 * M_E / sqrt (sqr (t / h) + 
					    sqr (M_1_PI / (W / t + 1.10))));
    }
    else dW1 = 0;
    dWr = (1 + 1 / er) / 2 * dW1;
    Wr  = WEff = W + dWr;

    // compute characteristic impedance
    if (W / h < 3.3) {
      c = log (4 * h / Wr + sqrt (sqr (4 * h / Wr) + 2));
      b = (er - 1) / (er + 1) / 2 * (log (M_PI_2) + log (2 * M_2_PI) / er);
      z = (c - b) * Z0 / M_PI / sqrt (2 * (er + 1));
    }
    else {
      c = 1 + log (M_PI_2) + log (Wr / h / 2 + 0.94);
      d = M_1_PI / 2 * (1 + log (sqr (M_PI) / 16)) * (er - 1) / sqr (er);
      x = 2 * M_LN2 / M_PI + Wr / h / 2 + (er + 1) / 2 / M_PI / er * c + d;
      z = Z0 / 2 / x / sqrt (er);
    }

    // compute effective dielectric constant
    if (W / h < 1.3) {
      a = log (8 * h / Wr) + sqr (Wr / h) / 32;
      b = (er - 1) / (er + 1) / 2 * (log (M_PI_2) + log (2 * M_2_PI) / er);
      e = (er + 1) / 2 * sqr (a / (a - b));
    }
    else {
      a = (er - 1) / 2 / M_PI / er * (log (2.1349 * Wr / h + 4.0137) - 
				      0.5169 / er);
      b = Wr / h / 2 + M_1_PI * log (8.5397 * Wr / h + 16.0547);
      e = er * sqr ((b - a) / b);
    }
  }
  // SCHNEIDER
  else if (!strcmp (Model, "Schneider")) {
    nr_double_t d;
    Hammerstad_er (W / h, er, 1, 0.5, e);
    Hammerstad_zl (W / h, d);
    z = d / sqrt (e);
  }
  // HAMMERSTAD and JENSEN
  else if (!strcmp (Model, "Hammerstad")) {
    nr_double_t a, b, du1, du, u, ur, u1, zr, z1;

    u = W / h; // normalized width
    t = t / h; // normalized thickness

    // compute strip thickness effect
    if (t != 0) {
      du1 = t / M_PI * log (1 + 4 * M_E / t / 
			    sqr (coth (sqrt (6.517 * W / h))));
    }
    else du1 = 0;
    du = du1 * (1 + sech (sqrt (er - 1))) / 2;
    u1 = u + du1;
    ur = u + du;
    WEff = ur * h;

    // compute impedances for homogeneous medium
    Hammerstad_zl (ur, zr);
    Hammerstad_zl (u1, z1);
    
    // compute effective dielectric constant
    Hammerstad_ab (ur, er, a, b);
    Hammerstad_er (ur, er, a, b, e);

    // compute final characteristic impedance and dielectric constant
    // including strip thickness effects
    z = zr / sqrt (e);
    e = e * sqr (z1 / zr);
  }

  ZlEff = z;
  ErEff = e;
}

/* This function calculates the frequency dependent value of the
   effective dielectric constant and the microstrip line impedance for
   the given frequency. */
void msline::analyseDispersion (nr_double_t W, nr_double_t h, nr_double_t er,
				nr_double_t ZlEff, nr_double_t ErEff,
				nr_double_t frequency, char * Model,
				nr_double_t& ZlEffFreq,
				nr_double_t& ErEffFreq) {

  nr_double_t e, z;

  // default values
  z = ZlEffFreq = ZlEff;
  e = ErEffFreq = ErEff;

  // GETSINGER
  if (!strcmp (Model, "Getsinger")) {
    Getsinger_disp (h, er, ErEff, ZlEff, frequency, e, z);
  }
  // SCHNEIDER
  else if (!strcmp (Model, "Schneider")) {
    nr_double_t k, f;
    k = sqrt (ErEff / er);
    f = 4 * h * frequency / C0 * sqrt (er - 1);
    e = ErEff * sqr ((1 + sqr (f)) / (1 + k * sqr (f)));
  }
  // YAMASHITA
  else if (!strcmp (Model, "Yamashita")) {
    nr_double_t k, f;
    k = sqrt (er / ErEff);
    f = 4 * h * frequency / C0 * sqrt (er - 1) *
      (0.5 + sqr (1 + 2 * log10 (1 + W / h)));
    e = ErEff * sqr ((1 + k * pow (f, 1.5) / 4) / (1 + pow (f, 1.5) / 4));
  }  
  // KOBAYASHI
  else if (!strcmp (Model, "Kobayashi")) {
    nr_double_t n, no, nc, fh, fk;
    fk = C0 * atan (er * sqrt ((ErEff - 1) / (er - ErEff))) /
      (2 * M_PI * h * sqrt (er - ErEff));
    fh = fk / (0.75 + (0.75 - 0.332 / pow (er, 1.73)) * W / h);
    no = 1 + 1 / (1 + sqrt (W / h)) + 0.32 * cubic (1 / (1 + sqrt (W / h))); 
    if (W / h < 0.7) {
      nc = 1 + 1.4 / (1 + W / h) * (0.15 - 0.235 * 
				    exp (-0.45 * frequency / fh));
    }
    else nc = 1;
    n = no * nc < 2.32 ? no * nc : 2.32;
    e = er - (er - ErEff) / (1 + pow (frequency / fh, n));
  }
  // PRAMANICK and BHARTIA
  else if (!strcmp (Model, "Pramanick")) {
    nr_double_t Weff, We, f;
    f = 2 * MU0 * h * frequency * sqrt (ErEff / er) / ZlEff;
    e = er - (er - ErEff) / (1 + sqr (f));
    Weff = Z0 * h / ZlEff / sqrt (ErEff);
    We = W + (Weff - W) / (1 + sqr (f));
    z = Z0 * h / We / sqrt (e);
  }
  // HAMMERSTAD and JENSEN
  else if (!strcmp (Model, "Hammerstad")) {
    nr_double_t f, g;
    g = sqr (M_PI) / 12 * (er - 1) / ErEff * sqrt (2 * M_PI * ZlEff / Z0);
    f = 2 * MU0 * h * frequency / ZlEff;
    e = er - (er - ErEff) / (1 + g * sqr (f));
    z = ZlEff * sqrt (ErEff / e) * (e - 1) / (ErEff - 1);
  }
  // KIRSCHNING and JANSEN
  else if (!strcmp (Model, "Kirschning")) {
    nr_double_t fs, p, p1, p2, p3, p4;
    fs = frequency * h / 1e6;
    p1 = 0.27488 + (0.6315 + 0.525 / pow (1 + 0.0157 * fs, 20)) * W / h - 
      0.065683 * exp (-8.7513 * W / h);
    p2 = 0.33622 * (1 - exp (-0.03442 * er));
    p3 = 0.0363 * exp (-4.6 * W / h) * (1 - exp (- pow (fs / 38.7, 4.97)));
    p4 = 1 + 2.751 * (1 - exp (- pow (er / 15.916, 8)));
    p  = p1 * p2 * pow ((0.1844 + p3 * p4) * fs, 1.5763);
    e  = er - (er - ErEff) / (1 + p);

    nr_double_t r01, r02, r03, r04, r05, r06, r07, r08, r09, r10;
    nr_double_t r11, r12, r13, r14, r15, r16, r17;
    r01 = 0.03891 * pow (er, 1.4);
    r02 = 0.267 * pow (W / h, 7);
    r03 = 4.766 * exp (-3.228 * pow (W / h, 0.641));
    r04 = 0.016 + pow (0.0514 * er, 4.524);
    r05 = pow (fs / 28.843, 12);
    r06 = 22.20 * pow (W / h, 1.92);
    r07 = 1.206 - 0.3144 * exp (-r01) * (1 - exp (-r02));
    r08 = 1 + 1.275 * (1 - exp (-0.004625 * r03 * 
				pow (er, 1.674) * pow (fs / 18.365, 2.745)));
    r09 = 5.086 * r04 * r05 / (0.3838 + 0.386 * r04) * 
      exp (-r06) / (1 + 1.2992 * r05) * 
      pow (er - 1, 6) / (1 + 10 * pow (er - 1, 6));
    r10 = 0.00044 * pow (er, 2.136) + 0.0184;
    r11 = pow (fs / 19.47, 6) / (1 + 0.0962 * pow (fs / 19.47, 6));
    r12 = 1 / (1 + 0.00245 * sqr (W / h));
    r13 = 0.9408 * pow (e, r08) - 0.9603;
    r14 = (0.9408 - r09) * pow (ErEff, r08) - 0.9603;
    r15 = 0.707 * r10 * pow (fs / 12.3, 1.097);
    r16 = 1 + 0.0503 * sqr (er) * r11 * (1 - exp (- pow (W / 15 / h, 6)));
    r17 = r07 * (1 - 1.1241 * r12 / r16 * 
		 exp (-0.026 * pow (fs, 1.15656) - r15));
    z = ZlEff * pow (r13 / r14, r17);
  }

  ZlEffFreq = z;
  ErEffFreq = e;
}

/* Computes the exponent factors a(u) and b(er) used within the
   effective relative dielectric constant calculations for single and
   coupled microstrip lines by Hammerstad and Jensen. */
void msline::Hammerstad_ab (nr_double_t u, nr_double_t er, nr_double_t& a,
			    nr_double_t& b) {
  a = 1 + log ((quadr (u) + sqr (u / 52)) / (quadr (u) + 0.432)) / 49 + 
    log (1 + cubic (u / 18.1)) / 18.7;
  b = 0.564 * pow ((er - 0.9) / (er + 3), 0.053);
}

/* The function computes the effective dielectric constant of a single
   microstrip.  The equation is used in single and coupled microstrip
   calculations. */
void msline::Hammerstad_er (nr_double_t u, nr_double_t er, nr_double_t a,
			    nr_double_t b, nr_double_t& e) {
  e = (er + 1) / 2 + (er - 1) / 2 * pow (1 + 10 / u, -a * b);
}

/* This function computes the characteristic impedance of single
   microstrip line based upon the given width-height ratio.  The
   equation is used in single and coupled microstrip calculations as
   well. */
void msline::Hammerstad_zl (nr_double_t u, nr_double_t& zl) {
  nr_double_t fu = 6 + (2 * M_PI - 6) * exp (- pow (30.666 / u, 0.7528));
  zl = Z0 / 2 / M_PI * log (fu / u + sqrt (1 + sqr (2 / u)));
}

/* Calculates dispersion effects for effective dielectric constant and
   characteristic impedance as defined by Getsinger (for single and
   coupled microstrips). */
void msline::Getsinger_disp (nr_double_t h, nr_double_t er, nr_double_t ErEff,
			     nr_double_t ZlEff, nr_double_t frequency,
			     nr_double_t& e, nr_double_t& z) {
  nr_double_t g, f, d;
  g = 0.6 + 0.009 * ZlEff;
  f = frequency * 2 * MU0 * h / ZlEff;
  e = er - (er - ErEff) / (1 + g * sqr (f));
  d = (er - e) * (e - ErEff) / e / (er - ErEff);
  z = ZlEff * sqrt (e / ErEff) / (1 + d);  // group delay model
}

/* The function calculates the conductor and dielectric losses of a
   single microstrip line. */
void msline::analyseLoss (nr_double_t W, nr_double_t t, nr_double_t er,
			  nr_double_t rho, nr_double_t D, nr_double_t tand,
			  nr_double_t ZlEff1, nr_double_t ZlEff2, 
			  nr_double_t ErEff,
			  nr_double_t frequency, char * Model,
			  nr_double_t& ac, nr_double_t& ad) {
  ac = ad = 0;

  // HAMMERSTAD and JENSEN
  if (!strcmp (Model, "Hammerstad")) {
    nr_double_t Rs, ds, l0, Kr, Ki;

    // conductor losses
    if (t != 0.0) {
      Rs = sqrt (M_PI * frequency * MU0 * rho); // skin resistance
      ds = rho / Rs;                            // skin depth
      // valid for t > 3 * ds
      if (t < 3 * ds) {
	logprint (LOG_STATUS,
		  "WARNING: conductor loss calculation invalid for line "
		  "height t (%g) < 3 * skin depth (%g)\n", t, 3 * ds);
      }
      // current distribution factor
      Ki = exp (-1.2 * pow ((ZlEff1 + ZlEff2) / 2 / Z0, 0.7));
      // D is RMS surface roughness
      Kr = 1 + M_2_PI * atan (1.4 * sqr (D / ds));
      ac = Rs / (ZlEff1 * W) * Ki * Kr;
    }

    // dielectric losses
    l0 = C0 / frequency;
    ad = M_PI * er / (er - 1) * (ErEff - 1) / sqrt (ErEff) * tand / l0;
  }
}

void msline::initDC (dcsolver *) {
  nr_double_t l     = getPropertyDouble ("L");
  nr_double_t W     = getPropertyDouble ("W");
  substrate * subst = getSubstrate ();
  nr_double_t t     = subst->getPropertyDouble ("t");
  nr_double_t rho   = subst->getPropertyDouble ("rho");

  if (t != 0.0 && rho != 0.0) {
    // tiny resistance
    nr_double_t g = t * W / rho / l;
    setY (1, 1, +g); setY (2, 2, +g); setY (1, 2, -g); setY (2, 1, -g);
    setVoltageSources (0);
  }
  else {
    // a DC short (voltage source V = 0 volts)
    setY (1, 1, 0); setY (2, 2, 0); setY (1, 2, 0); setY (2, 1, 0);
    setC (1, 1, +1.0); setC (1, 2, -1.0);
    setB (1, 1, +1.0); setB (2, 1, -1.0);
    setE (1, 0.0);
    setD (1, 1, 0.0);
    setVoltageSources (1);
    setInternalVoltageSource (1);
  }
}
