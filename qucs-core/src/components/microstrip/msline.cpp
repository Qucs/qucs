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
 * $Id: msline.cpp,v 1.14 2004-05-20 18:06:34 ela Exp $
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

#define SQR(x)   ((x) * (x))
#define CUBIC(x) ((x) * (x) * (x))
#define QUAD(x)  ((x) * (x) * (x) * (x))

#define coth(x)   (1 / tanh (x))
#define sech(x)   (1 / cosh (x))
#define cosech(x) (1 / sinh (x))

msline::msline () : circuit (2) {
  type = CIR_MSLINE;
}

void msline::calcSP (nr_double_t frequency) {

  /* how to get properties of this component, e.g. L, W */
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t W = getPropertyDouble ("W");
  char *  Model = getPropertyString ("Model");

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
  nr_double_t z, y, a, b, k0, ds, Kr, Rs, Ki, ac, ad, l0;

  // quasi-static effective dielectric constant of substrate + line and
  // the impedance of the microstrip line
  analyseQuasiStatic (W, h, t, er, "Hammerstad");

  // TODO: influence of t

  // analyse dispersion of Zl and Er
  analyseDispersion (W, h, er, frequency, Model);

  // conductor losses: HAMMERSTAD and JENSEN
  Rs = sqrt (M_PI * frequency * MU0 * rho); // skin resistance
  ds = rho / Rs;                            // skin depth
  // valid for t > 3 * ds
  if (t < 3 * ds) {
    logprint (LOG_STATUS, "line height t (%g) < 3 * skin depth (%g)\n", t, ds);
  }
  Ki = exp (-1.2 * pow (ZlEffFreq / Z0, 0.7)); // current distribution factor
  Kr = 1 + M_2_PI * atan (1.4 * SQR (D / ds)); // D is RMS surface roughness
  ac = Rs / (ZlEffFreq * W) * Ki * Kr;

  // dielectric losses
  l0 = C0 / frequency;
  ad = M_PI * er / (er - 1) * (ErEffFreq - 1) / sqrt (ErEffFreq) * tand / l0;

  z = ZlEffFreq / z0;
  y = 1 / z;
  k0 = 2 * M_PI * frequency / C0;
  a = ac + ad;
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
   line and the value of the effective dielectric constant for the
   given microstrip line and substrate properties. */
void msline::analyseQuasiStatic (nr_double_t W, nr_double_t h, nr_double_t t,
				 nr_double_t er, char * Model) {

  nr_double_t z, e;

  e = er;
  z = z0;

  // WHEELER
  if (!strcmp (Model, "Wheeler")) {
    nr_double_t a, b, c, d, x, dW1, dWr, Wr;

    dW1 = t / M_PI * log (4 * M_E / sqrt (SQR (t / h) + 
					  SQR (M_1_PI / (W / t + 1.10))));
    dWr = (1 + 1 / er) / 2 * dW1;
    Wr  = W + dWr;

    if (W / h < 3.3) {
      c = log (4 * h / Wr + sqrt (SQR (4 * h / Wr) + 2));
      b = (er - 1) / (er + 1) / 2 * (log (M_PI_2) + log (2 * M_2_PI) / er);
      z = (c - b) * Z0 / M_PI / sqrt (2 * (er + 1));
    }
    else {
      c = 1 + log (M_PI_2) + log (Wr / h / 2 + 0.94);
      d = M_1_PI / 2 * (1 + log (SQR (M_PI) / 16)) * (er - 1) / SQR (er);
      x = 2 * M_LN2 / M_PI + Wr / h / 2 + (er + 1) / 2 / M_PI / er * c + d;
      z = Z0 / 2 / x / sqrt (er);
    }

    if (W / h < 1.3) {
      a = log (8 * h / Wr) + SQR (Wr / h) / 32;
      b = (er - 1) / (er + 1) / 2 * (log (M_PI_2) + log (2 * M_2_PI) / er);
      e = (er + 1) / 2 * SQR (a / (a - b));
    }
    else {
      a = (er - 1) / 2 / M_PI / er * (log (2.1349 * Wr / h + 4.0137) - 
				      0.5169 / er);
      b = Wr / h / 2 + M_1_PI * log (8.5397 * Wr / h + 16.0547);
      e = er * SQR ((b - a) / b);
    }

    fprintf (stderr, "WHEELER e = %g, z = %g\n", e, z);
  }
  // SCHNEIDER
  else if (!strcmp (Model, "Schneider")) {
    nr_double_t c, d;
    e = (er + 1) / 2 + (er - 1) / 2 / sqrt (1 + 10 * h / W);
    c = 6 + (2 * M_PI - 6) * exp (- pow (30.666 * h / W, 0.7528));
    d = Z0 / 2 / M_PI * log (c * h / W + sqrt (1 + SQR (2 * h / W)));
    z = d / sqrt (e);
    fprintf (stderr, "SCHNEIDER e = %g, z = %g\n", e, z);
  }
  // HAMMERSTAD and JENSEN
  else if (!strcmp (Model, "Hammerstad")) {
    nr_double_t a, b, du1, du, u, ur, u1, cr, c1, zr, z1;

    u = W / h;
    du1 = t / M_PI * log (1 + 4 * M_E / t / SQR (coth (sqrt (6.517 * W / h))));
    du = du1 * (1 + sech (sqrt (er - 1))) / 2;
    u1 = u + du1;
    ur = u + du;

    cr = 6 + (2 * M_PI - 6) * exp (- pow (30.666 / ur, 0.7528));
    zr = Z0 / 2 / M_PI * log (cr / ur + sqrt (1 + SQR (2 / ur)));
    c1 = 6 + (2 * M_PI - 6) * exp (- pow (30.666 / u1, 0.7528));
    z1 = Z0 / 2 / M_PI * log (c1 / u1 + sqrt (1 + SQR (2 / u1)));
    
    a = 1 +
      1 / 49 * log ((QUAD (ur) + SQR (ur / 52)) / (QUAD (ur) + 0.432)) + 
      1 / 18.7 * log (1 + CUBIC (ur / 18.1));
    b = 0.564 * pow ((er - 0.9) / (er + 3), 0.053);
    e = (er + 1) / 2 + (er - 1) / 2 * pow (1 + 10 / ur, -a * b);

    z = zr / sqrt (e);
    e = e * SQR (z1 / zr);

    fprintf (stderr, "HAMMERSTAD e = %g, z = %g\n", e, z);
  }

  ZlEff = z;
  ErEff = e;
}

/* This function calculates the frequency dependent value of the
   effective dielectric constant and the microstrip line impedance for
   the given frequency. */
void msline::analyseDispersion (nr_double_t W, nr_double_t h, nr_double_t er,
				nr_double_t frequency, char * Model) {

  nr_double_t e, z;

  z = ZlEffFreq = ZlEff;
  e = ErEffFreq = ErEff;

  // GETSINGER
  if (!strcmp (Model, "Getsinger")) {
    nr_double_t g, f, d;
    g = 0.6 + 0.009 * ZlEff;
    f = 2 * MU0 * h * frequency / ZlEff;
    e = er - (er - ErEff) / (1 + g * SQR (f));
    d = (er - e) * (e - ErEff) / e / (er - ErEff);
    z = ZlEff * sqrt (e / ErEff) / (1 + d);
    fprintf (stderr, "GETSINGER e = %g, z = %g\n", e, z);
  }
  // SCHNEIDER
  else if (!strcmp (Model, "Schneider")) {
    nr_double_t k, f;
    k = sqrt (ErEff / er);
    f = 4 * h * frequency / C0 * sqrt (er - 1);
    e = ErEff * SQR ((1 + SQR (f)) / (1 + k * SQR (f)));
    fprintf (stderr, "SCHNEIDER e = %g\n", e);
  }
  // YAMASHITA
  else if (!strcmp (Model, "Yamashita")) {
    nr_double_t k, f;
    k = sqrt (er / ErEff);
    f = 4 * h * frequency / C0 * sqrt (er - 1) *
      (0.5 + SQR (1 + 2 * log10 (1 + W / h)));
    e = ErEff * SQR ((1 + k * pow (f, 1.5) / 4) / (1 + pow (f, 1.5) / 4));
    fprintf (stderr, "YAMASHITA e = %g\n", e);
  }  
  // KOBAYASHI
  else if (!strcmp (Model, "Kobayashi")) {
    nr_double_t n, no, nc, fh, fk;
    fk = C0 * atan (er * sqrt ((ErEff - 1) / (er - ErEff))) /
      (2 * M_PI * h * sqrt (er - ErEff));
    fh = fk / (0.75 + (0.75 - 0.332 / pow (er, 1.73)) * W / h);
    no = 1 + 1 / (1 + sqrt (W / h)) + 0.32 * CUBIC (1 / (1 + sqrt (W / h))); 
    if (W / h < 0.7) {
      nc = 1 + 1.4 / (1 + W / h) * (0.15 - 0.235 * 
				    exp (-0.45 * frequency / fh));
    }
    else nc = 1;
    n = no * nc < 2.32 ? no * nc : 2.32;
    e = er - (er - ErEff) / (1 + pow (frequency / fh, n));
    fprintf (stderr, "KOBAYASHI e = %g\n", e);
  }
  // PRAMANICK and BHARTIA
  else if (!strcmp (Model, "Pramanick")) {
    nr_double_t k, f;
    k = ErEff / er;
    f = 2 * MU0 * h * frequency / ZlEff;
    e = ErEff * (1 + SQR (f)) / (1 + k * SQR (f));
    fprintf (stderr, "PRAMANICK e = %g\n", e);
  }
  // HAMMERSTAD and JENSEN
  else if (!strcmp (Model, "Hammerstad")) {
    nr_double_t f, g;
    g = SQR (M_PI) / 12 * (er - 1) / ErEff * sqrt (2 * M_PI * ZlEff / Z0);
    f = 2 * MU0 * h * frequency / ZlEff;
    e = er - (er - ErEff) / (1 + g * SQR (f));
    z = ZlEff * sqrt (ErEff / e) * (e - 1) / (ErEff - 1);
    fprintf (stderr, "HAMMERSTAD e = %g, z = %g\n", e, z);
  }
  // KIRSCHNING and JANSEN
  else if (!strcmp (Model, "Kirschning")) {
    nr_double_t fs, p, p1, p2, p3, p4;
    fs = frequency * h / 1e6;
    p1 = 0.27488 + (0.6315 + 0.525 / pow (1 + 0.0157 * fs, 20)) * W / h - 
      0.065683 * exp (-8.7513 * W / h);
    p2 = 0.33622 * (1 - exp (-0.03442 * er));
    p3 = 0.0363 * exp (-4.6 * W / h) * (1 - exp (- pow (fs / 38.7, 4.97)));
    p4 = 1 + 2.751 * (1 - exp (- (pow (er / 15.916, 8))));
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
    r12 = 1 / (1 + 0.00245 * SQR (W / h));
    r13 = 0.9408 * pow (e, r08) - 0.9603;
    r14 = (0.9408 - r09) * pow (ErEff, r08) - 0.9603;
    r15 = 0.707 * r10 * pow (fs / 12.3, 1.097);
    r16 = 1 + 0.0503 * SQR (er) * r11 * (1 - exp (- pow (W / 15 / h, 6)));
    r17 = r07 * (1 - 1.1241 * r12 / r16 * 
		 exp (-0.026 * pow (fs, 1.15656) - r15));
    z = ZlEff * pow (r13 / r14, r17);
    fprintf (stderr, "KIRSCHNING e = %g, z = %g\n", e, z);
  }

  ZlEffFreq = z;
  ErEffFreq = e;
}
